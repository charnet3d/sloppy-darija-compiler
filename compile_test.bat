@echo off
REM Compile a .darija source file to executable using the Darija compiler
REM Usage: compile_test.bat <source.darija>
REM Assumes darija-compiler.exe is already built

setlocal

if "%~1"=="" (
    echo Usage: compile_test.bat ^<source.darija^>
    exit /b 1
)

set SRC=%~1
set BASE=%~n1

call "D:\1_Programs\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

echo [COMPILE] %SRC% ...
build\darija-compiler.exe "%SRC%" -o %BASE%.s
if errorlevel 1 (
    echo ERROR: Compilation failed
    exit /b 1
)

echo [ASSEMBLE] %BASE%.s ...
ml64.exe "%BASE%.s" /c
if errorlevel 1 (
    echo ERROR: Assembly failed
    exit /b 1
)

echo [LINK] %BASE%.obj ...
link.exe /ENTRY:_start /SUBSYSTEM:CONSOLE "%BASE%.obj" "build\darija-runtime.lib" kernel32.lib user32.lib vcruntime.lib ucrt.lib legacy_stdio_definitions.lib /OUT:%BASE%.exe
if errorlevel 1 (
    echo ERROR: Linking failed
    exit /b 1
)

echo [SUCCESS] %BASE%.exe built
exit /b 0
