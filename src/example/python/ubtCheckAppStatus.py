#!/usr/bin/python
# _*_ coding: utf-8 -*-

import time
import RobotApi

    
RobotApi.ubtRobotInitialize()
#------------------------------Connect-------------------------------------
gIPAddr = ""

robotinfo = RobotApi.UBTEDU_ROBOTINFO_T()
#The robot name you want to connect
robotinfo.acName="Yanshee_8F83"
ret = RobotApi.ubtRobotDiscovery("SDK", 15, robotinfo)
if (0 != ret):
	print ("Return value: %d" % ret)
	exit(1)

gIPAddr = robotinfo.acIPAddr
ret = RobotApi.ubtRobotConnect("SDK", "1", gIPAddr)
if (0 != ret):
	print ("Can not connect to robot %s" % robotinfo.acName)
	exit(1)

#-----------------------Cheak App Status-------------------------
pcBuf = ["static","slant_forward","slant_backward","slant_left","slant_right","swaying"]
iWaitTime = 100
#ret = RobotApi.ubtCheckAPPStatus(pcBuf,iWaitTime)
for buff in pcBuf:
    ret = RobotApi.ubtCheckAPPStatus(buff,iWaitTime)
    if ret != 0:
        print("Can not get App status. It is possible that the App is not connect to the robot. Error code: %d" % ret)
        exit(3)
    else:
        print("pcBuf: %s" % buff)

#---------------------------Disconnect--------------------------------------
RobotApi.ubtRobotDisconnect("SDK","1",gIPAddr)
RobotApi.ubtRobotDeinitialize()
