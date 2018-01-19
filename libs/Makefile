# --------------------------------------------------------------------------
#
#	FILE NAME:		Makefile
#	AUTHOR:			UBTEDU
#	DESCRIPTION:	SDK LIBS
#
# --------------------------------------------------------------------------
#common makefile header

SUBDIRS=json log robotlogstr

.PHONY:
all: $(SUBDIRS) 

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

