@echo off
if NOT "%NPF_COMPILED%" == "1" (
		set Include=%BASEDIR%\src\network\inc;%BASEDIR%\inc;%Include%
		set NPF_COMPILED=1
			)
set NDISVERSION=NDIS30
md \i386\free >nul 2>nul
md \i386\checked >nul 2>nul
build -cefw
