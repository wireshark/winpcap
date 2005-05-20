@echo off
cd dox\prj\
@echo #### Delete existing WinPcap documentation
del /q docs\*.*
@echo #### Create new documentation
doxygen winpcap_c.dox
@echo #### Copy all gif files
copy ..\pics\*.gif docs\. /v /y /q
copy ..\*.gif docs\. /v /y /q
cd ..
cd ..

