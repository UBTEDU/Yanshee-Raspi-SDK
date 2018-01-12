# --------------------------------------------------------------------------
#
#	FILE NAME:		Makefile
#	AUTHOR:			UBTEDU
#	DESCRIPTION:	Voice recognition
#
# --------------------------------------------------------------------------
#common makefile header

SUBDIRS=python

.PHONY:prebless
all: swig $(SUBDIRS) 

swig:
	@cp -rf ../sdk/RobotApi.c python/
	swig -python python/RobotApi.i

$(SUBDIRS):ECHO
	make -C $@
	
ECHO:
	@echo "###################"$(SUBDIRS)"###################"


clean_python:
	make clean -C python

	
.PHONY:clean
clean: clean_python


#common makefile foot

