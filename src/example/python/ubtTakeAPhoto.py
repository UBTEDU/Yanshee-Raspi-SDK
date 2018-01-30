#!/usr/bin/python
# _*_ coding: utf-8 -*-

import time
import RobotApi

#------------------------------Connect----------------------------------------
RobotApi.ubtRobotInitialize()

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

#---------------------------Take a photo ----------------------------
#take a photo and saved in /mnt/1xrobot/tmp
photoname= str("photosample")
namelen = len(photoname)
ret = RobotApi.ubtTakeAPhoto(photoname,namelen)
if ret != 0:
    print("Can not take a photo. Error Code: %d" % ret)
    exit(3)
print("take a photo and saved in /mnt/1xrobot/tmp")
#--------------------------DisConnect--------------------------------- 
RobotApi.ubtRobotDisconnect("SDK","1","127.0.0.1")
RobotApi.ubtRobotDeinitialize()




