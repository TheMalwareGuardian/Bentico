// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:							KernelRootkit011_WinSockKernel
// IDE:								Open Visual Studio
// Template:						Create a new project -> Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:							Project Name: KMDFDriver_WinSockKernel -> Solution Name: KernelRootkit011_WinSockKernel -> Create
// Source File:						Source Files -> Add -> New Item... -> Driver.c
// Source Code:						Open Driver.c and copy the corresponding source code
// Library Dependencies:			Open Project Properties -> Linker -> Additional Dependencies -> Add '$(DDK_LIB_PATH)Netio.lib;'
// LibWSK:							
// Build Project:					Set Configuration to Release, x64 -> Build -> Build Solution
// Locate App:						C:\Users\%USERNAME%\source\repos\KernelRootkit011_WinSockKernel\x64\Release\ConsoleApp_DKOM.exe
// Locate Driver:					C:\Users\%USERNAME%\source\repos\KernelRootkit011_WinSockKernel\x64\Release\KMDFDriver_WinSockKernel.sys
// Virtual Machine:					Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move Driver:						Copy KMDFDriver_WinSockKernel.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_WinSockKernel.sys (VM)
// Enable Test Mode:				Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:				Open a CMD window as Administrator -> sc.exe create WindowsKernelWinSockKernel type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_WinSockKernel.sys"
// Registered Driver:				Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelWinSockKernel
// Service Status:					Run in CMD as Administrator -> sc.exe query WindowsKernelWinSockKernel -> driverquery.exe
// Registry Entry:					Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelWinSockKernel
// Monitor Messages:				Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:					Run in CMD as Administrator -> sc.exe start WindowsKernelWinSockKernel
// Clean:							Run in CMD as Administrator -> sc.exe stop WindowsKernelWinSockKernel
// Remove:							Run in CMD as Administrator -> sc.exe delete WindowsKernelWinSockKernel



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// #pragma warning(disable: 4047)
// #pragma warning(disable: 4024)



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>

// https://github.com/MiroKaku/libwsk
// The Kernel-Mode Winsock library, supporting TCP, UDP and Unix sockets (DGRAM and STREAM). 
#include "libwsk\libwsk.h"



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



// The WSK_SOCKET structure defines a socket object for a socket.
// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wsk/ns-wsk-_wsk_socket
PWSK_SOCKET g_Socket = NULL;



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief		Initializes the Windows Socket Kernel (WSK) environment.

				This function initializes the WSK subsystem, which is required for network communication in a Windows kernel-mode driver.
**/
NTSTATUS
WinSockKernelInitialize()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (WinSockKernelInitialize) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	WSKDATA WskData;
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Start WSK environment
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (WinSockKernelInitialize) - Start WSK environment\n");

	status = WSKStartup(MAKE_WSK_VERSION(1, 0), &WskData);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (WinSockKernelInitialize) - ERROR: WSKStartup() failed. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (WinSockKernelInitialize) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Converts an IPv4 string to a SOCKADDR_IN structure.

				This function takes a string representation of an IPv4 address and converts it into a SOCKADDR_IN structure. It sets the address family to AF_INET and assigns a default port (port 80).
**/
VOID
ConvertIPStringToSockaddr(
	_In_ 		CHAR*					IpString,
	_Out_		SOCKADDR_IN*			SockAddr)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	RtlZeroMemory(SockAddr, sizeof(SOCKADDR_IN));
	SockAddr->sin_family = AF_INET;
	SockAddr->sin_port = RtlUshortByteSwap(80);
	IN_ADDR ipAddr;
	CHAR* TerminatingChar = NULL;


	// ---------------------------------------------------------------------------------------------------------------------
	// Convert IP string to binary format
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (ConvertIPStringToSockaddr) - Calling RtlIpv4StringToAddressA\n");

	// The RtlIpv4StringToAddress function converts a string representation of an IPv4 address to a binary IPv4 address.
	// https://learn.microsoft.com/en-us/windows/win32/api/ip2string/nf-ip2string-rtlipv4stringtoaddressa
	if (RtlIpv4StringToAddressA(IpString, TRUE, &TerminatingChar, &ipAddr) == STATUS_SUCCESS)
	{
		SockAddr->sin_addr.s_addr = ipAddr.S_un.S_addr;
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (ConvertIPStringToSockaddr) - Conversion successful\n");
	}
	else
	{
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (ConvertIPStringToSockaddr) - ERROR: RtlIpv4StringToAddressA() failed to convert IP string to address\n");
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
}



