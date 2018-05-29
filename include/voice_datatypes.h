      
/** @file 	
  * @note 	UBT Education Co., Ltd. All Right Reserved.
  * @brief    
  * 
  * @author   Cygnus Yang
  * @date     2017-8-10
  * @version  1.0.0.0
  * 
  * @note ///Description here 
  * @note History:    
  * @note     <author>   <time>    <version >   <desc>
  * @note  
  * @warning  
  */


#ifndef __VOICE_TYPES_H__
#define __VOICE_TYPES_H__



/** 
 * @brief    Return value for SDK
 */


typedef enum
{
    UBTEDU_EVENT_RECOGNITION_START,
    UBTEDU_EVENT_RECOGNITION_STOP,
    UBTEDU_EVENT_RECOGNITION_ONLINE,
    UBTEDU_EVENT_RECOGNITION_OFFLINE,
    UBTEDU_EVENT_RECOGNITION_MAXTIMEOUT,
    UBTEDU_EVENT_RECOGNITION_CONTINUS,    
    UBTEDU_EVENT_RECOGNITION_LAST,
    UBTEDU_EVENT_RECOGNITION_TIMER_CONTINUS_RESTART = 50,
    UBTEDU_EVENT_ACTION_PLAY = 100,
    UBTEDU_EVENT_VOICE_DETECTING = 200,  /* Event indicate that the blockly want to detect a message */
    UBTEDU_EVENT_VOICE_DETECTED,
    UBTEDU_EVENT_VOICE_NOTDETECTED,
    UBTEDU_EVENT_TTS_PLAY = 300,
    UBTEDU_EVENT_VISION = 400,
    UBTEDU_EVENT_LOCALCOMMANDS_VOLUME_PLUS = 500,
    UBTEDU_EVENT_LOCALCOMMANDS_VOLUME_MINUS,
    UBTEDU_EVENT_LOCALCOMMANDS_CUSTOMER1,
    UBTEDU_EVENT_LOCALCOMMANDS_CUSTOMER2,
    UBTEDU_EVENT_LOCALCOMMANDS_CUSTOMER3,
    UBTEDU_EVENT_LOCALCOMMANDS_CUSTOMER4,
    UBTEDU_EVENT_LOCALCOMMANDS_CUSTOMER5,
    UBTEDU_EVENT_LOCALCOMMANDS_CUSTOMER6,
    UBTEDU_EVENT_LOCALCOMMANDS_CUSTOMER7,
    UBTEDU_EVENT_LOCALCOMMANDS_CUSTOMER8,
    UBTEDU_EVENT_LOCALCOMMANDS_CUSTOMER9,
    UBTEDU_EVENT_UPGRADE = 600,
    UBTEDU_EVENT_INVLIAD,
    UBTEDU_EVENT_LAST
} UBTEDU_VOICE_EVENT_E;


/* SDK software version */
#define UBTEDU_SDK_SW_VER              "01"


/* Define the MAX length of the robot name and IP address */
#define UBTEDU_ROBOT_NAME_LEN		(32)
#define UBTEDU_ROBOT_IP_ADDR_LEN		(16)

/**
 * @brief	Robot status type
*/
typedef enum
{
	UBTEDU_ROBOT_STATUS_TYPE_PLAYACTION = 1,	/**< Play an action file */
	UBTEDU_ROBOT_STATUS_TYPE_VOLUME,			/**< Volume status */
	UBTEDU_ROBOT_STATUS_TYPE_POWER_VOLTAGE,		/**< Power voltage status */
	UBTEDU_ROBOT_STATUS_TYPE_POWER_RECHARGE,	/**< Power recharge status */
	UBTEDU_ROBOT_STATUS_TYPE_POWER_PERCENT,		/**< Power percent status */
	UBTEDU_ROBOT_STATUS_TYPE_INVALID			/**< Invalid type */
} UBTEDU_ROBOT_STATUS_TYPE_e;

/**
 * @brief	Play music status
*/
typedef enum
{
	UBTEDU_ROBOT_PLAY_STATUS_IDLE,			/**< Idle status */
	UBTEDU_ROBOT_PLAY_STATUS_PLAYING,		/**< Playing */
	UBTEDU_ROBOT_PLAY_STATUS_PAUSED,		/**< Paused */
	UBTEDU_ROBOT_PLAYSTATUS_END,			/**< End */
	UBTEDU_ROBOT_PLAY_STATUS_INVALID		/**< Invalid status */
} UBTEDU_ROBOT_PLAYMUSIC_STATUS_e;

