@echo off

del *.err

del .\BIN\test.exe

echo Compiling...           
for %%f in (*.c) do wcc %%f -1 -fpc -w4 -e25 -zq -oi -otexan -d2 -ms -bt=dos  
if errorlevel 1 goto END
for %%f in (*.asm) do wasm %%f -zq
if errorlevel 1 goto END


move /Y *.obj ..\BIN > NUL
cd ..\BIN

echo Linking...
wlink op q @test.lk1
if errorlevel 1 goto END

rem del *.obj
rem del *.map

echo Uploading...
mpcloader run.t2

cd ..\SRC
:END

