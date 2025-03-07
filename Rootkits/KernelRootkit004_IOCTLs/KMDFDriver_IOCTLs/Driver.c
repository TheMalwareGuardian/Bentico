// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:							KernelRootkit004_IOCTLs
// IDE:								Open Visual Studio
// Template:						Create a new project -> Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:							Project Name: KMDFDriver_IOCTLs -> Solution Name: KernelRootkit004_IOCTLs -> Create
// Source File:						Source Files -> Add -> New Item... -> Driver.c
// Source Code:						Open Driver.c and copy the corresponding source code
// Build Project:					Set Configuration to Release, x64 -> Build -> Build Solution
// Add Project:						In Solution Explorer -> Right-click the solution (KernelRootkit004_IOCTLs) -> Add -> New Project...
// Template:						Search for templates (Alt + S) -> Console App -> Next
// Project:							Project name: ConsoleApp_IOCTLs -> Create
// Source File:						In Solution Explorer -> Find ConsoleApp_IOCTLs.cpp -> Rename to Application.c
// Source Code:						Open Application.c and copy the corresponding source code
// Build Project:					Set Configuration to Release, x64 -> Build -> Build Solution
// Locate Driver:					C:\Users\%USERNAME%\source\repos\KernelRootkit004_IOCTLs\x64\Release\KMDFDriver_IOCTLs.sys
// Locate App:						C:\Users\%USERNAME%\source\repos\KernelRootkit004_IOCTLs\x64\Release\ConsoleApp_IOCTLs.exe
// Virtual Machine:					Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move Driver:						Copy KMDFDriver_IOCTLs.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_IOCTLs.sys (VM)
// Move App:						Move ConsoleApp_IOCTLs.exe (Host) to C:\Users\%USERNAME%\Downloads\ConsoleApp_IOCTLs.exe (VM)
// Enable Test Mode:				Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:				Open a CMD window as Administrator -> sc.exe create WindowsKernelIOCTLs type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_IOCTLs.sys"
// Registered Driver:				Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelIOCTLs
// Service Status:					Run in CMD as Administrator -> sc.exe query WindowsKernelIOCTLs -> driverquery.exe
// Registry Entry:					Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelIOCTLs
// Monitor Messages:				Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:					Run in CMD as Administrator -> sc.exe start WindowsKernelIOCTLs
// User Mode App:					Open CMD -> Navigate to the directory -> Run ConsoleApp_IOCTLs.exe
// Clean:							Run in CMD as Administrator -> sc.exe stop WindowsKernelIOCTLs
// Remove:							Run in CMD as Administrator -> sc.exe delete WindowsKernelIOCTLs



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/
// This header contains reference material that includes specific details about the routines, structures, and data types that you will need to use to write kernel-mode drivers.
#include <wdm.h>



// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------
// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------



/**
	@brief		An I/O control code is a 32-bit value that consists of several fields (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-i-o-control-codes).

				When defining new IOCTLs, it is important to remember the following rules:

					- If a new IOCTL will be available to user-mode software components, the IOCTL must be used with IRP_MJ_DEVICE_CONTROL requests. User-mode components send IRP_MJ_DEVICE_CONTROL requests by calling the DeviceIoControl, which is a Win32 function.
					- If a new IOCTL will be available only to kernel-mode driver components, the IOCTL must be used with IRP_MJ_INTERNAL_DEVICE_CONTROL requests. Kernel-mode components create IRP_MJ_INTERNAL_DEVICE_CONTROL requests by calling IoBuildDeviceIoControlRequest.

				Use the system-supplied CTL_CODE macro, which is defined in Wdm.h and Ntddk.h, to define new I/O control codes. The definition of a new IOCTL code, whether intended for use with IRP_MJ_DEVICE_CONTROL or IRP_MJ_INTERNAL_DEVICE_CONTROL requests, uses the following format:

				#define IOCTL_Device_Function CTL_CODE(DeviceType, Function, Method, Access)


				Supply the following parameters to the CTL_CODE macro:

				DeviceType			This value must match the value that is set in the DeviceType member of the driver's DEVICE_OBJECT structure (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/specifying-device-types).

				FunctionCode		Identifies the function to be performed by the driver. Values of less than 0x800 are reserved for Microsoft. Values of 0x800 and higher can be used by vendors.

				TransferType		Indicates how the system will pass data between the caller of DeviceIoControl (or IoBuildDeviceIoControlRequest) and the driver that handles the IRP (METHOD_BUFFERED, METHOD_IN_DIRECT, METHOD_OUT_DIRECT, METHOD_NEITHER).

				RequiredAccess		Indicates the type of access that a caller must request when opening the file object that represents the device (FILE_ANY_ACCESS, FILE_READ_DATA, FILE_READ_DATA, FILE_READ_DATA and FILE_WRITE_DATA).
**/
#define IOCTL_COMMAND_0 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_1 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



