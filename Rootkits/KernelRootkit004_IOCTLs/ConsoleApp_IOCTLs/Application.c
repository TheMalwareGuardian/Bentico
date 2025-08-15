// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:                            KernelRootkit004_IOCTLs
// IDE:                             Open Visual Studio
// Template:                        Create a new project -> Search for templates (Alt + S) -> Console App -> Next
// Project:                         Project Name: ConsoleApp_IOCTLs -> Solution Name: KernelRootkit004_IOCTLs -> Create
// Source File:                     In Solution Explorer -> Find ConsoleApp_IOCTLs.cpp -> Rename to Application.c
// Source Code:                     Open Application.c and copy the corresponding source code
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Add Project:                     In Solution Explorer -> Right-click the solution (KernelRootkit004_IOCTLs) -> Add -> New Project...
// Template:                        Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:                         Project name: KMDFDriver_IOCTLs -> Create
// Source File:                     Source Files -> Add -> New Item... -> Driver.c
// Source Code:                     Open Driver.c and copy the corresponding source code
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Locate App:                      C:\Users\%USERNAME%\source\repos\KernelRootkit004_IOCTLs\x64\Release\ConsoleApp_IOCTLs.exe
// Locate Driver:                   C:\Users\%USERNAME%\source\repos\KernelRootkit004_IOCTLs\x64\Release\KMDFDriver_IOCTLs.sys
// Virtual Machine:                 Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move App:                        Move ConsoleApp_IOCTLs.exe (Host) to C:\Users\%USERNAME%\Downloads\ConsoleApp_IOCTLs.exe (VM)
// Move Driver:                     Copy KMDFDriver_IOCTLs.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_IOCTLs.sys (VM)
// Enable Test Mode:                Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:             Open a CMD window as Administrator -> sc.exe create WindowsKernelIOCTLs type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_IOCTLs.sys"
// Registered Driver:               Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelIOCTLs
// Service Status:                  Run in CMD as Administrator -> sc.exe query WindowsKernelIOCTLs -> driverquery.exe
// Registry Entry:                  Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelIOCTLs
// Monitor Messages:                Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:                   Run in CMD as Administrator -> sc.exe start WindowsKernelIOCTLs
// User Mode App:                   Open CMD -> Navigate to the directory -> Run ConsoleApp_IOCTLs.exe
// Clean:                           Run in CMD as Administrator -> sc.exe stop WindowsKernelIOCTLs
// Remove:                          Run in CMD as Administrator -> sc.exe delete WindowsKernelIOCTLs



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

	@details	When defining new IOCTLs, it is important to remember the following rules:
	@details		- If a new IOCTL will be available to user-mode software components, the IOCTL must be used with IRP_MJ_DEVICE_CONTROL requests. User-mode components send IRP_MJ_DEVICE_CONTROL requests by calling the DeviceIoControl, which is a Win32 function.
	@details		- If a new IOCTL will be available only to kernel-mode driver components, the IOCTL must be used with IRP_MJ_INTERNAL_DEVICE_CONTROL requests. Kernel-mode components create IRP_MJ_INTERNAL_DEVICE_CONTROL requests by calling IoBuildDeviceIoControlRequest.

	@details	Use the system-supplied CTL_CODE macro, which is defined in Wdm.h and Ntddk.h, to define new I/O control codes. The definition of a new IOCTL code, whether intended for use with IRP_MJ_DEVICE_CONTROL or IRP_MJ_INTERNAL_DEVICE_CONTROL requests, uses the following format:
	@details		#define IOCTL_Device_Function CTL_CODE(DeviceType, Function, Method, Access)

	@details	Supply the following parameters to the CTL_CODE macro:
	@param			DeviceType			This value must match the value that is set in the DeviceType member of the driver's DEVICE_OBJECT structure (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/specifying-device-types).
	@param			FunctionCode		Identifies the function to be performed by the driver. Values of less than 0x800 are reserved for Microsoft. Values of 0x800 and higher can be used by vendors.
	@param			TransferType		Indicates how the system will pass data between the caller of DeviceIoControl (or IoBuildDeviceIoControlRequest) and the driver that handles the IRP (METHOD_BUFFERED, METHOD_IN_DIRECT, METHOD_OUT_DIRECT, METHOD_NEITHER).
	@param			RequiredAccess		Indicates the type of access that a caller must request when opening the file object that represents the device (FILE_ANY_ACCESS, FILE_READ_DATA, FILE_READ_DATA, FILE_READ_DATA and FILE_WRITE_DATA).
**/
#define IOCTL_COMMAND_0 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_1 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief		Prints a banner with application details and a welcome message
**/
void PrintBanner()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Print the enhanced banner header
	printf("\n");
	printf("=================================================================================================================\n");
	printf("                               Benthic Zone UserModeKernelModeCommunication IOCTLs                               \n");
	printf("=================================================================================================================\n");
	printf("              This application is designed to interact with a kernel-mode driver via IOCTL commands.             \n");
	printf("         Use the menu to send commands and explore how user-mode applications communicate with the kernel        \n");
	printf("=================================================================================================================\n");
	printf(".................................................................................................................\n");
	printf("=================================================================================================================\n");
	// ---------------------------------------------------------------------------------------------------------------------
	// Explain the purpose of each command
	printf("                                          Explanation of IOCTL commands                                          \n");
	printf("=================================================================================================================\n");
	printf("   0. IOCTL_COMMAND_0:   Sends a basic command to the kernel driver. No additional input or output is involved.  \n");
	printf("   1. IOCTL_COMMAND_1:   Sends a command to the kernel driver and receives an output message in response.        \n");
	printf("   2. IOCTL_COMMAND_2:   Sends a command to the kernel driver with an input message and expects a response.      \n");
	printf("   3. Exit:              Closes the handle to the kernel driver and exits the application.                       \n");
	printf("=================================================================================================================\n");
	printf("\n");
}