/**
	@brief		Sends an HTTP request to a remote server.

				This function establishes a TCP connection to a remote server, sends an HTTP GET request, and retrieves the server's response.
**/
NTSTATUS
SendHttpRequest()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;
	SOCKADDR_IN serverAddress;
	SIZE_T bytesSent, bytesReceived;
	CHAR request[] = "GET / HTTP/1.1\r\nHost: google.com\r\nConnection: close\r\n\r\n";
	CHAR response[512] = { 0 };


	// ---------------------------------------------------------------------------------------------------------------------
	// Create socket
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - Create socket\n");

	status = WSKSocket(&g_Socket, AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - ERROR: WSKSocket() failed. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Convert IP string to SOCKADDR_IN format
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - Converting IP string to SOCKADDR_IN\n");

	ConvertIPStringToSockaddr("142.250.200.78", &serverAddress);


	// ---------------------------------------------------------------------------------------------------------------------
	// Connect to remote server
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - Connect to remote server\n");

	status = WSKConnect(g_Socket, (PSOCKADDR)&serverAddress, sizeof(serverAddress));
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - ERROR: WSKConnect() failed. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Send HTTP request
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - Send HTTP request\n");

	status = WSKSend(g_Socket, request, sizeof(request) - 1, &bytesSent, 0, NULL, NULL);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - ERROR: WSKSend() failed. Status: 0x%08X\n", status);
		return status;
	}

	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - Sent %llu bytes\n", bytesSent);


	// ---------------------------------------------------------------------------------------------------------------------
	// Receive HTTP response
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - Receive HTTP response\n");

	status = WSKReceive(g_Socket, response, sizeof(response) - 1, &bytesReceived, 0, NULL, NULL);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - ERROR: WSKReceive() failed. Status: 0x%08X\n", status);
		return status;
	}

	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - Received %llu bytes:\n\n%s\n\n", bytesReceived, response);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (SendHttpRequest) - HTTP request completed successfully\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Unloads a Windows kernel-mode driver.

				This function is called when the driver is being unloaded from memory. It is responsible for cleaning up resources and performing necessary cleanup tasks before the driver is removed from the system. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload


				PDRIVER_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param		pDriverObject			Pointer to a DRIVER_OBJECT structure representing the driver.
**/
VOID
DriverUnload(
	_In_		PDRIVER_OBJECT			pDriverObject
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverUnload) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate the driver is being unloaded
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverUnload) - Unload routine invoked");
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverUnload) - Unloading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the device object, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pDriverObject);


	// ---------------------------------------------------------------------------------------------------------------------
	// Close socket if open
	if (g_Socket)
	{
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverUnload) - Closing socket\n");

		WSKCloseSocket(g_Socket);
		g_Socket = NULL;
	}
	else
	{
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverUnload) - No open socket to close\n");
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Cleanup WSK
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverUnload) - Cleanup WSK\n");

	WinSockKernelCleanup();


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverUnload) - Bye\n");
}



/**
	@brief		Entry point for a Windows kernel-mode driver.
	
				This function is called when the driver is loaded into memory. It initializes the driver and performs necessary setup tasks. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/wdf/driverentry-for-kmdf-drivers


				PDRIVER_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param		pDriverObject			Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.

				PUNICODE_STRING			https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param		pRegistryPath			Pointer to a UNICODE_STRING structure, containing the driver's registry path as a Unicode string, indicating the driver's location in the Windows registry.


	@return		A NTSTATUS value indicating success or an error code if initialization fails.
**/
NTSTATUS
DriverEntry(
	_In_		PDRIVER_OBJECT			pDriverObject,
	_In_		PUNICODE_STRING			pRegistryPath
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverEntry) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate DriverEntry (main) function has been invoked
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverEntry) - DriverEntry routine invoked");
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverEntry) - Loading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the registry path, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pRegistryPath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Set DriverUnload routine
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverEntry) - Set DriverUnload routine");

	// The Unload routine performs any operations that are necessary before the system unloads the driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
	pDriverObject->DriverUnload = DriverUnload;


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize WinSock Kernel (WSK)
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverEntry) - Initialize WinSock Kernel (WSK)\n");

	status = WinSockKernelInitialize();
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverEntry) - ERROR: WinSockKernelInitialize() failed. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Send HTTP request
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverEntry) - Send HTTP request\n");

	status = SendHttpRequest();
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverEntry) - ERROR: SendHttpRequest() failed. Status: 0x%08X\n", status);
		
		// Cleaning up WSK due to failure
		DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverEntry) - Cleaning up WSK due to failure\n");
		WinSockKernelCleanup();

		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone WinSockKernel -> KMDFDriverWinSockKernel [Driver.c] (DriverEntry) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Driver initialization was completed successfully
	return STATUS_SUCCESS;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------