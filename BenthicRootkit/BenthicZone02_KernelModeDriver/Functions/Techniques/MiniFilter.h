// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



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
);



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
);



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
);



/**
	@brief          Initializes the mini-filter and registers directory control callbacks.


	@see            PDRIVER_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param[in]      pDriverObject           Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.

	@see            PUNICODE_STRING         https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param[in]      pRegistryPath           Pointer to a UNICODE_STRING structure, containing the driver's registry path as a Unicode string, indicating the driver's location in the Windows registry.


	@return         A NTSTATUS value indicating success or an error code if initialization fails.
**/
NTSTATUS
TechniquesMiniFilter_Init(
	_In_            PDRIVER_OBJECT          pDriverObject,
	_In_            PUNICODE_STRING         pRegistryPath
);



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
);



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
