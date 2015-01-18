# !/bin/sh
#
# File:
#     makefile
#
# Description:
#     A generic Makefile, used by wmake.
#     Default target is all.
#     For successful compilation Watcom 11.0(clear - without C or B patches) is required.
#
# Author:
#     Simakov V.V.
!include custom.mk

all: .symbolic # like a phony
	@set INCLUDE=$(INCDIR)
	cd src && $(MAKE) all
	@echo ...
	@echo Compilation is completed.
	
clean: .symbolic # like a phony
	cd src && $(MAKE) clean
	@echo ...
	@echo Cleaning is completed.