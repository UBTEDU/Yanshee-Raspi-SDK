/** @file   RobotApi.h
 * @note    UBT Education Co., Ltd. All Right Reserved.
 * @brief   Defines the APIs for UBTEDU SDK
 *
 * @author   Cygnus Yang
 * @date     2017-8-14
 * @version  1.0.0.0
 *
 * @note
 * @note History:
 * @note     Cygnus Yang   2017-12-01    1.0   first import
 * @note
 * @warning
 */


#ifndef _ROBOT_API_H_
#define _ROBOT_API_H_



#define UBTEDU_SDK_SW_VER              "01" /**< SDK software version */


#define UBTEDU_ROBOT_NAME_LEN       (32)	/**< MAX length of the robot name */
#define UBTEDU_ROBOT_IP_ADDR_LEN    (16)	/**< MAX length of the IP address */
#define MAX_SHELL_CMD_LEN   256             /**< MAX length of the Shell cmd */


/**
 * @brief   Robot status type
*/
typedef enum
{
    UBTEDU_ROBOT_STATUS_TYPE_PLAYACTION = 1,    /**< Play an action file */
    UBTEDU_ROBOT_STATUS_TYPE_VOLUME,            /**< Volume status */
    UBTEDU_ROBOT_STATUS_TYPE_POWER_VOLTAGE,     /**< Power voltage status */
    UBTEDU_ROBOT_STATUS_TYPE_POWER_RECHARGE,    /**< Power recharge status */
    UBTEDU_ROBOT_STATUS_TYPE_POWER_PERCENT,     /**< Power percent status */
    UBTEDU_ROBOT_STATUS_TYPE_INVALID            /**< Invalid type */
} UBTEDU_ROBOT_STATUS_TYPE_e;

/**
 * @brief   Play music status
*/
typedef enum
{
    UBTEDU_ROBOT_PLAY_STATUS_IDLE,          /**< Idle status */
    UBTEDU_ROBOT_PLAY_STATUS_PLAYING,       /**< Playing */
    UBTEDU_ROBOT_PLAY_STATUS_PAUSED,        /**< Paused */
    UBTEDU_ROBOT_PLAYSTATUS_END,            /**< End */
    UBTEDU_ROBOT_PLAY_STATUS_INVALID        /**< Invalid status */
} UBTEDU_ROBOT_PLAYMUSIC_STATUS_e;

/**
 * @brief   Robot software version type
*/
typedef enum
{
    UBTEDU_ROBOT_SOFTVERSION_TYPE_STM32 = 0,    /**< Embedded system version */
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS1,      /**< Servos' version */
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS2,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS3,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS4,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS5,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS6,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS7,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS8,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS9,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS10,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS11,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS12,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS13,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS14,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS15,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS16,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS17,
    UBTEDU_ROBOT_SOFTVERSION_TYPE_SDK = 30,         /**< SDK version */
    UBTEDU_ROBOT_SOFTVERSION_TYPE_RASPI = 31,       /**< Robot management application version */
    UBTEDU_ROBOT_SOFTVERSION_TYPE_INVALID       /**< Invalid type */
} UBTEDU_ROBOT_SOFTVERSION_TYPE_e;

/**
 * @brief    Return value for SDK
 */
