// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:                            KernelRootkit008_Minifilter
// IDE:                             Open Visual Studio
// Template:                        Create a new project -> Search for templates (Alt + S) -> Console App -> Next
// Project:                         Project Name: ConsoleApp_MinifilterInstallation -> Solution Name: KernelRootkit008_Minifilter -> Create
// Source File:                     In Solution Explorer -> Find ConsoleApp_Application.cpp -> Rename to Application.c
// Source Code:                     Open Application.c and copy the corresponding source code
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Add Project:                     In Solution Explorer -> Right-click the solution (KernelRootkit008_Minifilter) -> Add -> New Project...
// Template:                        Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:                         Project name: KMDFDriver_Minifilter -> Create
// Source File:                     Source Files -> Add -> New Item... -> Driver.c
// Source Code:                     Open Driver.c and copy the corresponding source code
// Library Dependencies:            Open Project Properties -> Linker -> Additional Dependencies -> Add '$(DDK_LIB_PATH)fltMgr.lib;'
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Locate App:                      C:\Users\%USERNAME%\source\repos\KernelRootkit008_Minifilter\x64\Release\ConsoleApp_MinifilterInstallation.exe
// Locate Driver:                   C:\Users\%USERNAME%\source\repos\KernelRootkit008_Minifilter\x64\Release\KMDFDriver_Minifilter.sys
// Virtual Machine:                 Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move App:                        Move ConsoleApp_MinifilterInstallation.exe (Host) to C:\Users\%USERNAME%\Downloads\ConsoleApp_MinifilterInstallation.exe (VM)
// Move Driver:                     Copy KMDFDriver_Minifilter.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_Minifilter.sys (VM)
// Enable Test Mode:                Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:
    // Option 1: Manual Registration with sc.exe
            // 1.1 Register Minifilter: Open a CMD window as Administrator -> sc.exe create WindowsKernelMinifilter type=filesys start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_Minifilter.sys"
            // 1.2 Set Required Registry Keys: Run ConsoleApp_MinifilterInstallation.exe as Administrator
    // or
    // Option 2: Using OSR Driver Loader
            // 2.1 Open OSR Driver Loader -> Select KMDFDriver_Minifilter.sys
            // 2.2 Set type to MiniFilter -> Click Register Service
            // 2.3 Click Start Service to load the driver
    // or
    // Option 3: Auto-Configuration via Driver Entry
            // 3.1 Uncomment the registry setup code inside DriverEntry in KMDFDriver_Minifilter
            // 3.2 Compile and rebuild the driver
            // 3.3 Loading the driver will automatically set the required registry keys
// Check Registered Driver:         Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelMinifilter
// Monitor Minifilter Messages:     Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Load:                            Run in CMD as Administrator -> fltmc load WindowsKernelMinifilter
// List Minifilters:                Run in CMD as Administrator -> fltmc
// Unload:                          Run in CMD as Administrator -> fltmc unload WindowsKernelMinifilter
// Remove:                          Run in CMD as Administrator -> sc.exe delete WindowsKernelMinifilter



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://en.wikibooks.org/wiki/Windows_Programming/windows.h
// The primary C header file for accessing the Win32 API is the <windows.h> header file. To make a Win32 executable, the first step is to include this header file in your source code. The windows.h header file should be included before any other library include, even the C standard library files such as stdio.h or stdlib.h. This is because the windows.h file includes macros and other components that may modify, extend, or replace things in these libraries. This is especially true when dealing with UNICODE, because windows.h will cause all the string functions to use UNICODE instead. Also, because many of the standard C library functions are already included in the Windows kernel, many of these functions will be available to the programmer without needing to load the standard libraries. For example, the function sprintf is included in windows.h automatically. 
#include <windows.h>

// https://en.wikibooks.org/wiki/C_Programming/stdio.h
// The C programming language provides many standard library functions for file input and output. These functions make up the bulk of the C standard library header <stdio.h>. The I/O functionality of C is fairly low-level by modern standards; C abstracts all file operations into operations on streams of bytes, which may be "input streams" or "output streams". Unlike some earlier programming languages, C has no direct support for random-access data files; to read from a record in the middle of a file, the programmer must create a stream, seek to the middle of the file, and then read bytes in sequence from the stream. 
#include <stdio.h>

// https://en.wikibooks.org/wiki/C_Programming/stdlib.h
// stdlib.h is the header of the general purpose standard library of C programming language which includes functions involving memory allocation, process control, conversions and others. It is compatible with C++ and is known as cstdlib in C++. The name "stdlib" stands for "standard library". 
#include <stdlib.h>

