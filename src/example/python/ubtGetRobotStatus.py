#!/usr/bin/python
# _*_ coding: utf-8 -*-


import time
import RobotApi

    

#------------------------------Connect-------------------------------------
RobotApi.ubtRobotInitialize()



robotinfo = RobotApi.UBTEDU_ROBOTINFO_T()
robotinfo.acName="Yanshee_1EFE"
ret = RobotApi.ubtRobotDiscovery("SDK", 15, robotinfo)
if (0 != ret):
	print ("Return value: %d" % ret)
	exit(1)

gIPAddr = robotinfo.acIPAddr
ret = RobotApi.ubtRobotConnect("SDK", "1" , gIPAddr)
if ret != 0:
    print("Can not connect to robot %s" % robotinfo.acName)
    exit(2)



#-----------------------------Getrobotstatus---------------------------------
#-----------------------------Volumeinformation------------------------------
status_info = RobotApi.UBTEDU_ROBOTINFRARED_SENSOR_T()
ret = RobotApi.ubtGetRobotStatus(2,status_info)
if ret != 0:
    print("Can not get volume information. Error code: %d" % ret)
    exit(4)
else:
    print("Value: %d" % status_info.iValue)

#---------------------------Disconnect--------------------------------------
RobotApi.ubtRobotDisconnect("SDK","1",gIPAddr)
RobotApi.ubtRobotDeinitialize()
