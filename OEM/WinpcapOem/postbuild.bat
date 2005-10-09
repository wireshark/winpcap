@echo off

echo Copying dll files for debug
REM This copies are needed to debug all the stuff internally, in the cpcap output folders

if exist debug\packet.lib copy debug\packet.lib ..\..\..\winpcap\common
if exist release\packet.lib copy release\packet.lib ..\..\..\winpcap\common
