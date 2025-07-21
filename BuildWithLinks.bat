@echo off
zig cc build\bin\dream.c -Isrc/runtime -Lzig-out/lib -ldreamrt -o build/bin/dream.exe

REM then run it:
.\build\bin\dream.exe