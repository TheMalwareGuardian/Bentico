// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



#pragma once



// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------
// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------



// Tag
#define MACRO_UTILSRESOURCES_POOL_TAG 'URPT'



// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------
// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Defines the type of resource to be handled.
**/
typedef enum _STRUCTURE_UTILSRESOURCES_TYPE {
	STRUCTURE_UTILSRESOURCES_TYPE_PROCESS,
	STRUCTURE_UTILSRESOURCES_TYPE_FILE,
	STRUCTURE_UTILSRESOURCES_TYPE_FOLDER,
	STRUCTURE_UTILSRESOURCES_TYPE_IP,
	STRUCTURE_UTILSRESOURCES_TYPE_PORT,
	STRUCTURE_UTILSRESOURCES_TYPE_CONNECTION,
	STRUCTURE_UTILSRESOURCES_TYPE_REGISTRY,
	STRUCTURE_UTILSRESOURCES_TYPE_OTHER
} STRUCTURE_UTILSRESOURCES_TYPE;



/**
	@brief      Defines the action to be performed on the resource.
**/
typedef enum _STRUCTURE_UTILSRESOURCES_ACTION {
	STRUCTURE_UTILSRESOURCES_ACTION_HIDE,
	STRUCTURE_UTILSRESOURCES_ACTION_BLOCK,
	STRUCTURE_UTILSRESOURCES_ACTION_PROTECT,
	STRUCTURE_UTILSRESOURCES_ACTION_MONITOR
} STRUCTURE_UTILSRESOURCES_ACTION;



/**
	@brief      Generic structure to represent a resource and its intended action.
**/
typedef struct _STRUCTURE_UTILSRESOURCES_GENERIC_ENTRY {
	ULONG Index;
	STRUCTURE_UTILSRESOURCES_TYPE Type;
	STRUCTURE_UTILSRESOURCES_ACTION Action;
	CHAR Identifier[260];
	struct _STRUCTURE_UTILSRESOURCES_GENERIC_ENTRY* Next;
} STRUCTURE_UTILSRESOURCES_GENERIC_ENTRY, *PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY;



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



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
);



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
);



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
);



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
);



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
);



/**
	@brief          Frees all entries in a custom resource list and resets the count.


	@param[in,out]  listHead                                    Pointer to the list head to clear.

	@param[in,out]  count                                       Pointer to the element count to reset.
**/
VOID
UtilsResources_Clear(
	_Inout_         PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY*    listHead,
	_Inout_         ULONG*                                      count
);



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
