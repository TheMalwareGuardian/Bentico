// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:							KernelRootkit010_WindowsFilteringPlatform
// IDE:								Open Visual Studio
// Template:						Create a new project -> Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:							Project Name: KMDFDriver_WindowsFilteringPlatform -> Solution Name: KernelRootkit010_WindowsFilteringPlatform -> Create
// Source File:						Source Files -> Add -> New Item... -> Driver.c
// Source Code:						Open Driver.c and copy the corresponding source code
// Library Dependencies:			Open Project Properties -> Linker -> Additional Dependencies -> Add '$(DDK_LIB_PATH)fwpkclnt.lib;'
// Enable NDIS Support:				Go to Project Properties -> Configuration Properties -> C/C++ -> Preprocessor -> Add 'NDIS_SUPPORT_NDIS6;' to Preprocessor Definitions.
// Build Project:					Set Configuration to Release, x64 -> Build -> Build Solution
// Locate Driver:					C:\Users\%USERNAME%\source\repos\KernelRootkit010_WindowsFilteringPlatform\x64\Release\KMDFDriver_WindowsFilteringPlatform.sys
// Virtual Machine:					Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move Driver:						Copy KMDFDriver_WindowsFilteringPlatform.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_WindowsFilteringPlatform.sys (VM)
// Enable Test Mode:				Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:				Open a CMD window as Administrator -> sc.exe create WindowsKernelWindowsFilteringPlatform type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_WindowsFilteringPlatform.sys"
// Registered Driver:				Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelWindowsFilteringPlatform
// Service Status:					Run in CMD as Administrator -> sc.exe query WindowsKernelWindowsFilteringPlatform -> driverquery.exe
// Registry Entry:					Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelWindowsFilteringPlatform
// Monitor Messages:				Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:					Run in CMD as Administrator -> sc.exe start WindowsKernelWindowsFilteringPlatform
// Clean:							Run in CMD as Administrator -> sc.exe stop WindowsKernelWindowsFilteringPlatform
// Remove:							Run in CMD as Administrator -> sc.exe delete WindowsKernelWindowsFilteringPlatform



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// #pragma warning(disable: 4201)



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpmk/
// This header is used by network.
#include <fwpmk.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpsk/
// This header is used by network.
#include <fwpsk.h>

#define INITGUID

// https://learn.microsoft.com/en-us/windows/win32/api/guiddef/
// This header is used by Component Object Model (COM).
#include <guiddef.h>

// https://learn.microsoft.com/en-us/windows/win32/api/fwpmu/
// This header is used by Windows Filtering Platform.
#include <fwpmu.h>



// START -> GUIDS -----------------------------------------------------------------------------------------------------------------------------
// START -> GUIDS -----------------------------------------------------------------------------------------------------------------------------



// Define V4 GUIDs for the callout and sublayer
// https://www.uuidgenerator.net/version4

// You define a new GUID for an item the driver exports to other system components, drivers, or applications. For example, you define a new GUID for a custom PnP event on one of its devices. Use the DEFINE_GUID macro (defined in Guiddef.h) to associate the GUID symbolic name with its value.
// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-and-exporting-new-guids
DEFINE_GUID(WFP_SAMPLE_ESTABLISHED_CALLOUT_V4_GUID, 0x3d78de67, 0x9242, 0x47be, 0xa7, 0xc4, 0x38, 0xb1, 0x3e, 0x50, 0x14, 0x34);
DEFINE_GUID(WFP_SAMPLE_SUB_LAYER_GUID, 0x906f1a88, 0x0efa, 0x4548, 0x94, 0xb3, 0xcd, 0xa2, 0xf3, 0x18, 0xb3, 0x69);



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



PDEVICE_OBJECT Global_DeviceObject = NULL;			// Pointer to the device object created by the driver.
HANDLE Global_EngineHandle = NULL;					// Handle to the Windows Filtering Platform (WFP) engine session.
UINT32 Global_RegCalloutId = 0, Global_AddCalloutId;		// IDs for the registered callout and added callout.
UINT64 Global_FilterId = 0;						// ID for the filter rule added to WFP.



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------


