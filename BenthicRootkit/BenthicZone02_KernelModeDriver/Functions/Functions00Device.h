// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



extern UNICODE_STRING Global_FunctionsDevice_Device_Name;
extern UNICODE_STRING Global_FunctionsDevice_Device_Symbolic_Link;
extern PDEVICE_OBJECT Global_FunctionsDevice_Device_Object;



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Initializes the device object and its symbolic link.


	@see        PDRIVER_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param      pDriverObject           Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.


	@return     A NTSTATUS value indicating success or an error code if device and symbolic link creation fails.
**/
NTSTATUS
FunctionsDevice_InitializeDevice(
	_In_        PDRIVER_OBJECT          pDriverObject
);



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
