
#include "robotlogstr.h"

/* Message define */
char *pcStr_Msg_Cmd = "cmd";
char *pcStr_Msg_Account = "account";
char *pcStr_Msg_Name = "name";
char *pcStr_Msg_Type = "type";
char *pcStr_Msg_Port = "port";
char *pcStr_Msg_Data = "data";
char *pcStr_Msg_IP = "ip";
char *pcStr_Msg_Para = "para";
char *pcStr_Msg_Time_Out = "time_out";
char *pcStr_Msg_Repeat = "repeat";
char *pcStr_Msg_TotalTime = "total_time";
char *pcStr_Msg_Index = "index";
char *pcStr_Ret_Msg_Status = "status";
char *pcStr_Msg_Account_Voice = "voice";

char *pcStr_Msg_Cmd_Discovery = "discovery";
char *pcStr_Msg_Cmd_Discovery_Ack = "discovery_ack";
char *pcStr_Msg_Cmd_Connect = "connect";
char *pcStr_Msg_Cmd_Connect_Ack = "connect_ack";
char *pcStr_Msg_Cmd_Disconnect = "disconnect";
char *pcStr_Msg_Cmd_Disconnect_Ack = "disconnect_ack";
char *pcStr_Msg_Cmd_Heartbeat = "heartbeat";
char *pcStr_Msg_Cmd_Heartbeat_Ack = "heartbeat_ack";
char *pcStr_Msg_Cmd_Query = "query";
char *pcStr_Msg_Cmd_Query_Ack = "query_ack";
char *pcStr_Msg_Cmd_Action = "action";
char *pcStr_Msg_Cmd_Action_Ack = "action_ack";
char *pcStr_Msg_Cmd_Music = "music";
char *pcStr_Msg_Cmd_Music_Ack = "music_ack";
char *pcStr_Msg_Cmd_TakePhoto = "takephoto";
char *pcStr_Msg_Cmd_TakePhoto_Ack = "takephoto_ack";
char *pcStr_Msg_Cmd_Servo = "servo";
char *pcStr_Msg_Cmd_Servo_Ack = "servo_ack";
char *pcStr_Msg_Cmd_Servo_Offset = "servo_offset";
char *pcStr_Msg_Cmd_Servo_Offset_Ack = "servo_offset_ack";
char *pcStr_Msg_Cmd_Set = "set";
char *pcStr_Msg_Cmd_Set_Ack = "set_ack";
char *pcStr_Msg_Cmd_Monitor = "monitor";
char *pcStr_Msg_Cmd_Monitor_Ack = "monitor_ack";
char *pcStr_Msg_Cmd_Transmit = "transmit";
char *pcStr_Msg_Cmd_Transmit_Ack = "transmit_ack";
char *pcStr_Msg_Cmd_Transparent = "transparent";
char *pcStr_Msg_Cmd_Transparent_Ack = "transparent_ack";
char *pcStr_Msg_Cmd_Query_App = "query_app";
char *pcStr_Msg_Cmd_Query_App_Ack = "query_app_ack";
char *pcStr_Msg_Cmd_Voice = "voice";
char *pcStr_Msg_Cmd_Voice_Ack = "voice_ack";
char *pcStr_Msg_Cmd_Voice_Switch2CN = "voice_2cn";
char *pcStr_Msg_Cmd_Voice_Switch2EN = "voice_2en";
char *pcStr_Msg_Cmd_Upgrade = "upgrade";
char *pcStr_Msg_Cmd_Upgrade_Ack = "upgrade_ack";
char *pcStr_Msg_Cmd_Report = "report_action";
char *pcStr_Msg_Cmd_Report_Ack = "report_action_ack";
char *pcStr_Msg_Cmd_Vision = "vision";
char *pcStr_Msg_Cmd_Vision_Ack = "vision_ack";
char *pcStr_Msg_Cmd_Key = "key";
char *pcStr_Msg_Cmd_Key_Ack = "key_ack";
char *pcStr_Msg_Cmd_Poweroff = "power_off";
char *pcStr_Msg_Cmd_ProcessInfo = "process_info";
char *pcStr_Msg_Cmd_Setloglevel = "set_log_level";
char *pcStr_Msg_Cmd_Sensor_Config = "sensor_config";
char *pcStr_Msg_Cmd_Sensor_Config_Ack = "sensor_config_ack";
char *pcStr_Msg_Cmd_Swarm = "swarm";

