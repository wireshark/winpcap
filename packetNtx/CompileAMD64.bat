@echo off
if NOT %_BUILDARCH%==AMD64 goto error_buildenv

set NDISVERSION=NDIS50
set NPF_ARCH=__NPF_AMD64__
md \AMD64\free >nul 2>nul
md \AMD64\checked >nul 2>nul

echo foo > driver\Sources
xcopy driver\Sources.AMD64 driver\Sources /y /v /q 

build -cefw

del /f /q driver\Sources 

goto end

:error_buildenv

echo Wrong build Environment. Please use the Build Environment for x86-64 (AMD64)

:end