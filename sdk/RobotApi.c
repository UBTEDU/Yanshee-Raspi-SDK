/**
 * @file                RobotApi.c
 * @brief           UBTEDU Alpha 1x SDK
 * @author          Cygnus Yang
 * @date            Wednesday, August 16, 2017
 * @version         Initial Draft
 * @par             Copyright (C),  2017-2023, UBT Education
 * @par History:
 * 1.Date:          Wednesday, August 16, 2017
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

#include "cJSON.h"
#include "robotlogstr.h"
#include "list.h"

#include "RobotApi.h"
#include "RobotMsg.h"


#define MOTION_TYPE_TURNAROUND          "turn around"
#define MOTION_TYPE_HEAD_STR            "head"
#define MOTION_DIRECTION_LEFT_STR       "left"
#define MOTION_DIRECTION_RIGHT_STR      "right"
#define MOTION_DIRECTION_BOTHHAND_STR   "both"
#define MOTION_DIRECTION_FRONT_STR      "front"
#define MOTION_DIRECTION_BACK_STR       "back"


/* define all servo index */
#if 1
#define SERVO_LEFT_SHOULD_INDEX     (1<<0)
#define SERVO_LEFT_ELBOW_INDEX      (1<<1)
#define SERVO_LEFT_WRIST_INDEX      (1<<2)
#define SERVO_RIGHT_SHOULD_INDEX    (1<<3)
#define SERVO_RIGHT_ELBOW_INDEX     (1<<4)
#define SERVO_RIGHT_WRIST_INDEX     (1<<5)
#define SERVO_LEFT_WAIST_INDEX      (1<<6)
#define SERVO_LEFT_HIPBONE_INDEX    (1<<7)
#define SERVO_LEFT_KNEE_INDEX       (1<<8)
#define SERVO_LEFT_ANKLE_INDEX      (1<<9)
#define SERVO_LEFT_FOOT_INDEX       (1<<10)
#define SERVO_RIGHT_WAIST_INDEX     (1<<11)
#define SERVO_RIGHT_HIPBONE_INDEX   (1<<12)
#define SERVO_RIGHT_KNEE_INDEX      (1<<13)
#define SERVO_RIGHT_ANKLE_INDEX     (1<<14)
#define SERVO_RIGHT_FOOT_INDEX      (1<<15)
#define SERVO_HEAD_INDEX            (1<<16)
#define SERVO_RANG_INDEX(min, max)   ((0x1ffff<<(min-1))&(0x1ffff>>(17-(max))))
#define SERVO_MAX_LOW_SPEED 60 //=> 1.2S

#define SERVO_DEFAULT_ANGLE        (0)

#else
#define SERVO_LEFT_SHOULD_INDEX     (1<<12)
#define SERVO_LEFT_ELBOW_INDEX      (1<<3)
#define SERVO_LEFT_WRIST_INDEX      (1<<4)
#define SERVO_RIGHT_SHOULD_INDEX    (1<<6)
#define SERVO_RIGHT_ELBOW_INDEX     (1<<1)
#define SERVO_RIGHT_WRIST_INDEX     (1<<2)
#define SERVO_LEFT_WAIST_INDEX      (1<<13)
#define SERVO_LEFT_HIPBONE_INDEX    (1<<14)
#define SERVO_LEFT_KNEE_INDEX       (1<<15)
#define SERVO_LEFT_ANKLE_INDEX      (1<<16)
#define SERVO_LEFT_FOOT_INDEX       (1<<17)
#define SERVO_RIGHT_WAIST_INDEX     (1<<7)
#define SERVO_RIGHT_HIPBONE_INDEX   (1<<8)
#define SERVO_RIGHT_KNEE_INDEX      (1<<9)
#define SERVO_RIGHT_ANKLE_INDEX     (1<<10)
#define SERVO_RIGHT_FOOT_INDEX      (1<<11)
#define SERVO_HEAD_INDEX            (1<<5)
#endif


/** @brief    Servo's speed
   */
typedef enum
{
    SERVO_MOVE_SPEED_VERY_LOW = 1, /**< Very slow. */
    SERVO_MOVE_SPEED_LOW,          /**< Slow. */
    SERVO_MOVE_SPEED_DEFAULT,      /**< Default */
    SERVO_MOVE_SPEED_FAST,         /**< Fast */
    SERVO_MOVE_SPEED_VERY_FAST,    /**< Very Fast */
    SERVO_MOVE_SPEED_LAST_INVALID  /**< Invalid value */
} SERVO_MOVE_SPEED_e;


/* Socket to robot */
#define SDK_REMOTE_SOCKET_PORT      20001
/* SDK_LOCAL_IP */
#define SDK_LOCAL_IP                "127.0.0.1"
/* Max message length which sent to robot */
#define SDK_MESSAGE_MAX_LEN     (1024)
/* Flags indicate that the timer is used */
#define UBTEDU_ROBOT_TIMER_USED 1
/* Flags indicate that the timer is not used */
#define UBTEDU_ROBOT_TIMER_NOTUSED 0

/* Receive message from robot via this socket */
static int g_iRobot2SDK = -1;
/* Receive message from robot via this port */
static int g_iRobot2SDKPort = -1;
/* Send message to robot via this socket */
static int g_iSDK2Robot = -1;
/* Send message to robot via this port */
static int g_iSDK2RobotPort = -1;
/* Socket address struct for sending message to robot */
static struct sockaddr_in g_stSDK2RobotSockAddr;
/* Connected robot infomation */
UBTEDU_ROBOTINFO_T g_pstConnectedRobotInfo;

#define __DEBUG_PRINT__ 

#ifdef __DEBUG_PRINT__                                            // 对于DEBUG版本，增加打印信息
#define DebugTrace(...)\
        do{\
            fprintf(stderr,__VA_ARGS__);\
        }while(0)
#else
#define DebugTrace( ... )                // 对于RELEASE版本，把__DEBUG_PRINT__宏关闭 
#endif


static int _udpServerInit(int *piPort)
{
    int fd, ret, i;
    struct sockaddr_in addr;
    int addr_len = sizeof(struct sockaddr_in);
    struct timeval tsock = {3, 0};


    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        printf("create socket failed!\r\n");
        return -1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tsock, sizeof(tsock)) < 0)
    {
        printf("set SO_RCVTIMEO setsockopt failed!\r\n");
    }

    memset(&addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);

    for (i = 9100; i < 10000; i++)
    {
        addr.sin_port = htons(i);
        ret     = bind(fd, (struct sockaddr *)&addr, addr_len);
        if (ret == 0)
        {
            break;
        }
        else if (EADDRINUSE == errno)
        {
            printf(" udp iPort:%d in use!\r\n", i);
        }
        else
        {
            printf("udp server bind failed!\r\n");
        }
    }

    if (i >= 10000)
    {
        close(fd);
        return -1;
    }

    *piPort = i;
    return fd;
}

/**
 * @brief:      _ubtMsgSend2Robot
 * @details:    Send UDP message out
 * @param[in]   int iFd         Socket ID
 * @param[in]   char *pcIpAddr  IP address
 * @param[in]   int iPort       socket port
 * @param[in]   char *pcBuf     Send buffer
 * @param[in]   int iLen        Buffer length
 * @param[out]  None
 * @retval:     int
 */
static int _ubtMsgSend2Robot(int iFd, char *pcIpAddr, int iPort, char *pcBuf, int iLen)
{
    int iRet = 0;
    struct sockaddr_in stAddr;

    stAddr.sin_family = AF_INET;
    stAddr.sin_port = htons(SDK_REMOTE_SOCKET_PORT);
    stAddr.sin_addr.s_addr = inet_addr(pcIpAddr);

    DebugTrace("Send message to %s, Port %d, Buffer %s, iLen %d, Socket ID %d\n", pcIpAddr, iPort, pcBuf, iLen, iFd);
    iRet = sendto(iFd, pcBuf, iLen, 0, (struct sockaddr *)&stAddr, sizeof(struct sockaddr));
    if (-1 == iRet)
    {
        printf("Send message error. %s\r\n", strerror(errno));
    }
    return iRet;
}

