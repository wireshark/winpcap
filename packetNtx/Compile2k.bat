set Include=%BASEDIR%\src\network\inc;%BASEDIR%\inc;%Include%
set NDISVERSION=NDIS50
set TS_TYPE=RDTSC_TS
if "%1"=="KQPC" set TS_TYPE=KQPC_TS
if "%1"=="kqpc" set TS_TYPE=KQPC_TS
md \i386\free
md \i386\checked
build -c -w
