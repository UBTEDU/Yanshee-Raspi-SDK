#!/usr/bin/python
# _*_ coding: utf-8 -*-

import time
import RobotApi



#------------------------------Connect-------------------------------------
RobotApi.ubtRobotInitialize()

#The robot name you want to connect
robotname="Yanshee_8F83"
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

	print ("Name: %s" % (robotinfo.acName))
	print ("IP: %s" % (robotinfo.acIPAddr))
	if (robotinfo.acName == robotname):
		gIPAddr = robotinfo.acIPAddr
		break
	time.sleep(1)
        timeout -= 1

print "gIPAddr = %s" %(gIPAddr)
ret = RobotApi.ubtRobotConnect("sdk", "1", gIPAddr)
if (0 != ret):
	print ("Return value: %d" % ret)
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
