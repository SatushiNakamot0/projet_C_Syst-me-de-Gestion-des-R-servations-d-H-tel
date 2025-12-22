@echo off
if not exist build mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
cd ..
.\build\hotel_app.exe
pause
