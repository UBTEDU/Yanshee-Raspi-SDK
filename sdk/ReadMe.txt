This file introduces how to use Yanshee robot SDK python script examples


step 1: Download, unzip and install the patch "Alpha1X_patch_1.1.2-0.tar.gz" in your Raspiberry system.


Type commond "tar -xzvf Alpha1X_patch_1.1.2-0.tar.gz" to unzip.
Type commond "sudo apt install *.deb" to install all files.


step 2: Download and unzip the correct SDK zip from website



step 3: Set up dynamic library path. Type "export LD_LIBRARY_PATH=(The path for the librobot.so):$LD_LIBRARY_PATH".



step 4: Go to the the folder you SDK folder and Type commond "make"



step 5: Go to /sdk/example/python inside your current folder.



step 6: Enjoy the python example. Type "python (example name).py".


PS: Make sure the python scripts and "RobotApi.c";"RobotApi.h";"RobotApi.i";"RobotApi.o";"RobotApi.py";"_RobotApi.so". 
are in same folder!  