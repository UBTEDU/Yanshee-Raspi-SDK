# yanshee-raspi-sdk


### Overiew of the Yanshee-raspi-sdk(Yanshee-sdk)

The Yanshee-sdk provides arm-linux based c and python library that allows developers to control yanshee robot


### Hardware
 
1. Yanshee robot(Ex. Raspberry Pi 3 Main board)
2. Monitor
3. Output cable (Ex. HDMI-DVI/HDMI-VGA/HDMI-HDMI)
4. USB cable/wireless(Recommond)  mouse
5. Stick sensors(Ex. infrared sensor) 


### Software

1. Ex. noobs developing system
[Go to official website] (https://www.raspberrypi.org/downloads/noobs/) 


### Get started


1. Prepare your workspace and download the SDK

```bash
git clone https://github.com/UBTEDU/yanshee-raspi-sdk.git
```


2. Compile the files

```bash
# cd (The path you put your sdk)/yanshee-raspi-sdk"
# make
```

Ex(Raspberrypi system).<br> 

Commond 1: "cd /home/pi/yanshee-raspi-sdk"<br>
Commond 2: "make"<br>


3. Set up dynamic library path 

Commond: "export LD_LIBRARY_PATH=(The path for the librobot.so;librobotlogstr.so;librobotutils.so;libjson.so):$LD_LIBRARY_PATH"

Ex (Raspberrypi system). "export LD_LIBRARY_PATH=/home/pi/yanshee-raspi-sdk/libs/sdklib:$LD_LIBRARY_PATH"


4. Get the example 

Find the demonstration python scripts in folder "(The path you put your sdk)/yanshee-raspi-sdk/sdk/example/python"

Ex (Raspberrypi system). "/home/pi/sdk3/yanshee-raspi-sdk/sdk/example/python"


5. Execute and learn the example 

Commond: "python (example name).py".

PS: MAKE SURE THE PYTHON SCRIPTS AND "RobotApi.py";"_RobotApi.so". 
ARE IN SAME FOLDER! 


6. Build your own project
