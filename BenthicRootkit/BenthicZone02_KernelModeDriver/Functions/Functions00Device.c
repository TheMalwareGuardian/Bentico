// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "Strings/Strings00Device.h"



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



UNICODE_STRING Global_FunctionsDevice_Device_Name;
UNICODE_STRING Global_FunctionsDevice_Device_Symbolic_Link;
PDEVICE_OBJECT Global_FunctionsDevice_Device_Object = NULL;



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
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize device name and symbolic link
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/Device.c] (InitializeDevice) - Initialize device name and symbolic link");

	// The RtlInitUnicodeString function initializes a counted string of Unicode characters.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlinitunicodestring
	RtlInitUnicodeString(&Global_FunctionsDevice_Device_Name, STRINGSDEVICE_DRIVERDEVICE_NAME);
	RtlInitUnicodeString(&Global_FunctionsDevice_Device_Symbolic_Link, STRINGSDEVICE_DRIVERSYMBOLICLINK);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Create a device object
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/Device.c] (InitializeDevice) - Create a device object");

	// The IoCreateDevice routine creates a device object for use by a driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatedevice
	status = IoCreateDevice(pDriverObject, 0, &Global_FunctionsDevice_Device_Name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &Global_FunctionsDevice_Device_Object);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Functions/Device.c] (InitializeDevice) - ERROR: Failed to create device (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Create a symbolic link
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/Device.c] (InitializeDevice) - Create a symbolic link");

	// The IoCreateSymbolicLink routine sets up a symbolic link between a device object name and a user-visible name for the device.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatesymboliclink
	status = IoCreateSymbolicLink(&Global_FunctionsDevice_Device_Symbolic_Link, &Global_FunctionsDevice_Device_Name);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Functions/Device.c] (InitializeDevice) - ERROR: Failed to create symbolic link (Status 0x%08X).\n", status);
		// The IoDeleteDevice routine removes a device object from the system, for example, when the underlying device is removed from the system.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice
		IoDeleteDevice(Global_FunctionsDevice_Device_Object);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/Device.c] (InitializeDevice) - Device and symbolic link successfully initialized");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
