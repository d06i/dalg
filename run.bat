set OUT=out.ll
echo off 
 
del %OUT%
del %OUT%.exe

dalg test.dalg %OUT%
type %OUT%

clang -O3 %OUT% -o %OUT%.exe

%OUT%.exe

pause