typedef enum
{
    UBTEDU_RC_SUCCESS = 0,    /**< Success */
    UBTEDU_RC_FAILED,         /**< Failed */
    UBTEDU_RC_NORESOURCE,     /**< No resource */
    UBTEDU_RC_NOT_FOUND,      /**< Not found */
    UBTEDU_RC_WRONG_PARAM,    /**< Wrong parameter */
		
	UBTEDU_RC_IGNORE,		  /**< Ignore this return value */

    UBTEDU_RC_SOCKET_FAILED = 100,    /**< Socket error */
    UBTEDU_RC_SOCKET_NORESOURCE,      /**< No resource when sending message out */
    UBTEDU_RC_SOCKET_TIMEOUT,         /**< Recevied message timeout */
    UBTEDU_RC_SOCKET_ENCODE_FAILED,   /**< Encode the message failed */
    UBTEDU_RC_SOCKET_DECODE_FAILED,   /**< Decode the message failed */
    UBTEDU_RC_SOCKET_ENCODE_ERROR,    /**< Encode the message error */
    UBTEDU_RC_SOCKET_DECODE_ERROR,    /**< Decode the message error. It is possible that received a wrong message */
    UBTEDU_RC_SOCKET_SENDERROR,       /**< Error when sending message out */

    UBTEDU_RC_VOICE_FAILED,           /**< Voice recognition failed */
    UBTEDU_RC_VOICE_GRAMMAR_ERROR,    /**< Voiice recognition grammer error */
    UBTEDU_RC_VOICE_AIUIDECODE_ERROR, /**< Decode AIUI message failed */
    UBTEDU_RC_LAST                    /**< The last return value */
} UBTEDU_RC_T;

/**
 * @brief    All the servos angle
 */
typedef struct _RobotServo
{
    int SERVO1_ANGLE;	/**< The 1st servo's angle */
    int SERVO2_ANGLE;	/**< The 2nd servo's angle */
    int SERVO3_ANGLE;	/**< The 3rd servo's angle */
    int SERVO4_ANGLE;	/**< The 4th servo's angle */
    int SERVO5_ANGLE;	/**< The 5th servo's angle */
    int SERVO6_ANGLE;	/**< The 6th servo's angle */
    int SERVO7_ANGLE;	/**< The 7th servo's angle */
    int SERVO8_ANGLE;	/**< The 8th servo's angle */
    int SERVO9_ANGLE;	/**< The 9th servo's angle */
    int SERVO10_ANGLE;	/**< The 10th servo's angle */
    int SERVO11_ANGLE;	/**< The 11th servo's angle */
    int SERVO12_ANGLE;	/**< The 12th servo's angle */
    int SERVO13_ANGLE;	/**< The 13th servo's angle */
    int SERVO14_ANGLE;	/**< The 14th servo's angle */
    int SERVO15_ANGLE;	/**< The 15th servo's angle */
    int SERVO16_ANGLE;	/**< The 16th servo's angle */
    int SERVO17_ANGLE;	/**< The 17th servo's angle */
} UBTEDU_ROBOTSERVO_T;

/**
 * @brief    Face expression value
 */
typedef struct _FaceExpression
{ 
    float fHappinessValue;      /**< happiness */
    float fSurpriseValue;       /**< surprise */
    float fAngerValue;      /**< anger */
    float fSadnessValue;        /**< sadness */
    float fNeutralValue;        /**< neutral */
    float fDisgustValue;        /**< disgust */
    float fFearValue;       /**< fear */
} UBTEDU_FACEEXPRE_T;

/**
 * @brief   Robot infomation
*/
typedef struct _RobotInfo
{
    char acName[UBTEDU_ROBOT_NAME_LEN];			/**< Robot's name */
    char acIPAddr[UBTEDU_ROBOT_IP_ADDR_LEN];	/**< Robot's IP address */
} UBTEDU_ROBOTINFO_T;

/**
 * @brief   Gyro sensor data
*/
typedef struct _RobotGyroSensor
{
	double  dGyroxValue; /**< Gyro x value */
	double  dGyroyValue; /**< Gyro y value */
	double  dGyrozValue; /**< Gyro z value */
	double  dAccexValue; /**< accelerate x value */
	double  dAcceyValue; /**< accelerate y value */
	double  dAccezValue; /**< accelerate z value */
	double  dCompassxValue; /**< compass x value */
	double  dCompassyValue; /**< compass y value */
	double  dCompasszValue; /**< compass z value */
	double  dEulerxValue; /**< euler x value */
	double  dEuleryValue; /**< euler y value */
	double  dEulerzValue; /**< euler z value */
} UBTEDU_ROBOTGYRO_SENSOR_T;