static int _ubtMsgRecvFromRobot(int iFd, char *pcRecvBuf, int iBufLen)
{
    int iRet = 0;
    struct sockaddr_in stAddr;
    int iAddrLen = sizeof(struct sockaddr_in);


    memset(&stAddr, 0, sizeof(stAddr));
    do
    {
        memset(pcRecvBuf, 0, iBufLen);
        iRet     = recvfrom(iFd, pcRecvBuf, iBufLen, 0, (struct sockaddr *)&stAddr,
                            (socklen_t *)&iAddrLen);
        DebugTrace("Received from %s, Buffer %s, Length %d, Socket ID %d\n",
                   inet_ntoa(stAddr.sin_addr), pcRecvBuf, iBufLen, iFd);
        if (iRet <= 0)
        {
            if (errno == EINTR)   //(errno == EAGAIN))
            {
                continue;
            }
            /* Socket receiving data timeout */
            DebugTrace("_ubtMsgRecvFromRobot err:%s\n",strerror(errno));
        }
        break;
    }
    while (1);

    return iRet;
}




/**
 * @brief:      _ubtTimerTimeout
 * @details:    When the heart beat timer timeout, send the hart beat message
             to robot
 * @param[in]   None
 * @param[out]  None
 * @retval:     void
 */
static void * _ubtTimerTimeout()
{
    cJSON *pJsonRoot = NULL;
    char *pcSendBuf = NULL;


    pJsonRoot = cJSON_CreateObject();
    if (pJsonRoot == NULL)
    {
        printf("Failed to create json message!\r\n");
        return NULL ;
    }

    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Cmd, pcStr_Msg_Cmd_Heartbeat);
    cJSON_AddStringToObject(pJsonRoot, pcStr_Msg_Account, "sdk");

    pcSendBuf = cJSON_Print(pJsonRoot);

    while ( 1 )
    {
        if(!strcmp(g_pstConnectedRobotInfo.acIPAddr, SDK_LOCAL_IP))
        {
            DebugTrace("local ip no heartbeat\n");
            break;
        }

        sleep(5);
        _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                          g_iSDK2RobotPort, pcSendBuf, strlen(pcSendBuf));
    }

    cJSON_Delete(pJsonRoot);
    free(pcSendBuf);

    return NULL ;
    /* TODO: Should receive heart beat message from robot */
    /* Start the heart beat timer every 5 seconds */
}


static UBTEDU_RC_T _ubtGetLocalBcastIP(char *pcBcastIP)
{
    int iFd;
    struct ifreq ifr;


    iFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (iFd == -1)
    {
        printf("create socket failed!");
        return UBTEDU_RC_SOCKET_NORESOURCE;
    }

    strcpy(ifr.ifr_name, "wlan0");
    if (ioctl(iFd, SIOCGIFBRDADDR, &ifr) < 0)    //get ip addr
    {
        printf("ip ioctl error!");
        return UBTEDU_RC_SOCKET_FAILED;
    }

    strcpy(pcBcastIP, inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

    close(iFd);
    return UBTEDU_RC_SUCCESS;
}

/**
 * @brief:      ubtGetSWVersion
 * @details:    Get the robot versions including embedded system, raspberry,
             SDK and servos
 * @param[in]   UBTEDU_ROBOT_SOFTVERSION_TYPE_e eType
 *                                      Please see the defination UBTEDU_ROBOT_SOFTVERSION_TYPE_e
 * @param[out]  char *pcVersion
 *                                      The output buffer for versions.
 *                                      In most cases, the version length is no more than 20 bytes.
 * @param[in]   int iVersionLen
 *                                      The max output buffer for versions length.
 * @param[out]  None
 * @retval:     UBTEDU_RC_T
 */
UBTEDU_RC_T ubtGetSWVersion(UBTEDU_ROBOT_SOFTVERSION_TYPE_e eType, char *pcVersion, int iVersionLen)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        *pcParam = NULL;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];
    char acParam[16];


    acSocketBuffer[0] =- '\0';

    if (eType == UBTEDU_ROBOT_SOFTVERSION_TYPE_SDK)   //sdk version
    {
        strncpy(pcVersion, UBTEDU_SDK_SW_VER, iVersionLen);
        return UBTEDU_RC_SUCCESS;
    }
    switch ( eType )
    {
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_STM32: //stm32
            pcParam = pcStr_Msg_Param_Query_Version_STM32;
            break;

        case UBTEDU_ROBOT_SOFTVERSION_TYPE_RASPI: //raspberry
            pcParam = pcStr_Msg_Param_Query_Version_RaspPi;
            break;

        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS1: //servo
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS2:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS3:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS4:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS5:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS6:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS7:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS8:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS9:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS10:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS11:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS12:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS13:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS14:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS15:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS16:
        case UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS17:
            sprintf(acParam,"%s%d", pcStr_Msg_Param_Query_Version_Servos,eType-UBTEDU_ROBOT_SOFTVERSION_TYPE_STM32);
            pcParam = acParam;
            break;
        default:
            return UBTEDU_RC_WRONG_PARAM;
    }


    ubtRet = ubtRobot_Msg_Encode_SWVersion(pcParam,
                                           g_iRobot2SDKPort,
                                           acSocketBuffer,
                                           sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));

    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }


    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }


    ubtRet = ubtRobot_Msg_Decode_SWVersion(acSocketBuffer, pcVersion, iVersionLen);

    return ubtRet;
}

/**
 * @brief:      ubtGetRobotStatus
 * @details:    Get the status of the robot
 * @param[in]   UBTEDU_ROBOT_STATUS_TYPE_e eType  Please see the defination for UBTEDU_ROBOT_STATUS_TYPE_e
 * @param[in]   int *piStatus
 *                      0 Sound status (Status:1 Mute,0 Turn on)
 *                      1 Playing music status (Status:1 Playing; 0 Pause)
 *                      2 Volume status (Status:0~ff)
 *                      3 Power status (Status:Voltage(mv),
 *                                              Recharge(0 No,1 Yes,2 No battery),
 *                                              Percent(Range 0~64)
 *                      4 Low voltage alert (Status:1 Low voltage 0 OK)
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtGetRobotStatus(UBTEDU_ROBOT_STATUS_TYPE_e eType, void *pStatus)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        *pcType = NULL;
    char        *pcParam = NULL;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];



    /* Check the parameters */

    if ((eType >= UBTEDU_ROBOT_STATUS_TYPE_INVALID) || (NULL == pStatus))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    acSocketBuffer[0] = '\0';

    switch ( eType )
    {
        case UBTEDU_ROBOT_STATUS_TYPE_PLAYACTION: //play
            pcType = pcStr_Msg_Type_Play;
            break;

        case UBTEDU_ROBOT_STATUS_TYPE_VOLUME: //volume
            pcType = pcStr_Msg_Type_Volume;
            break;

        case UBTEDU_ROBOT_STATUS_TYPE_POWER_VOLTAGE: //power
            pcType = pcStr_Msg_Type_Battery;
            pcParam = pcStr_Msg_Param_Query_Power_Voltage;
            break;

        case UBTEDU_ROBOT_STATUS_TYPE_POWER_RECHARGE: //power
            pcType = pcStr_Msg_Type_Battery;
            pcParam = pcStr_Msg_Param_Query_Power_Charging;
            break;

        case UBTEDU_ROBOT_STATUS_TYPE_POWER_PERCENT: //power
            pcType = pcStr_Msg_Type_Battery;
            pcParam = pcStr_Msg_Param_Query_Power_Percent;
            break;

        case UBTEDU_ROBOT_STATUS_TYPE_POWER_LOWALERT: //power
            /* Not supported yet */
            pcType = pcStr_Msg_Type_Battery;
            pcParam = NULL;
            break;

        default:
            printf("GetRobotStatus eType:%d not surport!\r\n", eType);
            return UBTEDU_RC_WRONG_PARAM;
    }
    ubtRet = ubtRobot_Msg_Encode_RobotStatus(pcStr_Msg_Cmd_Query,
             pcType,
             pcParam,
             g_iRobot2SDKPort,
             acSocketBuffer,
             sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }


    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }


    ubtRet = ubtRobot_Msg_Decode_RobotStatus(pcType, acSocketBuffer, pStatus);

    return ubtRet;
}