/**
	@brief		Main entry point
**/
int main()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables

	// Handle for interacting with the driver
	HANDLE hDevice;
	// Stores the number of bytes returned
	DWORD bytesReturned;
	// Indicates the success of an operation
	BOOL success;
	// Stores the user's menu selection
	int option;
	// Input buffer for data to be sent to the driver
	char inBuffer[15] = { 0 };
	// Output buffer for data received from the driver
	char outBuffer[15] = { 0 };


	// ---------------------------------------------------------------------------------------------------------------------
	// Banner
	PrintBanner();


	// ---------------------------------------------------------------------------------------------------------------------
	// Open I/O device

	// The function returns a handle that can be used to access the file or device for various types of I/O depending on the file or device and the flags and attributes specified.
	// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
	hDevice = CreateFile(L"\\\\.\\MyKernelDriverIOCTLs", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	
	// Error
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Benthic Zone IOCTLs -> ConsoleAppIOCTLs [Application.c] (main) - ERROR: Failed to access the device (MyKernelDriverIOCTLs). Please make sure the driver is installed.\n");
		return 1;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Loop
	while (1)
	{

		// Menu
		printf("\nMenu:\n");
		printf("0. Send IOCTL_COMMAND_0\n");
		printf("1. Send IOCTL_COMMAND_1\n");
		printf("2. Send IOCTL_COMMAND_2\n");
		printf("3. Exit\n");
		printf("Select an option: ");


		// Ask user. Reads formatted data from the standard input stream.
		// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/scanf-s-scanf-s-l-wscanf-s-wscanf-s-l
		scanf_s("%d", &option);


		// Handle different options
		switch (option)
		{
		
			// Send IOCTL_COMMAND_0
			case 0:
				// Sends a control code directly to a specified device driver, causing the corresponding device to perform the corresponding operation.
				// https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-deviceiocontrol
				success = DeviceIoControl(hDevice, IOCTL_COMMAND_0, NULL, 0, NULL, 0, &bytesReturned, NULL);
				if (success)
				{
					printf("Benthic Zone IOCTLs -> ConsoleAppIOCTLs [Application.c] (main) - IOCTL_COMMAND_0 sent successfully\n");
				}
				else
				{
					printf("Benthic Zone IOCTLs -> ConsoleAppIOCTLs [Application.c] (main) - ERROR: Error sending IOCTL_COMMAND_0 (Error Code: %lu).\n", GetLastError());
				}
				break;
			
			// Send IOCTL_COMMAND_1 and receive a message from the kernel driver
			case 1:
				// Sends a control code directly to a specified device driver, causing the corresponding device to perform the corresponding operation.
				// https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-deviceiocontrol
				success = DeviceIoControl(hDevice, IOCTL_COMMAND_1, NULL, 0, outBuffer, sizeof(outBuffer), &bytesReturned, NULL);
				if (success)
				{
					printf("Benthic Zone IOCTLs -> ConsoleAppIOCTLs [Application.c] (main) - IOCTL_COMMAND_1 sent successfully\n");
					printf("Benthic Zone IOCTLs -> ConsoleAppIOCTLs [Application.c] (main) - Message received from kernel driver: %s\n", outBuffer);
				}
				else
				{
					printf("Benthic Zone IOCTLs -> ConsoleAppIOCTLs [Application.c] (main) - ERROR: Error sending IOCTL_COMMAND_1 (Error Code: %lu).\n", GetLastError());
				}
				break;
			
			// Send IOCTL_COMMAND_2 with a message to the kernel driver and receive a response
			case 2:
				// Initialize input buffer. Copies a string.
				// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strcpy-s-wcscpy-s-mbscpy-s?view=msvc-170
				strcpy_s(inBuffer, 13, "Hello Kernel");
				// Sends a control code directly to a specified device driver, causing the corresponding device to perform the corresponding operation.
				// https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-deviceiocontrol
				success = DeviceIoControl(hDevice, IOCTL_COMMAND_2, inBuffer, sizeof(inBuffer), outBuffer, sizeof(outBuffer), &bytesReturned, NULL);
				if (success)
				{
					printf("Benthic Zone IOCTLs -> ConsoleAppIOCTLs [Application.c] (main) - IOCTL_COMMAND_2 sent successfully\n");
					printf("Benthic Zone IOCTLs -> ConsoleAppIOCTLs [Application.c] (main) - Message sent to kernel driver: %s\n", inBuffer);
					printf("Benthic Zone IOCTLs -> ConsoleAppIOCTLs [Application.c] (main) - Message received from kernel driver: %s\n", outBuffer);
				}
				else
				{
					printf("Benthic Zone IOCTLs -> ConsoleAppIOCTLs [Application.c] (main) - ERROR: Error sending IOCTL_COMMAND_2 (Error Code: %lu).\n", GetLastError());
				}
				break;
			
			// Exit
			case 3:
				// Closes an open object handle.
				// https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
				CloseHandle(hDevice);
				return 0;
			
			// Invalid menu option
			default:
				printf("Benthic Zone IOCTLs -> ConsoleAppIOCTLs [Application.c] (main) - ERROR: Invalid option\n");
				break;
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Closes an open object handle.
	// https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
	CloseHandle(hDevice);


	// ---------------------------------------------------------------------------------------------------------------------
	// Exit
	return 0;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------