/**
	@brief		Uninitializes the Windows Filtering Platform (WFP).

				This function removes the registered filter, sublayer, and callout and closes the WFP engine handle to properly clean up resources.

**/
VOID
WindowsFilteringPlatformClean()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (WindowsFilteringPlatformClean) - Cleaning up WFP\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if WFP engine handle is valid before attempting cleanup
	if (Global_EngineHandle != NULL)
	{


		// -----------------------------------------------------------------------------------------------------------------
		// If a filter was added, remove it
		if (Global_FilterId != 0)
		{
			// Delete the filter by its ID

			// The FwpmFilterDeleteById0 function removes a filter object from the system.
			// https://learn.microsoft.com/en-us/windows/win32/api/fwpmu/nf-fwpmu-fwpmfilterdeletebyid0
			FwpmFilterDeleteById(Global_EngineHandle, Global_FilterId);

			// Remove the associated sublayer

			// The FwpmSubLayerDeleteByKey0 function deletes a sublayer from the system by its key.
			// https://learn.microsoft.com/en-us/windows/win32/api/fwpmu/nf-fwpmu-fwpmsublayerdeletebykey0
			FwpmSubLayerDeleteByKey(Global_EngineHandle, &WFP_SAMPLE_SUB_LAYER_GUID);
		}


		// -----------------------------------------------------------------------------------------------------------------
		// If a callout was added, remove it
		if (Global_AddCalloutId != 0)
		{
			// Delete the callout by its ID

			// The FwpmCalloutDeleteById0 function removes a callout object from the system.
			// https://learn.microsoft.com/en-us/windows/win32/api/fwpmu/nf-fwpmu-fwpmcalloutdeletebyid0
			FwpmCalloutDeleteById(Global_EngineHandle, Global_AddCalloutId);
		}


		// -----------------------------------------------------------------------------------------------------------------
		// If a callout was registered, unregister it
		if (Global_RegCalloutId != 0)
		{
			// Unregister the callout from WFP

			// The FwpsCalloutUnregisterById0 function unregisters a callout from the filter engine.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpsk/nf-fwpsk-fwpscalloutunregisterbyid0
			FwpsCalloutUnregisterById(Global_RegCalloutId);
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Close the WFP engine session

		// The FwpmEngineClose0 function closes a previously opened session to the filter engine.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpmk/nf-fwpmk-fwpmengineclose0
		FwpmEngineClose(Global_EngineHandle);


		// -----------------------------------------------------------------------------------------------------------------
		// Reset the handle to avoid dangling references
		Global_EngineHandle = NULL;
	}
	

	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (WindowsFilteringPlatformClean) - WFP Cleanup Complete\n");
}



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
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverUnload) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate the driver is being unloaded
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverUnload) - Unload routine invoked");
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverUnload) - Unloading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Cleanup WFP resources (filters, callouts, sublayers, and engine handle)
	WindowsFilteringPlatformClean();


	// ---------------------------------------------------------------------------------------------------------------------
	// Remove device object from the system
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverUnload) - Remove device object from the system");

	// The IoDeleteDevice routine removes a device object from the system, for example, when the underlying device is removed from the system.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice
	IoDeleteDevice(Global_DeviceObject);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverUnload) - Bye");
}



