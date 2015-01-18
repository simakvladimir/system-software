# !/bin/sh
#
# File:
#     custom.mk
#
# Description:
#     Tools
#
# Author:
#     Simakov V.V.

# App name
TARGET = sdk.exe

# Output dir
DEBUGDIR = ../_Debug/

# Inc 
INCDIR = ../include

MAKE = wmake

CC = wcc
CFLAGS = -1 -fpc -w4 -e25 -zq -oi -otexan -d2 -ms -bt=dos  #-w4 -e25 -zq -od -d2 -bt=dos -ms

LINKER = wlink
LFLAGS = op q sys dos #d all sys dos op m op maxe=25 op q op

ASM = wasm
ASMFLAGS = -zq