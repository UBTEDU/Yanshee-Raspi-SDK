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

#-----------------------------Get all servos' angle------------------------------


faceinfo = RobotApi.UBTEDU_FACEEXPRE_T()

ret = RobotApi.ubtFaceExpression(20,faceinfo)
if ret !=0:
	print "faceinfo error %d" %(ret)
else:
	print "face Happiness = %0.3f" %(faceinfo.fHappinessValue)
	print "face Surprise = %0.3f" %(faceinfo.fSurpriseValue)
	print "face Anger = %0.3f" %(faceinfo.fAngerValue)
	print "face Sadness = %0.3f" %(faceinfo.fSadnessValue)
	print "face Neutral = %0.3f" %(faceinfo.fNeutralValue)
	print "face Disgust = %0.3f" %(faceinfo.fDisgustValue)
	print "face Fear = %0.3f" %(faceinfo.fFearValue)

#--------------------------DisConnect--------------------------------- 
RobotApi.ubtRobotDisconnect("SDK","1",gIPAddr)
RobotApi.ubtRobotDeinitialize()

