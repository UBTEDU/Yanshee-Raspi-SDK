#!/usr/bin/python
# _*_ coding: utf-8 -*-

import time
import RobotApi
import sys
#from ctypes import *

    

#------------------------------Connect-------------------------------------
RobotApi.ubtRobotInitialize()


robotname = "Yanshee_8F83"
gIPAddr = "127.0.0.1"


robotinfo = RobotApi.UBTEDU_ROBOTINFO_T()
ret = RobotApi.ubtRobotDiscovery(1,"SDK", robotinfo)

if 0 != ret:
    print("Can not Discover Robot! Error code: %d." % ret)
    exit(1)

if robotname == robotinfo.acName:
    timeout = 0
else:
    timeout = 20


#Repeat searching 20
    
while(timeout!=0):
    ret = RobotApi.ubtRobotDiscovery(0,"SDK", robotinfo)
    if ret != 0:
        print("Can not Discover Robot (timeout)! Error code: %d." % ret)
        exit(timeout)
    print("Robot Name: %s" % robotinfo.acName)
    print("Robot IP: %s" % robotinfo.acIPAddr)
    time.sleep(1)
    timeout = timeout - 1
    if robotinfo.acName == robotname:
        gIPAddr = robotinfo.acIPAddr
        break
print("gIPAddr = %s." % gIPAddr)

ret = RobotApi.ubtRobotConnect("sdk", "1" , gIPAddr)
if ret != 0:
    print("Can not connect to robot. Error code: %d" % ret)
    exit(2)





#---------------------------Read Sensor Value-------------------------------
#-----------------Only have infrared sensor on Yanshee_8F83-----------------
pcSensorType = "infrared"
infrared_sensor = RobotApi.UBTEDU_ROBOTINFRARED_SENSOR_T()
print("Sensor: INFRARED \t Value: %d" % infrared_sensor.iValue)
RobotApi.ubtReadSensorValue(pcSensorType,infrared_sensor,4)# Use ctypes size for sensor
if ret != 0:
    print("Can not read infrared sensor. Error code: %d" % ret)
    exit(3)
else:
    print("Sensor: INFRARED(Distance) \t Value: %d mm" % infrared_sensor.iValue)






#---------------------------Disconnect--------------------------------------
RobotApi.ubtRobotDisconnect("SDK","1",gIPAddr)
RobotApi.ubtRobotDeinitialize()