/**
 * @brief:      ubtCheckAPPStatus
 * @details:    Get the status of the mobile phone
 * @param[in]   char *pcBuf    static, slant_forward, slant_backward, slant_left, slant_right
 *                              forward_and_back, swaying
 * @param[in]   int iWaitTime  The mix value is 10s, the max value is 600s
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtCheckAPPStatus(char *pcBuf, int iWaitTime)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    struct timeval tsock = {30, 0};
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    if((iWaitTime >= 10)&&(iWaitTime <= 600))
    {
        tsock.tv_sec = iWaitTime;
    }

    acSocketBuffer[0] = '\0';
    ubtRet = ubtRobot_Msg_Encode_CheckAPPStatus(pcStr_Msg_Cmd_Query_App, pcStr_Ret_Msg_Status,
             g_iRobot2SDKPort, tsock,
             pcBuf, acSocketBuffer, sizeof(acSocketBuffer));

    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }


    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    if (setsockopt(g_iRobot2SDK, SOL_SOCKET, SO_RCVTIMEO, &tsock, sizeof(tsock)) < 0)
    {
        printf("set SO_RCVTIMEO setsockopt failed!\r\n");
    }
    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }
    /* Change the timeout value to 3s as default */
    tsock.tv_sec = 3;
    if (setsockopt(g_iRobot2SDK, SOL_SOCKET, SO_RCVTIMEO, &tsock, sizeof(tsock)) < 0)
    {
        printf("set SO_RCVTIMEO setsockopt failed!\r\n");
    }

    ubtRet = ubtRobot_Msg_Decode_CheckAPPStatus(acSocketBuffer);
    return ubtRet;
}

/**
 * @brief:      ubtDetectVoiceMsg
 * @details:    Detect a text message from robot voice recognition
 * @param[in]   char *pcBuf   The message to be detected
 * @param[in]   int iTimeout  The max time for detecting
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtDetectVoiceMsg(char *pcBuf, int iTimeout)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    struct timeval tsock = {30, 0};
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    /* Check parameters */
    if (NULL == pcBuf)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    if((iTimeout >= 10)&&(iTimeout <= 600))
    {
        tsock.tv_sec = iTimeout;
    }
    acSocketBuffer[0] = '\0';

    ubtRet = ubtRobot_Msg_Encode_DetectVoiceMsg(g_iRobot2SDKPort, pcBuf,
             acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }


    if (setsockopt(g_iRobot2SDK, SOL_SOCKET, SO_RCVTIMEO, &tsock, sizeof(tsock)) < 0)
    {
        printf("set SO_RCVTIMEO setsockopt failed!\r\n");
    }
    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }
    /* Change the timeout value to 3s as default */
    tsock.tv_sec = 3;
    if (setsockopt(g_iRobot2SDK, SOL_SOCKET, SO_RCVTIMEO, &tsock, sizeof(tsock)) < 0)
    {
        printf("set SO_RCVTIMEO setsockopt failed!\r\n");
    }

    ubtRet = ubtRobot_Msg_Decode_DetectVoiceMsg(acSocketBuffer);
    return ubtRet;
}

/**
 * @brief:      _ubtTranslat
 * @details:    just for Translat hex to dec number 
            
 * @param[in]   char c 
 * @param[out]  int 
 * @retval:     char
 */
static int _ubtTranslat(char c)
{
   if(c<='9'&&c>='0') return c-'0';
   if(c>='a' && c<='f') return c-87;
   if(c>='A' && c<='F') return c-55;
   return -1;
}

/**
 * @brief:      _ubt_Htoi
 * @details:    just for Translat hex to dec number 
 * @param[in]   char *str
 * @param[out]  int 
 * @retval:     n
 */
static int _ubt_Htoi(char *str)
{
  int length=strlen(str);
  if(length==0) return 0;
  int i,n=0,stat;
  for(i=0;i<length;i++) 
  {
   stat=_ubtTranslat(str[i]);
   if(stat>=0) n=n*16+stat;
  }
  return n;
}

