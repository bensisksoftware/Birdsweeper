@echo off
:: 1. Load the Visual Studio Compiler Environment
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

:: 2. Launch Emacs
"C:\Users\wabew\Emacs\emacs-30.1\bin\runemacs.exe"
