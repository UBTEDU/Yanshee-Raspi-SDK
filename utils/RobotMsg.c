/**
 * @file                RobotMsg.c
 * @brief           Defines the messages to robot
 * @author          Cygnus Yang
 * @date            Wednesday, August 30, 2017
 * @version         Initial Draft
 * @par             Copyright (C),  2017-2023, UBT Education
 * @par History:
 * 1.Date:          Wednesday, August 30, 2017
 *   Author:            Cygnus Yang
 *   Modification:      Created file
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/sockios.h>

#include "voice_datatypes.h"

#include "cJSON.h"
#include "robotlogstr.h"
#include "RobotMsg.h"

#ifdef __DEBUG_PRINT__                                            // 对于DEBUG版本，增加打印信息
#define DebugTrace(...)\
        do{\
            fprintf(stderr,__VA_ARGS__);\
        }while(0)
#else
#define DebugTrace( ... )                // 对于RELEASE版本，把__DEBUG_PRINT__宏关闭 
#endif


UBTEDU_RC_T ubtRobot_Msg_Encode_SWVersion(char *pcParam, int iPort,
        char *pcSendBuf, int iBufLen)
{
    cJSON *pJsonRoot = NULL;

    /* Check parameters */
    if ((NULL == pcSendBuf) || (NULL == pcParam))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Query);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Version);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Para, pcParam);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port, iPort);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SWVersion(char *pcRecvBuf, char *pcVersion, int iVersionLen)
{
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acRet[MSG_CMD_STR_MAX_LEN];
    char acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcVersion) || (NULL == pcRecvBuf))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acRet[0] = '\0';
    acType[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (0 != strcmp(pNode->valuestring, pcStr_Msg_Cmd_Query_Ack))
                {
                    /* Wrong cmd value */
                    ret = UBTEDU_RC_SOCKET_DECODE_ERROR;
                    break;
                }
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Type);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strcpy(acType, pNode->valuestring) ;
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strcpy(acRet, pNode->valuestring) ;
            }
        }

        if (!strcmp(acRet, "ok") && !strcmp(acType, pcStr_Msg_Type_Version))
        {
            pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Type_Version);
            if (pNode != NULL)
            {
                if (pNode->type == cJSON_String)
                {
                    strncpy(pcVersion, pNode->valuestring, iVersionLen);
                    ret = UBTEDU_RC_SUCCESS;
                    break;
                }
            }
        }
    }
    while (0);
    cJSON_Delete(pJson);
    return ret;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_RobotStatus(char *pcCmd, char *pcType, char *pcParam, int iPort,
        char *pcSendBuf, int iBufLen)
{
    cJSON *pJsonRoot = NULL;

    /* Check parameters */
    if ((NULL == pcSendBuf) || (NULL == pcCmd) || (NULL == pcType))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcCmd);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcType);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port, iPort);
    if (NULL != pcParam)
    {
        cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Para, pcParam);
    }
    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_RobotStatus(char *pcType, char *pcRecvBuf, void *pStatus)
{
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    cJSON *pSubNode = NULL;
    char acType[MSG_CMD_STR_MAX_LEN];
    char acParam[MSG_CMD_STR_MAX_LEN];
    UBTEDU_ROBOT_Battery_T *pstRobotBattery;
    int *piValue;

    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == pcType))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    acType[0] = '\0';
    acParam[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pJson   = cJSON_Parse(pcRecvBuf);
        if (pJson == NULL)
        {
            printf("Parse json message filed!\r\n");
            return UBTEDU_RC_SOCKET_DECODE_FAILED;
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                /* Check the cmd value */
                if (0 != strcmp(pNode->valuestring, pcStr_Msg_Cmd_Query_Ack))
                {
                    /* Wrong cmd value */
                    ret = UBTEDU_RC_SOCKET_DECODE_ERROR;
                    break;
                }
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Type);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (0 != strcmp(pNode->valuestring, pcType))
                {
                    /* Wrong cmd value */
                    ret = UBTEDU_RC_SOCKET_DECODE_ERROR;
                    break;
                }
                strncpy(acType, pNode->valuestring, sizeof(acType));
            }
        }


        if (!strcmp(acType, pcStr_Msg_Type_Play))
        {
            pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Type_Play);
            if (pNode != NULL)
            {
                if (pNode->type == cJSON_String)
                {
                    strncpy(acParam, pNode->valuestring, sizeof(acParam));
                }
                piValue = (int *)pStatus;
                if (!strcmp(acParam, pcStr_Msg_Param_Query_Play_Playing))
                {
                    *piValue = UBTEDU_ROBOT_PLAY_STATUS_PLAYING;
                }
                else if (!strcmp(acParam, pcStr_Msg_Param_Query_Play_Pause))
                {
                    *piValue = UBTEDU_ROBOT_PLAY_STATUS_PAUSED;
                }
                else if (!strcmp(acParam, pcStr_Msg_Param_Query_Play_End))
                {
                    *piValue = UBTEDU_ROBOT_PLAYSTATUS_END;
                }
                else if (!strcmp(acParam, pcStr_Msg_Param_Query_Play_idle))
                {
                    *piValue = UBTEDU_ROBOT_PLAY_STATUS_IDLE;
                }
                else
                {
                    ret = UBTEDU_RC_SOCKET_DECODE_ERROR;
                }
                ret     = UBTEDU_RC_SUCCESS;
            }
        }


        if (!strcmp(acType, pcStr_Msg_Type_Volume))
        {
            pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Type_Volume);
            if (pNode != NULL)
            {
                piValue = (int *)pStatus;
                if (pNode->type == cJSON_Number)
                {
                    *piValue = pNode->valueint;
                }
                ret     = UBTEDU_RC_SUCCESS;
            }
        }

        if (!strcmp(acType, pcStr_Msg_Type_Battery))
        {
            if ((pNode = cJSON_GetObjectItem(pJson, pcStr_Msg_Type_Battery)) != NULL)
            {
                pstRobotBattery = (UBTEDU_ROBOT_Battery_T*)pStatus;
                if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Power_Charging)) != NULL)
                {
                    if (pSubNode->type == cJSON_Number)
                    {
                        pstRobotBattery->iValue = pSubNode->valueint;
                    }
                }

                if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Power_Voltage)) != NULL)
                {
                    if (pSubNode->type == cJSON_Number)
                    {
                        pstRobotBattery->iValue = pSubNode->valueint;
                    }
                }

                if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Power_Percent)) != NULL)
                {
                    if (pSubNode->type == cJSON_Number)
                    {
                        pstRobotBattery->iValue = pSubNode->valueint;
                        ret     =  UBTEDU_RC_SUCCESS;
                    }
                }
            }
        }

    }
    while (0);
    cJSON_Delete(pJson);
    return ret;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_CheckAPPStatus(char *pcCmd, char *pcType,
        int iPort, struct timeval stTimeVal,
        char *pcBuf, char *pcSendBuf, int iBufLen)
{
    cJSON *pJsonRoot = NULL;
    struct timeval tsock = {30, 0};


    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    memcpy(&tsock, &stTimeVal, sizeof(struct timeval));
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Query_App);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Ret_Msg_Status);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Time_Out, tsock.tv_sec);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Data, pcBuf);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);


    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_CheckAPPStatus(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char acCmd[MSG_CMD_STR_MAX_LEN];
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Query_App_Ack))
                {
                    ubtRet = UBTEDU_RC_SUCCESS;
                }
            }
        }

    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_StopVoiceRecognition(int iPort,
        char *pcBuf, char *pcSendBuf, int iBufLen)
{
    cJSON *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Voice);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Recognition_Stop);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Data, pcBuf);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);

    cJSON_Delete(pJsonRoot);
    return UBTEDU_RC_SUCCESS;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_DetectVoiceMsg(int iPort,
        char *pcBuf, char *pcSendBuf, int iBufLen)
{
    cJSON *pJsonRoot = NULL;



    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Voice);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Voice_Detecting);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Data, pcBuf);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);

    cJSON_Delete(pJsonRoot);
    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_DetectVoiceMsg(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];



    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ubtRet = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ubtRet;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Voice_Ack))
                {
                    ubtRet     =  UBTEDU_RC_SUCCESS;
                }
            }
        }
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_ReadRobotServo(int iPort, char *pcSendBuf, int iBufLen)
{
    cJSON *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Servo);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Servo_Read_Hold);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_ReadRobotServo(char *pcRecvBuf, char *pcAllAngle, int iAngleLen)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL, *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ubtRet = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ubtRet;
    }

    do
    {
        pNode = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode = cJSON_GetObjectItem(pJson, pcStr_Msg_Type_Servo_Angle);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(pcAllAngle, pNode->valuestring, iAngleLen);
                ubtRet = UBTEDU_RC_SUCCESS;
            }
        }

        if (strcmp(acCmd, pcStr_Msg_Cmd_Servo_Ack))
        {
            ubtRet = UBTEDU_RC_SOCKET_DECODE_ERROR;
            break;
        }
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}



UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotServo(int iPort, char *pcAllAngle, int iTime,
        char *pcSendBuf, int iBufLen)
{
    cJSON *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    /* 20 means 1s */
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Servo);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Servo_Write);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Type_Servo_Time, iTime);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type_Servo_Angle, pcAllAngle);
    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SetRobotServo(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Servo_Ack))
                {
                    ret = UBTEDU_RC_SUCCESS;
                }
            }
        }

    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotVolume_Plus(int iPort, char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Set);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Volume_Plus);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotVolume_Minus(int iPort, char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Set);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Volume_Minus);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotVolume(int iPort, int iVolume,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Set);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Volume);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Type_Volume, iVolume);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SetRobotVolume(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ubtRet = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ubtRet;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Set_Ack))
                {
                    ubtRet = UBTEDU_RC_SUCCESS;
                }
            }
        }
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}



UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotMotion(char *pcCmd, char *pcType,
        int iPort, int iVolume,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcCmd);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcType);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Cmd_Voice, iVolume);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SetRobotMotion(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Set_Ack))
                {
                    ret = UBTEDU_RC_SUCCESS;
                }
            }
        }
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}



UBTEDU_RC_T ubtRobot_Msg_Encode_ReadSensorValue(char *pcSensorType, int iPort,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Query);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Sensor);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Para, pcSensorType);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port, iPort);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_ReadSensorValueByAddr(char *pcSensorType, int iAddr, int iPort,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Query);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Sensor);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Para, pcSensorType);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Param_Query_Sensor_ID, iAddr);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port, iPort);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_ReadSensorValue(char *pcRecvBuf, char *pcSensorType, void *pValue, int iValueLen)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON       *pJson = NULL;
    cJSON       *pNode = NULL;
    cJSON       *pSubArray = NULL, *pSubNode = NULL;
    UBTEDU_ROBOTGYRO_SENSOR_T       *pstRobotGyro = NULL;
    UBTEDU_ROBOTENV_SENSOR_T        *pstRobotEnv = NULL;
    UBTEDU_ROBOTRASPBOARD_SENSOR_T  *pstRobotBrdTemp = NULL;
    UBTEDU_ROBOTINFRARED_SENSOR_T   *pstRobotInfrared = NULL;
    UBTEDU_ROBOTULTRASONIC_SENSOR_T *pstRobotUltrasnic = NULL;
    UBTEDU_ROBOTTOUCH_SENSOR_T   *pstRobotTouch = NULL;
    UBTEDU_ROBOTCOLOR_SENSOR_T   *pstRobotColor = NULL;
    UBTEDU_ROBOTPRESSURE_SENSOR_T   *pstRobotPressure = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];
    char acType[MSG_CMD_STR_MAX_LEN];
    int iCount, i;

    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    pstRobotInfrared = pstRobotInfrared;
    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == pValue) || (NULL == pcSensorType))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';
    acType[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Type);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strcpy(acType, pNode->valuestring);
            }
        }

        if ((pNode = cJSON_GetObjectItem(pJson, pcStr_Msg_Param_Query_Sensor_GYRO)) != NULL)
        {
            pstRobotGyro = (UBTEDU_ROBOTGYRO_SENSOR_T *)pValue;
            if (iValueLen != sizeof(UBTEDU_ROBOTGYRO_SENSOR_T))
            {
                return UBTEDU_RC_WRONG_PARAM;
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_GYRO_X)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dGyroxValue = pSubNode->valuedouble;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_GYRO_Y)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dGyroyValue = pSubNode->valuedouble;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_GYRO_Z)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dGyrozValue = pSubNode->valuedouble;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_ACCEL_X)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dAccexValue = pSubNode->valuedouble;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_ACCEL_Y)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dAcceyValue = pSubNode->valuedouble;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_ACCEL_Z)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dAccezValue = pSubNode->valuedouble;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_COMPASS_X)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dCompassxValue = pSubNode->valuedouble;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_COMPASS_Y)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dCompassyValue = pSubNode->valuedouble;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_COMPASS_Z)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dCompasszValue = pSubNode->valuedouble;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_EULER_X)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dEulerxValue = pSubNode->valuedouble;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_EULER_Y)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dEuleryValue = pSubNode->valuedouble;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_EULER_Z)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotGyro->dEulerzValue = pSubNode->valuedouble;
                }
            }
            ubtRet = UBTEDU_RC_SUCCESS;

        }
        else if ((pNode = cJSON_GetObjectItem(pJson, pcStr_Msg_Param_Query_Sensor_ENV)) != NULL)
        {
            pstRobotEnv = (UBTEDU_ROBOTENV_SENSOR_T *)pValue;
            if (iValueLen != sizeof(UBTEDU_ROBOTENV_SENSOR_T))
            {
                return UBTEDU_RC_WRONG_PARAM;
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_ENV_Temperature)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotEnv->iTempValue = pSubNode->valueint;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_ENV_Humidity)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotEnv->iHumiValue = pSubNode->valueint;
                }
            }
            if ((pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_ENV_Pressure)) != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotEnv->iPresValue = pSubNode->valueint;
                }
            }

            ubtRet = UBTEDU_RC_SUCCESS;
        }
        else if ((pNode = cJSON_GetObjectItem(pJson, pcStr_Msg_Param_Query_Sensor_Board)) != NULL)
        {
            pstRobotBrdTemp = (UBTEDU_ROBOTRASPBOARD_SENSOR_T *)pValue;
            if (iValueLen != sizeof(UBTEDU_ROBOTRASPBOARD_SENSOR_T))
            {
                return UBTEDU_RC_WRONG_PARAM;
            }
            pSubNode = cJSON_GetObjectItem(pNode, pcStr_Msg_Param_Query_Sensor_Board_Temperature);
            if (pSubNode != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotBrdTemp->iValue = pSubNode->valueint;
                }
            }

            ubtRet = UBTEDU_RC_SUCCESS;
        }
        else if ((pNode= cJSON_GetObjectItem(pJson, pcStr_Msg_Param_Query_Sensor_Ultrasonic)) != NULL)
        {
            pstRobotUltrasnic = (UBTEDU_ROBOTULTRASONIC_SENSOR_T *)pValue;
            if (iValueLen != sizeof(UBTEDU_ROBOTULTRASONIC_SENSOR_T))
            {
                return UBTEDU_RC_WRONG_PARAM;
            }
            iCount = cJSON_GetArraySize(pNode);
            DebugTrace("Ultrasonic Array Number:%d\r\n", iCount);
            pstRobotUltrasnic->iValue = 0xffff;
            for(i=0; i<iCount; i++)
            {
                pSubArray = cJSON_GetArrayItem(pNode, i);
                pSubNode = cJSON_GetObjectItem(pSubArray, pcStr_Msg_Param_Query_Sensor_Value);
                if (pSubNode != NULL)
                {
                    if (pSubNode->type == cJSON_Number)
                    {
                        if(pSubNode->valueint  && (pstRobotUltrasnic->iValue > pSubNode->valueint))
                            pstRobotUltrasnic->iValue = pSubNode->valueint;
                        ubtRet = UBTEDU_RC_SUCCESS;
                    }
                }
            }
        }
        else if ((pNode = cJSON_GetObjectItem(pJson, pcStr_Msg_Param_Query_Sensor_Infrared)) != NULL)
        {
            pstRobotUltrasnic = (UBTEDU_ROBOTULTRASONIC_SENSOR_T *)pValue;
            if (iValueLen != sizeof(UBTEDU_ROBOTULTRASONIC_SENSOR_T))
            {
                return UBTEDU_RC_WRONG_PARAM;
            }
            iCount = cJSON_GetArraySize(pNode);
            DebugTrace("Infrared Array Number:%d\r\n", iCount);
            pstRobotUltrasnic->iValue = 0xffff;
            for(i=0; i<iCount; i++)
            {
                pSubArray = cJSON_GetArrayItem(pNode, i);
                pSubNode = cJSON_GetObjectItem(pSubArray, pcStr_Msg_Param_Query_Sensor_Value);
                if (pSubNode != NULL)
                {
                    if (pSubNode->type == cJSON_Number)
                    {
                        if(pSubNode->valueint  && (pstRobotUltrasnic->iValue > pSubNode->valueint))
                            pstRobotUltrasnic->iValue = pSubNode->valueint;
                        ubtRet = UBTEDU_RC_SUCCESS;
                    }
                }
            }
        }
        else if ((pNode= cJSON_GetObjectItem(pJson, pcStr_Msg_Param_Query_Sensor_Touch)) != NULL)
        {
            pstRobotTouch= (UBTEDU_ROBOTTOUCH_SENSOR_T *)pValue;
            if (iValueLen != sizeof(UBTEDU_ROBOTTOUCH_SENSOR_T))
            {
                return UBTEDU_RC_WRONG_PARAM;
            }
            iCount = cJSON_GetArraySize(pNode);
            if(iCount < 1)
            {
                DebugTrace("Touch Array is NULL");
                return UBTEDU_RC_NOT_FOUND;
            }
            pSubArray = cJSON_GetArrayItem(pNode, 0);
            pSubNode = cJSON_GetObjectItem(pSubArray, pcStr_Msg_Param_Query_Sensor_Value);
            if (pSubNode != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotTouch->iValue = pSubNode->valueint;
                    ubtRet = UBTEDU_RC_SUCCESS;
                }
            }
        }
        else if ((pNode= cJSON_GetObjectItem(pJson, pcStr_Msg_Param_Query_Sensor_Color)) != NULL)
        {
            pstRobotColor= (UBTEDU_ROBOTCOLOR_SENSOR_T *)pValue;
            if (iValueLen != sizeof(UBTEDU_ROBOTCOLOR_SENSOR_T))
            {
                return UBTEDU_RC_WRONG_PARAM;
            }
            iCount = cJSON_GetArraySize(pNode);
            if(iCount < 1)
            {
                DebugTrace("Color Array is NULL");
                return UBTEDU_RC_NOT_FOUND;
            }
            pSubArray = cJSON_GetArrayItem(pNode, 0);
            pSubNode = cJSON_GetObjectItem(pSubArray, pcStr_Msg_Param_Query_Sensor_Value);
            if (pSubNode != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotColor->iRedValue = pSubNode->valueint&0xff;
                    pstRobotColor->iGreenValue = (pSubNode->valueint>>8) &0xff;
                    pstRobotColor->iBlueValue = (pSubNode->valueint>>16) &0xff;
                    pstRobotColor->iClearValue = (pSubNode->valueint>>24) &0xff;
                    ubtRet = UBTEDU_RC_SUCCESS;
                }
            }
        }
        else if ((pNode= cJSON_GetObjectItem(pJson, pcStr_Msg_Param_Query_Sensor_Pressure)) != NULL)
        {
            pstRobotPressure= (UBTEDU_ROBOTPRESSURE_SENSOR_T *)pValue;
            if (iValueLen != sizeof(UBTEDU_ROBOTPRESSURE_SENSOR_T))
            {
                return UBTEDU_RC_WRONG_PARAM;
            }
            iCount = cJSON_GetArraySize(pNode);
            if(iCount < 1)
            {
                DebugTrace("Pressure Array is NULL");
                return UBTEDU_RC_NOT_FOUND;
            }
            pSubArray = cJSON_GetArrayItem(pNode, 0);
            pSubNode = cJSON_GetObjectItem(pSubArray, pcStr_Msg_Param_Query_Sensor_Value);
            if (pSubNode != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    pstRobotPressure->iValue = pSubNode->valueint;
                    ubtRet = UBTEDU_RC_SUCCESS;
                }
            }
        }
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotLED(int iPort, char *pcType,
        char *pcColor, char *pcMode,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;
    cJSON   *pArraySubJson = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Set);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_LED);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    if ((pArraySubJson = cJSON_CreateObject()) == NULL)
    {
        cJSON_Delete(pJsonRoot);
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    /* Only the voice recognition LED can be set by user */
    /* The other LED such as servo, camera and mic LED cannot be controled
    by the user */
    cJSON_AddStringToObject(pArraySubJson, pcStr_Msg_Type, pcType);
    cJSON_AddStringToObject(pArraySubJson, pcStr_Msg_Param_Led_Color, pcColor);
    cJSON_AddStringToObject(pArraySubJson, pcStr_Msg_Param_Led_Mode, pcMode);
    cJSON_AddItemToObject(pJsonRoot, pcStr_Msg_Para, pArraySubJson);


    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SetRobotLED(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Set_Ack))
                {
                    ret = UBTEDU_RC_SUCCESS;
                }
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}



