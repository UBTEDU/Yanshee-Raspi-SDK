#!/usr/bin/python
# _*_ coding: utf-8 -*-

import time
import RobotApi


RobotApi.ubtRobotInitialize()
#------------------------------Connect----------------------------------------
gIPAddr = ""

robotinfo = RobotApi.UBTEDU_ROBOTINFO_T()
#The robot name you want to connect
robotinfo.acName="Yanshee_05B8"
ret = RobotApi.ubtRobotDiscovery("SDK", 15, robotinfo)
if (0 != ret):
	print ("Return value: %d" % ret)
	exit(1)

gIPAddr = robotinfo.acIPAddr
ret = RobotApi.ubtRobotConnect("SDK", "1", gIPAddr)
if (0 != ret):
	print ("Can not connect to robot %s" % robotinfo.acName)
	exit(1)

#---------------------------Set Robot Motion-------------------------------
# Test bow , direction front, speed 3, repeat once
pcType = "bow"
pcDirect = "front"
iSpeed = 3
iRepeat = 1
ret = RobotApi.ubtSetRobotMotion(pcType,pcDirect,iSpeed,iRepeat)
if ret != 0:
    print("Can not set motion for robot! Error Code: %d" % ret)
    exit(3)


#---------------------------Disconnect--------------------------------------
RobotApi.ubtRobotDisconnect("SDK","1",gIPAddr)
RobotApi.ubtRobotDeinitialize()