/**
	@brief		Callback function triggered when a callout notification event occurs.

				This function is invoked by the Windows Filtering Platform (WFP) when a filter-related event happens, such as filter addition, deletion, or modification.


				FWPS_CALLOUT_NOTIFY_TYPE	https://learn.microsoft.com/en-us/previous-versions/aa364335(v=vs.85)
	@param		type						Type of the notification event.


				GUID						https://learn.microsoft.com/en-us/windows-hardware/drivers/network/filtering-layer
	@param		filterkey					Pointer to the GUID of the filter that triggered the event.
	
	
				FWPS_FILTER					https://learn.microsoft.com/en-us/windows/win32/api/fwpstypes/ns-fwpstypes-fwps_filter0
	@param		filter						Pointer to the filter structure.


	@return		Always returns STATUS_SUCCESS.
**/
NTSTATUS
NotifyCallback(
	_In_		FWPS_CALLOUT_NOTIFY_TYPE	type,
	_In_		const GUID*					filterkey,
	_In_		const FWPS_FILTER*			filter
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing warnings for unused parameters
	UNREFERENCED_PARAMETER(type);
	UNREFERENCED_PARAMETER(filterkey);
	UNREFERENCED_PARAMETER(filter);


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Callback function triggered when a flow deletion event occurs.

				This function is invoked by the Windows Filtering Platform (WFP) when a flow is being deleted. It allows cleanup of any state associated with the flow.


				UINT16					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#UINT16
	@param		layerid					The layer ID where the flow was created.


				UINT32					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#UINT32
	@param		calloutid				The callout ID that is associated with this flow deletion.


				UINT64					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#UINT64
	@param		flowcontext				Context information for the flow being deleted.
**/
VOID FlowDeleteCallback(
	_In_		UINT16					layerid,
	_In_		UINT32					calloutid,
	_In_		UINT64					flowcontext
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing warnings for unused parameters
	UNREFERENCED_PARAMETER(layerid);
	UNREFERENCED_PARAMETER(calloutid);
	UNREFERENCED_PARAMETER(flowcontext);
}



/**
	@brief		Callback function for filtering network connections.

				This function is invoked when a network connection attempt occurs. It checks the local and remote IP addresses of the connection and logs the details. If the remote IP matches the targetip, the connection is blocked.


				FWPS_INCOMING_VALUES			https://learn.microsoft.com/en-us/windows/win32/api/fwpstypes/ns-fwpstypes-fwps_incoming_values0
	@param		Values							Pointer to incoming connection metadata (IP addresses, ports, etc.).


				FWPS_INCOMING_METADATA_VALUES0	https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpsk/ns-fwpsk-fwps_incoming_metadata_values0_
	@param		MetaData						Contains additional metadata (process ID, executable path, etc.).


				PVOID							https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param		layerdata						Pointer to layer-specific data (not used in this implementation).


	@param		context							User-defined context (not used in this implementation).


				FWPS_FILTER						https://learn.microsoft.com/en-us/windows/win32/api/fwpstypes/ns-fwpstypes-fwps_filter0
	@param		filter							Pointer to the filter that triggered this callback.


				UINT64							https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#UINT64
	@param		flowcontext						Flow context associated with the connection (not used here).


				FWPS_CLASSIFY_OUT				https://learn.microsoft.com/en-us/windows/win32/api/fwpstypes/ns-fwpstypes-fwps_classify_out0
	@param		classifyout						Output structure to determine whether the connection is allowed or blocked.


	@return		None
**/
VOID
FilterCallback(
	const		FWPS_INCOMING_VALUES*			Values,
	const		FWPS_INCOMING_METADATA_VALUES0	MetaData,
	const		PVOID							layerdata,
	const		void*							context,
	const		FWPS_FILTER*					filter,
				UINT64							flowcontext,
				FWPS_CLASSIFY_OUT*				classifyout
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing warnings for unused parameters
	UNREFERENCED_PARAMETER(flowcontext);
	UNREFERENCED_PARAMETER(context);
	UNREFERENCED_PARAMETER(layerdata);


	// ---------------------------------------------------------------------------------------------------------------------
	// Define IP Variables

	ULONG LocalIp;					// Local machine's IP
	ULONG RemoteIp;					// Remote machine's IP
	ULONG targetip = 0x0EB8FA8E;	// Target IP to block (142.250.184.14 0E B8 FA 8E - 142 -> 0x8E 250 -> 0xFA 184 -> 0xB8 14 -> 0x0E)


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the classifyout structure allows modifying the action type

	// The FWPS_RIGHT_ACTION_WRITE flag allows us to change the action type (block/allow). If this right is not granted, we skip further processing.
	if (!(classifyout->rights & FWPS_RIGHT_ACTION_WRITE))
	{
		goto end;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Extract IP Addresses from connection metadata

	// FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_ADDRESS gets the local IP address.
	LocalIp = Values->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_ADDRESS].value.uint32;

	// FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_ADDRESS gets remote IP address.
	RemoteIp = Values->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_ADDRESS].value.uint32;


	// ---------------------------------------------------------------------------------------------------------------------
	// Log Connection Details
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (FilterCallback) - PID: %ld | Process Path: %S | Local IP: %u.%u.%u.%u | Remote IP: %u.%u.%u.%u\n", (ULONG)(MetaData.processId), (PWCHAR)(MetaData.processPath->data), (LocalIp >> 24) & 0xFF, (LocalIp >> 16) & 0xFF, (LocalIp >> 8) & 0xFF, (LocalIp) & 0xFF, (RemoteIp >> 24) & 0xFF, (RemoteIp >> 16) & 0xFF, (RemoteIp >> 8) & 0xFF, (RemoteIp) & 0xFF);


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the connection matches the target IP
	if (targetip == RemoteIp)
	{
		DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (FilterCallback) - Blocking connection to %u.%u.%u.%u\n", (RemoteIp >> 24) & 0xFF, (RemoteIp >> 16) & 0xFF, (RemoteIp >> 8) & 0xFF, (RemoteIp) & 0xFF);

		// Block the connection
		classifyout->actionType = FWP_ACTION_BLOCK;

		// Remove the write permission to prevent other filters from overriding this decision
		classifyout->rights &= ~FWPS_RIGHT_ACTION_WRITE;
		return;
	}
	else
	{
		// Allow the connection
		classifyout->actionType = FWP_ACTION_PERMIT;
	}

end:
	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the filter requires clearing action rights

	// If FWPS_FILTER_FLAG_CLEAR_ACTION_RIGHT is set, we remove the write permission so that no other filter can modify the action after this one.
	if (filter->flags & FWPS_FILTER_FLAG_CLEAR_ACTION_RIGHT)
	{
		classifyout->rights &= ~FWPS_RIGHT_ACTION_WRITE;
	}
}