/**
 * @brief   Environment sensor data
*/
typedef struct _RobotEnvSensor
{
    int iTempValue;      /**<  temperature value */
	int iHumiValue;      /**<  humidity value */
	int iPresValue;      /**<  pressure value */	
} UBTEDU_ROBOTENV_SENSOR_T;

/**
 * @brief   Raspberry Pi board PCB data
*/
typedef struct _RobotRaspPiBoardSensor
{
    int iValue;         /**<    Board temperature */
} UBTEDU_ROBOTRASPBOARD_SENSOR_T;

/**
 * @brief   Ultrasonic sensor data
*/
typedef struct _RobotUltrasonicSensor
{
    int iValue;         /**<    The distance via ultrasonic sensor */
} UBTEDU_ROBOTULTRASONIC_SENSOR_T;

/**
 * @brief   Infrared sensor data
*/
typedef struct _RobotInfraredSensor
{
    int iValue;         /**<    The distance via infrared sensor */
} UBTEDU_ROBOTINFRARED_SENSOR_T;

/**
 * @brief   Touch sensor data
*/
typedef struct _RobotTouchSensor
{
    int iValue;         /**<    The Touch  sensor */
} UBTEDU_ROBOTTOUCH_SENSOR_T;

/**
 * @brief   Color sensor data
*/
typedef struct _RobotColorSensor
{
    int iRedValue;         /**<    The red value of color sensor */
    int iGreenValue;         /**<    The Green value of color sensor */
    int iBlueValue;         /**<    The Bluevalue of color sensor */
    int iClearValue;         /**<    The Clear value of color sensor */
} UBTEDU_ROBOTCOLOR_SENSOR_T;

/**
 * @brief   Pressure sensor data
*/
typedef struct _RobotPressureSensor
{
    int iValue;         /**<    The Pressure via Pressure sensor */
} UBTEDU_ROBOTPRESSURE_SENSOR_T;

/**
 * @brief   Battery data
*/
typedef struct _RobotBatteryInfo
{
    int iValue;     
} UBTEDU_ROBOT_Battery_T;

/**
 * @brief   Environment sensor data
*/
typedef struct _RobotColorDetect
{
    int iHue;      /**<  Hue value */
        int iSaturation; /**<  Saturation value */
        int iValue;      /**<  value */ 
} UBTEDU_COLOR_HSV_T;


/**
 * @brief      ubtGetSWVersion
 * @details    Get the robot versions including embedded system, raspberry,
             SDK and servos
 * @param[in]   eType
 *                                      Please see the defination UBTEDU_ROBOT_SOFTVERSION_TYPE_e
 * @param[out]  pcVersion
 *                                      The output buffer for versions.
 *                                      In most cases, the version length is no more than 20 bytes.
 * @param[in]   iVersionLen
 *                                      The max output buffer for versions length.

 * @retval     UBTEDU_RC_T
 */
 
UBTEDU_RC_T ubtGetSWVersion(UBTEDU_ROBOT_SOFTVERSION_TYPE_e eType, char *pcVersion, int iVersionLen);

/**
 * @brief      ubtGetRobotStatus
 * @details    Get the status of the robot
 * @param[in]   eType  Please see the defination for UBTEDU_ROBOT_STATUS_TYPE_e
 * @param[in]   piStatus
 *                      0 Sound status (Status:1 Mute,0 Turn on)
 *                      1 Playing music status (Status:1 Playing; 0 Pause)
 *                      2 Volume status (Status:0~ff)
 *                      3 Power status (Status:Voltage(mv),
 *                                              Recharge(0 No,1 Yes,2 No battery),
 *                                              Percent(Range 0~64)
 *                      4 Low voltage alert (Status:1 Low voltage 0 OK)

 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtGetRobotStatus(UBTEDU_ROBOT_STATUS_TYPE_e eType, void *piStatus);


/**
 * @brief      ubtCheckAPPStatus
 * @details    Get the status of the mobile phone
 * @param[in]   pcBuf    	static, 
 *							slant_forward, 
 *							slant_backward,
 *							slant_left, 
 *							slant_right,
 *							forward_and_back,
 *							swaying
 * @param[in]   iWaitTime  The mix value is 10s, the max value is 600s
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtCheckAPPStatus(char *pcBuf, int iWaitTime);

/**
 * @brief      ubtDetectVoiceMsg
 * @details    Detect a text message from robot voice recognition
 * @param[in]   pcBuf   The message to be detected
 * @param[in]   iTimeout  The max time for detecting
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtDetectVoiceMsg(char *pcBuf, int iTimeout);

/**
 * @brief:      ubtRecordMotion
 * @details:    Read all servo's angle 
 * @param[in]   UBTEDU_ROBOTSERVO_T *servoAngle
 * @param[out]  None
 * @retval: UBTEDU_RC_T
 */
