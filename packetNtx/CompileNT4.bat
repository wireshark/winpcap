@echo off
if NOT "%NPF_COMPILED%" == "1" (
		set Include=%BASEDIR%\src\network\inc;%BASEDIR%\inc;%Include%
		set NPF_COMPILED=1
			)

rem if NOT "%_BUILDARCH%"=="x86" goto error_buildenv

set NPF_ARCH=__NPF_x86__
set NDISVERSION=NDIS30
md \i386\free >nul 2>nul
md \i386\checked >nul 2>nul

echo foo > driver\Sources
xcopy driver\Sources.x86 driver\Sources /v /q 

build -cefw

del driver\Sources

goto end

:error_buildenv

echo Wrong build Environment. Please use the Build Environment for x86 (32 bit)

:end
