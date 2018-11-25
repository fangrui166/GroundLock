@echo off
set OUT=%1.out
set HEX=%1.hex
set BIN=%1.bin

:: calculate application checksum
ielftool --fill 0xFF;checksum_start-checksum_end+3 --checksum ielftool_checksum:2,crc16,0x0;checksum_start-checksum_end+3 --verbose %OUT% %OUT%

:: generate additional output: hex
ielftool.exe --ihex --verbose %OUT% %HEX%

:: generate additional output: binary
ielftool.exe --bin --verbose %OUT% %BIN%

rem get version info
for /f tokens^=2*^ delims^=^" %%i in (%2) do (set VER=%%i)

rem get build info debug/release
if %3 == Release (
    set BUILD=rel
) else (
    set BUILD=dbg
)

rem rename build file
copy %OUT% %1_v%VER%_%BUILD%.out /y
rem del %OUT% /q
copy %HEX% %1_v%VER%_%BUILD%.hex /y
del %HEX% /q
copy %BIN% %1_v%VER%_%BUILD%.bin /y
del %BIN% /q

rem merge hex file
set PROJECT_ROOT_DIR=%~dp0..\..
if %3 == Release (
    set BOOTLOADER_HEX=%PROJECT_ROOT_DIR%\nrf_bootloader\iar\Release\Exe\nrf_bootloader.hex
) else (
    set BOOTLOADER_HEX=%PROJECT_ROOT_DIR%\nrf_bootloader\iar\Debug\Exe\nrf_bootloader.hex
)
if exist %BOOTLOADER_HEX% (
	%PROJECT_ROOT_DIR%\nrf_bootloader\iar\mergehex.exe --merge %BOOTLOADER_HEX% %1_v%VER%_%BUILD%.hex --output %1_v%VER%_%BUILD%_all.hex
)
