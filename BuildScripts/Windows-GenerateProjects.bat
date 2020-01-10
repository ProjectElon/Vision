@echo off
pushd %~dp0\..\
call Premake\Windows\premake5.exe vs2019
popd
PAUSE