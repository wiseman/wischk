@echo off
if "empty%1"=="empty" goto usage
if "empty%2"=="empty" goto usage
if exist dumchk.exe goto skip
   copy wischk.exe dumchk.exe
:skip
copy master.cfg checkers.cfg
:loop
     echo WISCHK
     wischk %1
     type checkers.cfg
     if not exist checkers.cfg goto done
     echo DUMCHK
     dumchk %1
     type checkers.cfg
     if exist checkers.cfg goto loop
goto done
:usage
      echo usage: run [time1] [time2]
      echo   where [time1] is the number of seconds wischk should think,
      echo   and [time2] is the number of seconds dumchk should think.
:done