/**
 * @brief:      ubtGetRobotServo
 * @details:    Read one/multiple/all servo's angle
 * @param[in]   int iIndexMask  bit0 - 16 Servo's index. 1 Read 0 ignore
 * @param[in]   char *pcAngle
 * @param[in]   int iAngleLen   [0 - m] The servos' angle.
 *                  bit 0 indicates the first servo's angle.
 *                  FF means the invalid value.
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtGetRobotServo(UBTEDU_ROBOTSERVO_T *servoAngle)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];
    int iIndexMask = 0x1FFFF; 
    char    ucAngle[MAX_SERVO_NUM*2];
    char *pcAngle = ucAngle;
    int i;
    char	exAngle[2];
    int iAngleLen = 0;

    memset(exAngle, 'F', sizeof(exAngle));              // null is "FF"
    exAngle[2] = '\0';
    
    acSocketBuffer[0] = '\0';

    ubtRet = ubtRobot_Msg_Encode_ReadRobotServo(g_iRobot2SDKPort, acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }


    ubtRet = ubtRobot_Msg_Decode_ReadRobotServo(acSocketBuffer, iIndexMask, pcAngle ,iAngleLen);

   for (i = 0; i < MAX_SERVO_NUM; i++)
    {
        if ((iIndexMask >> i) & 0x01)
        {       
		exAngle[0]=*pcAngle;
		exAngle[1]=*(pcAngle + 1);
		pcAngle += 2;
		if(i == 0){
			servoAngle->SERVO1_ANGLE =_ubt_Htoi(exAngle);
		}
		if(i == 1){
			servoAngle->SERVO2_ANGLE =_ubt_Htoi(exAngle);
		}
		if(i == 2){
			servoAngle->SERVO3_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 3){
			servoAngle->SERVO4_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 4){
			servoAngle->SERVO5_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 5){
			servoAngle->SERVO6_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 6){
			servoAngle->SERVO7_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 7){
			servoAngle->SERVO8_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 8){
			servoAngle->SERVO9_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 9){
			servoAngle->SERVO10_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 10){
			servoAngle->SERVO11_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 11){
			servoAngle->SERVO12_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 12){
			servoAngle->SERVO13_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 13){
			servoAngle->SERVO14_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 14){
			servoAngle->SERVO15_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 15){
			servoAngle->SERVO16_ANGLE=_ubt_Htoi(exAngle);
		}
		if(i == 16){
			servoAngle->SERVO17_ANGLE=_ubt_Htoi(exAngle);
		}
			
        }
    }

    return ubtRet;
}

/**
 * @brief:      ubtSetRobotServo
 * @details:    Set the servo's acAngle with speed
 * @param[in]   int iIndexMask  bit0 - 16 Servo's index.
 * @param[in]   char *pcAngle   The angle for the servos
 * @param[in]   int iTime       It is the time for servo, the value is smaller, the speed is faster.
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtSetRobotServo(UBTEDU_ROBOTSERVO_T *servoAngle, int iTime)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];
    int iIndexMask = 0;
    char    exAngle[MAX_SERVO_NUM];
    char    ucAngle[MAX_SERVO_NUM*2];
    char *pcAngle = ucAngle;
       
    memset(exAngle, 'F', sizeof(exAngle));              // null is "FF"
    exAngle[MAX_SERVO_NUM] = '\0';

    memset(ucAngle, 'F', sizeof(ucAngle));              // null is "FF"
    ucAngle[MAX_SERVO_NUM*2] = '\0';


    if(servoAngle->SERVO1_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<0);
	sprintf(exAngle,"%x",servoAngle->SERVO1_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
    }

    if(servoAngle->SERVO2_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<1);
	sprintf(exAngle,"%x",servoAngle->SERVO2_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
    }

    if(servoAngle->SERVO3_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<2);
	sprintf(exAngle,"%x",servoAngle->SERVO3_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
     }

    if(servoAngle->SERVO4_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<3);
	sprintf(exAngle,"%x",servoAngle->SERVO4_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
     }

    if(servoAngle->SERVO5_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<4);
	sprintf(exAngle,"%x",servoAngle->SERVO5_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;

     }

    if(servoAngle->SERVO6_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<5);
	sprintf(exAngle,"%x",servoAngle->SERVO6_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
     }

    if(servoAngle->SERVO7_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<6);
	sprintf(exAngle,"%x",servoAngle->SERVO7_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;

     }

    if(servoAngle->SERVO3_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<2);
	sprintf(exAngle,"%x",servoAngle->SERVO3_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
     }

    if(servoAngle->SERVO8_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<7);
	sprintf(exAngle,"%x",servoAngle->SERVO8_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
     }

    if(servoAngle->SERVO9_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<8);
	sprintf(exAngle,"%x",servoAngle->SERVO9_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;

     }

    if(servoAngle->SERVO10_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<9);
	sprintf(exAngle,"%x",servoAngle->SERVO10_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
     }

    if(servoAngle->SERVO11_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<10);
	sprintf(exAngle,"%x",servoAngle->SERVO11_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
     }

    if(servoAngle->SERVO12_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<11);
	sprintf(exAngle,"%x",servoAngle->SERVO12_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;

     }

    if(servoAngle->SERVO13_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<12);
	sprintf(exAngle,"%x",servoAngle->SERVO13_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
     }

    if(servoAngle->SERVO14_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<13);
	sprintf(exAngle,"%x",servoAngle->SERVO14_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
     }

    if(servoAngle->SERVO15_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<14);
	sprintf(exAngle,"%x",servoAngle->SERVO15_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
     }

    if(servoAngle->SERVO16_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<15);
	sprintf(exAngle,"%x",servoAngle->SERVO16_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
	pcAngle +=2;
     }

    if(servoAngle->SERVO17_ANGLE!= SERVO_DEFAULT_ANGLE )
    {
	iIndexMask |=(0x01<<16);
	sprintf(exAngle,"%x",servoAngle->SERVO17_ANGLE);
	*pcAngle = exAngle[0];
	*(pcAngle +1) = exAngle[1];		
     }


    if (NULL == pcAngle)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    acSocketBuffer[0] = '\0';

    ubtRet = ubtRobot_Msg_Encode_SetRobotServo(g_iRobot2SDKPort, iIndexMask, ucAngle, iTime,
             acSocketBuffer, sizeof(acSocketBuffer));

    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_SetRobotServo(acSocketBuffer);

    return ubtRet;
}

UBTEDU_RC_T ubtSetRobotServoi(int iIndexMask, char *pcAngle, int iTime)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    if (NULL == pcAngle)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acSocketBuffer[0] = '\0';

    ubtRet = ubtRobot_Msg_Encode_SetRobotServo(g_iRobot2SDKPort, iIndexMask, pcAngle, iTime,
             acSocketBuffer, sizeof(acSocketBuffer));

    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_SetRobotServo(acSocketBuffer);

    return ubtRet;
}



/**
 * @brief:      ubtSetRobotVolume
 * @details:    Set the volume for the Robot
 * @param[in]   int iVolume  [0-100] Volume percent
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtSetRobotVolume(int iVolume)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    acSocketBuffer[0] = '\0';

    ubtRet = ubtRobot_Msg_Encode_SetRobotVolume(g_iRobot2SDKPort, iVolume,
             acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_SetRobotVolume(acSocketBuffer);

    return ubtRet;
}

/**
 * @brief:      ubtSetRobotMotion
 * @details:    Set the robot's action
 * @param[in]   char *pcType    crouch
  *                             raise
  *                             stretch
  *                             come on
  *                             wave
  *                             bend
  *                             walk
  *                             turn around
  *                             bow
 * @param[in]   char *pcDirect  left
  *                             right
  *                             both
  *                             front
  *                             back
 * @param[in]   int iSpeed      1/2/3/4/5  The default value is 3
 * @param[in]   int iRepeat     Repeat times. 0 means infinite
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtSetRobotMotion(char *pcType, char *pcDirect, int iSpeed, int iRepeat)
{
    char acAngle[2] = {0};
    char angle_array[8][MAX_SERVO_NUM*2+1];
    int i, speed;


    if (pcType == NULL)
    {
        printf("SetRobotMotion pcType is null!\r\n");
        return UBTEDU_RC_WRONG_PARAM;
    }

    speed = SERVO_MAX_LOW_SPEED/iSpeed;
    if (!strcmp(pcType, "crouch"))
    {
        strcpy(&angle_array[0][0], "5A3A4A6E595A78684559");
        strcpy(&angle_array[1][0], "5E2E00A5555C86B40E5A");
        strcpy(&angle_array[2][0], "5A3A4A6E595A78684559");

        for(i=0; i< iRepeat; i++)
        {
            ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[0], speed);
            usleep(speed*20*1000);
            ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[1], speed);
            usleep(speed*20*1000);
            ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[2], speed);
            usleep(speed*20*1000);
        }
    }
    else if (!strcmp(pcType, "raise"))
    {
        if (!strcmp(pcDirect, "left"))
        {
            strcpy(&angle_array[0][0], "5b5a5a");
            strcpy(&angle_array[1][0], "5bad5a");
            strcpy(&angle_array[2][0], "5b5a5a");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[2], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "right"))
        {
            strcpy(&angle_array[0][0], "5b5a5a");
            strcpy(&angle_array[1][0], "5b075a");
            strcpy(&angle_array[2][0], "5b5a5a");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[2], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "both"))
        {
            strcpy(&angle_array[0][0], "5b5a5a5a5859");
            strcpy(&angle_array[1][0], "5b075a5aaf5e");
            strcpy(&angle_array[2][0], "5b5a5a5a5859");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[2], speed);
                usleep(speed*20*1000);
            }
        }
        else
            printf("raise direct[%s]  error\r\n", pcDirect);
    }
    else if (!strcmp(pcType, "stretch"))
    {
        if (!strcmp(pcDirect, "left"))
        {
            strcpy(&angle_array[0][0], "59585a");
            strcpy(&angle_array[1][0], "b30059");
            strcpy(&angle_array[2][0], "59585a");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[2], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "right"))
        {
            strcpy(&angle_array[0][0], "5b5a5a");
            strcpy(&angle_array[1][0], "00b45a");
            strcpy(&angle_array[2][0], "5b5a5a");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[2], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "both"))
        {
            strcpy(&angle_array[0][0], "595a5a5a5959");
            strcpy(&angle_array[1][0], "00b45ab30059");
            strcpy(&angle_array[2][0], "595a5a5a5959");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[2], speed);
                usleep(speed*20*1000);
            }
        }
        else
            printf("stretch direct:%s error\r\n", pcDirect);
    }
    else if (!strcmp(pcType, "come on"))
    {
        if (!strcmp(pcDirect, "left"))
        {
            strcpy(&angle_array[0][0], "59585a");
            strcpy(&angle_array[1][0], "ae0016");
            strcpy(&angle_array[2][0], "ae005c");
            strcpy(&angle_array[3][0], "ad000a");
            strcpy(&angle_array[4][0], "ad005e");
            strcpy(&angle_array[5][0], "59585a");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[2], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[3], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[4], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[5], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "right"))
        {
            strcpy(&angle_array[0][0], "585a5a");
            strcpy(&angle_array[1][0], "00b4ae");
            strcpy(&angle_array[2][0], "00b45e");
            strcpy(&angle_array[3][0], "00b4a8");
            strcpy(&angle_array[4][0], "00b45c");
            strcpy(&angle_array[5][0], "585a5a");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[2], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[3], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[4], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[5], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "both"))
        {
            strcpy(&angle_array[0][0], "585a5a59585a");
            strcpy(&angle_array[1][0], "00b4aeae0016");
            strcpy(&angle_array[2][0], "00b45eae005c");
            strcpy(&angle_array[3][0], "00b4a8ad000a");
            strcpy(&angle_array[4][0], "00b45cad005e");
            strcpy(&angle_array[5][0], "585a5a59585a");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[2], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[3], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[4], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[5], speed);
                usleep(speed*20*1000);
            }
        }
        else
            printf("come on direct:%s error\r\n", pcDirect);
    }
    else if (!strcmp(pcType, "wave"))
    {
        if (!strcmp(pcDirect, "left"))
        {
            strcpy(&angle_array[0][0], "595859");
            strcpy(&angle_array[1][0], "591e59");
            strcpy(&angle_array[2][0], "59825a");
            strcpy(&angle_array[3][0], "591d59");
            strcpy(&angle_array[4][0], "59865a");
            strcpy(&angle_array[5][0], "595859");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[2], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[3], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[4], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(4,6), angle_array[5], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "right"))
        {
            strcpy(&angle_array[0][0], "585a5a");
            strcpy(&angle_array[1][0], "58985a");
            strcpy(&angle_array[2][0], "58315a");
            strcpy(&angle_array[3][0], "588f5a");
            strcpy(&angle_array[4][0], "58335a");
            strcpy(&angle_array[5][0], "585a5a");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[2], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[3], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[4], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,3), angle_array[5], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "both"))
        {
            strcpy(&angle_array[0][0], "585a5a595859");
            strcpy(&angle_array[1][0], "58985a591e59");
            strcpy(&angle_array[2][0], "58315a59825a");
            strcpy(&angle_array[3][0], "588f5a591d59");
            strcpy(&angle_array[4][0], "58335a59865a");
            strcpy(&angle_array[5][0], "585a5a595859");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[2], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[3], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[4], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(1,6), angle_array[5], speed);
                usleep(speed*20*1000);
            }
        }
        else
            printf("wave direct %s error\r\n", pcDirect);
    }
    else if (!strcmp(pcType, "bend"))
    {
        if (!strcmp(pcDirect, "left"))
        {
            strcpy(&angle_array[0][0], "597769445a");
            strcpy(&angle_array[1][0], "6788973661");
            strcpy(&angle_array[2][0], "597769445a");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(12,16), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(12,16), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(12,16), angle_array[2], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "right"))
        {
            strcpy(&angle_array[0][0], "5a3a4a6e59 ");
            strcpy(&angle_array[1][0], "5830317252");
            strcpy(&angle_array[2][0], "5a3a4a6e59");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,11), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,11), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,11), angle_array[2], speed);
                usleep(speed*20*1000);
            }
        }
        else
            printf("bend direct %s error\r\n", pcDirect);
    }
    else if (!strcmp(pcType, "walk"))
    {
        if(!strcmp(pcDirect, "front"))
        {
            strcpy(&angle_array[0][0], "602724815565a3904650 ");
            strcpy(&angle_array[1][0], "4932159c664f9a9d3460");
            strcpy(&angle_array[2][0], "49160c8962508c8f3367");
            strcpy(&angle_array[3][0], "4c11207464528c8f3364 ");
            strcpy(&angle_array[4][0], "601d247a51667c8e224a");
            strcpy(&angle_array[5][0], "602724814e699aac264f");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[2], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[3], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[4], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[5], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "back"))
        {
            strcpy(&angle_array[0][0], "602a1d8d50649a973b4d");
            strcpy(&angle_array[1][0], "602a1d8d506683b10f55");
            strcpy(&angle_array[2][0], "612a1d8d5066779a164e");
            strcpy(&angle_array[3][0], "4e1b1d7a67528e962d66");
            strcpy(&angle_array[4][0], "4f230c9861528e962d66");
            strcpy(&angle_array[5][0], "4c371c9967528e962d66");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[1], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[2], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[3], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[4], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[5], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "left"))
        {
            strcpy(&angle_array[0][0], "5a271b8c595c8e9b2855");
            strcpy(&angle_array[1][0], "42271b8e67598e9b2855");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[1], speed);
                usleep(speed*20*1000);
            }
        }
        else if (!strcmp(pcDirect, "right"))
        {
            strcpy(&angle_array[0][0], "5a271d875e5a8b972b5d");
            strcpy(&angle_array[1][0], "5c271d875c708b972b51");
            for(i=0; i< iRepeat; i++)
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[0], speed);
                usleep(speed*20*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[1], speed);
                usleep(speed*20*1000);
            }
        }
        else
        {
            printf("work direct:%s error\r\n", pcDirect);
        }
    }
    else if (!strcmp(pcType, MOTION_TYPE_TURNAROUND))
    {
        if (!strcmp(pcDirect, MOTION_DIRECTION_LEFT_STR))
        {
            strcpy(&angle_array[0][0], "4f10306068636e8e1453");
            strcpy(&angle_array[1][0], "5a281d895e5c8a972a5a");
            for(i=0; i< iRepeat; i++)   // 16 O
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[0], 15);
                usleep(300*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[1], 15);
                usleep(300*1000);
            }
        }
        else if (!strcmp(pcDirect, MOTION_DIRECTION_RIGHT_STR))
        {
            strcpy(&angle_array[0][0], "54532e9f6369a08a554d");
            strcpy(&angle_array[1][0], "5b281d875a5a89972a5a");
            for(i=0; i< iRepeat; i++)   // 16 O
            {
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[0], 15);
                usleep(300*1000);
                ubtSetRobotServoi(SERVO_RANG_INDEX(7,16), angle_array[1], 15);
                usleep(300*1000);
            }
        }
        else
        {
            printf("turn round direct %s error\r\n", pcDirect);
        }
    }
    else if (!strcmp(pcType, MOTION_TYPE_HEAD_STR))
    {
        if (!strcmp(pcDirect, MOTION_DIRECTION_LEFT_STR))
        {
            acAngle[0] = '2';
            acAngle[1] = '8';
            ubtSetRobotServoi(SERVO_HEAD_INDEX, acAngle, iSpeed);
        }
        else if (!strcmp(pcDirect, MOTION_DIRECTION_RIGHT_STR))
        {
            acAngle[0] = '7';
            acAngle[1] = '8';
            ubtSetRobotServoi(SERVO_HEAD_INDEX, acAngle, iSpeed);
        }
        else if (!strcmp(pcDirect, MOTION_DIRECTION_FRONT_STR))
        {
            acAngle[0] = '5';
            acAngle[1] = 'a';
            ubtSetRobotServoi(SERVO_HEAD_INDEX, acAngle, iSpeed);
        }
        else
        {
            printf("head direct:%s error\r\n", pcDirect);
        }
    }
    else if (!strcmp(pcType, "bow"))
    {
        strcpy(&angle_array[0][0], "595a5b59585b5a3a4a6e595977674459");
        strcpy(&angle_array[1][0], "59b45b59005b5a179e3159589d19845b");
        strcpy(&angle_array[2][0], "59b45b59005a5a179e3259589d18825a");
        strcpy(&angle_array[3][0], "59b45b59005a5c679e485a5b49186b5a");
        strcpy(&angle_array[4][0], "595a5b59585b5a3a4a6e595977674459");
        for(i=0; i< iRepeat; i++)
        {
            ubtSetRobotServoi(SERVO_RANG_INDEX(1,16), angle_array[0], speed);
            usleep(speed*20*1000);
            ubtSetRobotServoi(SERVO_RANG_INDEX(1,16), angle_array[1], speed);
            usleep(speed*20*1000);
            ubtSetRobotServoi(SERVO_RANG_INDEX(1,16), angle_array[2], speed);
            usleep(speed*20*1000);
            ubtSetRobotServoi(SERVO_RANG_INDEX(1,16), angle_array[3], speed);
            usleep(speed*20*1000);
            ubtSetRobotServoi(SERVO_RANG_INDEX(1,16), angle_array[4], speed);
            usleep(speed*20*1000);
        }
    }
    else
        printf("The Type[%s] motion is unsurpport!\r\n", pcType);

    return UBTEDU_RC_SUCCESS;
}


/**
 * @brief:      ubtReadSensorValue
 * @details:    Read the sensor's value
 * @param[in]   char *pcSensorType  The sensor's type.
 *                                  gryo
 *                                  environment
 *                                  board
 *                                  infrared 
 *                                  ultrasonic
 *                                  touch
 *                                  color
 *                                  pressure
 *                                  gas
 * @param[out]  void *pValue        The sensor value. More details please see the defination as below variable type
 *                                  UBTEDU_ROBOTGYRO_SENSOR_T
 *                                  UBTEDU_ROBOTENV_SENSOR_T
 *                                  UBTEDU_ROBOTRASPBOARD_SENSOR_T
 *                                  UBTEDU_ROBOTINFRARED_SENSOR_T 
 *                                  UBTEDU_ROBOTULTRASONIC_SENSOR_T
 *                                  UBTEDU_ROBOTTOUCH_SENSOR_T
 *                                  UBTEDU_ROBOTCOLOR_SENSOR_T
 *                                  UBTEDU_ROBOTPRESSURE_SENSOR_T
 *                                  UBTEDU_ROBOTGAS_SENSOR_T
 * @param[in]   int iValueLen       The max length of pValue
 * @retval:
 */