char *pcStr_Msg_Type_Version = "version";
char *pcStr_Msg_Type_Play = "play";
char *pcStr_Msg_Type_Battery = "battery";
char *pcStr_Msg_Type_Battery_Voltage = "voltage";
char *pcStr_Msg_Type_Battery_Recharging = "recharging";
char *pcStr_Msg_Type_Battery_Percent = "percent";
char *pcStr_Msg_Type_Battery_LowAlert = "lowalert";
char *pcStr_Msg_Type_Volume = "volume";
char *pcStr_Msg_Type_Volume_Plus = "volume_plus";
char *pcStr_Msg_Type_Volume_Minus = "volume_minus";
char *pcStr_Msg_Type_Servo_Write = "write";
char *pcStr_Msg_Type_Servo_Read = "read";
char *pcStr_Msg_Type_Servo_Read_Hold = "read_hold";
char *pcStr_Msg_Type_Servo_Active = "active";
char *pcStr_Msg_Type_Servo_ID = "id";
char *pcStr_Msg_Type_Servo_Angle = "angle";
char *pcStr_Msg_Type_Servo_Time = "time";
char *pcStr_Msg_Type_Sensor = "sensor";
char *pcStr_Msg_Type_File = "file";
char *pcStr_Msg_Type_Blockly = "blockly";
char *pcStr_Msg_Type_Blockly_Start = "blockly_start";
char *pcStr_Msg_Type_Blockly_Stop = "blockly_stop";
char *pcStr_Msg_Type_Blockly_Report = "report_blockly";

char *pcStr_Msg_Type_Vision = "vision";
char *pcStr_Msg_Type_Vision_Face = "face";
char *pcStr_Msg_Type_Vision_Hand = "hand";
char *pcStr_Msg_Type_Monitor_Start =  "start";
char *pcStr_Msg_Type_Monitor_Stop =   "stop";
char *pcStr_Msg_Type_Channel = "channel";
char *pcStr_Msg_Type_Detect_Fall  = "detect_fall";
char *pcStr_Msg_Type_MAC = "mac";
char *pcStr_Msg_Type_Key = "key";

char *pcStr_Msg_Type_Voice_TTS = "tts";
char *pcStr_Msg_Type_Recognition_Button = "button";
char *pcStr_Msg_Type_Recognition_Start = "recogntion_start";
char *pcStr_Msg_Type_Recognition_Stop = "recogntion_stop";
char *pcStr_Msg_Type_Recognition_Event_Online = "online";
char *pcStr_Msg_Type_Recognition_Event_Offline = "offline";
char *pcStr_Msg_Type_Recognition_Event_Timeout = "timeout";
char *pcStr_Msg_Type_Recognition_Event_Continus = "continus";


char *pcStr_Msg_Type_Music_Play = "play";
char *pcStr_Msg_Type_Music_Pause = "Pause";
char *pcStr_Msg_Type_Music_Stop = "stop";
char *pcStr_Msg_Type_Music_GetList = "getlist";

char *pcStr_Msg_Type_TakePhoto_Start = "start";
char *pcStr_Msg_Type_TakePhoto_GetList = "getlist";

char *pcStr_Msg_Type_Voice_Detecting = "voice_detecting";
char *pcStr_Msg_Type_Voice_Detected = "voice_detected";
char *pcStr_Msg_Type_Voice_NotDetected = "voice_notdetected";
char *pcStr_Msg_Type_Voice_TTS_IsInterrupted = "is_interrupted";

char *pcStr_Msg_Type_Action_Start = "start";
char *pcStr_Msg_Type_Action_Stop = "stop";
char *pcStr_Msg_Type_Action_Pause =   "pause";
char *pcStr_Msg_Type_Action_Continue = "continue";
char *pcStr_Msg_Type_Action_Reset = "reset";

char *pcStr_Msg_Type_Upgrade_Raspberry =   "raspberry";
char *pcStr_Msg_Type_Upgrade_STM = "stm32";
char *pcStr_Msg_Type_Upgrade_STM_BOOT = "stm32_boot";
char *pcStr_Msg_Type_Upgrade_Servo = "servo";

char *pcStr_Msg_Type_LED = "led";
char *pcStr_Msg_Type_SN = "sn";

char *pcStr_Msg_Type_CALIBRATE = "calibrate";
char *pcStr_Msg_Type_SEARCH = "search";
char *pcStr_Msg_Type_MODIFY = "modify";

