// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



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
);



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
);



/**
	@brief          Lists all processes currently hidden via Direct Kernel Object Manipulation technique.


	@param[out]     responseBuffer                              Output buffer to fill.

	@param[in]      responseBufferSize                          Size of the output buffer.


	@return         A NTSTATUS value indicating success or an error code if the list was truncated.
**/
NTSTATUS TechniquesDirectKernelObjectManipulation_List(
	_Out_           char*                                       responseBuffer,
	_In_            size_t                                      responseBufferSize
);



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