// Device name and symbolic link
UNICODE_STRING Global_Device_Name;
UNICODE_STRING Global_Device_Symbolic_Link;



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief		Unloads a Windows kernel-mode driver.

				This function is called when the driver is being unloaded from memory. It is responsible for cleaning up resources and performing necessary cleanup tasks before the driver is removed from the system. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload


				PDRIVER_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param		pDriverObject			Pointer to a DRIVER_OBJECT structure representing the driver.
**/
VOID
DriverUnload(
	_In_		PDRIVER_OBJECT			pDriverObject
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverUnload) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate the driver is being unloaded
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverUnload) - Unload routine invoked");
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverUnload) - Unloading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Remove symbolic link from the system
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverUnload) - Remove symbolic link from the system");

	// The IoDeleteSymbolicLink routine removes a symbolic link from the system.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletesymboliclink
	IoDeleteSymbolicLink(&Global_Device_Symbolic_Link);


	// ---------------------------------------------------------------------------------------------------------------------
	// Remove device object from the system
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverUnload) - Remove device object from the system");

	// The IoDeleteDevice routine removes a device object from the system, for example, when the underlying device is removed from the system.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice
	IoDeleteDevice(pDriverObject->DeviceObject);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverUnload) - Bye");
}



/**
	@brief		A passthrough function for handling IRPs (I/O Request Packets).


				PDEVICE_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param		pDeviceObject			Pointer to a DEVICE_OBJECT structure representing the device.


				PIRP					https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
	@param		pIrp					Pointer to an IRP (I/O Request Packet) to be processed.


	@return		A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
DriverPassthrough(
	_In_		PDEVICE_OBJECT			pDeviceObject,
	_In_		PIRP					pIrp
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverPassthrough) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the device object, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pDeviceObject);


	// ---------------------------------------------------------------------------------------------------------------------
	// Set I/O status information
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverPassthrough) - Set I/O status information");

	// A driver sets an IRP's I/O status block to indicate the final status of an I/O request, before calling IoCompleteRequest for the IRP.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_io_status_block
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;


	// ---------------------------------------------------------------------------------------------------------------------
	// Complete IRP processing and indicate no increment in stack location
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverPassthrough) - Complete IRP processing and indicate no increment in stack location");

	// The IoCompleteRequest macro indicates that the caller has completed all processing for a given I/O request and is returning the given IRP to the I/O manager. IoCompleteRequest wraps IofCompleteRequest.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocompleterequest
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverPassthrough) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Operation was completed successfully
	return STATUS_SUCCESS;
}



/**
	@brief		Handles IOCTLs (Input/Output Control) requests from userland.


				PDEVICE_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param		pDeviceObject			Pointer to a DEVICE_OBJECT structure representing the device.


				PIRP					https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
	@param		pIrp					Pointer to an IRP (I/O Request Packet) to be processed.


	@return		A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
DriverHandleIOCTL(
	_In_		PDEVICE_OBJECT			pDeviceObject,
	_In_		PIRP					pIrp
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverHandleIOCTL) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the device object, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pDeviceObject);


	// ---------------------------------------------------------------------------------------------------------------------
	// Get current stack location in IRP
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverHandleIOCTL) - Get current stack location in IRP");

	// IO_STACK_LOCATION structure defines an I/O stack location, which is an entry in the I/O stack that is associated with each IRP. Each I/O stack location in an IRP has some common members and some request-type-specific members.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_io_stack_location
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iogetcurrentirpstacklocation
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);


	// ---------------------------------------------------------------------------------------------------------------------
	// Get IOCTL code from IRP
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverHandleIOCTL) - Get IOCTL code from IRP");

	// The I/O Manager, other operating system components, and other kernel-mode drivers send IRP_MJ_DEVICE_CONTROL requests. Normally this IRP is sent on behalf of a user-mode application that has called the Win32 DeviceIoControl function or on behalf of a kernel-mode component that has called ZwDeviceIoControlFile.
	// https://learn.microsoft.com/en-us/previous-versions/windows/drivers/ifs/irp-mj-device-control
	ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize a message variable with the greeting message intended for userland communication
	CHAR* message = "Hello User";


	// ---------------------------------------------------------------------------------------------------------------------
	// Handle different IOCTL codes
	switch (controlCode)
	{

		// Handle IOCTL_COMMAND_0: Processes a command (No input or output data is expected).
		case IOCTL_COMMAND_0:
			DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverHandleIOCTL) - Handle IOCTL_COMMAND_0: Processes a command (No input or output data is expected).\n");
			pIrp->IoStatus.Information = 0;
			break;

		// Handle IOCTL_COMMAND_1: Processes a command and sends a predefined message back to the caller.
		case IOCTL_COMMAND_1:
			DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverHandleIOCTL) - Handle IOCTL_COMMAND_1: Processes a command and sends a predefined message back to the caller.\n");
			pIrp->IoStatus.Information = strlen(message);

			// Copy the content of a source memory block to a destination memory block
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlcopymemory
			RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, message, strlen(message));
			break;

		// Handle IOCTL_COMMAND_2: Processes a command with input data and returns a processed response.
		case IOCTL_COMMAND_2:
			DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverHandleIOCTL) - Handle IOCTL_COMMAND_2: Processes a command with input data and returns a processed response.\n");
			DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverHandleIOCTL) - Input received from userland -> %s", (char*)pIrp->AssociatedIrp.SystemBuffer);
			pIrp->IoStatus.Information = strlen(message);

			// Copy the content of a source memory block to a destination memory block
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlcopymemory
			RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, message, strlen(message));
			break;

		// Handle invalid IOCTL requests
		default:
			DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverHandleIOCTL) - Invalid IOCTL\n");
			break;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Set I/O status information
	pIrp->IoStatus.Status = STATUS_SUCCESS;


	// ---------------------------------------------------------------------------------------------------------------------
	// Complete IRP processing and indicate no increment in stack location
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverHandleIOCTL) - Complete IRP processing and indicate no increment in stack location");

	// The IoCompleteRequest macro indicates that the caller has completed all processing for a given I/O request and is returning the given IRP to the I/O manager. IoCompleteRequest wraps IofCompleteRequest.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocompleterequest
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverHandleIOCTL) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Operation was completed successfully
	return STATUS_SUCCESS;
}



/**
	@brief		Entry point for a Windows kernel-mode driver.
	
				This function is called when the driver is loaded into memory. It initializes the driver and performs necessary setup tasks. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/wdf/driverentry-for-kmdf-drivers


				PDRIVER_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param		pDriverObject			Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.


				PUNICODE_STRING			https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param		pRegistryPath			Pointer to a UNICODE_STRING structure, containing the driver's registry path as a Unicode string, indicating the driver's location in the Windows registry.


	@return		A NTSTATUS value indicating success or an error code if initialization fails.
**/
NTSTATUS
DriverEntry(
	_In_		PDRIVER_OBJECT			pDriverObject,
	_In_		PUNICODE_STRING			pRegistryPath
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverEntry) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate DriverEntry (main) function has been invoked
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverEntry) - DriverEntry routine invoked");
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverEntry) - Loading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the registry path, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pRegistryPath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize device name and symbolic link
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverEntry) - Initialize device name and symbolic link");

	// The RtlInitUnicodeString function initializes a counted string of Unicode characters.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlinitunicodestring
	RtlInitUnicodeString(&Global_Device_Name, L"\\Device\\MyKernelDriverIOCTLs");
	RtlInitUnicodeString(&Global_Device_Symbolic_Link, L"\\DosDevices\\MyKernelDriverIOCTLs");


	// ---------------------------------------------------------------------------------------------------------------------
	// Set DriverUnload routine
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverEntry) - Set DriverUnload routine");

	// The Unload routine performs any operations that are necessary before the system unloads the driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
	pDriverObject->DriverUnload = DriverUnload;


	// ---------------------------------------------------------------------------------------------------------------------
	// Set MajorFunction for different IRP types
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverEntry) - Set MajorFunction for different IRP types");

	// Each driver-specific I/O stack location (IO_STACK_LOCATION) for every IRP contains a major function code (IRP_MJ_XXX), which tells the driver what operation it or the underlying device driver should carry out to satisfy the I/O request. Each kernel-mode driver must provide dispatch routines for the major function codes that it must support (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-major-function-codes).

	// Every kernel-mode driver must handle IRP_MJ_CREATE requests in a DRIVER_DISPATCH callback function.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-create
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DriverPassthrough;

	// Every driver must handle close requests in a DispatchClose routine, with the possible exception of a driver whose device cannot be disabled or removed from the machine without bringing down the system. A disk driver whose device holds the system page file is an example of such a driver. Note that the driver of such a device also cannot be unloaded dynamically.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-close
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverPassthrough;

	// Every device driver that transfers data from its device to the system must handle read requests in a DispatchRead or DispatchReadWrite routine, as must any higher-level driver layered over such a device driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-read
	pDriverObject->MajorFunction[IRP_MJ_READ] = DriverPassthrough;

	// Every device driver that transfers data from the system to its device must handle write requests in a DispatchWrite or DispatchReadWrite routine, as must any higher-level driver layered over such a device driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-write
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = DriverPassthrough;

	// Every driver whose device objects belong to a particular device type (see Specifying Device Types) is required to support this request in a DispatchDeviceControl routine, if a set of system-defined I/O control codes (IOCTLs) exists for the type. For more info about IOCTLs, see Introduction to I/O Control Codes. Higher-level drivers usually pass these requests on to an underlying device driver. Each device driver in a driver stack is assumed to support this request, along with a set of device type-specific, public or private IOCTLs. For more information about IOCTLs for specific device types, see device type-specific documentation in the Microsoft Windows Driver Kit (WDK).
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-device-control
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverHandleIOCTL;


	// ---------------------------------------------------------------------------------------------------------------------
	// Create a device object
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverEntry) - Create a device object");

	// The IoCreateDevice routine creates a device object for use by a driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatedevice
	status = IoCreateDevice(pDriverObject, 0, &Global_Device_Name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDriverObject->DeviceObject);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverEntry) - ERROR: Error creating device. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Create a symbolic link for the device
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverEntry) - Create a symbolic link for the device");

	// The IoCreateSymbolicLink routine sets up a symbolic link between a device object name and a user-visible name for the device.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatesymboliclink
	status = IoCreateSymbolicLink(&Global_Device_Symbolic_Link, &Global_Device_Name);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverEntry) - ERROR: Error creating symbolic link. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone IOCTLs -> KMDFDriverIOCTLs [Driver.c] (DriverEntry) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Driver initialization was completed successfully
	return STATUS_SUCCESS;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------