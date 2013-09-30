@echo off
wgs2sgf *.wgs
set year=%date:~0,4%
set month=%date:~5,2%
set fullname=%year%%month%
if not exist qqgo_wgs mkdir qqgo_wgs
if not exist qqgo_sgf mkdir qqgo_sgf
if not exist qqgo_sgf\%fullname% mkdir qqgo_sgf\%fullname%
if not exist qqgo_wgs\%fullname% mkdir qqgo_wgs\%fullname%
move *.sgf qqgo_sgf\%fullname%
move *.wgs qqgo_wgs\%fullname%
pause