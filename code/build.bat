@echo off

SET CompilerFlags=/nologo /Z7 /Od /Oi /fp:precise
SET LinkerFlags=/incremental:no user32.lib

REM SET CompilerFlags=/nologo /Ox /Oi /fp:precise /favor:INTEL64 /MD

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

rc.exe /nologo ..\code\app.rc
cl.exe %CompilerFlags% ..\code\app.res ..\code\main.cpp /link %LinkerFlags%

popd
