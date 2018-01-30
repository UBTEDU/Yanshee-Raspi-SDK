#!/usr/bin/python
# _*_ coding: utf-8 -*-

import time
import RobotApi


RobotApi.ubtRobotInitialize()
#------------------------------Connect----------------------------------------
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


#--------------------------------VisionDetect-------------------------------
pcVisionType = str("face")
iTimeout = 30
pcValue = "0"
ret = RobotApi.ubtVisionDetect(pcVisionType,pcValue,iTimeout)
if ret != 0:
    print("Can not detect vision. Error code: %d" % ret)
    exit(3)
print(" find : %s face in camera" % pcValue[0])





#--------------------------DisConnect--------------------------------- 
RobotApi.ubtRobotDisconnect("SDK","1",gIPAddr)
RobotApi.ubtRobotDeinitialize()