/**
	@brief		Opens the Windows Filtering Platform (WFP) engine.

				This function establishes a connection with the WFP engine, allowing further configuration of callouts, sublayers, and filters.


	@return		NTSTATUS code indicating success or failure.
**/
NTSTATUS
WfpOpenEngine()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Open WFP Engine

	// The FwpmEngineOpen function connects to the WFP engine, allowing the driver to register callouts, create filters, and manage network traffic control.

	// The FwpmEngineOpen0 function opens a session to the filter engine.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpmk/nf-fwpmk-fwpmengineopen0
	NTSTATUS status = FwpmEngineOpen(NULL, RPC_C_AUTHN_WINNT, NULL, NULL, &Global_EngineHandle);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (WfpOpenEngine) - ERROR: Failed to open WFP engine. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Registers a callout in the Windows Filtering Platform.

				This function registers a callout for monitoring and controlling network traffic at a specific layer.


	@return		NTSTATUS code indicating success or failure.
**/
NTSTATUS
WfpRegisterCallout()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Define Callout Structure

	// The FWPS_CALLOUT0 structure defines the data that is required for a callout driver to register a callout with the filter engine.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpsk/ns-fwpsk-fwps_callout0_
	FWPS_CALLOUT Callout = { 0 };
	Callout.calloutKey = WFP_SAMPLE_ESTABLISHED_CALLOUT_V4_GUID;
	Callout.flags = 0;
	Callout.classifyFn = (FWPS_CALLOUT_CLASSIFY_FN3)FilterCallback;
	Callout.notifyFn = (FWPS_CALLOUT_NOTIFY_FN3)NotifyCallback;
	Callout.flowDeleteFn = (FWPS_CALLOUT_FLOW_DELETE_NOTIFY_FN0)FlowDeleteCallback;


	// ---------------------------------------------------------------------------------------------------------------------
	// Register the Callout

	// The FwpsCalloutRegister function registers the callout with the WFP engine.

	// The FwpsCalloutRegister0 function registers a callout with the filter engine.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpsk/nf-fwpsk-fwpscalloutregister0
	NTSTATUS status = FwpsCalloutRegister(Global_DeviceObject, &Callout, &Global_RegCalloutId);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (WfpRegisterCallout) - ERROR: Failed to register callout. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Adds a callout to the Windows Filtering Platform engine.

				This function creates a new callout in the filtering engine, allowing it to interact with network traffic at a specified layer.


	@return		NTSTATUS code indicating success or failure.
