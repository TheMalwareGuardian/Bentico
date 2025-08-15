// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/
// If you use the safe string functions instead of the string manipulation functions that are provided by C-language run-time libraries, you protect your code from buffer overrun errors that can make code untrustworthy. This header declares functions that copy, concatenate, and format strings in a manner that prevents buffer overrun errors.
#include <ntstrsafe.h>



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "Utils00Resources.h"



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief          Converts a STRUCTURE_UTILSRESOURCES_TYPE enum value to its string representation.

	
	@param[in]      type                                        The resource type to convert.
	
	
	@return         A constant string representing the resource type.
**/
const char* UtilsResources_TypeToString(
	_In_            STRUCTURE_UTILSRESOURCES_TYPE               type
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Match enum values with readable strings
	switch (type)
	{
		case STRUCTURE_UTILSRESOURCES_TYPE_PROCESS:     return "PROCESS";
		case STRUCTURE_UTILSRESOURCES_TYPE_FILE:        return "FILE";
		case STRUCTURE_UTILSRESOURCES_TYPE_FOLDER:      return "FOLDER";
		case STRUCTURE_UTILSRESOURCES_TYPE_IP:          return "IP";
		case STRUCTURE_UTILSRESOURCES_TYPE_PORT:        return "PORT";
		case STRUCTURE_UTILSRESOURCES_TYPE_CONNECTION:  return "CONNECTION";
		case STRUCTURE_UTILSRESOURCES_TYPE_REGISTRY:    return "REGISTRY";
		case STRUCTURE_UTILSRESOURCES_TYPE_OTHER:       return "OTHER";
		default:                                        return "UNKNOWN";
	}
}



/**
	@brief          Converts a STRUCTURE_UTILSRESOURCES_ACTION enum value to its string representation.


	@param[in]      action                                      The resource action to convert.


	@return         A constant string representing the resource action.
**/
const char* UtilsResources_ActionToString(
	_In_            STRUCTURE_UTILSRESOURCES_ACTION             action
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Match enum values with readable strings
	switch (action)
	{
		case STRUCTURE_UTILSRESOURCES_ACTION_HIDE:     return "HIDE";
		case STRUCTURE_UTILSRESOURCES_ACTION_BLOCK:    return "BLOCK";
		case STRUCTURE_UTILSRESOURCES_ACTION_PROTECT:  return "PROTECT";
		case STRUCTURE_UTILSRESOURCES_ACTION_MONITOR:  return "MONITOR";
		default:                                       return "UNKNOWN";
	}
}



/**
	@brief          Recalculates and assigns sequential indexes to each entry in the custom list.


	@param[in,out]  listHead                                    The head of the custom resource list.
**/
static VOID UtilsResources_ReassignIndexes(
	_Inout_         PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY     listHead
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	ULONG newIndex = 0;


	// ---------------------------------------------------------------------------------------------------------------------
	// Iterate over the list and assign new indexes
	while (listHead)
	{
		listHead->Index = newIndex++;
		listHead = listHead->Next;
	}
}



