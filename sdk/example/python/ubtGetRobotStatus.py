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

#robotstatus=pointer(c_int(0))
robotstatus=c_int()
ret = api.ubtGetRobotStatus(1, byref(robotstatus))
if (0 != ret):
	print("Failed to call the SDK api. ret %d " % (ret))
print("Robot status for action file %d" % (robotstatus.value))

api.ubtGetRobotStatus(2, byref(robotstatus))
if (0 != ret):
	print("Failed to call the SDK api. ret %d " % (ret))
print("Robot status for volume %d" % (robotstatus.value))

api.ubtGetRobotStatus(3, byref(robotstatus))
if (0 != ret):
	print("Failed to call the SDK api. ret %d " % (ret))
print("Robot status for Power voltage %d" % (robotstatus.value))

api.ubtGetRobotStatus(4, byref(robotstatus))
if (0 != ret):
	print("Failed to call the SDK api. ret %d " % (ret))
print("Robot status for Power recharge %d" % (robotstatus.value))

api.ubtGetRobotStatus(5, byref(robotstatus))
if (0 != ret):
	print("Failed to call the SDK api. ret %d " % (ret))
print("Robot status for Power percent %d" % (robotstatus.value))

api.ubtRobotDeinitialize()
