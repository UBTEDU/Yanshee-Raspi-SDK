# --------------------------------------------------------------------------
#
#	FILE NAME:		Makefile
#	AUTHOR:			UBTEDU
#	DESCRIPTION:	Voice recognition
#
# --------------------------------------------------------------------------
#common makefile header
include config.mk

SUBDIRS=libs utils src swig

.PHONY:
all: $(SUBDIRS) install_libs install_python install_doc

$(SUBDIRS):ECHO
	make -C $@
	
ECHO: 
	@echo "###################"$(SUBDIRS)"###################"
install_libs:
	@mkdir -p output/libs
	@install -m 0644 libs/json/*so output/libs/
	@install -m 0644 libs/log/*so output/libs/
	@install -m 0644 libs/robotlogstr/*so output/libs/
	@install -m 0644 src/*so output/libs/
	@install -m 0644 utils/*so output/libs/
	@install -m 0644 swig/python/_RobotApi.so output/libs

install_python:
	@mkdir -p output/python/
	@install -m 0644 swig/python/RobotApi.py output/python/
	@install -m 0644 swig/python/_RobotApi.so output/python/
	@mkdir -p output/python/example
	@install -m 0644 src/example/python/*.py output/python/example/
	@install -m 0644 src/setup.py output/python
	
install_doc:
	@mkdir -p output/doc
	@cp -r src/YANSHEE_USER_MANUAL output/doc


#clean_swig:
#	make clean -C swig

.PHONY:clean
#clean:  clean_swig	
clean:
	rm -rf output/
	@ (                   \
		for dir in $(SUBDIRS) ; do           \
		DIR=$${dir}/;           \
		$(MAKE) -C $$DIR clean ;          \
		done                                                                    \
		)


#common makefile foot