**/
NTSTATUS
WfpAddCallout()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Define Callout Parameters

	// The FWPM_CALLOUT0 structure stores the state associated with a callout.
	// https://learn.microsoft.com/en-us/windows/win32/api/fwpmtypes/ns-fwpmtypes-fwpm_callout0
	FWPM_CALLOUT callout = { 0 };
	callout.flags = 0;
	callout.displayData.name = L"EstablishedCalloutName";
	callout.displayData.description = L"EstablishedCalloutName";
	callout.calloutKey = WFP_SAMPLE_ESTABLISHED_CALLOUT_V4_GUID;
	callout.applicableLayer = FWPM_LAYER_ALE_AUTH_CONNECT_V4;


	// ---------------------------------------------------------------------------------------------------------------------
	// Add Callout to WFP Engine

	// The FwpmCalloutAdd0 function adds a new callout object to the system.
	// https://learn.microsoft.com/en-us/windows/win32/api/fwpmu/nf-fwpmu-fwpmcalloutadd0
	NTSTATUS status = FwpmCalloutAdd(Global_EngineHandle, &callout, NULL, &Global_AddCalloutId);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (WfpAddCallout) - ERROR: Failed to add callout. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Adds a sublayer to the Windows Filtering Platform.

				This function defines a custom sublayer that will be used to group filtering rules for easier management.


	@return		NTSTATUS code indicating success or failure.
**/
NTSTATUS
WfpAddSublayer()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Define Sublayer Parameters

	// The FWPM_SUBLAYER0 structure stores the state associated with a sublayer.
	// https://learn.microsoft.com/en-us/windows/win32/api/fwpmtypes/ns-fwpmtypes-fwpm_sublayer0
	FWPM_SUBLAYER sublayer = { 0 };
	sublayer.displayData.name = L"Establishedsublayername";
	sublayer.displayData.description = L"Establishedsublayername";
	sublayer.subLayerKey = WFP_SAMPLE_SUB_LAYER_GUID;
	sublayer.weight = 65500;


	// ---------------------------------------------------------------------------------------------------------------------
	// Add Sublayer to WFP Engine

	// The FwpmSubLayerAdd0 function adds a new sublayer to the system.
	// https://learn.microsoft.com/en-us/windows/win32/api/fwpmu/nf-fwpmu-fwpmsublayeradd0
	NTSTATUS status = FwpmSubLayerAdd(Global_EngineHandle, &sublayer, NULL);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (WfpAddSublayer) - ERROR: Failed to add sublayer. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Adds a filter to the Windows Filtering Platform.

				This function creates a filter that applies to specific network traffic based on defined conditions.


	@return		NTSTATUS code indicating success or failure.
