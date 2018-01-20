# --------------------------------------------------------------------------
#
#	FILE NAME:		Makefile
#	AUTHOR:			UBTEDU
#	DESCRIPTION:	SDK LIBS
#
# --------------------------------------------------------------------------
#common makefile header

SUBDIRS=python

.PHONY:
all: python_swig $(SUBDIRS) 

python_swig:
	@cp ../src/RobotApi.c python/
	@cp ../src/RobotApi.h python/
	@swig -python python/RobotApi.i

$(SUBDIRS):ECHO
	make -C $@
	
ECHO: 
	@echo "###################"$(SUBDIRS)"###################"


.PHONY:clean
clean:  
	@ (                   \
		for dir in $(SUBDIRS) ; do           \
		DIR=$${dir}/;           \
		$(MAKE) -C $$DIR clean ;          \
		done                                                                    \
		)


#common makefile foot