UBTEDU_RC_T ubtRecordMotion(UBTEDU_ROBOTSERVO_T *servoAngle);

/**
 * @brief:      ubtGetRobotServo
 * @details:    Read one/multiple/all servo's angle
 * @param[in]   pstServoAngle	The angle for the servos, details please see UBTEDU_ROBOTSERVO_T
 *                  bit 0 indicates the first servo's angle.
 *                  FF means the invalid value.
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtGetRobotServo(UBTEDU_ROBOTSERVO_T *pstServoAngle);


/**
 * @brief:      ubtSetRobotServo
 * @details:    Set the servo's acAngle with speed
 * @param[in]   pstServoAngle   The angle for the servos, details please see UBTEDU_ROBOTSERVO_T
 * @param[in]   iTime       It is the time for servo, the value is smaller, the speed is faster.
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtSetRobotServo(UBTEDU_ROBOTSERVO_T *pstServoAngle, int iTime);

/**
 * @brief      ubtSetRobotVolume
 * @details    Set the volume for the Robot
 * @param[in]   iVolume  [0-100] Volume percent
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtSetRobotVolume(int iVolume);

/**
 * @brief      ubtSetRobotMotion
 * @details    Set the robot's action
 * @param[in]   pcType
 *                              crouch
 *                             raise
 *                             stretch
 *                             come on
 *                             wave
 *                             bend
 *                             walk
 *                             turn around
 *                             bow
 * @param[in]   pcDirect
 *                              left
 *                             right
 *                             both
 *                             front
 *                             back
 * @param[in]   iSpeed      1/2/3/4/5  The default value is 3
 * @param[in]   iRepeat     Repeat times. 0 means infinite
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtSetRobotMotion(char *pcType, char *pcDirect, int iSpeed, int iRepeat);


/**
 * @brief      ubtReadSensorValue
 * @details    Read the sensor's value
 * @param[in]   pcSensorType  The sensor's type.
 *                                  gyro
 *                                  environment
 *                                  board
 *                                  infrared
 *                                  ultrasonic
 *                                  touch
 *                                  color
 *                                  pressure
 *                                  gas
 * @param[out]  pValue        The sensor value. More details please see the defination as below variable type
 *                                  UBTEDU_ROBOTGYRO_SENSOR_T
 *                                  UBTEDU_ROBOTENV_SENSOR_T
 *                                  UBTEDU_ROBOTRASPBOARD_SENSOR_T
 *                                  UBTEDU_ROBOTINFRARED_SENSOR_T
 *                                  UBTEDU_ROBOTULTRASONIC_SENSOR_T
 *                                  UBTEDU_ROBOTTOUCH_SENSOR_T
 *                                  UBTEDU_ROBOTCOLOR_SENSOR_T
 *                                  UBTEDU_ROBOTPRESSURE_SENSOR_T
 *                                  UBTEDU_ROBOTGAS_SENSOR_T
 * @param[in]   iValueLen       The max length of pValue
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtReadSensorValue(char *pcSensorType, void *pValue, int iValueLen);

/**
 * @brief      ubtReadSensorValueByAddr
 * @details    Read the sensor's value by it's type and address
 * @param[in]   pcSensorType  The sensor's type.
 *                                  gryo
 *                                  environment
 *                                  board
 *                                  infrared
 *                                  ultrasonic
 *                                  touch
 *                                  color
 *                                  pressure
 *                                  gas
 * @param[in]   iAddr             The sensor's 7bit I2C address
 * @param[out]  pValue      The sensor value. More details please see the defination as below variable type
 *                                  UBTEDU_ROBOTGYRO_SENSOR_T
 *                                  UBTEDU_ROBOTENV_SENSOR_T
 *                                  UBTEDU_ROBOTRASPBOARD_SENSOR_T
 *                                  UBTEDU_ROBOTINFRARED_SENSOR_T
 *                                  UBTEDU_ROBOTULTRASONIC_SENSOR_T
 *                                  UBTEDU_ROBOTTOUCH_SENSOR_T
 *                                  UBTEDU_ROBOTCOLOR_SENSOR_T
 *                                  UBTEDU_ROBOTPRESSURE_SENSOR_T
 *                                  UBTEDU_ROBOTGAS_SENSOR_T
 * @param[in]   iValueLen       The max length of pValue
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtReadSensorValueByAddr(char *pcSensorType, int iAddr, void *pValue, int iValueLen);

/**
 * @brief      ubtSetRobotLED
 * @details    Set the LED mode
 * @param[in]   pcType
 *                              botton
 *                              camera
 *                              mic
 * @param[in]   pcColor
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
 * @param[in]   pcMode
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
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtSetRobotLED(char *pcType, char *pcColor, char *pcMode);

/**
 * @brief      ubtStartRobotAction
 * @details    Let the robot play an action
 * @param[in]   pcName  The action file's name For
 *                              example: push up, bow
 * @param[in]   iRepeat   Repeat times. 0 means infinite
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtStartRobotAction(char *pcName, int iRepeat);

/**
  * @brief      Stop to run the robot action file
  *
  * @return  UBTEDU_RC_T 0 Success,  Others    Failed
  *
  */
