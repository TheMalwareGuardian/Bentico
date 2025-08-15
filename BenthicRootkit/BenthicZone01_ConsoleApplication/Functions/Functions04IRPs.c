// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://en.wikibooks.org/wiki/Windows_Programming/windows.h
// The primary C header file for accessing the Win32 API is the <windows.h> header file. To make a Win32 executable, the first step is to include this header file in your source code. The windows.h header file should be included before any other library include, even the C standard library files such as stdio.h or stdlib.h. This is because the windows.h file includes macros and other components that may modify, extend, or replace things in these libraries. This is especially true when dealing with UNICODE, because windows.h will cause all the string functions to use UNICODE instead. Also, because many of the standard C library functions are already included in the Windows kernel, many of these functions will be available to the programmer without needing to load the standard libraries. For example, the function sprintf is included in windows.h automatically. 
#include <windows.h>

// https://en.wikibooks.org/wiki/C_Programming/stdio.h
// The C programming language provides many standard library functions for file input and output. These functions make up the bulk of the C standard library header <stdio.h>. The I/O functionality of C is fairly low-level by modern standards; C abstracts all file operations into operations on streams of bytes, which may be "input streams" or "output streams". Unlike some earlier programming languages, C has no direct support for random-access data files; to read from a record in the middle of a file, the programmer must create a stream, seek to the middle of the file, and then read bytes in sequence from the stream. 
#include <stdio.h>



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "../Structures/Structures00Techniques.h"
#include "../Globals/Globals01Techniques.h"



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Search for a CLI command in the global command table.
**/
static const STRUCTURES_TECHNIQUES_COMMAND_ENTRY* FunctionsIRPs_FindCommandEntry(const char* cmd)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	size_t i;


	// ---------------------------------------------------------------------------------------------------------------------
	// Iterate through the global command table to find a matching keyword
	for (i = 0; i < sizeof(Globals_Techniques_Commands) / sizeof(Globals_Techniques_Commands[0]); ++i)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// If the current command matches the input, return its entry
		if (_stricmp(cmd, Globals_Techniques_Commands[i].command) == 0)
		{
			return &Globals_Techniques_Commands[i];
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// No match
	return NULL;
}



/**
	@brief      Send an IOCTL request to the driver based on a parsed CLI command.
**/
void FunctionsIRPs_SendIOCTL_ExecuteCommand(HANDLE hDriver, const char* command, const char* parameter)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Resolve keyword
	const STRUCTURES_TECHNIQUES_COMMAND_ENTRY* entry = FunctionsIRPs_FindCommandEntry(command);

	if (entry == NULL)
	{
		printf(" \033[0;31m[!] Unknown command.\033[0m\n");
		return;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Parameter checks
	if (entry->requiresParameter && (parameter == NULL || *parameter == '\0'))
	{
		printf(" \033[0;31m[!] Missing parameter. Usage: %s <param>\033[0m\n", entry->command);
		return;
	}
	if (!entry->requiresParameter && parameter != NULL && *parameter != '\0')
	{
		printf(" \033[0;33m[!] Command '%s' takes no parameter. Ignoring extra text.\033[0m\n", entry->command);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Validate driver handle
	if (hDriver == NULL || hDriver == INVALID_HANDLE_VALUE)
	{
		printf(" \033[0;31m[!] Driver handle is not open. Execute kstatus.\033[0m\n");
		return;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	CHAR inBuffer[256] = { 0 };
	CHAR outBuffer[1024] = { 0 };
	DWORD bytesReturned = 0;


	// ---------------------------------------------------------------------------------------------------------------------
	// If a parameter was provided and it's not an empty string
	if (parameter && *parameter)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Copy the parameter string safely into the input buffer
		strncpy_s(inBuffer, sizeof(inBuffer), parameter, _TRUNCATE);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Display detailed information for the current operation

	// IOCTL
	printf(" \033[0;32m[+]\033[0m IOCTL:     0x%08lX\n", entry->ioctlCode);

	// Command
	printf(" \033[0;32m[+]\033[0m Command:   %s%s%s\n", entry->command, entry->requiresParameter ? " " : "", entry->requiresParameter ? inBuffer : "");


	// ---------------------------------------------------------------------------------------------------------------------
	// Send the IOCTL request to the driver

	// Sends a control code directly to a specified device driver, causing the corresponding device to perform the corresponding operation.
	// https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-deviceiocontrol
	if (DeviceIoControl(hDriver, entry->ioctlCode, inBuffer, (DWORD)sizeof(inBuffer), outBuffer, (DWORD)sizeof(outBuffer), &bytesReturned, NULL))
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Sanitize the byte count to ensure it's within bounds
		if (bytesReturned >= sizeof(outBuffer)) bytesReturned = sizeof(outBuffer) - 1;


		// -----------------------------------------------------------------------------------------------------------------
		// Ensure the output buffer is null-terminated so it prints safely
		outBuffer[bytesReturned] = '\0';


		// -----------------------------------------------------------------------------------------------------------------
		// If the driver returned a message in the output buffer
		if (bytesReturned && outBuffer[0])
		{
			printf(" \033[0;32m[+]\033[0m Response:  %s\n\n", outBuffer);
		}
		else
		{
			printf(" \033[0;32m[+]\033[0m IOCTL succeeded (returned %lu bytes), but no response message was provided by the driver.\n\n", bytesReturned);
		}
	}
	else
	{
		printf(" \033[0;31m[!] DeviceIoControl failed (Win32 error %lu)\033[0m\n", GetLastError());
	}

}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
