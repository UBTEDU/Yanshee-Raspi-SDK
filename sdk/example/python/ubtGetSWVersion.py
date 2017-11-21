#!/usr/bin/python
# _*_ coding: utf-8 -*-
from ctypes import *

import time

ll = cdll.LoadLibrary
api=ll("/mnt/1xrobot/lib/librobot.so")
api.ubtRobotInitialize()
#--------------------------------------------
robotname="Alpha1X_8492"
class UBTEDU_ROBOTINFO_t(Structure):
	_fields_ = [
		("acName", c_char*32),
		("acIPAddr", c_char*16)
	]
robotinfo = pointer(UBTEDU_ROBOTINFO_t())
ret = api.ubtRobotDiscovery(1, "sdk", robotinfo)
if (0 != ret):
	print ("Return value: %d" % ret)
	exit(1)
if (robotinfo[0].acName == robotname):
	timeout = 0
else:
	timeout = 5
# Search the robot
while (0 != timeout):
	ret = api.ubtRobotDiscovery(0, "sdk", robotinfo)
	if (0 != ret):
		print ("Return value: %d" % ret)

	time.sleep(1)
	timeout -= 1

	print ("Name: %s" % (robotinfo[0].acName))
	print ("IP: %s" % (robotinfo[0].acIPAddr))
	if (robotinfo[0].acName == robotname):
		break
ret = api.ubtRobotConnect("sdk", "1", robotinfo[0].acIPAddr)
if (0 != ret):
	print ("Return value: %d" % ret)
	exit(1)

class VERAION(Structure):
	_fields_ = [
		("version", c_char*32)
	]	
ver=pointer(VERAION())
verlen=sizeof(ver)
api.ubtGetSWVersion(0, ver, verlen)
print ("#########################")
print("%s %s" % (ver[0].version, verlen))



api.ubtGetSWVersion(1, ver, verlen)
print ("#########################")
print("%s %s" % (ver[0].version, verlen))



api.ubtGetSWVersion(2, ver, verlen)
print ("#########################")
print("%s %s" % (ver[0].version, verlen))


api.ubtGetSWVersion(3, ver, verlen)
print ("#########################")
print("%s %s" % (ver[0].version, verlen))

api.ubtRobotDisconnect()
api.ubtRobotDeinitialize()
