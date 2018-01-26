/**
 * @file                RobotMsg.h
 * @brief           RobotMsg.c header file
 * @author          Cygnus Yang
 * @date            Thursday, August 31, 2017
 * @version         Initial Draft
 * @par             Copyright (C),  2017-2023, UBT Education
 * @par History:
 * 1.Date:          Thursday, August 31, 2017
 *   Author:            Cygnus Yang
 *   Modification:      Created file
*/
#ifndef __ROBOTMSG_H__
#define __ROBOTMSG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
	char acMacAddr[ROBOT_MAC_LEN];
} ROBOTAGENT_MAC_T;

typedef struct robotagent_sc_coordinates_t
{
    int iX;
    int iY;
    int iZ;
    int iOrder;
} ROBOTAGENT_SC_COORDINATES_T;


extern UBTEDU_RC_T ubtRobot_Msg_Decode_CheckAPPStatus(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_ConnectRobot(char *pcRecvBuf, char *pcRobotName, int iRobotNameLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_DiscoveryRobot(char *pcRecvBuf, char *pcRobotName, int iRobotNameLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_DetectVoiceMsg(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_DisconnectRobot(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_GetMusic(char *pcRecvBuf,
        char *pcMusicName[], int iEachMusicNameLen,
        int iMusicNameNum, int *piIndex);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_PlayMusic(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_ReadRobotServo(char *pcRecvBuf, int iIndexMask, char *pcAngle, int iAngleLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_ReadSensorValue(char *pcRecvBuf, char *pcSensorType, void *pValue, int iValueLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_ReportStatusToApp(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_RobotStatus(char *pcType, char *pcRecvBuf, void *pStatus);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_RobotDiscovery(char *pcRecvBuf, UBTEDU_ROBOTINFO_T  *pstRobotInfo);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_RobotStatus(char *pcType, char *pcRecvBuf, void *pStatus);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SetRobotLED(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SetRobotMotion(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SetRobotServo(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SetRobotVolume(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_StartRobotAction(char *pcRecvBuf, int *piTime);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_StopRobotAction(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionCancel(char *pcRecvBuf, int *piSeq);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionCancelAck(char *pcRecvBuf, int *piSeq);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionStart(char *pcRecvBuf,
        int *piSeq,
        int *piActionRepeatTime,
        double *pdDelayTime,
        char *pcActionName,
        int iActionNameLen);

extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmGotoXYZ(char *pcAccount, int iPort, int iSeq, int iId,
        ROBOTAGENT_SC_COORDINATES_T *pstXYZ, char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmGotoXYZ(char *pcRecvBuf, int *piId,
        ROBOTAGENT_SC_COORDINATES_T *pstXYZ, int *piSeq);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionGotoXYZAck(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionGotoXYZAck(char *pcRecvBuf,
        ROBOTAGENT_SC_COORDINATES_T *pstXYZ, int *piSeq, int *piId);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmReachXYZ(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmReachXYZ(char *pcRecvBuf,
        int *piSeq, int *piId);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmReachXYZAck(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmReachXYZAck(char *pcRecvBuf,
        int *piSeq, int *piId);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmQueryXYZ(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmQueryXYZ(char *pcRecvBuf,
        int *piSeq, int *piId);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmQueryXYZAck(char *pcAccount, int iPort, int iSeq, int iId,
        ROBOTAGENT_SC_COORDINATES_T *pstXYZ, char *pcStatus,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmQueryXYZAck(char *pcRecvBuf,
        int *piSeq, int *piId, ROBOTAGENT_SC_COORDINATES_T *pstXYZ, char *pcStatus);

extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionStartAck(char *pcRecvBuf, int *piSeq);

extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionEnd(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionEnd(char *pcRecvBuf,
        int *piSeq, int *piId);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionEndAck(char *pcAccount, int iPort, int iSeq, int iId,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmActionEndAck(char *pcRecvBuf, int *piSeq, int *piId);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmAllocID(char *pcRecvBuf, int *piSeq, 
						int *piID, int* piCount, ROBOTAGENT_MAC_T *pcMacAddr, int iMacLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmAllocIDAck(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmInit(char *pcRecvBuf, int *piSeq, char *pcTimezone, double *pdGyroOffset);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SwarmInitAck(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_SWVersion(char *pcRecvBuf, char *pcVersion, int iVersionLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_TakePhotos(char *pcRecvBuf, int iPhotoNameLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_TransmitCMD(char *pcRecvBuf, char *pcRemoteCmdRetData, int iRemoteCmdRetDataLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_VisionDetect(char *pcRecvBuf, char *pcValue);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_VoiceStart(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_VoiceStop(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_VoiceTTS(char *pcRecvBuf);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_CheckAPPStatus(char *pcCmd, char *pcType,
        int iPort, struct timeval stTimeVal,
        char *pcBuf, char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_ConnectRobot(char *pcAccount, int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_DetectVoiceMsg(int iPort,
        char *pcBuf, char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_DisconnectRobot(char *pcAccount, int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_GetMusic(int iPort, int iIndex,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_PlayMusic(char *pcPlayMusicType, int iPort, char *pcName,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_ReadRobotServo(int iPort, char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_ReadSensorValue(char *pcSensorType, int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_ReadSensorValueByAddr(char *pcSensorType, int iAddr, int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_ReportStatusToApp(char *pcName, char *pcString,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_RobotStatus(char *pcCmd, char *pcType, char *pcParam, int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_RobotDiscovery(char *pcAccount, int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotLED(int iPort, char *pcType,
        char *pcColor, char *pcMode,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotMotion(char *pcCmd, char *pcType,
        int iPort, int iVolume,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotServo(int iPort, int iServoMask, char *pcAngle, int iTime,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotVolume(int iPort, int iVolume,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotVolume_Minus(int iPort, char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SetRobotVolume_Plus(int iPort, char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_StartRobotAction(int iPort,
        char *pcName, int iRepeat,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_StopRobotAction(int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionCancel(char *pcAccount, int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionCancelAck(char *pcAccount, int iPort, int iSeq,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionStart(char *pcAccount, int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmActionStartAck(char *pcAccount, int iPort, int iSeq,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmAllocID(char *pcAccount, int iPort,
        char *pcTimezone, int iSeq,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmAllocIDAck(char *pcAccount, int iPort, int iSeq, char *pcMacAddr,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmInit();
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SwarmInitAck(char* pcAccount, int iPort,
        int iSeq, char *pcMac, char* pcSendBuf,
        int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_SWVersion(char *pcParam, int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_TakePhotos(char *pacPhotoName, int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_EventDetect(char *pcEventType, int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_EventDetect(char *pcRecvBuf, char *pcValue);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_VisionDetect(char *pcVisionType, int iPort,
        char *pcSendBuf, int iBufLen, int iTimeout);
extern UBTEDU_RC_T ubtRobot_Msg_Decode_VisionDetect(char *pcRecvBuf, char *pcValue);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_TransmitCMD(int iPort, char *pcRemoteCmd,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_VoiceStart(int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_VoiceStop(int iPort,
        char *pcSendBuf, int iBufLen);
extern UBTEDU_RC_T ubtRobot_Msg_Encode_VoiceTTS(int iPort,
        int isInterrupted, char *pcTTS,
        char *pcSendBuf, int iBufLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __ROBOTMSG_H__ */
