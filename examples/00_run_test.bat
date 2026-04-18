@echo off

setlocal

if "%~1"=="" (
    echo Usage: 00_run_test.bat ^<test_name^>
    exit /b 1
)

cd ..
set test_name=%~1
call compile_test.bat examples\%test_name%.darija
if errorlevel 1 (
    cd examples
    exit /b 1
)
%test_name%.exe
echo Exit code: %errorlevel%
del /q %test_name%.* 2>nul
cd examples