/**
 * @brief	Robot software version type
*/
typedef enum
{
	UBTEDU_ROBOT_SOFTVERSION_TYPE_STM32 = 0,	/**< Embedded system version */
	UBTEDU_ROBOT_SOFTVERSION_TYPE_SERVOS1,		/**< Servos' version */
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
	UBTEDU_ROBOT_SOFTVERSION_TYPE_SDK = 30,			/**< SDK version */
	UBTEDU_ROBOT_SOFTVERSION_TYPE_RASPI = 31,		/**< Robot management application version */
	UBTEDU_ROBOT_SOFTVERSION_TYPE_INVALID		/**< Invalid type */
} UBTEDU_ROBOT_SOFTVERSION_TYPE_e;

/** 
 * @brief    Return value for SDK
 */
typedef enum
{
  UBTEDU_RC_SUCCESS = 0,	/**< Success */
  UBTEDU_RC_FAILED,			/**< Failed */
  UBTEDU_RC_NORESOURCE,		/**< No resource */
  UBTEDU_RC_NOT_FOUND,		/**< Not found */
  UBTEDU_RC_WRONG_PARAM,	/**< Wrong parameter */

  UBTEDU_RC_IGNORE, 		  /**< Ignore this return value */

  UBTEDU_RC_SOCKET_FAILED = 100,	/**< Socket error */
  UBTEDU_RC_SOCKET_NORESOURCE,		/**< No resource when sending message out */
  UBTEDU_RC_SOCKET_TIMEOUT,			/**< Recevied message timeout */
  UBTEDU_RC_SOCKET_ENCODE_FAILED,	/**< Encode the message failed */
  UBTEDU_RC_SOCKET_DECODE_FAILED,	/**< Decode the message failed */
  UBTEDU_RC_SOCKET_ENCODE_ERROR,	/**< Encode the message error */
  UBTEDU_RC_SOCKET_DECODE_ERROR,	/**< Decode the message error. It is possible that received a wrong message */
  UBTEDU_RC_SOCKET_SENDERROR,   	/**< Error when sending message out */

  UBTEDU_RC_VOICE_FAILED,			/**< Voice recognition failed */
  UBTEDU_RC_VOICE_GRAMMAR_ERROR,	/**< Voiice recognition grammer error */
  UBTEDU_RC_VOICE_AIUIDECODE_ERROR, /**< Decode AIUI message failed */
  UBTEDU_RC_LAST					/**< The last return value */
} UBTEDU_RC_T;

/**
 * @brief	Robot infomation
*/
typedef struct _RobotInfo
{
	char acName[UBTEDU_ROBOT_NAME_LEN];
	char acIPAddr[UBTEDU_ROBOT_IP_ADDR_LEN];
} UBTEDU_ROBOTINFO_T;

/**
 * @brief	Gyro sensor data
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
 * @brief	Environment sensor data
*/
typedef struct _RobotEnvSensor
{

    int iTempValue;      /**<  temperature value */
    int iHumiValue;      /**<  humidity value */
    int iPresValue;      /**<  pressure value */
	
} UBTEDU_ROBOTENV_SENSOR_T;

/**
 * @brief	Raspberry Pi board PCB data
*/
typedef struct _RobotRaspPiBoardSensor
{
	int iValue;			/**<	Board temperature */
} UBTEDU_ROBOTRASPBOARD_SENSOR_T;

/**
 * @brief	Ultrasonic sensor data
*/
typedef struct _RobotUltrasonicSensor
{
	int iValue;			/**<	The distance via ultrasonic sensor */
} UBTEDU_ROBOTULTRASONIC_SENSOR_T;

/**
 * @brief	Infrared sensor data
*/
typedef struct _RobotInfraredSensor
{
	int iValue;			/**<	The distance via infrared sensor */
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
	int iValue;		/**<	RobotBatteryInfo */
} UBTEDU_ROBOT_Battery_T;


#endif /* __VOICE_TYPES_H__ */
