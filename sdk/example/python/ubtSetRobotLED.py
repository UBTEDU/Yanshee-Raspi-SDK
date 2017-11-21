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

pcColor=['white', 'red', 'green', 'blue', 'yellow', 'purple', 'cyan']
pcMode=['off', 'on', 'blink', 'breath', 'alternation']
for color in pcColor:
    for mode in pcMode:
        print ("#############################")
        print ("ubtSetRobotLED color %s, mode %s" % (color, mode))
        ret = api.ubtSetRobotLED(color, mode)
        if (0 != ret):
	        print("Failed to call the SDK api. ret %d " % (ret))



api.ubtRobotDeinitialize()
