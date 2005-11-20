@echo off

call create_include.bat %1 %2
PAUSE
call create_lib.bat %1 %2
PAUSE
call create_examples.bat %1 %2
PAUSE
call create_docs.bat %1 %2


