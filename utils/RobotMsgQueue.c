/**
 * @file				RobotMsgQueue.c
 * @brief			Multi-Process Applications
 * @author			Cygnus Yang
 * @date			Monday, October 23, 2017
 * @version			Initial Draft
 * @par				Copyright (C),  2017-2023, UBT Education
 * @par History:
 * 1.Date: 			Monday, October 23, 2017
 *   Author: 			Cygnus Yang
 *   Modification: 		Created file
*/

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>


#include "voice_datatypes.h"
#include "log_pub.h"

#include "RobotMsgQueue.h"


void * RobotMessageQueueCreate(ROBOT_MSGQ_ID_E eQueueID, int iQueueSize, int iMessageSize)
{
    ROBOT_MSGQ_t *pstMsgQueue;
    pthread_mutexattr_t attr;

    pstMsgQueue = malloc(sizeof(ROBOT_MSGQ_t));

    pstMsgQueue->uiMaxSize = iQueueSize;
    pstMsgQueue->uiMsgSize = iMessageSize;
    pstMsgQueue->uiMsgNum = 0;
    pstMsgQueue->uiHead = 0;
    pstMsgQueue->uiTail = 0;

    pstMsgQueue->uiBufSize = iQueueSize * iMessageSize;
    pstMsgQueue->pcBuf = malloc (pstMsgQueue->uiBufSize);
    memset (pstMsgQueue->pcBuf, 0, pstMsgQueue->uiBufSize);

    pthread_mutexattr_init(&attr);
    pthread_mutex_init(&pstMsgQueue->mutex, &attr);

    sem_init (&pstMsgQueue->tx_sema, 0, iQueueSize);
    sem_init (&pstMsgQueue->rx_sema, 0, 0);

	pstMsgQueue->eQueueID = eQueueID;
    return pstMsgQueue;
}

UBTEDU_RC_T RobotMessageQueueGetNumMsgs(void *pQueue, int *piMsgNum)
{
    ROBOT_MSGQ_t *pstMsgQ = pQueue;

    pthread_mutex_lock(&pstMsgQ->mutex);
    *piMsgNum = pstMsgQ->uiMsgNum;
    pthread_mutex_unlock(&pstMsgQ->mutex);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T RobotMessagePeek(void *pQueue, void *pMessage,
                                 unsigned int uiSize, unsigned int uiMsgOffset)
{
    ROBOT_MSGQ_t *pstMsgQ = pQueue;
    unsigned int uiMsgToRead;

    pthread_mutex_lock(&pstMsgQ->mutex);
    if ((pstMsgQ->uiMsgNum == 0) || ((pstMsgQ->uiMsgNum - 1) < uiMsgOffset)) {
        pthread_mutex_unlock(&pstMsgQ->mutex);
        return UBTEDU_RC_FAILED;
    }

    uiMsgToRead = (pstMsgQ->uiHead + (uiMsgOffset * pstMsgQ->uiMsgSize)) % pstMsgQ->uiBufSize;
    memcpy(pMessage, &pstMsgQ->pcBuf[uiMsgToRead],
           ((uiSize < pstMsgQ->uiMsgSize) ? uiSize : pstMsgQ->uiMsgSize));


    pthread_mutex_unlock(&pstMsgQ->mutex);
    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T RobotMessageReceive(void *pQueue, void *pMessage,
                                    unsigned int uiSize, unsigned int uiWait)
{
    ROBOT_MSGQ_t *pstMsgQ = pQueue;
    int err;

    if((uiWait != ROBOT_MSGQ_WAIT_FOREVER) && (uiWait != ROBOT_MSGQ_NO_WAIT)) {
        return UBTEDU_RC_WRONG_PARAM;
    }

    if(uiWait == ROBOT_MSGQ_WAIT_FOREVER) {
        do {
            err = sem_wait(&pstMsgQ->rx_sema);
        } while (err != 0 && errno == EINTR);
    } else {
        err = sem_trywait(&pstMsgQ->rx_sema);
    }
    if(err) {
        return UBTEDU_RC_FAILED;
    }

    pthread_mutex_lock(&pstMsgQ->mutex);
    memcpy(pMessage, &pstMsgQ->pcBuf[pstMsgQ->uiHead],
           ((uiSize < pstMsgQ->uiMsgSize) ? uiSize : pstMsgQ->uiMsgSize));
    pstMsgQ->uiHead += pstMsgQ->uiMsgSize;
    pstMsgQ->uiHead %= pstMsgQ->uiBufSize;
    pstMsgQ->uiMsgNum--;
    pthread_mutex_unlock(&pstMsgQ->mutex);

    sem_post (&pstMsgQ->tx_sema);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T RobotMessageSend(void *pQueue, void *pMessage, unsigned int uiSize,
                                 unsigned int uiWait, unsigned int uiPriority)
{
    ROBOT_MSGQ_t *pstMsgQ = pQueue;
    int err;

    if((uiWait != ROBOT_MSGQ_WAIT_FOREVER) && (uiWait != ROBOT_MSGQ_NO_WAIT)) {
        return UBTEDU_RC_WRONG_PARAM;
    }

    if(uiWait == ROBOT_MSGQ_WAIT_FOREVER) {
        do {
            err = sem_wait(&pstMsgQ->tx_sema);
        } while (err != 0 && errno == EINTR);
    } else {
        err = sem_trywait(&pstMsgQ->tx_sema);
    }
    if(err) {
        return UBTEDU_RC_FAILED;
    }

    pthread_mutex_lock(&pstMsgQ->mutex);
    memcpy(&pstMsgQ->pcBuf[pstMsgQ->uiTail], pMessage,
           ((uiSize < pstMsgQ->uiMsgSize) ? uiSize : pstMsgQ->uiMsgSize));
    pstMsgQ->uiTail += pstMsgQ->uiMsgSize;
    pstMsgQ->uiTail %= pstMsgQ->uiBufSize;
    pstMsgQ->uiMsgNum++;
    pthread_mutex_unlock(&pstMsgQ->mutex);

    sem_post (&pstMsgQ->rx_sema);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T RobotMessageQueueDelete(void *pQueue)
{
    ROBOT_MSGQ_t *pstMsgQ = pQueue;

    pthread_mutex_destroy(&pstMsgQ->mutex);

    sem_destroy (&pstMsgQ->tx_sema);
    sem_destroy (&pstMsgQ->rx_sema);


	if (NULL != pstMsgQ->pcBuf) 
	{
    	free (pstMsgQ->pcBuf);
	}
	if (NULL != pstMsgQ)
	{
    	free (pstMsgQ);
	}

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T RobotMessageQueueLimitGet(void *pQueue, unsigned int *puiQLimit)
{
    ROBOT_MSGQ_t *pstMsgQ = pQueue;

    pthread_mutex_lock(&pstMsgQ->mutex);
    *puiQLimit = pstMsgQ->uiMaxSize;
    pthread_mutex_unlock(&pstMsgQ->mutex);

    return UBTEDU_RC_SUCCESS;
}

