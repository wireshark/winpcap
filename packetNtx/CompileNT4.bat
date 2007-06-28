@echo off

echo ************************************************************
echo *
echo * This compilation script has been OBSOLETED.
echo *
echo * Please use CompileDriver.bat to compile the driver
echo * for all the supported platforms (NT4,2k,xp,2k3,vista,x64)
echo *
echo ************************************************************

goto end

if NOT "%NPF_COMPILED%" == "1" (
		set Include=%BASEDIR%\src\network\inc;%BASEDIR%\inc;%Include%
		set NPF_COMPILED=1
			)
set USER_C_DEFINES=-D__NPF_NT4__ -DNDIS30
md \i386\free >nul 2>nul
md \i386\checked >nul 2>nul
build -cefw
set USER_C_DEFINES=

:end