UBTEDU_RC_T ubtStopRobotAction(void);

/**
 * @brief   Start voice recognition
 *
 * @return  UBTEDU_RC_T 0 Success,  Others    Failed
 *
 */
UBTEDU_RC_T ubtVoiceStart();

/**
 * @brief   Stop voice recognition
 *
 * @return  UBTEDU_RC_T 0 Success,  Others    Failed
 *
 */
UBTEDU_RC_T ubtVoiceStop();

/**
 * @brief   Play the TTS voice
 *
 * @return  UBTEDU_RC_T 0 Success,  Others    Failed
 * @param   isInterrputed   Interrupt the previous TTS, if it is not finished.
 *                              0   Not interrupt the previous TTS
 *                              1   Interrupt the previous TTS, start the current TTS immediately
 * @param   pcTTS The message to be sent to TTS
 *
 */
UBTEDU_RC_T ubtVoiceTTS(int isInterrputed, char *pcTTS);

/**
 * @brief   Play a music
 *
 * @return  UBTEDU_RC_T 0 Success,  Others    Failed
 * @param   pcPlayMusicType  The directory of the music file in Robot
 * @param   pcName Music file name. Only support
 *              wav file without ".wav"
 *
 */
UBTEDU_RC_T ubtPlayMusic(char * pcPlayMusicType, char *pcName);

/**
 * @brief      ubtGetMusicList
 * @details    Get the music list from robot (Not supported yet)
 * @param[out]  *pacMusicName[]	     The music file name list.
 * @param[in]   iEachMusicNameLen   The max length for earch music file name.
 * @param[in]   iMusicNameNum       The max number of music file name for pcMusicName
 * @param[inout]   piIndex            The music file index, it can used when you trying to get all the music
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtGetMusicList(char *pacMusicName[], int iEachMusicNameLen,
                            int iMusicNameNum, int *piIndex);

/**
 * @brief	ubtEventDetect
 * @details	Detect robot's event include the push power button, voice detecting etc.
 * @param[in]	pcEventType	
 * @param[in]	iTimeout
 * @param[out]	pcValue
 * @retval	UBTEDU_RC_T
 */
