// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:                            KernelRootkit009_FilterCommunicationPort
// IDE:                             Open Visual Studio
// Template:                        Create a new project -> Search for templates (Alt + S) -> Console App -> Next
// Project:                         Project Name: ConsoleApp_FilterCommunicationPort -> Solution Name: KernelRootkit009_FilterCommunicationPort -> Create
// Source File:                     In Solution Explorer -> Find ConsoleApp_FilterCommunicationPort.cpp -> Rename to Application.c
// Source Code:                     Open Application.c and copy the corresponding source code
// Library Dependencies:            Open Project Properties -> Linker -> Additional Dependencies -> Add 'FltLib.lib;'
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Add Project:                     In Solution Explorer -> Right-click the solution (KernelRootkit009_FilterCommunicationPort) -> Add -> New Project...
// Template:                        Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:                         Project name: KMDFDriver_FilterCommunicationPort -> Create
// Source File:                     Source Files -> Add -> New Item... -> Driver.c
// Source Code:                     Open Driver.c and copy the corresponding source code
// Library Dependencies:            Open Project Properties -> Linker -> Additional Dependencies -> Add '$(DDK_LIB_PATH)fltMgr.lib;'
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Locate App:                      C:\Users\%USERNAME%\source\repos\KernelRootkit009_FilterCommunicationPort\x64\Release\ConsoleApp_FilterCommunicationPort.exe
// Locate Driver:                   C:\Users\%USERNAME%\source\repos\KernelRootkit009_FilterCommunicationPort\x64\Release\KMDFDriver_FilterCommunicationPort.sys
// Virtual Machine:                 Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move App:                        Move ConsoleApp_FilterCommunicationPort.exe (Host) to C:\Users\%USERNAME%\Downloads\ConsoleApp_FilterCommunicationPort.exe (VM)
// Move Driver:                     Copy KMDFDriver_FilterCommunicationPort.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_FilterCommunicationPort.sys (VM)
// Enable Test Mode:                Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:             Open a CMD window as Administrator -> sc.exe create WindowsKernelFilterCommunicationPort type=filesys start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_FilterCommunicationPort.sys"
// Registered Driver:               Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelFilterCommunicationPort
// Service Status:                  Run in CMD as Administrator -> sc.exe query WindowsKernelFilterCommunicationPort -> driverquery.exe
// Registry Entry:                  Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelFilterCommunicationPort
// Monitor Messages:                Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:                   Run in CMD as Administrator -> sc.exe start WindowsKernelFilterCommunicationPort
// User Mode App:                   Open CMD as Administrator -> Navigate to the directory -> Run ConsoleApp_FilterCommunicationPort.exe
// Clean:                           Run in CMD as Administrator -> sc.exe stop WindowsKernelFilterCommunicationPort
// Remove:                          Run in CMD as Administrator -> sc.exe delete WindowsKernelFilterCommunicationPort



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://en.wikibooks.org/wiki/Windows_Programming/windows.h
// The primary C header file for accessing the Win32 API is the <windows.h> header file. To make a Win32 executable, the first step is to include this header file in your source code. The windows.h header file should be included before any other library include, even the C standard library files such as stdio.h or stdlib.h. This is because the windows.h file includes macros and other components that may modify, extend, or replace things in these libraries. This is especially true when dealing with UNICODE, because windows.h will cause all the string functions to use UNICODE instead. Also, because many of the standard C library functions are already included in the Windows kernel, many of these functions will be available to the programmer without needing to load the standard libraries. For example, the function sprintf is included in windows.h automatically. 
#include <windows.h>

// https://learn.microsoft.com/en-us/windows/win32/api/fltuser/
// This header is used by Installable file systems DDI reference.
#include <fltUser.h>

// https://en.wikibooks.org/wiki/C_Programming/stdio.h
// The C programming language provides many standard library functions for file input and output. These functions make up the bulk of the C standard library header <stdio.h>. The I/O functionality of C is fairly low-level by modern standards; C abstracts all file operations into operations on streams of bytes, which may be "input streams" or "output streams". Unlike some earlier programming languages, C has no direct support for random-access data files; to read from a record in the middle of a file, the programmer must create a stream, seek to the middle of the file, and then read bytes in sequence from the stream. 
#include <stdio.h>



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	Main entry point
**/
int main() {
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	printf("Benthic Zone FilterCommunicationPort -> ConsoleAppFilterCommunicationPort [Application.c] (main) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables

	// Handle to the communication port
	HANDLE port = NULL;
	// Stores the number of bytes received from the driver
	DWORD bytesReceived = 0;
	// Message to be sent to the driver
	PCHAR buffer = "user message";
	// Buffer to store the response from the driver
	char recBuffer[50] = { 0 };


	// ------------------------------------------------------------------------------------
	// Attempt to connect to the minifilter communication port
	printf("Benthic Zone FilterCommunicationPort -> ConsoleAppFilterCommunicationPort [Application.c] (main) - Attempt to connect to the minifilter communication port\n");

	// FilterConnectCommunicationPort opens a new connection to a communication server port that is created by a file system minifilter.
	// https://learn.microsoft.com/en-us/windows/win32/api/fltuser/nf-fltuser-filterconnectcommunicationport
	if (FilterConnectCommunicationPort(L"\\FilterCommunicationPort", 0, NULL, 0, NULL, &port) != S_OK)
	{
		printf("Benthic Zone FilterCommunicationPort -> ConsoleAppFilterCommunicationPort [Application.c] (main) - ERROR: Failed to connect to communication port.\n");
		return 1;
	}


	// ------------------------------------------------------------------------------------
	// Send a message to the minifilter driver and wait for a response
	printf("Benthic Zone FilterCommunicationPort -> ConsoleAppFilterCommunicationPort [Application.c] (main) - Send a message to the minifilter driver and wait for a response\n");


	// The FilterSendMessage function sends a message to a kernel-mode minifilter.
	// https://learn.microsoft.com/en-us/windows/win32/api/fltuser/nf-fltuser-filtersendmessage
	if (FilterSendMessage(port, buffer, (DWORD)strlen(buffer) + 1, recBuffer, sizeof(recBuffer), &bytesReceived) == S_OK)
	{
		printf("Benthic Zone FilterCommunicationPort -> ConsoleAppFilterCommunicationPort [Application.c] (main) - Message sent successfully\n");
		printf("Benthic Zone FilterCommunicationPort -> ConsoleAppFilterCommunicationPort [Application.c] (main) - Response received: %s\n", recBuffer);
	}
	else
	{
		printf("Benthic Zone FilterCommunicationPort -> ConsoleAppFilterCommunicationPort [Application.c] (main) - ERROR: Failed to send message.\n");
	}


	// ------------------------------------------------------------------------------------
	// Close the communication port handle
	printf("Benthic Zone FilterCommunicationPort -> ConsoleAppFilterCommunicationPort [Application.c] (main) - Close the communication port handle\n");

	if (port != NULL)
	{
		// Closes an open object handle.
		// https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
		CloseHandle(port);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	printf("Benthic Zone FilterCommunicationPort -> ConsoleAppFilterCommunicationPort [Application.c] (main) - Bye\n");


	// ------------------------------------------------------------------------------------
	// Return	
	return 0;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------