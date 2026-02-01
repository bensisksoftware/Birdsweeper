@echo off
call setup_vs_x64.bat
cl /Zi Birdsweeper.cpp user32.lib gdi32.lib