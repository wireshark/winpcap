@echo off

set USER_C_DEFINES=-DNDIS50

md \i386\free >nul 2>nul
md \i386\checked >nul 2>nul

build -cefw

set USER_C_DEFINES=

