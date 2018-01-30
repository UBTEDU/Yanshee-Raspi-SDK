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

#------------------------ReadSensorValueByAddr---------------------------------
#Have 16, 17, 18 on Yanshee_8F83
infrared_sensor = RobotApi.UBTEDU_ROBOTINFRARED_SENSOR_T()
iAddr = [16,17,18]
for Addr in iAddr:
    ret = RobotApi.ubtReadSensorValueByAddr("infrared",Addr,infrared_sensor,4)#Use ctypes size
    if ret != 0:
        print("Can not read Addr: %d value. Error code: %d" % (Addr,ret))
        exit(3)
    else:
        print("Addr: %d \t Value: %d" % (Addr,infrared_sensor.iValue))





#---------------------------Disconnect--------------------------------------
RobotApi.ubtRobotDisconnect("SDK","1",gIPAddr)
RobotApi.ubtRobotDeinitialize()
