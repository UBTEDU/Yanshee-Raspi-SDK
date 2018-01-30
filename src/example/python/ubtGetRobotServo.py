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

#-----------------------------Get all servos' angle------------------------------


servoinfo = RobotApi.UBTEDU_ROBOTSERVO_T()

ret = RobotApi.ubtGetRobotServo(servoinfo)
print "servoinfo.SERVO1_ANGLE = %d" %(servoinfo.SERVO1_ANGLE)
print "servoinfo.SERVO2_ANGLE = %d" %(servoinfo.SERVO2_ANGLE)
print "servoinfo.SERVO3_ANGLE = %d" %(servoinfo.SERVO3_ANGLE)
print "servoinfo.SERVO17_ANGLE = %d" %(servoinfo.SERVO17_ANGLE)

#--------------------------DisConnect--------------------------------- 
RobotApi.ubtRobotDisconnect("SDK","1",gIPAddr)
RobotApi.ubtRobotDeinitialize()

