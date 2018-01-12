# --------------------------------------------------------------------------
#
#	FILE NAME:		Makefile
#	AUTHOR:			UBTEDU
#	DESCRIPTION:	Voice recognition
#
# --------------------------------------------------------------------------
#common makefile header
include config.mk

SUBDIRS=swig 

.PHONY:prebless
all: $(SUBDIRS) 

$(SUBDIRS):ECHO
	make -C $@
	
ECHO: prebless
	@echo "###################"$(SUBDIRS)"###################"

prebless: 
	@echo "######Welcome to the robotic world######"
	@echo " 　　　　　　　　 ┏┓　　　 ┏┓                                                         "
	@echo " 　　　　　　　┏━━┛┻━━━━━━━┛┻┓                                                       "
	@echo " 　　　　　　　┃　　　　　　 ┃                                                       "
	@echo " 　　　　　　　┃　　　━　　　┃                                                       "
	@echo " 　　　　　　　┃　＞　　　＜ ┃                                                       "
	@echo " 　　　　　　　┃　　　　　　 ┃                                                       "
	@echo " 　　　　　　　┃...　⌒　...　┃                                                       "
	@echo " 　　　　　　　┃　　　　　　 ┃                                                       "
	@echo " 　　　　　　　┗━┓　　　   ┏━┛                                                       "
	@echo " 　　　　　　　　┃　　　   ┃　Code is far away from bug with the animal protecting     "
	@echo " 　　　　　　　　┃　　　   ┃   神兽保佑,代码无bug                                      "
	@echo " 　　　　　　　　┃　　　   ┃                                                           "
	@echo " 　　　　　　　　┃　　　   ┃                                                           "
	@echo " 　　　　　　　　┃　　　   ┃                                                           "
	@echo " 　　　　　　　　┃　　　   ┃                                                           "
	@echo " 　　　　　　　　┃　　　   ┗━━━┓                                                   "
	@echo " 　　　　　　　　┃　　　　　   ┣┓                                                 "
	@echo " 　　　　　　　　┃　　　　　   ┏┛                                                 "
	@echo " 　　　　　　　　┗┓┓┏━━━┳┓━━  ┏┛                                                   "
	@echo " 　　　　　　　　 ┃┫┫　┃┫┫                                                     "
	@echo " 　　　　　　　　 ┗┻┛　┗┻┛                                                     "

.PHONY:install
install: install_python

install_python:
	@mkdir -p output/python/
	@install -m 0644 swig/python/RobotApi.py output/python/
	@install -m 0644 swig/python/_RobotApi.so output/python/
	@install -m 0644 sdk/example/python/ubtGetSWVersion.py output/python


clean_swig:
	make clean -C swig

.PHONY:clean
clean:  clean_swig	
	rm -rf output/

#common makefile foot

