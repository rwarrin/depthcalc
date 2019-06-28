@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
start D:\Development\Vim\vim80\gvim.exe

SET PATH=%PATH%;E:\Dev\RemedyBG

cls
echo Shell Started