UBTEDU_RC_T ubtReadSensorValue(char *pcSensorType, void *pValue, int iValueLen)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];

    if ((NULL == pcSensorType) || (NULL == pValue))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acSocketBuffer[0] = '\0';

    ubtRet = ubtRobot_Msg_Encode_ReadSensorValue(pcSensorType, g_iRobot2SDKPort,
             acSocketBuffer, sizeof(acSocketBuffer));

    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_ReadSensorValue(acSocketBuffer, pcSensorType, pValue, iValueLen);

    return ubtRet;
}

/**
 * @brief:      ubtReadSensorValueByAddr
 * @details:    Read the sensor's value by it's type and address
 * @param[in]   char *pcSensorType  The sensor's type.
 *                                  gryo
 *                                  environment
 *                                  board
 *                                  infrared 
 *                                  ultrasonic
 *                                  touch
 *                                  color
 *                                  pressure
 *                                  gas
 * @param[in]   int iAddr             The sensor's 7bit I2C address
 * @param[out]  void *pValue      The sensor value. More details please see the defination as below variable type
 *                                  UBTEDU_ROBOTGYRO_SENSOR_T
 *                                  UBTEDU_ROBOTENV_SENSOR_T
 *                                  UBTEDU_ROBOTRASPBOARD_SENSOR_T
 *                                  UBTEDU_ROBOTINFRARED_SENSOR_T 
 *                                  UBTEDU_ROBOTULTRASONIC_SENSOR_T
 *                                  UBTEDU_ROBOTTOUCH_SENSOR_T
 *                                  UBTEDU_ROBOTCOLOR_SENSOR_T
 *                                  UBTEDU_ROBOTPRESSURE_SENSOR_T
 *                                  UBTEDU_ROBOTGAS_SENSOR_T
 * @param[in]   int iValueLen       The max length of pValue
 * @retval:
 */
