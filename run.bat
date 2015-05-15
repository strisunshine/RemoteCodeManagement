@echo off
echo.
echo ***************************** Demonstrating requirements ********************************
echo.  
echo  CommandLine Argument for Server 1: 8090 Server1Folder/ 
echo.  
echo  CommandLine Argument for Server 2: 6070 Server2Folder/
echo.
echo  CommandLine Argument for ClientWPF 1: 9080 Client1Folder/
echo.
echo  CommandLine Argument for ClientWPF 2: 7060 Client2Folder/
echo.
start Debug\Server.exe 8090 Server1Folder/
start Debug\Server.exe 6070 Server2Folder/
start Debug\ClientWPF.exe 9080 Client1Folder/
start Debug\ClientWPF.exe 7060 Client2Folder/
pause