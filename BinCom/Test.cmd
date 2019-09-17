@echo off

rem Author   KMS - Martin Dubois, ing.
rem Product  BinCom
rem File     BinCom/Test.cmd
rem Usage    ./BinCom.cmd {Config}

echo Executing  BinCom/Test.cmd  %1  ...

rem ===== Initialisation ====================================================

BIN_COM_EXE=..\%1\BinCom.exe

rem ===== Verification ======================================================

if not exist %BIN_COM_EXE% (
	echo FATAL ERROR  %BIN_COM_EXE%  does not exist
	pause
	exit 10
)

rem ====== Execution ========================================================

%BIN_COM_EXE% Command "ExecuteScript Tests\Test1.txt"
if ERRORLEVEL 1 (
	echo ERROR  %BIN_COM_EXE% Command "ExecuteScript Tests\Test1.txt"  failed - %ERRORLEVEL%
	pause
	exit 20
)

rem ===== End ===============================================================

echo OK