/**
	@brief          Retrieves the PID string associated with a given index in the resource list.


	@param[in]      listHead                                    Head of the resource list.

	@param[in]      index                                       Index to search for.

	@param[out]     outIdentifier                               Buffer to receive the identifier string (PID as string).

	@param[in]      outIdentifierSize                           Size of the buffer.


	@return         TRUE if found, FALSE if index not found.
**/
BOOLEAN
UtilsResources_GetIdentifierByIndex(
	_In_            PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY     listHead,
	_In_            ULONG                                       index,
	_Out_           char*                                       outIdentifier,
	_In_            size_t                                      outIdentifierSize
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Traverse the list to find the resource that matches the given index
	while (listHead)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Check if this entry's index matches the one we're looking for
		if (listHead->Index == index)
		{

			// -------------------------------------------------------------------------------------------------------------
			// Copy the identifier string into the output buffer
			RtlStringCbCopyA(outIdentifier, outIdentifierSize, listHead->Identifier);


			// -------------------------------------------------------------------------------------------------------------
			// Match found, return TRUE
			return TRUE;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Move to the next element in the list
		listHead = listHead->Next;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Index not found in the list
	return FALSE;
}



/**
	@brief          Checks if a resource is already present in the list.


	@param[in]      listHead                                    Head of the list to search.

	@param[in]      type                                        Resource type.

	@param[in]      action                                      Resource action.

	@param[in]      identifier                                  Resource identifier.


	@return         TRUE if found, FALSE otherwise.
**/
BOOLEAN
UtilsResources_IsPresent(
	_In_            PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY     listHead,
	_In_            STRUCTURE_UTILSRESOURCES_TYPE               type,
	_In_            STRUCTURE_UTILSRESOURCES_ACTION             action,
	_In_            const char* identifier
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Traverse the linked list of resources
	while (listHead)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Match by type, action, and identifier string
		if (listHead->Type == type && listHead->Action == action && strcmp(listHead->Identifier, identifier) == 0)
		{

			// -------------------------------------------------------------------------------------------------------------
			// Resource is already present
			return TRUE;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Move to the next entry
		listHead = listHead->Next;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// No match found in the list
	return FALSE;
}



/**
	@brief          Adds a new custom resource to a specific list.


	@param[in,out]  listHead                                    Pointer to the list head to which the entry will be added.

	@param[in,out]  count                                       Pointer to the count of entries in the list.

	@param[in]      type                                        The type of the resource.

	@param[in]      action                                      The action associated with the resource.

	@param[in]      identifier                                  The string identifier for the resource.


	@return         TRUE if added successfully, FALSE otherwise.
**/
BOOLEAN
UtilsResources_Add(
	_Inout_         PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY*    listHead,
	_Inout_         ULONG*                                      count,
	_In_            STRUCTURE_UTILSRESOURCES_TYPE               type,
	_In_            STRUCTURE_UTILSRESOURCES_ACTION             action,
	_In_            const char*                                 identifier
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Utils/Resources.c] (Add) - New resource requested");


	// ---------------------------------------------------------------------------------------------------------------------
	// Allocate memory for the new entry

	// The ExAllocatePool2 routine allocates pool memory of the specified type and returns a pointer to the allocated block.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-exallocatepool2
	PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY newEntry = (PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY)ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(STRUCTURE_UTILSRESOURCES_GENERIC_ENTRY), MACRO_UTILSRESOURCES_POOL_TAG);

	// Failed
	if (!newEntry)
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Utils/Resources.c] (Add) - Failed to allocate resource entry");
		return FALSE;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize entry fields

	// The RtlZeroMemory routine fills a block of memory with zeros, given a pointer to the block and the length, in bytes, to be filled.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlzeromemory
	RtlZeroMemory(newEntry, sizeof(STRUCTURE_UTILSRESOURCES_GENERIC_ENTRY));

	// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
	RtlStringCbCopyA(newEntry->Identifier, sizeof(newEntry->Identifier), identifier);

	// Set metadata fields
	newEntry->Type = type;
	newEntry->Action = action;
	newEntry->Index = (*count);
	newEntry->Next = NULL;


	// ---------------------------------------------------------------------------------------------------------------------
	// Insert at the end of the list

	if (*listHead == NULL)
	{
		// First entry in the list
		*listHead = newEntry;
	}
	else
	{
		// Traverse to the end and append
		PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY current = *listHead;
		while (current->Next != NULL)
		{
			current = current->Next;
		}
		current->Next = newEntry;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Update total count
	(*count)++;


	// ---------------------------------------------------------------------------------------------------------------------
	// Resource was added
	DbgPrint("Benthic Zone -> KernelModeDriver [Utils/Resources.c] (Add) - Resource '%s' (Type=%s, Action=%s, Index=%lu) added\n", identifier, UtilsResources_TypeToString(type), UtilsResources_ActionToString(action), newEntry->Index);


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return TRUE;
}



/**
	@brief          Removes a resource from a list by its index.


	@param[in,out]  listHead                                    Pointer to the list head.

	@param[in,out]  count                                       Pointer to the number of elements in the list.

	@param[in]      index                                       The index of the resource to remove.


	@return         TRUE if removed, FALSE if not found.
**/
BOOLEAN
UtilsResources_Remove(
	_Inout_         PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY*    listHead,
	_Inout_         ULONG*                                      count,
	_In_            ULONG                                       index
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Utils/Resources.c] (Remove) - Resource removal requested");


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY prev = NULL;
	PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY current = *listHead;


	// ---------------------------------------------------------------------------------------------------------------------
	// Search for the resource by index
	while (current)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Check if the current node's index matches the one we want to remove
		if (current->Index == index)
		{

			// -------------------------------------------------------------------------------------------------------------
			// Unlink from the list
			if (prev)
			{
				prev->Next = current->Next;
			}
			else
			{
				*listHead = current->Next;
			}


			// -------------------------------------------------------------------------------------------------------------
			// Log before freeing
			DbgPrint("Benthic Zone -> KernelModeDriver [Utils/Resources.c] (Remove) - Resource '%s' (Type=%s, Action=%s, Index=%lu) removed\n", current->Identifier, UtilsResources_TypeToString(current->Type), UtilsResources_ActionToString(current->Action), current->Index);


			// -------------------------------------------------------------------------------------------------------------
			// Free memory and update count

			// The ExFreePoolWithTag routine deallocates a block of pool memory allocated with the specified tag.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-exfreepoolwithtag
			ExFreePoolWithTag(current, MACRO_UTILSRESOURCES_POOL_TAG);
			(*count)--;


			// -------------------------------------------------------------------------------------------------------------
			// Reassign indexes to maintain consistency after deletion
			UtilsResources_ReassignIndexes(*listHead);


			// -------------------------------------------------------------------------------------------------------------
			// Return
			return TRUE;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Continue traversal
		prev = current;
		current = current->Next;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// No resource found
	DbgPrint("Benthic Zone -> KernelModeDriver [Utils/Resources.c] (Remove) - No resource found at index %lu\n", index);


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return FALSE;
}



