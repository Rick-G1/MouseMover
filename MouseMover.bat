@echo off
rem 
rem This batch file sends the run command to the MouseMover hardware. 
rem You must set the comport to the MouseMovers COM port number. 
rem 
rem Call this as follows: 
rem   MouseMover 555        	(sets run time to 555 minutes)
rem 
rem MoverPort is comport address in format \\.\COM# (set to your comport#)
set MoverPort=\\.\COM4
rem R=<# minutes to run> (9 hours=540) (0=off)(%1=get from cmd line)
echo R=%1 >%MoverPort%