UBTEDU_RC_T ubtEventDetect(char *pcEventType, char *pcValue, int iTimeout);


/**
 * @brief      ubtVisionDetect
 * @details    Detect Vision content include face gesture and object
 * @param[in]   pcVisionType
 * @param[in]   iTimeout
 * @param[out]  pcValue
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtVisionDetect(char *pcVisionType, char *pcValue, int iTimeout);

/**
 * @brief      ubtTakeAPhoto
 * @details    Take a photo. Not working yet
 * @param[in]   pacPhotoName
 * @param[in]   iPhotoNameLen
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtTakeAPhoto(char *pacPhotoName, int iPhotoNameLen);


/**
 * @brief      ubtTransmitCMD
 * @details    Send the commands to servo.
 * @param[in]   pcRemoteCmd         The content sent to servo
 * @param[in]   pcRemoteCmdRetData  The message received from servo
 * @param[in]   iRemoteCmdRetDataLen  The max length of pcRemoteCmdRetData
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtTransmitCMD(char *pcRemoteCmd, char *pcRemoteCmdRetData, int iRemoteCmdRetDataLen);

/**
 * @brief   Send the Blockly run status to mobile APP
 *
 * @return  UBTEDU_RC_T 0 Success,  Others    Failed
 * @param   pcName - [in] The name
 * @param   pcString - [in] The status
 *
 */
UBTEDU_RC_T ubtReportStatusToApp(char *pcName, char *pcString);


/**
 * @brief      ubtRobotDiscovery
 * @details    Search the robot in the local subnet.
 *              If there is more than one robot in the local subnet,
 *              please call this function multi times with iIsNeedSendRequest = 0
 * @param[in]   iIsNeedSendRequest            1: Send the search request, 0: Do not send search request
 * @param[in]   pcAccount                   The user account
 * @param[in]   pstRobotInfo  The robot infomation
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtRobotDiscovery(char *pcAccount, int iMaxTimes, UBTEDU_ROBOTINFO_T *pstRobotInfo);

/**
 * @brief      ubtRobotConnect
 * @details    Connect to Robot
 * @param[in]   pcAccount  The account which connected to robot
 * @param[in]   pcVersion  The SDK version
 * @param[in]   pcIPAddr   Robot IP address
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtRobotConnect(char *pcAccount, char *pcVersion, char *pcIPAddr);

/**
 * @brief      ubtRobotDisconnect
 * @details    Disconnect from the robot
 * @param[in]   pcAccount  The account which connected to robot
 * @param[in]   pcVersion  The SDK version
 * @param[in]   pcIPAddr   Robot IP address
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtRobotDisconnect(char *pcAccount, char *pcVersion, char *pcIPAddr);

/**
 * @brief       ubtOpenCameraSteam
 * @details     Open Yanshee's camera
 * @param[in]   pcMode  Video from local or remote
 * @param[in]   iPosX  Preview window start X Position
 * @param[in]   iPosY  Preview window start Y Position
 * @param[in]   iViewW  Preview window Width
 * @param[in]   iViewH  Preview window Height
 * @retval      UBTEDU_RC_T
 */
UBTEDU_RC_T ubtOpenCameraStream(char *pcMode,int iPosX,int iPosY,int iViewW,int iViewH);
/**
 * @brief       ubtCloseCameraSteam
 * @details     Close Yanshee's camera
 * @param[in]   NA
 * @retval      UBTEDU_RC_T
 */
UBTEDU_RC_T ubtCloseCameraSteam();

/**
 * @brief       ubtCaptureDLPhoto
 * @details     Capture a Photo When blockly preview windows opened
 * @param[in]   pcDataPath the dst path should copy to blockly
 * @retval      UBTEDU_RC_T
 */
UBTEDU_RC_T ubtCaptureDLPhoto(char* pcDataPath);

