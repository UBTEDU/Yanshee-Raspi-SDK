# --------------------------------------------------------------------------
#
#	FILE NAME:		Makefile
#	AUTHOR:			UBTEDU
#	DESCRIPTION:	Voice recognition
#
# --------------------------------------------------------------------------
#common makefile header
include config.mk

SUBDIRS=sdk

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


clean_sdk:
	make clean -C sdk

	
.PHONY:clean
clean: clean_sdk 	


#common makefile foot

