#!/usr/bin/python
# _*_ coding: utf-8 -*-

import RobotApi
import time

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

#----------------------- block program start ----------------------

pcEventType="button"
iTimeout = 15
pcValue = "0"

ret = RobotApi.ubtEventDetect(pcEventType,pcValue,iTimeout)
print "Example Button Detect pcValue = %s " %(pcValue)

#----------------------- block program end ----------------------

RobotApi.ubtRobotDisconnect("sdk", "1",gIPAddr)
RobotApi.ubtRobotDeinitialize()