UBTEDU_RC_T ubtReadSensorValueByAddr(char *pcSensorType, int iAddr, void *pValue, int iValueLen)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];

    if ((NULL == pcSensorType) || (NULL == pValue))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acSocketBuffer[0] = '\0';

    ubtRet = ubtRobot_Msg_Encode_ReadSensorValueByAddr(pcSensorType, iAddr, g_iRobot2SDKPort,
             acSocketBuffer, sizeof(acSocketBuffer));

    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_ReadSensorValue(acSocketBuffer, pcSensorType, pValue, iValueLen);

    return ubtRet;
}

/**
 * @brief:      ubtSetRobotLED
 * @details:    Set the LED mode
 * @param[in]   char *pcType
 *                              botton
 *                              camera
 *                              mic
 * @param[in]   char *pcColor
 *                                  When pcType == "botton"
 *                                  pcColor can be set as
 *                                  white
 *                                  red
 *                                  green
 *                                  blue
 *                                  yellow
 *                                  purple
 *                                  cyan
 *                                  When pcType == "camera"
 *                                  pcColor can be set as
 *                                  red
 *                                  green
 *                                  blue
 *                                  When pcType == "mic"
 *                                  pcColor can be set as
 *                                  green
 * @param[in]   char *pcMode
 *                                  When pcType == "button"
 *                                  pcMOde can be set as
 *                                  off
 *                                  on
 *                                  blink
 *                                  breath
 *                                  colorful
 *                                  When pcType == "camera"
 *                                  pcMOde can be set as
 *                                  on
 *                                  off
 *                                  When pcType == "mic"
 *                                  pcMOde can be set as
 *                                  on
 *                                  off
 *
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtSetRobotLED(char *pcType, char *pcColor, char *pcMode)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    if ((NULL == pcColor) || (NULL == pcMode))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    if ((0 != strcmp(pcStr_Msg_Type_Recognition_Button, pcType)) &&
        (0 != strcmp(pcStr_Msg_Param_Led_Type_Camera, pcType) ) &&
        (0 != strcmp(pcStr_Msg_Param_Led_Type_Mic, pcType) ))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    acSocketBuffer[0] = '\0';
    ubtRet = ubtRobot_Msg_Encode_SetRobotLED(g_iRobot2SDKPort, pcType, pcColor, pcMode,
             acSocketBuffer, sizeof(acSocketBuffer));

    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_SetRobotLED(acSocketBuffer);
    return ubtRet;
}

/**
 * @brief:      ubtStartRobotAction
 * @details:    Let the robot play an action
 * @param[in]   char *pcName  The action file's name For
 *                              example: push up, bow
 * @param[in]   int iRepeat   Repeat times. 0 means infinite
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtStartRobotAction(char *pcName, int iRepeat)
{
    int         iTime = 0;
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    if (NULL == pcName)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    acSocketBuffer[0] = '\0';
    ubtRet = ubtRobot_Msg_Encode_StartRobotAction(g_iRobot2SDKPort, pcName, iRepeat,
             acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_StartRobotAction(acSocketBuffer, &iTime);

    while(iRepeat--)
    {
        sleep(iTime/1000);
    }
    sleep(1);
    return ubtRet;
}




/**
  * @brief      Stop to run the robot action file
  *
  * @return  UBTEDU_RC_T 0 Success,  Others    Failed
  *
  */
UBTEDU_RC_T ubtStopRobotAction(void)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    acSocketBuffer[0] = '\0';
    ubtRet = ubtRobot_Msg_Encode_StopRobotAction(g_iRobot2SDKPort,
             acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_StopRobotAction(acSocketBuffer);
    return ubtRet;
}

/**
 * @brief   Start voice recognition
 *
 * @return  UBTEDU_RC_T 0 Success,  Others    Failed
 *
 */
UBTEDU_RC_T ubtVoiceStart()
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    acSocketBuffer[0] = '\0';
    ubtRet = ubtRobot_Msg_Encode_VoiceStart(g_iRobot2SDKPort,
                                            acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_VoiceStart(acSocketBuffer);
    return ubtRet;
}

/**
 * @brief   Stop voice recognition
 *
 * @return  UBTEDU_RC_T 0 Success,  Others    Failed
 *
 */
UBTEDU_RC_T ubtVoiceStop()
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    acSocketBuffer[0] = '\0';
    ubtRet = ubtRobot_Msg_Encode_VoiceStop(g_iRobot2SDKPort,
                                           acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_VoiceStop(acSocketBuffer);
    return ubtRet;
}

/**
 * @brief   Play the TTS voice
 *
 * @return  UBTEDU_RC_T 0 Success,  Others    Failed
 * @param   int isInterrputed   Interrupt the previous TTS, if it is not finished.
 *                              0   Not interrupt the previous TTS
 *                              1   Interrupt the previous TTS, start the current TTS immediately
 * @param   char* pcTTS The message to be sent to TTS
 *
 */
UBTEDU_RC_T ubtVoiceTTS(int isInterrputed, char *pcTTS)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    struct timeval tsock = {30, 0};
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    acSocketBuffer[0] = '\0';
    if (NULL == pcTTS)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    ubtRet = ubtRobot_Msg_Encode_VoiceTTS(g_iRobot2SDKPort, isInterrputed, pcTTS,
                                          acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }


    tsock.tv_sec = 30;
    if (setsockopt(g_iRobot2SDK, SOL_SOCKET, SO_RCVTIMEO, &tsock, sizeof(tsock)) < 0)
    {
        printf("set SO_RCVTIMEO setsockopt failed!\r\n");
    }
    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }
    tsock.tv_sec = 3;
    if (setsockopt(g_iRobot2SDK, SOL_SOCKET, SO_RCVTIMEO, &tsock, sizeof(tsock)) < 0)
    {
        printf("set SO_RCVTIMEO setsockopt failed!\r\n");
    }

    ubtRet = ubtRobot_Msg_Decode_VoiceTTS(acSocketBuffer);
    return ubtRet;
}

/**
 * @brief   Play a music
 *
 * @return  UBTEDU_RC_T 0 Success,  Others    Failed
 * @param   char* pcPath  The directory of the music file
 *              in Robot
 * @param   char* pcName Music file name. Only support
 *              wav file without ".wav"
 *
 */
UBTEDU_RC_T ubtPlayMusic(char * pcPlayMusicType, char *pcName)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    acSocketBuffer[0] = '\0';
    if ((NULL == pcPlayMusicType) || (NULL == pcName))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    ubtRet = ubtRobot_Msg_Encode_PlayMusic(pcPlayMusicType, g_iRobot2SDKPort, pcName,
                                           acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_PlayMusic(acSocketBuffer);
    return ubtRet;
}

/**
 * @brief:      ubtGetMusicList
 * @details:    Get the music list from robot
 * @param[out]  char *pcMusicName[]     The music file name list.
 * @param[in]   int iEachMusicNameLen   The max length for earch music file name.
 * @param[in]   int iMusicNameNum       The max number of music file name for pcMusicName
 * @param[in/out]   int *piIndex            The music file index, it can used when you trying to get all the music
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtGetMusicList(char *pacMusicName[], int iEachMusicNameLen,
                            int iMusicNameNum, int *piIndex)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];



    acSocketBuffer[0] = '\0';
    if ((NULL == pacMusicName) || (NULL == piIndex))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    ubtRet = ubtRobot_Msg_Encode_GetMusic(g_iRobot2SDKPort, *piIndex,
                                          acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iRobot2SDKPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_GetMusic(acSocketBuffer, pacMusicName, iEachMusicNameLen,
                                          iMusicNameNum, piIndex);
    return ubtRet;
}


/**
 * @brief:	ubtKeyDetect
 * @details:	Detect Key pulldown event include Power button etc.
 * @param[in]	pcKeyType
 * @param[in]	iTimeout
 * @param[out]	pcValue
 * @retval:
 */
