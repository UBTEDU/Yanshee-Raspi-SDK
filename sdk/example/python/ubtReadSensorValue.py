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

# Type ['gryo', 'environment', 'board', 'ultrasonic', 'infrared']

class GYROSENSOR(Structure):
	_fields_ = [
		("iValue", c_float*12)
	]
pcSensorType='gryo'
pValue=pointer(GYROSENSOR())
iValueLen=sizeof(GYROSENSOR)
ret = api.ubtReadSensorValue(pcSensorType, pValue, iValueLen)
if (0 != ret):
	print("Failed to call the SDK api. ret %d " % (ret))
print ("#############################")
for ii in (pValue[0].values):
    print (ii)


class ENVSENSOR(Structure):
	_fields_ = [
		("iValue", c_float*3)
	]
pcSensorType='environment'
pValue=pointer(ENVSENSOR())
iValueLen=sizeof(ENVSENSOR)
ret = api.ubtReadSensorValue(pcSensorType, pValue, iValueLen)
if (0 != ret):
	print("Failed to call the SDK api. ret %d " % (ret))
print ("#############################")
for ii in pValue[0].iValue:
    print ("Env's value: %f" % (ii))

class RASPBOARD(Structure):
	_fields_ = [
		("iValue", c_float)
	]
pcSensorType='board'
pValue=pointer(RASPBOARD())
iValueLen=sizeof(RASPBOARD)
ret = api.ubtReadSensorValue(pcSensorType, pValue, iValueLen)
if (0 != ret):
	print("Failed to call the SDK api. ret %d " % (ret))
print ("#############################")
print repr(pValue)
print ("Rasp Board's value: %f" % (pValue[0].iValue))


class TRASONICSENSOR(Structure):
	_fields_ = [
		("iValue", c_float)
	]
pcSensorType='ultrasonic'
pValue=pointer(TRASONICSENSOR())
iValueLen=sizeof(TRASONICSENSOR)
ret = api.ubtReadSensorValue(pcSensorType, pValue, iValueLen)
if (0 != ret):
	print("Failed to call the SDK api. ret %d " % (ret))
print ("#############################")
print ("Ultrasonic's value: %f" % (pValue[0].iValue))


class INFRAREDSENSOR(Structure):
	_fields_ = [
		("iValue", c_float)
	]

pcSensorType='infrared'
pValue=c_float()
iValueLen=sizeof(INFRAREDSENSOR)
ret = api.ubtReadSensorValue(pcSensorType, byref(pValue), iValueLen)
if (0 != ret):
	print("Failed to call the SDK api. ret %d " % (ret))
print ("#############################")
print ("infrared's value: %f" % (pValue))


api.ubtRobotDeinitialize()