**/
NTSTATUS
WfpAddFilter()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Define Filter Parameters

	// The FWPM_FILTER0 structure stores the state associated with a filter.
	// https://learn.microsoft.com/en-us/windows/win32/api/fwpmtypes/ns-fwpmtypes-fwpm_filter0
	FWPM_FILTER filter = { 0 };

	// The FWPM_FILTER_CONDITION0 structure expresses a filter condition that must be true for the action to be taken.
	// https://learn.microsoft.com/en-us/windows/win32/api/fwpmtypes/ns-fwpmtypes-fwpm_filter_condition0
	FWPM_FILTER_CONDITION condition[1] = { 0 };

	// The FWP_V4_ADDR_AND_MASK structure specifies IPv4 address and mask in host order.
	// https://learn.microsoft.com/en-us/windows/win32/api/fwptypes/ns-fwptypes-fwp_v4_addr_and_mask
	FWP_V4_ADDR_AND_MASK AddrAndMask = { 0 };

	filter.displayData.name = L"filterCalloutName";
	filter.displayData.description = L"filterCalloutName";
	filter.layerKey = FWPM_LAYER_ALE_AUTH_CONNECT_V4;
	filter.subLayerKey = WFP_SAMPLE_SUB_LAYER_GUID;
	filter.weight.type = FWP_EMPTY;
	filter.numFilterConditions = 1;
	filter.filterCondition = condition;
	filter.action.type = FWP_ACTION_CALLOUT_TERMINATING;
	filter.action.calloutKey = WFP_SAMPLE_ESTABLISHED_CALLOUT_V4_GUID;

	// Set Filter Condition (Remote IP Address)
	condition[0].fieldKey = FWPM_CONDITION_IP_REMOTE_ADDRESS;
	condition[0].matchType = FWP_MATCH_EQUAL;
	condition[0].conditionValue.type = FWP_V4_ADDR_MASK;
	condition[0].conditionValue.v4AddrMask = &AddrAndMask;


	// ---------------------------------------------------------------------------------------------------------------------
	// Add Filter to WFP Engine

	// The FwpmFilterAdd0 function adds a new filter object to the system.
	// https://learn.microsoft.com/en-us/windows/win32/api/fwpmu/nf-fwpmu-fwpmfilteradd0
	NTSTATUS status = FwpmFilterAdd(Global_EngineHandle, &filter, NULL, &Global_FilterId);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (WfpAddFilter) - ERROR: Failed to add filter. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Initializes the WFP driver and sets up filters.


	@return		NTSTATUS code indicating success or failure.
**/
NTSTATUS
WindowsFilteringPlatformInitialize()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Open the WFP engine to establish a session
	if (!NT_SUCCESS(WfpOpenEngine())) goto end;

	// ---------------------------------------------------------------------------------------------------------------------
	// Register a callout to define custom filtering logic
	if (!NT_SUCCESS(WfpRegisterCallout())) goto end;

	// ---------------------------------------------------------------------------------------------------------------------
	// Add the callout to WFP so it can be referenced in filters
	if (!NT_SUCCESS(WfpAddCallout())) goto end;

	// ---------------------------------------------------------------------------------------------------------------------
	// Add a sublayer to categorize the filter within the WFP filtering hierarchy
	if (!NT_SUCCESS(WfpAddSublayer())) goto end;

	// ---------------------------------------------------------------------------------------------------------------------
	// Add a filter rule that uses the callout for traffic inspection
	if (!NT_SUCCESS(WfpAddFilter())) goto end;

	// If everything succeeds, return success
	return STATUS_SUCCESS;

end:
	// ---------------------------------------------------------------------------------------------------------------------
	// If any step fails, clean up previously registered components to prevent leaks
	WindowsFilteringPlatformClean();
	return STATUS_UNSUCCESSFUL;
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
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverEntry) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate DriverEntry (main) function has been invoked
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverEntry) - DriverEntry routine invoked");
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverEntry) - Loading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the registry path, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pRegistryPath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Set DriverUnload routine
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverEntry) - Set DriverUnload routine");

	// The Unload routine performs any operations that are necessary before the system unloads the driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
	pDriverObject->DriverUnload = DriverUnload;


	// ---------------------------------------------------------------------------------------------------------------------
	// Create a device object
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverEntry) - Create a device object");

	// The IoCreateDevice routine creates a device object for use by a driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatedevice
	status = IoCreateDevice(DriverObject, 0, NULL, FILE_DEVICE_UNKNOWN, 0, FALSE, &Global_DeviceObject);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverEntry) - ERROR: Error creating device. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize Windows Filtering Platform (WFP)

	// WindowsFilteringPlatformInitialize sets up the necessary hooks for monitoring or modifying network traffic.
	status = WindowsFilteringPlatformInitialize();

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverEntry) - ERROR: Error initializing WFP. Status: 0x%08X\n", status);
		// The IoDeleteDevice routine removes a device object from the system, for example, when the underlying device is removed from the system.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice
		IoDeleteDevice(pDriverObject->DeviceObject);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone WindowsFilteringPlatform -> KMDFDriverWindowsFilteringPlatform [Driver.c] (DriverEntry) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------