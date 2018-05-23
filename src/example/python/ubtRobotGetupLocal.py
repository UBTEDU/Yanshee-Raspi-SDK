#!/usr/bin/python
# _*_ coding: utf-8 -*-

import time
import RobotApi as api

api.ubtRobotInitialize()
#------------------------------Connect----------------------------------------
ret = api.ubtRobotConnect("SDK", "1", "127.0.0.1")
if (0 != ret):
        print ("Can not connect to robot %s" % robotinfo.acName)
        exit(1)
#---------------------------Read Sensor Value-------------------------------
gyro_size = 96
gyro_sensor = api.UBTEDU_ROBOTGYRO_SENSOR_T()
while True:
        time.sleep(2)
        ret = api.ubtReadSensorValue("gyro",gyro_sensor,gyro_size)
        if ret != 0:
            print("Can not read Sensor value. Error code: %d" % (ret))
	    continue
        else:
            print("Read dEulerxValue : %f" % (gyro_sensor.dEulerxValue))
            print("Read dEuleryValue : %f" % (gyro_sensor.dEuleryValue))
            print("Read dEulerzValue : %f" % (gyro_sensor.dEulerzValue))
			
	if gyro_sensor.dEulerxValue > 160 or gyro_sensor.dEulerxValue < -160:
		print 'getup from fall back'
		api.ubtStartRobotAction("getup_in_back", 1)
	elif gyro_sensor.dEulerxValue > -20 and gyro_sensor.dEulerxValue < 20:
		print 'geup from fall front'
		api.ubtStartRobotAction("getup_in_front", 1)

#---------------------------Disconnect--------------------------------------
api.ubtRobotDisconnect("SDK","1","127.0.0.1")
api.ubtRobotDeinitialize()

