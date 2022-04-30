:loop
gen.exe
a.exe
fc res.out ans.out
if not errorlevel 1 goto loop
pause