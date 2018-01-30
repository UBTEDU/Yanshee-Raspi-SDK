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

#-----------------------------TransmitCMD--------------------------------
pcRemoteCmd = str("This is a test")
pcRemoteCmdRetData = str("11111111111111111111111111111111111")
ret = RobotApi.ubtTransmitCMD(pcRemoteCmd,pcRemoteCmdRetData,len(pcRemoteCmdRetData))
if ret != 0:
    print("Can not send commond and receive commond. Error code: %d" % ret)
    exit(3)
print("Received commond: %s" % pcRemoteCmdRetData)


#---------------------------Disconnect--------------------------------------
RobotApi.ubtRobotDisconnect("SDK","1",gIPAddr)
RobotApi.ubtRobotDeinitialize()