UBTEDU_RC_T ubtKeyDetect(char *pcKeyType, char *pcValue, int iTimeout)
{
	int 		iRet = 0;
	UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
	char		acSocketBuffer[SDK_MESSAGE_MAX_LEN];

	struct timeval tsock = {30, 0};

	DebugTrace("ubtKeyDetect called! iTimeout = %d ", iTimeout );

	if (NULL == pcKeyType)
	{
		return UBTEDU_RC_WRONG_PARAM;
	}

	if((iTimeout >= 10)&&(iTimeout <= 600))
	{
		tsock.tv_sec = iTimeout;
	}

	acSocketBuffer[0] = '\0';

	ubtRet = ubtRobot_Msg_Encode_KeyDetect(pcKeyType, g_iRobot2SDKPort,
			 acSocketBuffer, sizeof(acSocketBuffer));
	if (UBTEDU_RC_SUCCESS != ubtRet)
	{
		return ubtRet;
	}

	iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
							 g_iRobot2SDKPort, acSocketBuffer, strlen(acSocketBuffer));
	if (iRet != strlen(acSocketBuffer))
	{
		return UBTEDU_RC_SOCKET_SENDERROR;
	}


	if (setsockopt(g_iRobot2SDK, SOL_SOCKET, SO_RCVTIMEO, &tsock, sizeof(tsock)) < 0)
	{
		printf("set SO_RCVTIMEO setsockopt failed!\r\n");
	}

	/* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
	bot */
	iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
	if (iRet != strlen(acSocketBuffer))
	{
		return UBTEDU_RC_SOCKET_SENDERROR;
	}

	tsock.tv_sec = 3;
	if (setsockopt(g_iRobot2SDK, SOL_SOCKET, SO_RCVTIMEO, &tsock, sizeof(tsock)) < 0)
	{
		printf("set SO_RCVTIMEO setsockopt failed!\r\n");
	}

	ubtRet = ubtRobot_Msg_Decode_KeyDetect(acSocketBuffer,  pcValue);
	return ubtRet;
}



/**
 * @brief:      ubtVisionDetect
 * @details:    Detect Vision content include face gesture and object
 * @param[in]   pcVisionType
 * @param[in]   iTimeout
 * @param[out]  pcValue
 * @retval:
 */
UBTEDU_RC_T ubtVisionDetect(char *pcVisionType, char *pcValue, int iTimeout)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];

    struct timeval tsock = {30, 0};

    DebugTrace("ubtVisionDetect called! iTimeout = %d ", iTimeout );

    if (NULL == pcVisionType)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    if((iTimeout >= 10)&&(iTimeout <= 600))
    {
        tsock.tv_sec = iTimeout;
    }

    acSocketBuffer[0] = '\0';

    ubtRet = ubtRobot_Msg_Encode_VisionDetect(pcVisionType, g_iRobot2SDKPort,
             acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iRobot2SDKPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }


    if (setsockopt(g_iRobot2SDK, SOL_SOCKET, SO_RCVTIMEO, &tsock, sizeof(tsock)) < 0)
    {
        printf("set SO_RCVTIMEO setsockopt failed!\r\n");
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    tsock.tv_sec = 3;
    if (setsockopt(g_iRobot2SDK, SOL_SOCKET, SO_RCVTIMEO, &tsock, sizeof(tsock)) < 0)
    {
        printf("set SO_RCVTIMEO setsockopt failed!\r\n");
    }

    ubtRet = ubtRobot_Msg_Decode_VisionDetect(acSocketBuffer,  pcValue);
    return ubtRet;
}


/**
 * @brief:      ubtTakeAPhoto
 * @details:    Take a photo. Not working yet
 * @param[in]   char *pacPhotoName[]
 * @param[in]   int iPhotoNameLen
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtTakeAPhoto(char *pacPhotoName, int iPhotoNameLen)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];



    acSocketBuffer[0] = '\0';

    ubtRet = ubtRobot_Msg_Encode_TakePhotos(pacPhotoName, g_iRobot2SDKPort,
                                            acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iRobot2SDKPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_TakePhotos(acSocketBuffer,  iPhotoNameLen);
    return ubtRet;
}

/**
 * @brief:      ubtTransmitCMD
 * @details:    Send the commands to servo.
 * @param[in]   char *pcRemoteCmd         The content sent to servo
 * @param[in]   char *pcRemoteCmdRetData  The message received from servo
 * @param[in]   int iRemoteCmdRetDataLen  The max length of pcRemoteCmdRetData
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtTransmitCMD(char *pcRemoteCmd, char *pcRemoteCmdRetData, int iRemoteCmdRetDataLen)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];



    acSocketBuffer[0] = '\0';
    if ((NULL == pcRemoteCmd) || (NULL == pcRemoteCmdRetData))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    ubtRet = ubtRobot_Msg_Encode_TransmitCMD(g_iRobot2SDKPort, pcRemoteCmd,
             acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iRobot2SDKPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_TransmitCMD(acSocketBuffer, pcRemoteCmdRetData, iRemoteCmdRetDataLen);
    return ubtRet;
}

/**
 * @brief   Send Blockly run status to mobile APP
 *
 * @return  UBTEDU_RC_T 0 Success,  Others    Failed
 * @param   char* pcName - [in] The name
 * @param   char* pcString - [in] The status
 *
 */
UBTEDU_RC_T ubtReportStatusToApp(char* pcName, char *pcString)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];



    acSocketBuffer[0] = '\0';
    if (NULL == pcString)
    {
        return UBTEDU_RC_WRONG_PARAM;
    }
    ubtRet = ubtRobot_Msg_Encode_ReportStatusToApp(pcName, pcString,
             acSocketBuffer, sizeof(acSocketBuffer));
    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, g_pstConnectedRobotInfo.acIPAddr,
                             g_iRobot2SDKPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    /*iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer)) {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }*/

    ubtRet = ubtRobot_Msg_Decode_ReportStatusToApp(acSocketBuffer);
    return ubtRet;
}


/**
 * @brief:      ubtRobotDiscovery
 * @details:    Search the robot in the local subnet.
 *              If there is more than one robot in the local subnet,
 *              please call this function multi times with iIsNeedSendRequest = 0
 * @param[in]   int iIsNeedSendRequest            1: Send the search request, 0: Do not send search request
 * @param[in]   char *pcAccount                   The user account
 * @param[in]   UBTEDU_ROBOTINFO_T *pstRobotInfo  The robot infomation
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtRobotDiscovery(int iIsNeedSendRequest, char *pcAccount, UBTEDU_ROBOTINFO_T *pstRobotInfo)
{
    int             iRet = 0;
    int             iOn = 1;
    UBTEDU_RC_T     ubtRet = UBTEDU_RC_FAILED;
    UBTEDU_ROBOTINFO_T  stRobotInfo;
    char        acBcastIP[UBTEDU_ROBOT_IP_ADDR_LEN+1];
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];



    acSocketBuffer[0] = '\0';


    if ((NULL == pcAccount) || (NULL == pstRobotInfo))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    if (1 == iIsNeedSendRequest)
    {
        ubtRet = ubtRobot_Msg_Encode_RobotDiscovery(pcAccount, g_iRobot2SDKPort,
                 acSocketBuffer, sizeof(acSocketBuffer));
        if (UBTEDU_RC_SUCCESS != ubtRet)
        {
            return ubtRet;
        }
        ubtRet = _ubtGetLocalBcastIP(acBcastIP);

        if(UBTEDU_RC_SUCCESS != ubtRet)
        {
            return ubtRet;
        }

        setsockopt(g_iSDK2Robot, SOL_SOCKET, SO_BROADCAST, &iOn, sizeof(int));

        iRet = _ubtMsgSend2Robot(g_iSDK2Robot, acBcastIP,
                                 g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
        if (iRet != strlen(acSocketBuffer))
        {
            return UBTEDU_RC_SOCKET_SENDERROR;
        }

        iOn = 0;
        setsockopt(g_iSDK2Robot, SOL_SOCKET, SO_BROADCAST, &iOn, sizeof(int));
    }

    memset(&stRobotInfo, 0, sizeof(UBTEDU_ROBOTINFO_T));
    memset(&acSocketBuffer, 0, sizeof(acSocketBuffer));
    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }
    else if (iRet <= 0)
    {
        /* Socket receiving timeout, the default value is 3s */
        return UBTEDU_RC_SUCCESS;
    }

    ubtRet = ubtRobot_Msg_Decode_RobotDiscovery(acSocketBuffer, &stRobotInfo);
    memcpy(pstRobotInfo, &stRobotInfo, sizeof(UBTEDU_ROBOTINFO_T));

    return ubtRet;
}


