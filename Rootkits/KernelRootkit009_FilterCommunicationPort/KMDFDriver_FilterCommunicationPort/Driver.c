// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:                            KernelRootkit009_FilterCommunicationPort
// IDE:                             Open Visual Studio
// Template:                        Create a new project -> Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:                         Project name: KMDFDriver_FilterCommunicationPort -> Solution Name: KernelRootkit009_FilterCommunicationPort -> Create
// Source File:                     Source Files -> Add -> New Item... -> Driver.c
// Source Code:                     Open Driver.c and copy the corresponding source code
// Library Dependencies:            Open Project Properties -> Linker -> Input-> Additional Dependencies -> Add '$(DDK_LIB_PATH)fltMgr.lib;'
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Add Project:                     In Solution Explorer -> Right-click the solution (KernelRootkit009_FilterCommunicationPort) -> Add -> New Project...
// Template:                        Search for templates (Alt + S) -> Console App -> Next
// Project:                         Project Name: ConsoleApp_FilterCommunicationPort -> Create
// Source File:                     In Solution Explorer -> Find ConsoleApp_FilterCommunicationPort.cpp -> Rename to Application.c
// Source Code:                     Open Application.c and copy the corresponding source code
// Library Dependencies:            Open Project Properties -> Linker -> Input-> Additional Dependencies -> Add 'FltLib.lib;'
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Locate Driver:                   C:\Users\%USERNAME%\source\repos\KernelRootkit009_FilterCommunicationPort\x64\Release\KMDFDriver_FilterCommunicationPort.sys
// Locate App:                      C:\Users\%USERNAME%\source\repos\KernelRootkit009_FilterCommunicationPort\x64\Release\ConsoleApp_FilterCommunicationPort.exe
// Virtual Machine:                 Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move Driver:                     Copy KMDFDriver_FilterCommunicationPort.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_FilterCommunicationPort.sys (VM)
// Move App:                        Move ConsoleApp_FilterCommunicationPort.exe (Host) to C:\Users\%USERNAME%\Downloads\ConsoleApp_FilterCommunicationPort.exe (VM)
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


// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/
// This header is used by ifsk.
#include <fltkernel.h>
#include <dontuse.h>
#include <suppress.h>


// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/
// This header contains reference material that includes specific details about the routines, structures, and data types that you will need to use to write kernel-mode drivers.
#include <wdm.h>



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



// Global pointers for communication ports
PFLT_PORT gServerPort = NULL;
PFLT_PORT gClientPort = NULL;

// Global filter handle
PFLT_FILTER gFilterHandle = NULL;



// START -> FUNCTION PROTOTYPES ---------------------------------------------------------------------------------------------------------------
// START -> FUNCTION PROTOTYPES ---------------------------------------------------------------------------------------------------------------



NTSTATUS MinifilterUnload(FLT_FILTER_UNLOAD_FLAGS Flags);
NTSTATUS MinifilterConnect(PFLT_PORT ClientPort, PVOID ServerPortCookie, PVOID Context, ULONG Size, PVOID ConnectionCookie);
VOID MinifilterDisconnect(PVOID ConnectionCookie);
NTSTATUS MinifilterSendReceive(PVOID PortCookie, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength, PULONG ReturnLength);



// START -> CALLBACK AND FILTER DEFINITIONS ---------------------------------------------------------------------------------------------------
// START -> CALLBACK AND FILTER DEFINITIONS ---------------------------------------------------------------------------------------------------



// Callback registration array (empty as no specific operations are filtered)
// The FLT_OPERATION_REGISTRATION structure is used to register operation callback routines.
// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_operation_registration
const FLT_OPERATION_REGISTRATION OperationCallbacks[] = {
	{IRP_MJ_OPERATION_END}
};

