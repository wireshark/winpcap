@echo off
cd dox\prj\
@echo #### Delete existing WinPcap documentation
del docs\*.*
@echo #### Create new documentation
doxygen winpcap_c.dox
@echo #### Copy 'winpcap_small.gif' file, since it is not copied
copy ..\pics\winpcap_small.gif docs\.
cd ..
cd ..