/**
 * @brief:      ubtRobotConnect
 * @details:    Connect to Robot
 * @param[in]   char *pcAccount  The account which connected to robot
 * @param[in]   char *pcVersion  The SDK version
 * @param[in]   char *pcIPAddr   Robot IP address
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtRobotConnect(char *pcAccount, char *pcVersion, char *pcIPAddr)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acRobotName[MSG_CMD_STR_MAX_LEN];
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    if ((NULL == pcAccount) || (NULL == pcVersion) || (NULL == pcIPAddr))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    if(!strcmp(pcIPAddr,SDK_LOCAL_IP))
    {
        ubtRet = UBTEDU_RC_SUCCESS;
        strncpy(g_pstConnectedRobotInfo.acIPAddr, pcIPAddr, sizeof(g_pstConnectedRobotInfo.acIPAddr));
        strncpy(g_pstConnectedRobotInfo.acName, acRobotName, sizeof(g_pstConnectedRobotInfo.acName));
        return ubtRet;
    }

    acRobotName[0] = '\0';
    acSocketBuffer[0] = '\0';
    ubtRet = ubtRobot_Msg_Encode_ConnectRobot(pcAccount, g_iRobot2SDKPort,
             acSocketBuffer, sizeof(acSocketBuffer));

    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, pcIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    memset(&acSocketBuffer, 0, sizeof(acSocketBuffer));
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_ConnectRobot(acSocketBuffer, acRobotName,
             sizeof(acRobotName));

    if (UBTEDU_RC_SUCCESS == ubtRet)
    {
        /* Start the heart beat timer every 5 seconds */
        strncpy(g_pstConnectedRobotInfo.acIPAddr, pcIPAddr, sizeof(g_pstConnectedRobotInfo.acIPAddr));
        strncpy(g_pstConnectedRobotInfo.acName, acRobotName, sizeof(g_pstConnectedRobotInfo.acName));
    }

    return ubtRet;
}

/**
 * @brief:      ubtRobotDisconnect
 * @details:    Disconnect from the robot
 * @param[in]   char *pcAccount  The account which connected to robot
 * @param[in]   char *pcVersion  The SDK version
 * @param[in]   char *pcIPAddr   Robot IP address
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtRobotDisconnect(char *pcAccount, char *pcVersion, char *pcIPAddr)
{
    int         iRet = 0;
    UBTEDU_RC_T ubtRet = UBTEDU_RC_FAILED;
    char        acSocketBuffer[SDK_MESSAGE_MAX_LEN];


    if ((NULL == pcAccount) || (NULL == pcVersion) || (NULL == pcIPAddr))
    {
        return UBTEDU_RC_WRONG_PARAM;
    }

    if(!strcmp(pcIPAddr, SDK_LOCAL_IP))
    {
        ubtRet = UBTEDU_RC_SUCCESS;
        g_pstConnectedRobotInfo.acIPAddr[0] = '\0';
        g_pstConnectedRobotInfo.acName[0] = '\0';
        return ubtRet;
    }

    acSocketBuffer[0] = '\0';
    ubtRet = ubtRobot_Msg_Encode_DisconnectRobot(pcAccount, g_iRobot2SDKPort,
             acSocketBuffer, sizeof(acSocketBuffer));

    if (UBTEDU_RC_SUCCESS != ubtRet)
    {
        return ubtRet;
    }

    iRet = _ubtMsgSend2Robot(g_iSDK2Robot, pcIPAddr,
                             g_iSDK2RobotPort, acSocketBuffer, strlen(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    /* Please note, acSocketBuf has already been written when ubtMsgRecvFromRo-
    bot */
    iRet = _ubtMsgRecvFromRobot(g_iRobot2SDK, acSocketBuffer, sizeof(acSocketBuffer));
    if (iRet != strlen(acSocketBuffer))
    {
        return UBTEDU_RC_SOCKET_SENDERROR;
    }

    ubtRet = ubtRobot_Msg_Decode_DisconnectRobot(acSocketBuffer);

    if (UBTEDU_RC_SUCCESS == ubtRet)
    {
        /* Stop the heart beat timer */
        g_pstConnectedRobotInfo.acIPAddr[0] = '\0';
        g_pstConnectedRobotInfo.acName[0] = '\0';
    }

    return ubtRet;
}


/**
 * @brief:      ubtRobotInitialize
 * @details:    Init the SDK for 1x
 * @param[in]   None
 * @param[out]  None
 * @retval:
 */
UBTEDU_RC_T ubtRobotInitialize()
{
    int ret = UBTEDU_RC_SUCCESS;
    int iPort = -1;
    int iSocketFd = -1;
    pthread_attr_t attr;
    pthread_t  pid;

    memset(&g_stSDK2RobotSockAddr, 0, sizeof(g_stSDK2RobotSockAddr));
    iSocketFd = _udpServerInit(&iPort);
    if (iSocketFd < 0)
    {
        printf("Create robot to SDK socket failed!\r\n");
        return UBTEDU_RC_SOCKET_FAILED;
    }
    g_iRobot2SDKPort = iPort;
    g_iRobot2SDK = iSocketFd;


    iSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (iSocketFd < 0)
    {
        printf("Create socket to robot failed!\r\n");
        return UBTEDU_RC_SOCKET_FAILED;
    }
    g_iSDK2Robot = iSocketFd;
    g_iSDK2RobotPort = SDK_REMOTE_SOCKET_PORT;


    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (ret != 0)
    {
        printf("pthread_attr_setdetachstate error \n");
        return UBTEDU_RC_FAILED;
    }

    ret = pthread_create(&pid, &attr, _ubtTimerTimeout, NULL);
    if (ret < 0)
    {
        printf("pthread_create failed \n");
        return UBTEDU_RC_FAILED;
    }


    memset(&g_pstConnectedRobotInfo, 0, sizeof(g_pstConnectedRobotInfo));

    return ret;
}

/**
 * @brief:      ubtRobotDeinitialize
 * @details:    Destroy the SDK for 1x
 * @param[in]   None
 * @param[out]  None
 * @retval:
 */
void ubtRobotDeinitialize()
{

    if (-1 != g_iRobot2SDK)
    {
        close(g_iRobot2SDK);
        g_iRobot2SDK = -1;
    }
    g_iRobot2SDKPort = -1;

    if (-1 != g_iSDK2Robot)
    {
        close(g_iSDK2Robot);
        g_iSDK2Robot = -1;
    }
    g_iSDK2RobotPort = -1;

    memset(&g_stSDK2RobotSockAddr, 0, sizeof(g_stSDK2RobotSockAddr));

    memset(&g_pstConnectedRobotInfo, 0, sizeof(g_pstConnectedRobotInfo));

    return ;
}

#if 0


int main(int argc, char * argv[])
{
    char test[256];

    printf("test socket io main\r\n");


    /*  broadSocketThread();
        sleep(1);
    //  sprintf(test,"my socket iPort is %d",socket_iPort);
        send_broad_msg(test,strlen(test));
        while(1){
            sleep(1);
        }*/
    //ReadRobotServo(0x35,test);
    //GetSWVersion(atoi(argv[1]), test);
    //GetRobotStatus(atoi(argv[1]), test);
    // printf("test:%x %x %x %x\r\n",test[0],test[1],test[2],test[3]);
    //ReadRobotServo(atoi(argv[1]), test);
    //printf("acAngle:%s\r\n",test);
    //printf("ret:%d\r\n",ubtSetRobotServo(atoi(argv[1]), argv[2],atoi(argv[3])));
    //printf("ret:%d\r\n",setRobotVolume(atoi(argv[1])));
    // printf("ret:%d\r\n",PlayRobotAction(argv[1], atoi(argv[2])));
    //printf("ret:%d\r\n",StopRobot());
    // printf("ret:%d\r\n",PlayMusic(argv[1], argv[2]));
    // printf("ret:%d\r\n",ReadSensorValue(argv[1]));
    //    SetRobotLED(atoi(argv[1]), atoi(argv[2]));
    TransmitCMD(argv[1], test);
    printf("recv %s \r\n", test);

    return 0;
}


#endif





