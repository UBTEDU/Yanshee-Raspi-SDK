#!/usr/bin/python
# _*_ coding: utf-8 -*-

import RobotApi
import time

RobotApi.ubtRobotInitialize()
#--------------------------------------------

#The robot name you want to connect
robotname="Yanshee_70C2"
gIPAddr = "127.0.0.1"

robotinfo = RobotApi.UBTEDU_ROBOTINFO_T()
ret = RobotApi.ubtRobotDiscovery(1, "sdk", robotinfo)
if (0 != ret):
	print ("Return value: %d" % ret)
	exit(1)
if (robotinfo.acName == robotname):
	timeout = 0
else:
	timeout = 255
# Search the robot
while (0 != timeout):
	ret = RobotApi.ubtRobotDiscovery(0, "sdk", robotinfo)
	if (0 != ret):
		print ("Return value: %d" % ret)
		break

	time.sleep(1)
	timeout -= 1

	print ("Name: %s" % (robotinfo.acName))
	print ("IP: %s" % (robotinfo.acIPAddr))
	if (robotinfo.acName == robotname):
		gIPAddr = robotinfo.acIPAddr

print "gIPAddr = %s" %(gIPAddr)
ret = RobotApi.ubtRobotConnect("sdk", "1", gIPAddr)
if (0 != ret):
	print ("Return value: %d" % ret)
	exit(1)

#----------------------- block program start ----------------------

ver = str("111111111111111111")
verlen=len(ver)

RobotApi.ubtGetSWVersion(31, ver, verlen)
print("#### version = %s  verlen = %s " % (ver, verlen))

#----------------------- block program end ----------------------

RobotApi.ubtRobotDisconnect("sdk", "1",gIPAddr)
RobotApi.ubtRobotDeinitialize()