char *pcStr_Msg_Type_SwarmInit = "init";
char *pcStr_Msg_Type_SwarmInitAck = "init_ack";
char *pcStr_Msg_Type_SwarmAllocID = "alloc_id";
char *pcStr_Msg_Type_SwarmAllocIDAck = "alloc_id_ack";
char *pcStr_Msg_Type_SwarmActionStart = "action_start";
char *pcStr_Msg_Type_SwarmActionStartAck = "action_start_ack";
char *pcStr_Msg_Type_SwarmActionEnd = "action_end";
char *pcStr_Msg_Type_SwarmActionEndAck = "action_end_ack";
char *pcStr_Msg_Type_SwarmActionCancel = "action_cancel";
char *pcStr_Msg_Type_SwarmActionCancelAck = "action_cancel_ack";
char *pcStr_Msg_Type_SwarmActionGotoXYZ = "goto_xyz";
char *pcStr_Msg_Type_SwarmActionGotoXYZAck = "goto_xyz_ack";
char *pcStr_Msg_Type_SwarmActionReachXYZ = "reach_xyz";
char *pcStr_Msg_Type_SwarmActionReachXYZAck = "reach_xyz_ack";
char *pcStr_Msg_Type_SwarmActionQueryXYZ = "query_xyz";
char *pcStr_Msg_Type_SwarmActionQueryXYZAck = "query_xyz_ack";


char *pcStr_Msg_Param_AppStatus_Static = "static";
char *pcStr_Msg_Param_AppStatus_Slant_Right = "slant_right";
char *pcStr_Msg_Param_AppStatus_Slant_Left = "slant_left";
char *pcStr_Msg_Param_AppStatus_Slant_Forward = "slant_forward";
char *pcStr_Msg_Param_AppStatus_Slant_Backward = "slant_backward";
char *pcStr_Msg_Param_AppStatus_ForwardAndBack = "forward_and_back";
char *pcStr_Msg_Param_AppStatus_Swaying = "swaying";

char *pcStr_Msg_Param_Vision_FaceTracking_Start = "facetrackingstart";
char *pcStr_Msg_Param_Vision_FaceTracking_Stop = "facetrackingstop";
char *pcStr_Msg_Param_Vision_FaceAnalyse = "faceanalyse";
char *pcStr_Msg_Param_Vision_GestureAnalyse = "gestureanalyse";
char *pcStr_Msg_Param_Vision_ObjectRecognition = "objectrecognition";


char *pcStr_Msg_Param_Query_Action_List = "list";
char *pcStr_Msg_Param_Query_Action_Status = "actionstatus";

char *pcStr_Msg_Param_Query_Version_Servos = "servo";
char *pcStr_Msg_Param_Query_Version_RaspPi = "raspberry";
char *pcStr_Msg_Param_Query_Version_STM32 = "stm32";
char *pcStr_Msg_Param_Query_Version_SN = "sn";
char *pcStr_Msg_Param_Query_Version_ALL = "all";


char *pcStr_Msg_Param_Query_Play_Playing = "playing";
char *pcStr_Msg_Param_Query_Play_Pause = "pause";
char *pcStr_Msg_Param_Query_Play_End = "end";
char *pcStr_Msg_Param_Query_Play_idle = "idle";

char *pcStr_Msg_Param_Query_Power_Voltage = "voltage";
char *pcStr_Msg_Param_Query_Power_Charging = "charging";
char *pcStr_Msg_Param_Query_Power_Percent = "percent";

char *pcStr_Msg_Param_Query_Sensor_GYRO = "gyro";
char *pcStr_Msg_Param_Query_Sensor_GYRO_X = "gyro-x";
char *pcStr_Msg_Param_Query_Sensor_GYRO_Y = "gyro-y";
char *pcStr_Msg_Param_Query_Sensor_GYRO_Z = "gyro-z";
char *pcStr_Msg_Param_Query_Sensor_ACCEL_X = "accel-x";
char *pcStr_Msg_Param_Query_Sensor_ACCEL_Y = "accel-y";
char *pcStr_Msg_Param_Query_Sensor_ACCEL_Z = "accel-z";
char *pcStr_Msg_Param_Query_Sensor_COMPASS_X = "compass-x";
char *pcStr_Msg_Param_Query_Sensor_COMPASS_Y = "compass-y";
char *pcStr_Msg_Param_Query_Sensor_COMPASS_Z = "compass-z";
char *pcStr_Msg_Param_Query_Sensor_EULER_X = "euler-x";
char *pcStr_Msg_Param_Query_Sensor_EULER_Y = "euler-y";
char *pcStr_Msg_Param_Query_Sensor_EULER_Z = "euler-z";

