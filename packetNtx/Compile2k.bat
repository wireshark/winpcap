@echo off
set NDISVERSION=NDIS50
set TS_TYPE=KQPC_TS
if "%1"=="RDTSC" set TS_TYPE=RDTSC_TS
if "%1"=="rdtsc" set TS_TYPE=RDTSC_TS
md \i386\free >nul 2>nul
md \i386\checked >nul 2>nul
build -cefw
