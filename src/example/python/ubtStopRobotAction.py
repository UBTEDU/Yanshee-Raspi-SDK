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

#---------------------------Dooneoftheaction-----------------------------------
#pcName = ['Forward','Hit left','Hit right','Left slide tackle','reset','Right']
iRepeat = 2
#for name in pcName:
pcName = "Hit left"
#ret = RobotApi.ubtStartRobotAction(pcName,iRepeat)
#if ret != 0:
 #   print("Can not start robot action! Error Code: %d" % ret)
  #  exit(3)
#time.sleep(1)
ret = RobotApi.ubtStopRobotAction()
if ret != 0:
    print("Can not stop robot action. Error code: %d" % ret)
    exit(3)
else:
    print("Stop success")
    #print("Current action: %s" % name)
    #time.sleep(2)


#---------------------------Disconnect--------------------------------------
RobotApi.ubtRobotDisconnect("SDK","1",gIPAddr)
RobotApi.ubtRobotDeinitialize()

