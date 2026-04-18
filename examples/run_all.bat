@echo off
echo ================================
echo Running all Darija test cases
echo ================================
echo.

echo [1/5] hello_world ...
call 01_run.bat
if errorlevel 1 echo FAILED: hello_world
echo.

echo [2/5] variables_and_types ...
call 02_run.bat
if errorlevel 1 echo FAILED: variables_and_types
echo.

echo [3/5] control_flow ...
call 03_run.bat
if errorlevel 1 echo FAILED: control_flow
echo.

echo [4/5] functions ...
call 04_run.bat
if errorlevel 1 echo FAILED: functions
echo.

echo [5/5] classes ...
call 05_run.bat
if errorlevel 1 echo FAILED: classes
echo.

echo ================================
echo All tests complete
echo ================================
