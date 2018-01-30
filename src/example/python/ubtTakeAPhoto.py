#!/usr/bin/python
# _*_ coding: utf-8 -*-

import time
import RobotApi

#------------------------------Connect----------------------------------------
RobotApi.ubtRobotInitialize()

ret = RobotApi.ubtRobotConnect("sdk", "1" , "127.0.0.1")
if ret != 0:
    print("Can not connect to robot. Error code: %d" % ret)
    exit(2)

#---------------------------Take a photo ----------------------------
#ake a photo and saved in /mnt/1xrobot/tmp
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