/**
	@brief          Writes a formatted list of all resources in the list to the output buffer.


	@param[in]      listHead                                    Head of the resource list.

	@param[out]     responseBuffer                              Buffer to write the formatted list to.

	@param[in]      responseBufferSize                          Size of the output buffer.


	@return         STATUS_SUCCESS if all entries were listed, or STATUS_BUFFER_OVERFLOW if truncated.
**/
NTSTATUS
UtilsResources_List(
	_In_            PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY     listHead,
	_In_            char*                                       responseBuffer,
	_In_            size_t                                      responseBufferSize
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Utils/Resources.c] (List) - Resource list requested");


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status = STATUS_SUCCESS;
	size_t offset = 0;


	// ---------------------------------------------------------------------------------------------------------------------
	// Temporary buffer to store pointers in reverse order (max 512 elements)
	PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY entryArray[512];
	ULONG entryCount = 0;


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize response with a header

	// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
	RtlStringCbCopyA(responseBuffer, responseBufferSize, "Resources:");


	// ---------------------------------------------------------------------------------------------------------------------
	// Collect entries into array
	while (listHead && entryCount < ARRAYSIZE(entryArray))
	{
		entryArray[entryCount++] = listHead;
		listHead = listHead->Next;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Iterate over each resource
	for (ULONG i = 0; i < entryCount; i++)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Temporary buffer
		CHAR temp[300];


		// -----------------------------------------------------------------------------------------------------------------
		// Format one entry

		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbPrintfA(temp, sizeof(temp), "\n    [%lu] Type=%s Action=%s Resource=%s", entryArray[i]->Index, UtilsResources_TypeToString(entryArray[i]->Type), UtilsResources_ActionToString(entryArray[i]->Action), entryArray[i]->Identifier);


		// -----------------------------------------------------------------------------------------------------------------
		// Get length of formatted entry
		size_t tempLen = strlen(temp);


		// -----------------------------------------------------------------------------------------------------------------
		// Append to output buffer if there is room
		if (offset + tempLen < responseBufferSize)
		{
			// The RtlStringCbCatW and RtlStringCbCatA functions concatenate two byte-counted strings.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcata
			RtlStringCbCatA(responseBuffer, responseBufferSize, temp);
			offset += tempLen;
		}
		// -----------------------------------------------------------------------------------------------------------------
		// Stop if buffer space is insufficient
		else
		{
			DbgPrint("Benthic Zone -> KernelModeDriver [Utils/Resources.c] (List) - Output buffer too small, truncating output.");
			status = STATUS_BUFFER_OVERFLOW;
			break;
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return status;
}



/**
	@brief          Frees all entries in a custom resource list and resets the count.


	@param[in,out]  listHead                                    Pointer to the list head to clear.

	@param[in,out]  count                                       Pointer to the element count to reset.
**/
VOID
UtilsResources_Clear(
	_Inout_         PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY*    listHead,
	_Inout_         ULONG*                                      count
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Utils/Resources.c] (Clear) - Resource list reset requested");


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY current = *listHead;


	// ---------------------------------------------------------------------------------------------------------------------
	// Free each entry
	while (current)
	{
		PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY next = current->Next;
		// The ExFreePoolWithTag routine deallocates a block of pool memory allocated with the specified tag.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-exfreepoolwithtag
		ExFreePoolWithTag(current, MACRO_UTILSRESOURCES_POOL_TAG);
		current = next;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Reset head and count
	*listHead = NULL;
	*count = 0;


	// ---------------------------------------------------------------------------------------------------------------------
	// Log cleanup completion
	DbgPrint("Benthic Zone -> KernelModeDriver [Utils/Resources.c] (Clear) - Cleared resource list");
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