char *pcStr_Msg_Param_Query_Sensor_ENV = "environment";
char *pcStr_Msg_Param_Query_Sensor_ENV_Temperature = "temperature";
char *pcStr_Msg_Param_Query_Sensor_ENV_Humidity = "humidity";
char *pcStr_Msg_Param_Query_Sensor_ENV_Pressure = "pressure";

char *pcStr_Msg_Param_Query_Sensor_Board = "board";
char *pcStr_Msg_Param_Query_Sensor_Board_Temperature = "temperature";

char *pcStr_Msg_Param_Query_Sensor_ID  = "id";
char *pcStr_Msg_Param_Query_Sensor_Calibration  = "calibration";
char *pcStr_Msg_Param_Query_Sensor_Version = "version";
char *pcStr_Msg_Param_Query_Sensor_Value  = "value";

char *pcStr_Msg_Param_Query_Sensor_Ultrasonic = "ultrasonic";
char *pcStr_Msg_Param_Query_Sensor_Ultrasonic_Distance = "distance";

char *pcStr_Msg_Param_Query_Sensor_Infrared = "infrared";
char *pcStr_Msg_Param_Query_Sensor_Infrared_Distance = "distance";

char *pcStr_Msg_Param_Query_Sensor_Touch = "touch";
char *pcStr_Msg_Param_Query_Sensor_Color = "color";
char *pcStr_Msg_Param_Query_Sensor_Pressure = "pressure";
char *pcStr_Msg_Param_Query_Sensor_Gas = "gas";

char *pcStr_Msg_Param_Led_Type_Servo = "servo";
char *pcStr_Msg_Param_Led_Type_Button = "button";
char *pcStr_Msg_Param_Led_Type_Camera = "camera";
char *pcStr_Msg_Param_Led_Type_Mic = "mic";

char *pcStr_Msg_Param_Led_Mode = "mode";
char *pcStr_Msg_Param_Led_Mode_On = "on";
char *pcStr_Msg_Param_Led_Mode_Off = "off";
char *pcStr_Msg_Param_Led_Mode_Blink = "blink";
char *pcStr_Msg_Param_Led_Mode_Breath = "breath";
char *pcStr_Msg_Param_Led_Mode_Colorful = "colorful";

char *pcStr_Msg_Param_Led_Color = "color";
char *pcStr_Msg_Param_Led_Color_White = "white";
char *pcStr_Msg_Param_Led_Color_Red = "red";
char *pcStr_Msg_Param_Led_Color_Green = "green";
char *pcStr_Msg_Param_Led_Color_Blue = "blue";
char *pcStr_Msg_Param_Led_Color_Yellow = "yellow";
char *pcStr_Msg_Param_Led_Color_Purple = "purple";
char *pcStr_Msg_Param_Led_Color_Cyan = "cyan";

char *pcStr_Msg_Param_Compass_dx = "compass-dx";
char *pcStr_Msg_Param_Compass_dy = "compass-dy";
char *pcStr_Msg_Param_Compass_dz = "compass-dz";

char *pcStr_Msg_Param_Music_Name = "name";

char *pcStr_Msg_Param_Swarm_SEQ = "sequence";
char *pcStr_Msg_Param_Swarm_Timezone = "timezone";
char *pcStr_Msg_Param_Swarm_ID = "id";
char *pcStr_Msg_Param_Swarm_RobotList = "robotlist";
char *pcStr_Msg_Param_Swarm_MAC = "mac";
char *pcStr_Msg_Param_Swarm_DelayTime = "delaytime";
char *pcStr_Msg_Param_Swarm_ActionName = "actionname";
char *pcStr_Msg_Param_Swarm_GyroOffset = "gyroffset";
char *pcStr_Msg_Param_Swarm_Order = "order";
char *pcStr_Msg_Param_Swarm_X = "x";
char *pcStr_Msg_Param_Swarm_Y = "y";
char *pcStr_Msg_Param_Swarm_Z = "z";
char *pcStr_Msg_Param_Swarm_Status_Done = "done";
char *pcStr_Msg_Param_Swarm_Status_Running = "running";


char *pcStr_RobotMgmt_Name = "robotmgmt";
char *pcStr_RobotAgent_Name = "robotagent";
char *pcStr_RobotVoiceCN_Name = "robotvoice_cn";
char *pcStr_RobotVoiceEN_Name = "robotvoice_en";
char *pcStr_RobotVision_Name = "robotvision";

/***************************************************************************/
/* Json string */
char *pcStr_Json_ParseFailed = "Parse JSON failed!";

