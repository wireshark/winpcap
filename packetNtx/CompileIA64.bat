@echo off
if NOT %_BUILDARCH%==IA64 goto error_buildenv

set NDISVERSION=NDIS50
set NPF_ARCH=__NPF_IA64__
md \IA64\free >nul 2>nul
md \IA64\checked >nul 2>nul

echo foo > driver\Sources
xcopy driver\Sources.IA64 driver\Sources /y /v /q 

build -cefw

del /f /q driver\Sources 

goto end

:error_buildenv

echo Wrong build Environment. Please use the Build Environment for IA64

:end