UBTEDU_RC_T ubtRobot_Msg_Encode_StartRobotAction(int iPort,
        char *pcName, int iRepeat,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;
    cJSON   *pArrayJson = NULL;
    cJSON   *pArraySubJson = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Action);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Action_Start);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    if ((pArrayJson = cJSON_CreateArray()) == NULL)
    {
        cJSON_Delete(pJsonRoot);
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    if ((pArraySubJson = cJSON_CreateObject()) == NULL)
    {
        cJSON_Delete(pJsonRoot);
        cJSON_Delete(pArrayJson);
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pArraySubJson, pcStr_Msg_Name, pcName);
    cJSON_AddNumberToObject(pArraySubJson, pcStr_Msg_Repeat, iRepeat);
    cJSON_AddItemToObject(pJsonRoot, pcStr_Msg_Para, pArraySubJson);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_StartRobotAction(char *pcRecvBuf, int *piTime)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_TotalTime);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_Number)
            {
                *piTime = pNode->valueint;
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Action_Ack))
                {
                    ret = UBTEDU_RC_SUCCESS;
                }
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_StopRobotAction(int iPort,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Action);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Action_Stop);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_StopRobotAction(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Action_Ack))
                {
                    ret = UBTEDU_RC_SUCCESS;
                }
            }
        }

        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_VoiceStart(int iPort,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Voice);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Recognition_Start);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_VoiceStart(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Voice_Ack))
                {
                    ret = UBTEDU_RC_SUCCESS;
                }
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_VoiceStop(int iPort,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Voice);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Recognition_Stop);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_VoiceStop(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Voice_Ack))
                {
                    ret = UBTEDU_RC_SUCCESS;
                }
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}



