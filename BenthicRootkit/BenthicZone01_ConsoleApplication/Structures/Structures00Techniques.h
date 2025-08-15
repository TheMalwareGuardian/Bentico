// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



#pragma once



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://en.wikibooks.org/wiki/Windows_Programming/windows.h
// The primary C header file for accessing the Win32 API is the <windows.h> header file. To make a Win32 executable, the first step is to include this header file in your source code. The windows.h header file should be included before any other library include, even the C standard library files such as stdio.h or stdlib.h. This is because the windows.h file includes macros and other components that may modify, extend, or replace things in these libraries. This is especially true when dealing with UNICODE, because windows.h will cause all the string functions to use UNICODE instead. Also, because many of the standard C library functions are already included in the Windows kernel, many of these functions will be available to the programmer without needing to load the standard libraries. For example, the function sprintf is included in windows.h automatically. 
#include <windows.h>



// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------
// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Enumeration of all technique categories.
**/
typedef enum {
	STRUCTURES_TECHNIQUES_CMD_CAT_GENERAL = 0,
	STRUCTURES_TECHNIQUES_CMD_CAT_DKOM,
	STRUCTURES_TECHNIQUES_CMD_CAT_KEYBOARD,
	STRUCTURES_TECHNIQUES_CMD_CAT_NSI,
	STRUCTURES_TECHNIQUES_CMD_CAT_WFP,
	STRUCTURES_TECHNIQUES_CMD_CAT_WSK,
	STRUCTURES_TECHNIQUES_CMD_CAT_MINIFILTER,
	STRUCTURES_TECHNIQUES_CMD_CAT_TOTAL
	// STRUCTURES_TECHNIQUES_CMD_CAT_CALLBACKS,
} STRUCTURES_TECHNIQUES_COMMAND_CATEGORY;



/**
	@brief      Structure representing a command entry.
**/
typedef struct {
	const char* command;
	const char* description;
	const char* example;
	DWORD ioctlCode;
	BOOL requiresParameter;
	STRUCTURES_TECHNIQUES_COMMAND_CATEGORY category;
} STRUCTURES_TECHNIQUES_COMMAND_ENTRY;



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
