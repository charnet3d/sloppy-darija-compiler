@echo off
call "D:\1_Programs\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
if not exist build mkdir build
cd build
del /q darija-runtime.lib
rmdir /s /q CMakeFiles\darija-runtime.dir
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target darija-runtime --config Release -j 16 | more
cd ..
