// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



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
);



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
