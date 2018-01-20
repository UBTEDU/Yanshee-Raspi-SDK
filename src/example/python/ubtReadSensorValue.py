#!/usr/bin/python
# _*_ coding: utf-8 -*-

import RobotApi
import time

RobotApi.ubtRobotInitialize()
ret = RobotApi.ubtRobotConnect("sdk", "1", "127.0.0.1")
if (0 != ret):
	print ("Error --> ubtRobotConnect return value: %d" % ret)
	exit(1)
#--------------------------------------------------
ubtRobotEnv = RobotApi.UbtRobotTemperature()
ubtIRorUltrasonic = RobotApi.UbtRobotIRorUltrasonic()
ubtRobotColor = RobotApi.UbtRobotColor()

#----------------------- start ----------------------

ret = RobotApi.ubtReadSensorValue("environment", ubtRobotEnv, sizeof(UbtRobotTemperature))
if 0 == ret:
        print "temperature:",ubtRobotEnv[0].data[0], "℃"
        print "humidity:",ubtRobotEnv[0].data[1], "%RH"
        print "pressure:",ubtRobotEnv[0].data[2], "mb"
else:
        print "\033[1;31;40m Read environment sensor error:%d \033[0m" %(ret)

print "\n\033[1m[2]**********Get infrared sensor value**********\033[0m"
ret = RobotApi.ubtReadSensorValue("infrared", ubtIRorUltrasonic, sizeof(UbtRobotIRorUltrasonic))
if 0 == ret:
        print "distance:",ubtIRorUltrasonic[0].data,"mm"
else:
        print "\033[1;31;40m Read infrared sensor error:%d \033[0m" %ret
        
print "\n\033[1m[3]**********Get touch sensor value**********\033[0m"
ret = RobotApi.ubtReadSensorValue("touch", ubtIRorUltrasonic, sizeof(ubtIRorUltrasonic))
if 0 == ret:
        print "value:",ubtIRorUltrasonic[0].data
else:
        print "\033[1;31;40m Read touch sensor error:%d \033[0m" %ret
        

print "\n\033[1m[4]**********Get color sensor value**********\033[0m"
ret = RobotApi.ubtReadSensorValue("color", ubtRobotColor, sizeof(UbtRobotColor))
if 0 == ret:
        print "red:",ubtRobotColor[0].data[0]
        print "green:",ubtRobotColor[0].data[1]
        print "blue:",ubtRobotColor[0].data[2]
        print "clear:",ubtRobotColor[0].data[3]
else:
        print "\033[1;31;40m Read color sensor error:%d \033[0m" %ret

print "\n\033[1m[5]**********Get pressure sensor value**********\033[0m"
ret = RobotApi.ubtReadSensorValue("pressure", ubtIRorUltrasonic, sizeof(ubtIRorUltrasonic))
if 0 == ret:
        print "value:",ubtIRorUltrasonic[0].data,
else:
        print "\033[1;31;40m Read pressure sensor error:%d \033[0m" %ret

#----------------------- end ----------------------
RobotApi.ubtRobotDisconnect("sdk", "1", "127.0.0.1")
RobotApi.ubtRobotDeinitialize()