// https://en.wikibooks.org/wiki/C_Programming/wchar.h
// wchar.h is a header file in the C standard library. It is a part of the extension to the C programming language standard done in 1995. It contains extended multibyte and wide character utilities. The standard header <wchar.h> is included to perform input and output operations on wide streams. It can also be used to manipulate the wide strings.
#include <wchar.h>



// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------
// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------



// Subkey name for minifilter instances
#define MACRO_INSTANCES_SUBKEY L"Instances"
// Key name for storing altitude and flags
#define MACRO_DEFAULT_INSTANCE_KEY L"AltitudeAndFlags"
// Name of the default instance
#define MACRO_DEFAULT_INSTANCE_VALUE_NAME L"DefaultInstance"
// Registry value for altitude
#define MACRO_ALTITUDE_VALUE_NAME L"Altitude"
// Default altitude for the minifilter
#define MACRO_DEFAULT_ALTITUDE L"320000"
// Registry value for flags
#define MACRO_FLAGS_VALUE_NAME L"Flags"
// Service name
#define MACRO_MY_SERVICE L"WindowsKernelMinifilter"



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief		Ensures that the program is running with administrative privileges.
**/
void checkAdminPrivileges()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	BOOL isAdmin = FALSE;
	HANDLE hToken = NULL;


	// ---------------------------------------------------------------------------------------------------------------------
	// Open the current process token

	// The OpenProcessToken function opens the access token associated with a process.
	// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocesstoken

	// Retrieves a pseudo handle for the current process.
	// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentprocess
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Variables
		TOKEN_ELEVATION elevation;
		DWORD size;


		// -----------------------------------------------------------------------------------------------------------------
		// Get the elevation level of the process

		// The GetTokenInformation function retrieves a specified type of information about an access token. The calling process must have appropriate access rights to obtain the information.
		// https://learn.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-gettokeninformation
		if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &size))
		{
			isAdmin = elevation.TokenIsElevated;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Closes an open object handle.
		// https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
		CloseHandle(hToken);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// If the program is not running as an administrator, display an error and exit
	if (!isAdmin)
	{
		printf("Benthic Zone Minifilter -> ConsoleAppMinifilter [Application.c] (checkAdminPrivileges) - ERROR: This program must be run as an administrator.\n");
		exit(1);
	}
}



