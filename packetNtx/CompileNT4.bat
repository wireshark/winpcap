set Include=%BASEDIR%\src\network\inc;%BASEDIR%\inc;%Include%
set NDISVERSION=NDIS30
md \i386\free
md \i386\checked
copy ..\common\win_bpf_filter.c driver
build -c -w
