#!/usr/bin/python
# _*_ coding: utf-8 -*-

import RobotApi
import time

RobotApi.ubtRobotInitialize()
#--------------------------------------------
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

ver = str("111111111111111111")
verlen=len(ver)

RobotApi.ubtGetSWVersion(31, ver, verlen)
print("#### version = %s  verlen = %s " % (ver, verlen))

#----------------------- block program end ----------------------

RobotApi.ubtRobotDisconnect("SDK", "1",gIPAddr)
RobotApi.ubtRobotDeinitialize()

