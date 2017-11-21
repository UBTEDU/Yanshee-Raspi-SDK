export platformUser :=`id -un`
export platformGroup :=`id -gn`

#CROSS_COMPILE = arm-linux-gnueabihf
CROSS_COMPILE = /opt/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-
export CXX = $(CROSS_COMPILE)g++
export CC = $(CROSS_COMPILE)gcc
export AS = $(CROSS_COMPILE)as
export AR = $(CROSS_COMPILE)ar
export NM = $(CROSS_COMPILE)nm
export LD = $(CROSS_COMPILE)ld
export OBJDUMP = $(CROSS_COMPILE)objdump
export OBJCOPY = $(CROSS_COMPILE)objcopy
export RANLIB = $(CROSS_COMPILE)ranlib
export STRIP = $(CROSS_COMPILE)strip



export SHELL=/bin/bash
export ROOT_DIR=$(shell pwd)

OUTPUT_DIR := $(ROOT_DIR)/output/
TARGET_DIR := $(OUTPUT_DIR)/mnt/1xrobot
ROBOTLIB_DIR := "libs"




