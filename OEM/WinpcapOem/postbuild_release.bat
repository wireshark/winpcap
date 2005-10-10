@echo off

echo Copying dll files for debug
REM This copies are needed to debug all the stuff internally, in the cpcap output folders

if exist debug\packet.lib xcopy debug\packet.lib ..\..\..\winpcap\common\ /v /y /q
if exist release\packet.lib xcopy release\packet.lib ..\..\..\winpcap\common\ /v /y /q
