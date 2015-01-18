# !/bin/sh
#
# File:
#     makefile
#
# Description:
#     Compilation is executed there.
#     Linker starts work after succesful compilation .asm and .c files.
#     The source file list are tuned in OBJS variable.
#
# Author:
#     Simakov V.V.

!include ../custom.mk

# Source
OBJS = _start.obj main.obj pio.obj serial.obj watchdog.obj common.obj interrupt.obj ethernet.obj timer.obj led.obj

all: $(DEBUGDIR)$(TARGET)

.c.obj : .autodepend
	$(CC) $< $(CFLAGS)
    
.asm.obj : 
	$(ASM) $< $(ASMFLAGS)

$(DEBUGDIR)$(TARGET) : $(OBJS)
	mv -f *.obj $(DEBUGDIR)	
    $(LINKER) $(LFLAGS) path $(DEBUGDIR) name  $(DEBUGDIR)$@ file { $< } 

clean : .symbolic
    rm -f $(DEBUGDIR)*.obj $(DEBUGDIR)*.exe *.err *.obj