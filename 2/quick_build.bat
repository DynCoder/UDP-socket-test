@echo off

cd receive
start cmd /k build.bat
cd ..

cd send
start cmd /k build.bat
cd ..