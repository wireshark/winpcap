@echo off
if NOT %_BUILDARCH%==x86 goto error_buildenv

set NDISVERSION=NDIS50
set NPF_ARCH=__NPF_x86__
md \i386\free >nul 2>nul
md \i386\checked >nul 2>nul

echo foo > driver\Sources
xcopy driver\Sources.x86 driver\Sources /y /v /q 

build -cefw

del driver\Sources

goto end

:error_buildenv

echo Wrong build Environment. Please use the Build Environment for x86 (32 bit)

:end