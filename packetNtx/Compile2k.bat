@echo off
set NDISVERSION=NDIS50
md \i386\free >nul 2>nul
md \i386\checked >nul 2>nul
build -cefw