/**
	@brief		Uses the fltmc command to list currently loaded minifilter drivers. This helps the user verify active minifilters and potential altitude conflicts.
**/
void listLoadedMinifilters()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Message
	printf("Currently loaded minifilters (via fltmc):\n");
	printf("------------------------------------------------------\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Execute the fltmc command to list minifilters

	// Creates a pipe and executes a command.
	// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/popen-wpopen?view=msvc-170
	FILE* fp = _popen("fltmc", "r");

	// Failed
	if (fp == NULL)
	{
		printf("Benthic Zone Minifilter -> ConsoleAppMinifilter [Application.c] (listLoadedMinifilters) - ERROR: Unable to execute fltmc. Ensure it is available in your system PATH.\n");
		return;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Read and print the command output
	char buffer[256];
	while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		printf("%s", buffer);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Close pipe

	// Waits for a new command processor and closes the stream on the associated pipe.
	// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/pclose?view=msvc-170
	_pclose(fp);


	// ---------------------------------------------------------------------------------------------------------------------
	// Message
	printf("------------------------------------------------------\n");
}



/**
	@brief		Main function that configures the registry settings for a Windows minifilter driver.
**/
int main()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Ensure the program is running with administrator privileges
	checkAdminPrivileges();


	// ---------------------------------------------------------------------------------------------------------------------
	// Display currently loaded minifilters for reference
	// listLoadedMinifilters();


	// ---------------------------------------------------------------------------------------------------------------------
	// Define registry handles
	HKEY hServiceKey = NULL, hInstancesKey = NULL, hDefaultInstanceKey = NULL;
	LONG result;
	DWORD flags = 0;


	// ---------------------------------------------------------------------------------------------------------------------
	// Build the registry path for the service
	wchar_t serviceRegistryPath[512];
	swprintf(serviceRegistryPath, 512, L"SYSTEM\\CurrentControlSet\\Services\\%s", MACRO_MY_SERVICE);


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the minifilter service exists in the registry

	// Opens the specified registry key.
	// https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regopenkeyexa
	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, serviceRegistryPath, 0, KEY_READ | KEY_WRITE, &hServiceKey);

	// Failed
	if (result != ERROR_SUCCESS)
	{
		wprintf(L"Benthic Zone Minifilter -> ConsoleAppMinifilter [Application.c] (main) - ERROR: The service '%s' does not exist. Please create it first.\n", MACRO_MY_SERVICE);
		return 1;
	}

	// wprintf(L"Benthic Zone Minifilter -> ConsoleAppMinifilter [Application.c] (main) - The service '%s' exists. Configuring registry keys...\n", MACRO_MY_SERVICE);


	// ---------------------------------------------------------------------------------------------------------------------
	// Step 1: Create the Instances subkey

	// Creates the specified registry key. If the key already exists, the function opens it.
	// https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regcreatekeyexa
	result = RegCreateKeyEx(hServiceKey, MACRO_INSTANCES_SUBKEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hInstancesKey, NULL);

	// Failed
	if (result != ERROR_SUCCESS)
	{
		wprintf(L"Benthic Zone Minifilter -> ConsoleAppMinifilter [Application.c] (main) - ERROR: Could not create the Instances subkey. Error code: 0x%lX\n", result);

		// Closes a handle to the specified registry key.
		// https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regclosekey
		RegCloseKey(hServiceKey);
		return 1;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Step 2: Set the DefaultInstance value

	// Sets the data and type of a specified value under a registry key.
	// https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regsetvalueexa
	result = RegSetValueEx(hInstancesKey, MACRO_DEFAULT_INSTANCE_VALUE_NAME, 0, REG_SZ, (const BYTE*)MACRO_DEFAULT_INSTANCE_KEY, (DWORD)((wcslen(MACRO_DEFAULT_INSTANCE_KEY) + 1) * sizeof(wchar_t)));

	// Failed
	if (result != ERROR_SUCCESS)
	{
		wprintf(L"Benthic Zone Minifilter -> ConsoleAppMinifilter [Application.c] (main) - ERROR: Could not set the DefaultInstance value. Error code: 0x%lX\n", result);

		// Closes a handle to the specified registry key.
		// https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regclosekey
		RegCloseKey(hInstancesKey);
		RegCloseKey(hServiceKey);
		return 1;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Step 3: Create the AltitudeAndFlags subkey

	// Creates the specified registry key. If the key already exists, the function opens it.
	// https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regcreatekeyexa
	result = RegCreateKeyEx(hInstancesKey, MACRO_DEFAULT_INSTANCE_KEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hDefaultInstanceKey, NULL);

	// Failed
	if (result != ERROR_SUCCESS)
	{
		wprintf(L"Benthic Zone Minifilter -> ConsoleAppMinifilter [Application.c] (main) - ERROR: Could not create the AltitudeAndFlags key. Error code: 0x%lX\n", result);

		// Closes a handle to the specified registry key.
		// https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regclosekey
		RegCloseKey(hInstancesKey);
		RegCloseKey(hServiceKey);
		return 1;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Step 4: Set the Altitude value

	// Sets the data and type of a specified value under a registry key.
	// https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regsetvalueexa
	result = RegSetValueEx(hDefaultInstanceKey, MACRO_ALTITUDE_VALUE_NAME, 0, REG_SZ, (const BYTE*)MACRO_DEFAULT_ALTITUDE, (DWORD)((wcslen(MACRO_DEFAULT_ALTITUDE) + 1) * sizeof(wchar_t)));

	// Failed
	if (result != ERROR_SUCCESS)
	{
		wprintf(L"Benthic Zone Minifilter -> ConsoleAppMinifilter [Application.c] (main) - ERROR: Could not set the Altitude value. Error code: 0x%lX\n", result);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Step 5: Set the Flags value

	// Sets the data and type of a specified value under a registry key.
	// https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regsetvalueexa
	result = RegSetValueEx(hDefaultInstanceKey, MACRO_FLAGS_VALUE_NAME, 0, REG_DWORD, (const BYTE*)&flags, sizeof(flags));

	// Failed
	if (result != ERROR_SUCCESS)
	{
		wprintf(L"Benthic Zone Minifilter -> ConsoleAppMinifilter [Application.c] (main) - ERROR: Could not set the Flags value. Error code: 0x%lX\n", result);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Close all registry handles

	// Closes a handle to the specified registry key.
	// https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regclosekey
	RegCloseKey(hDefaultInstanceKey);
	RegCloseKey(hInstancesKey);
	RegCloseKey(hServiceKey);


	// ---------------------------------------------------------------------------------------------------------------------
	// Message
	wprintf(L"Everything is set up for service %s\n", MACRO_MY_SERVICE);
	return 0;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------