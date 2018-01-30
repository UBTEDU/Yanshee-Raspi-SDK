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

#----------------------------LED API------------------------------------
#The LED will test the button color and their modes
# pcColor = 'white','red','green','blue','yellow','purple','cyan'
# pcMode = 'off','on','blink','breath','alternation'

pcType = "button"
pcColor = ['white','red','green','blue','yellow','purple','cyan']
pcMode =  ['On','blink','breath','alternation','off']
for color in pcColor:
    for mode in pcMode:
        ret = RobotApi.ubtSetRobotLED(pcType,color,mode)
        time.sleep(1)
        if ret != 0:
            print("Can not set color for robot! Error code: %d" % ret)
            exit(3)


#--------------------------DisConnect---------------------------------            
RobotApi.ubtRobotDisconnect("SDK","1",gIPAddr)
RobotApi.ubtRobotDeinitialize()
