@echo off
zig cc build\bin\dream.c src\runtime\memory.c src\runtime\console.c src\runtime\custom.c ^
  -Isrc\runtime -o build\bin\dream.exe

REM then run it:
.\build\bin\dream.exe