@echo off

set DESTDIR=.\

set SOURCEDIR=..\winpcap\

echo Creating Local Examples folders
mkdir %DESTDIR%\Examples-pcap >nul 2>nul
mkdir %DESTDIR%\Examples >nul 2>nul

xcopy /s/e/v /Y %SOURCEDIR%\Examples		%DESTDIR%\Examples
 
xcopy /s/e/v /Y %SOURCEDIR%\Examples-pcap		%DESTDIR%\Examples-pcap

rem *** Apply the patches for the OEM initialization ***

copy /v /y Examples.diff %DESTDIR%\Examples\.
copy /v /y Examples-pcap.diff %DESTDIR%\Examples-pcap\.

pushd %DESTDIR%\Examples\
patch -p1 < Examples.diff 

if NOT %ERRORLEVEL%==0 (
		echo ---- Error patching the examples!!
		pause
		)
del Examples.diff > nul 2>nul

popd

pushd %DESTDIR%\Examples-pcap\
patch -p1 < Examples-pcap.diff 

if NOT %ERRORLEVEL%==0 (
		echo ---- Error patching the examples-pcap!!
		pause
		)
del Examples-pcap.diff > nul 2>nul

popd


echo Local Examples created successfully
set DESTDIR=
set SOURCEDIR=

echo *************************************************************************
echo *					      				     *
echo * Now you can build the documentation from the developers' pack folder! *
echo *									     *
echo *************************************************************************