UBTEDU_RC_T ubtRobot_Msg_Encode_VoiceTTS(int iPort,
        int isInterrupted, char *pcTTS,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Voice);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Voice_TTS);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Type_Voice_TTS_IsInterrupted,  isInterrupted);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Data, pcTTS);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_VoiceTTS(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Voice_Ack))
                {
                    ret = UBTEDU_RC_SUCCESS;
                }
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_PlayMusic(char *pcPlayMusicType, int iPort, char *pcName,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Music);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcPlayMusicType);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Param_Music_Name, pcName);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_PlayMusic(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Music_Ack))
                {
                    ret = UBTEDU_RC_SUCCESS;
                }
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_GetMusic(int iPort, int iIndex,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    /* Check parameters */
    if (NULL == pcSendBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, "music");
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_Music_GetList);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Index, iIndex);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_GetMusic(char *pcRecvBuf,
        char *pcMusicName[], int iEachMusicNameLen,
        int iMusicNameNum, int *piIndex)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Music_Ack))
                {
                    ret = UBTEDU_RC_SUCCESS;
                }
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
        /* TODO: Issue the music file list */
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_TakePhotos(char *pacPhotoName, int iPort,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_TakePhoto);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcStr_Msg_Cmd_Transmit);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    if((pacPhotoName != NULL) && strcmp(pacPhotoName,""))
        cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Name, pacPhotoName);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_TakePhotos(char *pcRecvBuf, int iPhotoNameLen)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_TakePhoto_Ack))
                {
                    ret = UBTEDU_RC_SUCCESS;
                }
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;

        /* TODO: Issue the return data from robot with photos file name */
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_EventDetect(char *pcEventType, int iPort,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Event);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcEventType);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}