/**
 * @brief       ubtAddDLSample
 * @details     Add new sample for deep learning
 * @param[in]   iType   The type of deep learning.
 *              For example UBTEDU_DL_TYPE_CLASSIFY
 * @param[in]   pcTagName   The samples name
 * @param[in]   pcData      The sample data. It is a image in most cases.
 * @retval      UBTEDU_RC_T
 */
 UBTEDU_RC_T ubtAddDLSample(int iType, char* pcTagName, char* pcData);

/**
 * @brief       ubtDeleteDLSample
 * @details     Delete all the sample data based the tag name
 * @param[in]   iType   The type of deep learning.
 *              For example UBTEDU_DL_TYPE_CLASSIFY
 * @param[in]   pcTagName   The samples name
 * @retval      UBTEDU_RC_T
 */
UBTEDU_RC_T ubtDeleteDLSample(int iType, char* pcTagName);

/**
 * @brief       ubtResetDLSample
 * @details     Delete all the sample data based the deep learning type
 * @param[in]   iType   The type of deep learning.
 *              For example UBTEDU_DL_TYPE_CLASSIFY
 * @retval      UBTEDU_RC_T
 */
UBTEDU_RC_T ubtResetDLSample(int iType);

/**
 * @brief:      ubtFaceAgeGender
 * @details:    Find the face's gender and age. 
 * @param[in]   iTimeout the time take photo
 * @param[out]  pcGender  the gender for the face
 * @param[out]  pcAge  the age range for the face
 * @retval: UBTEDU_RC_T
 */
UBTEDU_RC_T ubtFaceAgeGender(int iTimeout, char* pcGender, char* pcAge);

/**
 * @brief:      ubtFaceCompare
 * @details:    Match a face. 
 * @param[in]   iTimeout the time take photo
 * @param[out]  pcValue  the name have be matched!
 * @retval:
 */
UBTEDU_RC_T ubtFaceCompare(int iTimeout, char* pcValue);

/**
 * @brief:      ubtFaceExpression
 * @details:    Get a face expression. 
 * @param[in]   iTimeout the time used to get
 * @param[out]  pcFaceExpressValue  the val have be fetched!
 * @retval:             UBTEDU_RC_T
 */
UBTEDU_RC_T ubtFaceExpression(int iTimeout, UBTEDU_FACEEXPRE_T * pcFaceExpressValue);

/**
 * @brief       ubtSearchExtendSensor
 * @details     Search all extend sensor include infrared ultrsonic touch environment press
 * @retval      UBTEDU_RC_T
 */
UBTEDU_RC_T ubtSearchExtendSensor(void);

/**
 * @brief       ubtModifyExtendSensorID
 * @details     Modify   Yanshee's extend sensor ID
 * @param[in]   pcType   Sensor type
 * @param[in]   iCurrID  Sensor ID
 * @param[in]   iDstID   modify id value 
 * @retval      UBTEDU_RC_T
 */
UBTEDU_RC_T ubtModifyExtendSensorID(char *pcType,int iCurrID,int iDstID);

/**
 * @brief       ubtDetectColorExsit
 * @details     Detect color is exsit
 * @param[in]   stMin   min value in HSV color space
 * @param[in]   stMax   max value HSV color space
 * @param[in]   iTimeout  the time to detect (sec.) 
 * @retval      UBTEDU_RC_T
 */
UBTEDU_RC_T ubtDetectColorExsit(UBTEDU_COLOR_HSV_T stMin, UBTEDU_COLOR_HSV_T stMax, int iTimeout);


/**
 * @brief      ubtRobotInitialize
 * @details    Init the SDK for 1x
 * @retval		UBTEDU_RC_T
 */
UBTEDU_RC_T ubtRobotInitialize();

/**
 * @brief      ubtRobotDeinitialize
 * @details    Destroy the SDK for 1x
 * @retval		UBTEDU_RC_T
 */
void ubtRobotDeinitialize();


#endif

