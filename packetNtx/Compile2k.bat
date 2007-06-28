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


mkdir driver\bin 2> nul
mkdir driver\bin\2k 2> nul

set USER_C_DEFINES=-DNDIS50
set TARGETPATH=driver\bin\2k

copy driver\NPF_NT5.RC driver\NPF.RC

prefast build -cefw

set TARGETPATH=
set USER_C_DEFINES=

:end