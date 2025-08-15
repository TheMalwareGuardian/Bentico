// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Initializes the WFP driver and sets up filters.


	@param[in]  pDeviceObject           Pointer to the device object for the driver.


	@return     NTSTATUS code indicating success or failure.
**/
NTSTATUS
TechniquesWindowsFilteringPlatform_Init(
	_In_        PDEVICE_OBJECT          pDeviceObject
);



/**
	@brief      Uninitializes the Windows Filtering Platform (WFP).
	@details    This function removes the registered filter, sublayer, and callout and closes the WFP engine handle to properly clean up resources.
**/
VOID
TechniquesWindowsFilteringPlatform_Unload();



/**
	@brief           Wrapper that blocks an IP address using the WFP system and resource manager.


	@param[out]      responseBuffer          Output message buffer.

	@param[in]       responseBufferSize      Size of the buffer.

	@param[in]       ipAddress               IPv4 address as string.


	@return          A NTSTATUS value indicating success or an error code if the IP could not be blocked.
**/
NTSTATUS
TechniquesWindowsFilteringPlatform_Block(
	_Out_            char*                   responseBuffer,
	_In_             size_t                  responseBufferSize,
	_In_             const char*             ipAddress
);



/**
	@brief           Wrapper that unblocks an IP address using index in resource list.


	@param[out]      responseBuffer          Output message buffer.

	@param[in]       responseBufferSize      Size of the buffer.

	@param[in]       index                   Index of the IP address to unblock.


	@return          A NTSTATUS value indicating success or error if the IP could not be unblocked.
**/
NTSTATUS
TechniquesWindowsFilteringPlatform_Unblock(
	_Out_           char*                   responseBuffer,
	_In_            size_t                  responseBufferSize,
	_In_            ULONG                   index
);



/**
	@brief           Lists all IP addresses currently blocked via WFP and tracked in the resource manager.


	@param[out]      responseBuffer          Output buffer to fill.

	@param[in]       responseBufferSize      Size of the output buffer.


	@return          A NTSTATUS value indicating success or an error code if the list was truncated.
**/
NTSTATUS
TechniquesWindowsFilteringPlatform_List(
	_Out_            char*                   responseBuffer,
	_In_             size_t                  responseBufferSize
);



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