// Filter registration structure
// The FLT_REGISTRATION structure is passed as a parameter to FltRegisterFilter.
// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_registration
const FLT_REGISTRATION FilterRegistration = {
	sizeof(FLT_REGISTRATION),
	FLT_REGISTRATION_VERSION,
	0,
	NULL,
	OperationCallbacks,
	MinifilterUnload,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief		Unloads the minifilter driver and performs cleanup operations.
**/
NTSTATUS
MinifilterUnload(
	_In_		FLT_FILTER_UNLOAD_FLAGS		Flags
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Prevent warnings for unused parameters
	UNREFERENCED_PARAMETER(Flags);


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate the driver is being unloaded
	DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (MinifilterUnload) - Unload routine invoked");
	DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (MinifilterUnload) - Unloading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the server communication port is valid before attempting to close it
	if (gServerPort)
	{
		// -----------------------------------------------------------------------------------------------------------------
		// The FltCloseCommunicationPort function is being called to close the server-side communication port
		DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (MinifilterUnload) - The FltCloseCommunicationPort function is being called to close the server communication port\n");

		// FltCloseCommunicationPort closes a minifilter driver's communication server port.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltclosecommunicationport
		FltCloseCommunicationPort(gServerPort);

		// -----------------------------------------------------------------------------------------------------------------
		// Nullify the server port handle to avoid dangling pointers
		gServerPort = NULL;

		// -----------------------------------------------------------------------------------------------------------------
		// Message
		DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (MinifilterUnload) - Server communication port successfully closed\n");
	}

	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the global filter handle is valid before attempting to unregister
	if (gFilterHandle)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// The FltUnregisterFilter function is being called to unregister the minifilter driver and clean up its internal structures
		DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (MinifilterUnload) - The FltUnregisterFilter function is being called to unregister the minifilter driver and clean up its internal structures\n");

		// A registered minifilter driver calls FltUnregisterFilter to unregister itself so that the Filter Manager no longer calls it to process I/O operations.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltunregisterfilter
		FltUnregisterFilter(gFilterHandle);

		// -----------------------------------------------------------------------------------------------------------------
		// Nullify the global handle to avoid dangling pointers
		gFilterHandle = NULL;


		// -----------------------------------------------------------------------------------------------------------------
		// Message
		DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (MinifilterUnload) - Filter successfully unregistered\n");
	}
	else
	{
		DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (MinifilterUnload) - ERROR: No filter handle found to unregister.\n");
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Handles connection requests from user-mode applications to the communication port.


	@see		PFLT_PORT					https://microsoft.github.io/windows-docs-rs/doc/windows/Wdk/Storage/FileSystem/Minifilters/struct.PFLT_PORT.html
	@param[in]	ClientPort					Handle to the client communication port.

	@see		PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[in]	ServerPortCookie			Context information associated with the server port.

	@see		PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[in]	Context						Optional context from the user-mode application.

	@see		ULONG						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#ULONG
	@param[in]	Size						Size of the context data.

	@see		PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[out]	ConnectionCookie			Connection-specific context.


	@return		STATUS_SUCCESS
**/
NTSTATUS
MinifilterConnect(
	_In_		PFLT_PORT					ClientPort,
	_In_		PVOID						ServerPortCookie,
	_In_		PVOID						Context,
	_In_		ULONG						Size,
	_Out_		PVOID						ConnectionCookie
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Prevent warnings for unused parameters
	UNREFERENCED_PARAMETER(ServerPortCookie);
	UNREFERENCED_PARAMETER(Context);
	UNREFERENCED_PARAMETER(Size);
	UNREFERENCED_PARAMETER(ConnectionCookie);


	// ---------------------------------------------------------------------------------------------------------------------
	// Store the client port handle for communication
	gClientPort = ClientPort;


	// ---------------------------------------------------------------------------------------------------------------------
	// Connection established with user-mode application
	DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (MinifilterConnect) - Connection established with user-mode application\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Handles disconnection requests from the user-mode application.


	@see		PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[in]	ConnectionCookie			Connection-specific context (unused).
**/
VOID
MinifilterDisconnect(
	_In_		PVOID						ConnectionCookie
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Prevent warnings for unused parameters
	UNREFERENCED_PARAMETER(ConnectionCookie);


	// ---------------------------------------------------------------------------------------------------------------------
	// Disconnection received from user-mode application
	DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (MinifilterDisconnect) - Disconnection received from user-mode application\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Close the client communication port if it is still open
	if (gClientPort)
	{
		// FltCloseClientPort closes a communication client port.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltcloseclientport
		FltCloseClientPort(gFilterHandle, &gClientPort);
		gClientPort = NULL;
	}
}



/**
	@brief		Handles send and receive operations for the communication port.
	@details	This function processes messages received from the user-mode application and responds with a predefined kernel message.


	@see		PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[in]	PortCookie					Port-specific context.

	@see		PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[in]	InputBuffer					Pointer to the buffer containing data sent from the user-mode application.

	@see		PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[in]	InputBufferLength			Length, in bytes, of the input buffer provided by the caller.

	@see		PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[in]	OutputBuffer				Pointer to the buffer where the response from the driver will be written.

	@see		PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[in]	OutputBufferLength			Pointer to a variable that receives the number of bytes actually written to the output buffer.

	@see		PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[out]	ReturnLength				Pointer to a variable that receives the number of bytes actually written to the output buffer.


	@return		STATUS_SUCCESS
**/
NTSTATUS
MinifilterSendReceive(
	_In_		PVOID						PortCookie,
	_In_		PVOID						InputBuffer,
	_In_		ULONG						InputBufferLength,
	_Out_		PVOID						OutputBuffer,
	_In_		ULONG						OutputBufferLength,
	_Out_		PULONG						ReturnLength
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Prevent warnings for unused parameters
	UNREFERENCED_PARAMETER(PortCookie);
	UNREFERENCED_PARAMETER(InputBufferLength);
	UNREFERENCED_PARAMETER(OutputBufferLength);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	PCHAR responseMessage = "Kernel response message";


	// ---------------------------------------------------------------------------------------------------------------------
	// Handle message exchange between user-mode and kernel-mode
	DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (MinifilterSendReceive) - Message from user: %s\n", (PCHAR)InputBuffer);


	// ---------------------------------------------------------------------------------------------------------------------
	// Prepare and send response. A predefined response message is copied to the output buffer, ensuring the user-mode application receives a valid response from the kernel.
	strcpy((PCHAR)OutputBuffer, responseMessage);
	*ReturnLength = (ULONG)strlen(responseMessage) + 1;


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Entry point for the minifilter driver.
	@details	This function is called when the minifilter driver is loaded into memory. It initializes the driver, registers the filter with the Windows Filter Manager, and starts filtering operations.


	@see		PDRIVER_OBJECT		https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param[in]	DriverObject		Pointer to the driver object representing the minifilter driver in the Windows kernel.

	@see		PUNICODE_STRING		https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param[in]	RegistryPath		Pointer to the registry path where the driver’s configuration settings are stored.


	@return		A NTSTATUS value indicating success or failure of the driver initialization.
**/
NTSTATUS
DriverEntry(
	_In_		PDRIVER_OBJECT		DriverObject,
	_In_		PUNICODE_STRING		RegistryPath
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Prevent warnings for unused parameters
	UNREFERENCED_PARAMETER(RegistryPath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;
	PSECURITY_DESCRIPTOR sd;
	OBJECT_ATTRIBUTES oa = {0};
	UNICODE_STRING portName = RTL_CONSTANT_STRING(L"\\FilterCommunicationPort");


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello message
	DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message indicating that DriverEntry (main function) has been invoked
	DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - Initializing minifilter driver");


	// ---------------------------------------------------------------------------------------------------------------------
	// Register the minifilter with the Filter Manager
	DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - Registering filter with Filter Manager");

	// FltRegisterFilter registers a minifilter driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltregisterfilter
	status = FltRegisterFilter(DriverObject, &FilterRegistration, &gFilterHandle);

	// Success
	if (NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - Filter registered successfully");


		// FltBuildDefaultSecurityDescriptor builds a default security descriptor for use with FltCreateCommunicationPort.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltbuilddefaultsecuritydescriptor
		status = FltBuildDefaultSecurityDescriptor(&sd, FLT_PORT_ALL_ACCESS);

		// Success
		if (NT_SUCCESS(status))
		{
			DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - Security descriptor built successfully");

			// The InitializeObjectAttributes macro initializes the opaque OBJECT_ATTRIBUTES structure, which specifies the properties of an object handle to routines that open handles.
			// https://learn.microsoft.com/en-us/windows/win32/api/ntdef/nf-ntdef-initializeobjectattributes
			InitializeObjectAttributes(&oa, &portName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, sd);

			// FltCreateCommunicationPort creates a communication server port on which a minifilter can receive connection requests from user-mode applications and services.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltcreatecommunicationport
			status = FltCreateCommunicationPort(gFilterHandle, &gServerPort, &oa, NULL, MinifilterConnect, MinifilterDisconnect, MinifilterSendReceive, 1);

			// FltFreeSecurityDescriptor frees a security descriptor allocated by the FltBuildDefaultSecurityDescriptor routine.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltfreesecuritydescriptor
			FltFreeSecurityDescriptor(sd);

			// Success
			if (NT_SUCCESS(status))
			{
				DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - Communication port created successfully");


				// ---------------------------------------------------------------------------------------------------------
				// Start filtering operations

				// The FltStartFiltering routine enables a registered minifilter driver to start filtering I/O operations.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltstartfiltering
				status = FltStartFiltering(gFilterHandle);

				if (NT_SUCCESS(status))
				{
					DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - Filtering started successfully");
				}
				else
				{
					DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - ERROR: Failed to start filtering.");
				}
			}
			else
			{
				DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - ERROR: Failed to create communication port.");
			}
		}
		else
		{
			DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - ERROR: Failed to build security descriptor.");
		}

		// Failed
		if (!NT_SUCCESS(status))
		{
			DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - ERROR: Unregistering filter due to failure.");

			// The FltUnregisterFilter routine unregisters a minifilter driver and cleans up its internal structures.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltunregisterfilter
			FltUnregisterFilter(gFilterHandle);
		}
	}
	else
	{
		DbgPrint("Benthic Zone FilterCommunicationPort -> KMDFDriverFilterCommunicationPort [Driver.c] (DriverEntry) - ERROR: Failed to register filter with Filter Manager.");
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return status;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------