UBTEDU_RC_T ubtRobot_Msg_Decode_EventDetect(char *pcRecvBuf, char *pcValue)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];
    char type[32];

    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Type);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(type, pNode->valuestring, sizeof(type));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Event_Ack))
                {

                    pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Data);
                    if (pNode != NULL)
                    {
                        if (pNode->type == cJSON_String)
                        {
                            strcpy(pcValue, pNode->valuestring);
                            DebugTrace("OK buttonValue Detected!!!!! pcValue = %s \r\n", pcValue);

                            if( !strcmp(pcValue, "0"))
                            {
                                ubtRet = UBTEDU_RC_FAILED;
                            }
                            else
                            {
                                ubtRet = UBTEDU_RC_SUCCESS;
                            }
                        }
                    }

                }

                ret = UBTEDU_RC_SUCCESS;
            }
        }

    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_VisionDetect(char *pcVisionType, int iPort,
        char *pcSendBuf, int iBufLen, int iTimeout)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Vision);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type, pcVisionType);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Time_Out,  iTimeout);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}


UBTEDU_RC_T ubtRobot_Msg_Decode_VisionDetect(char *pcRecvBuf, char *pcValue)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];
    char type[32];

    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Type);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(type, pNode->valuestring, sizeof(type));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Vision_Ack))
                {
                    if( !strcmp(type, pcStr_Msg_Type_Vision_Face))
                    {
                        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Data);
                        if (pNode != NULL)
                        {
                            if (pNode->type == cJSON_String)
                            {
                                strcpy(pcValue, pNode->valuestring);
                                DebugTrace("OK faceValue Detected!!!!! pcValue = %s \r\n", pcValue);

                                if( !strcmp(pcValue, "0"))
                                {
                                    ubtRet = UBTEDU_RC_FAILED;
                                }
                                else
                                {
                                    ubtRet = UBTEDU_RC_SUCCESS;
                                }
                            }
                        }
                    }

                    if( !strcmp(type, pcStr_Msg_Type_Vision_Hand))
                    {
                        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Data);
                        if (pNode != NULL)
                        {
                            if (pNode->type == cJSON_String)
                            {
                                strcpy(pcValue, pNode->valuestring);
                                DebugTrace("OK handValue Detected!!!!! pcValue = %s \r\n", pcValue);

                                if( !strcmp(pcValue, "0"))
                                {
                                    ubtRet = UBTEDU_RC_FAILED;
                                }
                                else
                                {
                                    ubtRet = UBTEDU_RC_SUCCESS;
                                }
                            }
                        }
                    }

                }

                ret = UBTEDU_RC_SUCCESS;
            }
        }

    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_TransmitCMD(int iPort, char *pcRemoteCmd,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if ((pJsonRoot == NULL) || (NULL == pcRemoteCmd) || (NULL == pcSendBuf))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Transparent);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port,  iPort);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Data, pcRemoteCmd);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_TransmitCMD(char *pcRecvBuf, char *pcRemoteCmdRetData, int iRemoteCmdRetDataLen)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acCmd[MSG_CMD_STR_MAX_LEN];


    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL ==  pcRemoteCmdRetData))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acCmd[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acCmd, pNode->valuestring, sizeof(acCmd));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (!strcmp(pNode->valuestring, "ok") && !strcmp(acCmd, pcStr_Msg_Cmd_Transparent_Ack))
                {
                    pNode = cJSON_GetObjectItem(pJson, pcStr_Msg_Data);
                }

                if (pNode != NULL)
                {
                    if (pNode->type == cJSON_String)
                    {
                        strncpy(pcRemoteCmdRetData, pNode->valuestring, iRemoteCmdRetDataLen);
                    }

                    ret     = UBTEDU_RC_SUCCESS;
                }
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}



UBTEDU_RC_T ubtRobot_Msg_Encode_ReportStatusToApp(char *pcName, char *pcString,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if ((pJsonRoot == NULL) || (NULL == pcName) || (NULL == pcSendBuf) ||
        (NULL == pcString))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Type_Blockly_Report);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Name, pcName);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Ret_Msg_Status, pcString);


    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_ReportStatusToApp(char *pcRecvBuf)
{
    return UBTEDU_RC_SUCCESS;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_RobotDiscovery(char *pcAccount, int iPort, char *pcRobotName,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if ((pJsonRoot == NULL) || (NULL == pcAccount) || (NULL == pcSendBuf))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Discovery);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Name, pcRobotName);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type_Version, UBTEDU_SDK_SW_VER);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port, iPort);

    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_RobotDiscovery(char *pcRecvBuf, UBTEDU_ROBOTINFO_T  *pstRobotInfo)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acStatus[MSG_CMD_STR_MAX_LEN];

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == pstRobotInfo))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acStatus[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ubtRet = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ubtRet;
    }

    do
    {
        pJson   = cJSON_Parse(pcRecvBuf);
        if (pJson == NULL)
        {
            printf("Parse json message filed!\r\n");
            ubtRet = UBTEDU_RC_SOCKET_DECODE_FAILED;
            break;
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (0 != strcmp(pNode->valuestring, pcStr_Msg_Cmd_Discovery_Ack))
                {
                    break;
                }
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Name);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(pstRobotInfo->acName, pNode->valuestring, sizeof(pstRobotInfo->acName));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_IP);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(pstRobotInfo->acIPAddr, pNode->valuestring, sizeof(pstRobotInfo->acIPAddr));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strcpy(acStatus, pNode->valuestring);
            }
        }



        ubtRet  = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pJson);

    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_ConnectRobot(char *pcAccount, int iPort,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if ((pJsonRoot == NULL) || (NULL == pcAccount) || (NULL == pcSendBuf))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Connect);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddNumberToObject(pJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type_Version, UBTEDU_SDK_SW_VER);


    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_ConnectRobot(char *pcRecvBuf, char *pcRobotName, int iRobotNameLen)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acStatus[MSG_CMD_STR_MAX_LEN];

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == pcRobotName))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acStatus[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ubtRet = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ubtRet;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (0 != strcmp(pNode->valuestring, pcStr_Msg_Cmd_Connect_Ack))
                {
                    continue;
                }
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Name);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(pcRobotName, pNode->valuestring, iRobotNameLen);
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acStatus, pNode->valuestring, sizeof(acStatus));
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_DisconnectRobot(char *pcAccount, int iPort,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if ((pJsonRoot == NULL) || (NULL == pcAccount) || (NULL == pcSendBuf))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Disconnect);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Type_Version, UBTEDU_SDK_SW_VER);


    strncpy(pcSendBuf, cJSON_Print(pJsonRoot), iBufLen);
    cJSON_Delete(pJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_DisconnectRobot(char *pcRecvBuf)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pJson = NULL;
    cJSON *pNode = NULL;
    char acName[MSG_CMD_STR_MAX_LEN];
    char acStatus[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if (NULL == pcRecvBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acName[0] = '\0';
    acStatus[0] = '\0';

    pJson   = cJSON_Parse(pcRecvBuf);
    if (pJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Cmd);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                if (0 != strcmp(pNode->valuestring, pcStr_Msg_Cmd_Disconnect_Ack))
                {
                    continue;
                }
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Msg_Name);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acName, pNode->valuestring, sizeof(acName));
            }
        }

        pNode   = cJSON_GetObjectItem(pJson, pcStr_Ret_Msg_Status);
        if (pNode != NULL)
        {
            if (pNode->type == cJSON_String)
            {
                strncpy(acStatus, pNode->valuestring, sizeof(acStatus));
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pJson);
    return ubtRet;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmInit()
{
    return UBTEDU_RC_FAILED;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmInit(char *pcRecvBuf, int *piSeq, char *pcTimezone, double *pdGyroOffset)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pcJson = NULL;
    cJSON *pcNode = NULL;
    char acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == pcTimezone) || (NULL == piSeq) ||
        (NULL == pcTimezone) || (NULL == pdGyroOffset))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acType[0] = '\0';

    pcJson   = cJSON_Parse(pcRecvBuf);
    if (pcJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Cmd);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if (0 != strcmp(pcNode->valuestring, pcStr_Msg_Cmd_Swarm))
                {
                    break;
                }
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Type);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if ( 0 != strcmp(pcNode->valuestring, pcStr_Msg_Type_SwarmInit))
                {
                    break;
                }
                strncpy(acType, pcNode->valuestring, sizeof(acType));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_SEQ);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piSeq = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_Timezone);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                strncpy(pcTimezone, pcNode->valuestring, MSG_CMD_STR_MAX_LEN*sizeof(char));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_GyroOffset);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *pdGyroOffset = pcNode->valuedouble;
            }
        }

        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pcJson);
    return ubtRet;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmInitAck(char* pcAccount, int iPort,
        int iSeq, char *pcMac, char* pcSendBuf,
        int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    /* Check parameters */
    if ((NULL == pcAccount) || (NULL == pcMac) || (NULL == pcSendBuf))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    pcJsonRoot = cJSON_CreateObject();
    if (pcJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmInitAck);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_MAC, pcMac);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}


UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmInitAck(char *pcRecvBuf)
{
    return UBTEDU_RC_FAILED;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmAllocID(char *pcAccount, int iPort,
        char *pcTimezone, int iSeq,
        char *pcSendBuf, int iBufLen)
{
    return UBTEDU_RC_FAILED;
}


UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmAllocID(char *pcRecvBuf, int *piSeq,
        int *piID, int* piCount, ROBOTAGENT_MAC_T *pcMacAddr, int iMacLen)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    int     i = 0;
    int     iCount = 0;
    cJSON   *pcJson = NULL;
    cJSON   *pcNode = NULL;
    cJSON   *pSubArray = NULL;
    cJSON   *pSubNode = NULL;
    char    acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == piID) ||
        (NULL == piSeq) || (NULL == piCount) ||
        (NULL == pcMacAddr))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acType[0] = '\0';

    pcJson   = cJSON_Parse(pcRecvBuf);
    if (pcJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Cmd);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if (0 != strcmp(pcNode->valuestring, pcStr_Msg_Cmd_Swarm))
                {
                    continue;
                }
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Type);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if ( 0 != strcmp(pcNode->valuestring, pcStr_Msg_Type_SwarmAllocID))
                {
                    break;
                }
                strncpy(acType, pcNode->valuestring, sizeof(acType));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_SEQ);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piSeq = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_RobotList);
        iCount = cJSON_GetArraySize(pcNode);
        *piCount = iCount;
        for(i=0; i<iCount; i++)
        {
            pSubArray = cJSON_GetArrayItem(pcNode, i);
            pSubNode = cJSON_GetObjectItem(pSubArray, pcStr_Msg_Param_Swarm_ID);
            if (pSubNode != NULL)
            {
                if (pSubNode->type == cJSON_Number)
                {
                    piID[i] = pSubNode->valueint;
                }
            }

            pSubNode = cJSON_GetObjectItem(pSubArray, pcStr_Msg_Param_Swarm_MAC);
            if (pSubNode != NULL)
            {
                if (pSubNode->type == cJSON_String)
                {
                    strncpy((char *)(&(pcMacAddr[i])), pSubNode->valuestring, sizeof(ROBOTAGENT_MAC_T));
                }
            }
        }

        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pcJson);
    return ubtRet;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmAllocIDAck(char *pcAccount, int iPort, int iSeq, char *pcMacAddr,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    pcJsonRoot = cJSON_CreateObject();
    if ((pcJsonRoot == NULL) || (NULL == pcAccount) || (NULL == pcSendBuf))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmAllocIDAck);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_MAC, pcMacAddr);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}


UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmAllocIDAck(char *pcRecvBuf)
{
    return UBTEDU_RC_FAILED;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionStart(char *pcAccount, int iPort,
        char *pcSendBuf, int iBufLen)
{
    return UBTEDU_RC_FAILED;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionStart(char *pcRecvBuf,
        int *piSeq,
        int *piActionRepeatTime,
        double *pdDelayTime,
        char *pcActionName,
        int iActionNameLen)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pcJson = NULL;
    cJSON *pcNode = NULL;
    char acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == pdDelayTime) || (NULL == piSeq) ||
        (NULL == pcActionName))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acType[0] = '\0';

    pcJson   = cJSON_Parse(pcRecvBuf);
    if (pcJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Cmd);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if (0 != strcmp(pcNode->valuestring, pcStr_Msg_Cmd_Swarm))
                {
                    break;
                }
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Type);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if ( 0 != strcmp(pcNode->valuestring, pcStr_Msg_Type_SwarmActionStart))
                {
                    break;
                }
                strncpy(acType, pcNode->valuestring, sizeof(acType));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_SEQ);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piSeq = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Repeat);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piActionRepeatTime = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_DelayTime);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *pdDelayTime = pcNode->valuedouble;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_ActionName);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                strncpy(pcActionName, pcNode->valuestring, iActionNameLen);
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pcJson);
    return ubtRet;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionStartAck(char *pcAccount, int iPort, int iSeq,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    pcJsonRoot = cJSON_CreateObject();
    if ((pcJsonRoot == NULL) || (NULL == pcAccount) || (NULL == pcSendBuf))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmActionStartAck);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionStartAck(char *pcRecvBuf, int *piSeq)
{
    return UBTEDU_RC_FAILED;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionEnd(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    pcJsonRoot = cJSON_CreateObject();
    if ((pcJsonRoot == NULL) || (NULL == pcAccount) || (NULL == pcSendBuf))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmActionEnd);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_ID, iId);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}


UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionEnd(char *pcRecvBuf,
        int *piSeq, int *piId)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pcJson = NULL;
    cJSON *pcNode = NULL;
    char acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == piSeq))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acType[0] = '\0';

    pcJson   = cJSON_Parse(pcRecvBuf);
    if (pcJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Cmd);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if (0 != strcmp(pcNode->valuestring, pcStr_Msg_Cmd_Swarm))
                {
                    break;
                }
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Type);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if ( 0 != strcmp(pcNode->valuestring, pcStr_Msg_Type_SwarmActionEnd))
                {
                    break;
                }
                strncpy(acType, pcNode->valuestring, sizeof(acType));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_SEQ);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piSeq = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_ID);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piId = pcNode->valueint;
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pcJson);
    return ubtRet;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionEndAck(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    pcJsonRoot = cJSON_CreateObject();
    if ((pcJsonRoot == NULL) || (NULL == pcAccount) || (NULL == pcSendBuf))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmActionEndAck);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_ID, iId);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionEndAck(char *pcRecvBuf, int *piSeq, int *piId)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pcJson = NULL;
    cJSON *pcNode = NULL;
    char acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == piSeq))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acType[0] = '\0';

    pcJson   = cJSON_Parse(pcRecvBuf);
    if (pcJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Cmd);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if (0 != strcmp(pcNode->valuestring, pcStr_Msg_Cmd_Swarm))
                {
                    break;
                }
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Type);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if ( 0 != strcmp(pcNode->valuestring, pcStr_Msg_Type_SwarmActionEnd))
                {
                    break;
                }
                strncpy(acType, pcNode->valuestring, sizeof(acType));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_SEQ);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piSeq = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_ID);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piId = pcNode->valueint;
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pcJson);
    return ubtRet;
}




UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionCancel(char *pcAccount, int iPort,
        char *pcSendBuf, int iBufLen)
{
    return UBTEDU_RC_FAILED;
}


UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionCancel(char *pcRecvBuf, int *piSeq)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pcJson = NULL;
    cJSON *pcNode = NULL;
    char acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == piSeq))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acType[0] = '\0';

    pcJson   = cJSON_Parse(pcRecvBuf);
    if (pcJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Cmd);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if (0 != strcmp(pcNode->valuestring, pcStr_Msg_Cmd_Swarm))
                {
                    break;
                }
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Type);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if ( 0 != strcmp(pcNode->valuestring, pcStr_Msg_Type_SwarmActionCancel))
                {
                    break;
                }
                strncpy(acType, pcNode->valuestring, sizeof(acType));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_SEQ);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piSeq = pcNode->valueint;
            }
        }

        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pcJson);
    return ubtRet;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionCancelAck(char *pcAccount, int iPort, int iSeq,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    pcJsonRoot = cJSON_CreateObject();
    if ((pcJsonRoot == NULL) || (NULL == pcAccount) || (NULL == pcSendBuf))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmActionCancelAck);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionCancelAck(char *pcRecvBuf, int *piSeq)
{
    return UBTEDU_RC_FAILED;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmGotoXYZ(char *pcAccount, int iPort, int iSeq, int iId,
        ROBOTAGENT_SC_COORDINATES_T *pstXYZ, char *pcSendBuf, int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    pcJsonRoot = cJSON_CreateObject();
    if ((pcJsonRoot == NULL) || (pstXYZ == NULL) || (NULL == pcSendBuf) || (NULL == pcAccount))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmActionGotoXYZ);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_ID, iId);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_X, pstXYZ->iX);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_Y, pstXYZ->iY);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_Z, pstXYZ->iZ);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_Z, pstXYZ->iZ);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_Order, pstXYZ->iOrder);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmGotoXYZ(char *pcRecvBuf, int *piId,
        ROBOTAGENT_SC_COORDINATES_T *pstXYZ, int *piSeq)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pcJson = NULL;
    cJSON *pcNode = NULL;
    char acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == pstXYZ) || (NULL == piSeq) ||
        (NULL == piId))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acType[0] = '\0';

    pcJson   = cJSON_Parse(pcRecvBuf);
    if (pcJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Cmd);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if (0 != strcmp(pcNode->valuestring, pcStr_Msg_Cmd_Swarm))
                {
                    break;
                }
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Type);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if ( 0 != strcmp(pcNode->valuestring, pcStr_Msg_Type_SwarmActionGotoXYZ))
                {
                    break;
                }
                strncpy(acType, pcNode->valuestring, sizeof(acType));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_SEQ);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piSeq = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_ID);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piId = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_X);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                pstXYZ->iX = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_Y);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                pstXYZ->iY = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_Z);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                pstXYZ->iZ = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_Order);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                pstXYZ->iOrder = pcNode->valueint;
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pcJson);
    return ubtRet;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionGotoXYZAck(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    pcJsonRoot = cJSON_CreateObject();
    if ((pcJsonRoot == NULL) || (NULL == pcSendBuf) || (NULL == pcAccount))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_ID, iId);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmActionGotoXYZAck);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}


UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionGotoXYZAck(char *pcRecvBuf,
        ROBOTAGENT_SC_COORDINATES_T *pstXYZ, int *piSeq, int *piId)
{
    return UBTEDU_RC_FAILED;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmReachXYZ(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    pcJsonRoot = cJSON_CreateObject();
    if ((pcJsonRoot == NULL) || (NULL == pcSendBuf) || (NULL == pcAccount))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmActionReachXYZ);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_ID, iId);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmReachXYZ(char *pcRecvBuf,
        int *piSeq, int *piId)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pcJson = NULL;
    cJSON *pcNode = NULL;
    char acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == piSeq) || (NULL == piId))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acType[0] = '\0';

    pcJson   = cJSON_Parse(pcRecvBuf);
    if (pcJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Cmd);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if (0 != strcmp(pcNode->valuestring, pcStr_Msg_Cmd_Swarm))
                {
                    break;
                }
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Type);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if ( 0 != strcmp(pcNode->valuestring, pcStr_Msg_Type_SwarmActionReachXYZ))
                {
                    break;
                }
                strncpy(acType, pcNode->valuestring, sizeof(acType));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_SEQ);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piSeq = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_ID);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piId = pcNode->valueint;
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pcJson);
    return ubtRet;
}

UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmReachXYZAck(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    pcJsonRoot = cJSON_CreateObject();
    if ((pcJsonRoot == NULL) || (NULL == pcSendBuf) || (NULL == pcAccount))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmActionReachXYZAck);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_ID, iId);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmReachXYZAck(char *pcRecvBuf,
        int *piSeq, int *piId)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pcJson = NULL;
    cJSON *pcNode = NULL;
    char acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == piSeq))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acType[0] = '\0';

    pcJson   = cJSON_Parse(pcRecvBuf);
    if (pcJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Cmd);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if (0 != strcmp(pcNode->valuestring, pcStr_Msg_Cmd_Swarm))
                {
                    break;
                }
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Type);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if ( 0 != strcmp(pcNode->valuestring, pcStr_Msg_Type_SwarmActionReachXYZAck))
                {
                    break;
                }
                strncpy(acType, pcNode->valuestring, sizeof(acType));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_SEQ);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piSeq = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_ID);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piId = pcNode->valueint;
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pcJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmQueryXYZ(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    pcJsonRoot = cJSON_CreateObject();
    if ((pcJsonRoot == NULL) || (NULL == pcSendBuf))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmActionQueryXYZ);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_ID, iId);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Port, iPort);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmQueryXYZ(char *pcRecvBuf,
        int *piSeq, int *piId)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pcJson = NULL;
    cJSON *pcNode = NULL;
    char acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == piSeq) || (NULL == piId))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acType[0] = '\0';

    pcJson   = cJSON_Parse(pcRecvBuf);
    if (pcJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Cmd);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if (0 != strcmp(pcNode->valuestring, pcStr_Msg_Cmd_Swarm))
                {
                    break;
                }
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Type);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if ( 0 != strcmp(pcNode->valuestring, pcStr_Msg_Type_SwarmActionQueryXYZ))
                {
                    break;
                }
                strncpy(acType, pcNode->valuestring, sizeof(acType));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_SEQ);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piSeq = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_ID);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piId = pcNode->valueint;
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pcJson);
    return ubtRet;
}


UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmQueryXYZAck(char *pcAccount, int iPort, int iSeq, int iId,
        ROBOTAGENT_SC_COORDINATES_T *pstXYZ, char *pcStatus,
        char *pcSendBuf, int iBufLen)
{
    cJSON   *pcJsonRoot = NULL;

    pcJsonRoot = cJSON_CreateObject();
    if ((pcJsonRoot == NULL) || (NULL == pstXYZ) || (NULL == pcSendBuf))
    {
        printf("Failed to create json message!\r\n");
        return UBTEDU_RC_NORESOURCE;
    }

    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Swarm);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Account, pcAccount);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Msg_Type, pcStr_Msg_Type_SwarmActionQueryXYZAck);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_ID, iId);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_X, pstXYZ->iX);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_Y, pstXYZ->iY);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_Z, pstXYZ->iZ);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_Z, pstXYZ->iZ);
    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_Order, pstXYZ->iOrder);
    cJSON_AddStringToObject(pcJsonRoot, pcStr_Ret_Msg_Status, pcStatus);

    cJSON_AddNumberToObject(pcJsonRoot, pcStr_Msg_Param_Swarm_SEQ, iSeq);

    strncpy(pcSendBuf, cJSON_Print(pcJsonRoot), iBufLen);
    cJSON_Delete(pcJsonRoot);

    return UBTEDU_RC_SUCCESS;
}

UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmQueryXYZAck(char *pcRecvBuf,
        int *piSeq, int *piId, ROBOTAGENT_SC_COORDINATES_T *pstXYZ, char *pcStatus)
{
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    cJSON *pcJson = NULL;
    cJSON *pcNode = NULL;
    char acType[MSG_CMD_STR_MAX_LEN];
    UBTEDU_RC_T ret = UBTEDU_RC_FAILED;

    /* Check parameters */
    if ((NULL == pcRecvBuf) || (NULL == piSeq) || (NULL == piId) ||
        (NULL == pstXYZ) || (NULL == pcStatus))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acType[0] = '\0';

    pcJson   = cJSON_Parse(pcRecvBuf);
    if (pcJson == NULL)
    {
        printf("Parse json message filed!\r\n");
        ret = UBTEDU_RC_SOCKET_DECODE_FAILED;
        return ret;
    }

    do
    {
        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Cmd);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if (0 != strcmp(pcNode->valuestring, pcStr_Msg_Cmd_Swarm))
                {
                    break;
                }
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Type);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                if ( 0 != strcmp(pcNode->valuestring, pcStr_Msg_Type_SwarmActionReachXYZAck))
                {
                    break;
                }
                strncpy(acType, pcNode->valuestring, sizeof(acType));
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_SEQ);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piSeq = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_ID);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                *piId = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_X);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                pstXYZ->iX = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_Y);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                pstXYZ->iY = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_Z);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                pstXYZ->iZ = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Msg_Param_Swarm_Order);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_Number)
            {
                pstXYZ->iOrder = pcNode->valueint;
            }
        }

        pcNode   = cJSON_GetObjectItem(pcJson, pcStr_Ret_Msg_Status);
        if (pcNode != NULL)
        {
            if (pcNode->type == cJSON_String)
            {
                strncpy(pcStatus, pcNode->valuestring, (MSG_CMD_STR_MAX_LEN*sizeof(char)));
            }
        }
        ubtRet = UBTEDU_RC_SUCCESS;
    }
    while (0);
    cJSON_Delete(pcJson);
    return ubtRet;
}


