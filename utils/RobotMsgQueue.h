/**
 * @file				RobotMsgQueue.h
 * @brief			RobotMsgQueue.c header file
 * @author			Cygnus Yang
 * @date			Friday, November 24, 2017
 * @version			Initial Draft
 * @par				Copyright (C),  2017-2023, UBT Education
 * @par History:
 * 1.Date: 			Friday, November 24, 2017
 *   Author: 			Cygnus Yang
 *   Modification: 		Created file
*/
#ifndef __ROBOTMSGQUEUE_H__
#define __ROBOTMSGQUEUE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#define ROBOT_MSGQ_NO_WAIT      0
#define ROBOT_MSGQ_WAIT_FOREVER (-1)
#define ROBOT_MSGQ_MAXSIZE 	(4096)

typedef enum {
	ROBOT_VOICE_MSGQ_RECORD,
    ROBOT_VOICE_MSGQ_RECOGNITION,
    ROBOT_VOICE_MSGQ_RECORD_CMD,        
    ROBOT_VOICE_MSGQ_ACTION,    
    ROBOT_VOICE_MSGQ_TTS,
    ROBOT_AGENT_MSGQ_SWARM,
	ROBOT_VOICE_MSGQ_LAST
} ROBOT_MSGQ_ID_E;

typedef struct {
    ROBOT_MSGQ_ID_E eQueueID;        /* Queue ID */
    unsigned int uiMaxSize;  		/* Maximum messages in the queue */
    unsigned int uiMsgSize;  		/* Size of each message in the queue */
    unsigned int uiMsgNum;  		/* Current number of messages in the queue */

    unsigned int uiHead; 		/* First message in the queue (Next to be dequeued) */
    unsigned int uiTail; 		/* Last message in the queue */

    unsigned char *pcBuf;  		/* Buffer for storing the messages */
    unsigned int  uiBufSize; 	/* Number of bytes in the queue buffer */

    sem_t tx_sema; /* Block callers when queue is full */
    sem_t rx_sema; /* Block callers when queue is empty */

    pthread_mutex_t mutex; /* Protect access to the queue structure */

} ROBOT_MSGQ_t;

extern UBTEDU_RC_T RobotMessagePeek(void *pQueue, void *pMessage,
                                          unsigned int uiSize, unsigned int uiMsgOffset);
extern void * RobotMessageQueueCreate(ROBOT_MSGQ_ID_E eQueueID, int iQueueSize, int iMessageSize);
extern UBTEDU_RC_T RobotMessageQueueDelete(void *pQueue);
extern UBTEDU_RC_T RobotMessageQueueGetNumMsgs(void *pQueue, int *piMsgNum);
extern UBTEDU_RC_T RobotMessageQueueLimitGet(void *pQueue, unsigned int *puiQLimit);
extern UBTEDU_RC_T RobotMessageReceive(void *pQueue, void *pMessage,
                                             unsigned int uiSize, unsigned int uiWait);
extern UBTEDU_RC_T RobotMessageSend(void *pQueue, void *pMessage, unsigned int uiSize,
                                          unsigned int uiWait, unsigned int uiPriority);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __ROBOTMSGQUEUE_H__ */
