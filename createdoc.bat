@echo off
cd dox\prj\
@echo #### Delete existing WinPcap documentation
del /q docs\*.*
@echo #### Create new documentation
doxygen winpcap_noc.dox >nul
@echo #### Copy all gif files
xcopy ..\pics\*.gif docs\. /v /y /q >nul
xcopy ..\*.gif docs\. /v /y /q >nul
cd ..
cd ..

