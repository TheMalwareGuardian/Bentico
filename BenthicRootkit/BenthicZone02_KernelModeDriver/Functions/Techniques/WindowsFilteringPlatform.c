// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



#pragma warning(disable: 4201)



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/
// This header declares functions to handle strings safely (copy, format, concatenate, etc.) and avoid buffer overflows
#include <ntstrsafe.h>

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



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "../Utils/Utils00Resources.h"



// START -> GUIDS -----------------------------------------------------------------------------------------------------------------------------
// START -> GUIDS -----------------------------------------------------------------------------------------------------------------------------



// Define V4 GUIDs for the callout and sublayer
// https://www.uuidgenerator.net/version4

// You define a new GUID for an item the driver exports to other system components, drivers, or applications. For example, you define a new GUID for a custom PnP event on one of its devices. Use the DEFINE_GUID macro (defined in Guiddef.h) to associate the GUID symbolic name with its value.
// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-and-exporting-new-guids
DEFINE_GUID(WFP_SAMPLE_ESTABLISHED_CALLOUT_V4_GUID, 0x23bb0568, 0x4cc2, 0x4039, 0x8c, 0x3e, 0x39, 0x23, 0xde, 0x47, 0x6e, 0x5d);
DEFINE_GUID(WFP_SAMPLE_SUB_LAYER_GUID, 0x64051bab, 0x7763, 0x44a3, 0xba, 0xd0, 0x14, 0x11, 0xe6, 0xc7, 0x79, 0x48);



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



// Head of the linked list of blocked connections
PSTRUCTURE_UTILSRESOURCES_GENERIC_ENTRY Global_WindowsFilteringPlatform_BlockedConnectionsListHead = NULL;

// Counter for number of blocked connections
ULONG Global_WindowsFilteringPlatform_BlockedConnectionsListCount = 0;


// Handle to the Windows Filtering Platform (WFP) engine session.
HANDLE Global_WindowsFilteringPlatform_EngineHandle = NULL;

// IDs for the registered callout and added callout.
UINT32 Global_WindowsFilteringPlatform_RegCalloutId = 0, Global_WindowsFilteringPlatform_AddCalloutId;

// ID for the filter rule added to WFP.
UINT64 Global_WindowsFilteringPlatform_FilterId = 0;



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Uninitializes the Windows Filtering Platform (WFP).
	@details    This function removes the registered filter, sublayer, and callout and closes the WFP engine handle to properly clean up resources.
**/
VOID
TechniquesWindowsFilteringPlatform_Unload()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (Unload) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if WFP engine handle is valid before attempting cleanup
	if (Global_WindowsFilteringPlatform_EngineHandle != NULL)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// If a filter was added, remove it
		if (Global_WindowsFilteringPlatform_FilterId != 0)
		{
			// Delete the filter by its ID

			// The FwpmFilterDeleteById0 function removes a filter object from the system.
			// https://learn.microsoft.com/en-us/windows/win32/api/fwpmu/nf-fwpmu-fwpmfilterdeletebyid0
			FwpmFilterDeleteById(Global_WindowsFilteringPlatform_EngineHandle, Global_WindowsFilteringPlatform_FilterId);

			// Remove the associated sublayer

			// The FwpmSubLayerDeleteByKey0 function deletes a sublayer from the system by its key.
			// https://learn.microsoft.com/en-us/windows/win32/api/fwpmu/nf-fwpmu-fwpmsublayerdeletebykey0
			FwpmSubLayerDeleteByKey(Global_WindowsFilteringPlatform_EngineHandle, &WFP_SAMPLE_SUB_LAYER_GUID);
		}


		// -----------------------------------------------------------------------------------------------------------------
		// If a callout was added, remove it
		if (Global_WindowsFilteringPlatform_AddCalloutId != 0)
		{
			// Delete the callout by its ID

			// The FwpmCalloutDeleteById0 function removes a callout object from the system.
			// https://learn.microsoft.com/en-us/windows/win32/api/fwpmu/nf-fwpmu-fwpmcalloutdeletebyid0
			FwpmCalloutDeleteById(Global_WindowsFilteringPlatform_EngineHandle, Global_WindowsFilteringPlatform_AddCalloutId);
		}


		// -----------------------------------------------------------------------------------------------------------------
		// If a callout was registered, unregister it
		if (Global_WindowsFilteringPlatform_RegCalloutId != 0)
		{
			// Unregister the callout from WFP

			// The FwpsCalloutUnregisterById0 function unregisters a callout from the filter engine.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpsk/nf-fwpsk-fwpscalloutunregisterbyid0
			FwpsCalloutUnregisterById(Global_WindowsFilteringPlatform_RegCalloutId);
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Close the WFP engine session

		// The FwpmEngineClose0 function closes a previously opened session to the filter engine.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpmk/nf-fwpmk-fwpmengineclose0
		FwpmEngineClose(Global_WindowsFilteringPlatform_EngineHandle);


		// -----------------------------------------------------------------------------------------------------------------
		// Reset the handle to avoid dangling references
		Global_WindowsFilteringPlatform_EngineHandle = NULL;
	}
	

	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (Unload) - Bye");
}



/**
	@brief      Callback function triggered when a callout notification event occurs.
	@details    This function is invoked by the Windows Filtering Platform (WFP) when a filter-related event happens, such as filter addition, deletion, or modification.


	@see        FWPS_CALLOUT_NOTIFY_TYPE    https://learn.microsoft.com/en-us/previous-versions/aa364335(v=vs.85)
	@param[in]  type                        Type of the notification event.

	@see        GUID                        https://learn.microsoft.com/en-us/windows-hardware/drivers/network/filtering-layer
	@param[in]  filterkey                   Pointer to the GUID of the filter that triggered the event.
	
	@see        FWPS_FILTER                 https://learn.microsoft.com/en-us/windows/win32/api/fwpstypes/ns-fwpstypes-fwps_filter0
	@param[in]  filter                      Pointer to the filter structure.


	@return     Always returns STATUS_SUCCESS.
**/
NTSTATUS
TechniquesWindowsFilteringPlatform_NotifyCallback(
	_In_        FWPS_CALLOUT_NOTIFY_TYPE    type,
	_In_        const GUID*                 filterkey,
	_In_        const FWPS_FILTER*          filter
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing warnings for unused parameter.
	UNREFERENCED_PARAMETER(type);
	UNREFERENCED_PARAMETER(filterkey);
	UNREFERENCED_PARAMETER(filter);


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief      Callback function triggered when a flow deletion event occurs.
	@details    This function is invoked by the Windows Filtering Platform (WFP) when a flow is being deleted. It allows cleanup of any state associated with the flow.


	@see        UINT16                  https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#UINT16
	@param[in]  layerid                 The layer ID where the flow was created.

	@see        UINT32                  https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#UINT32
	@param[in]  calloutid               The callout ID that is associated with this flow deletion.

	@see        UINT64                  https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#UINT64
	@param[in]  flowcontext             Context information for the flow being deleted.
**/
VOID
TechniquesWindowsFilteringPlatform_FlowDeleteCallback(
	_In_        UINT16                  layerid,
	_In_        UINT32                  calloutid,
	_In_        UINT64                  flowcontext
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing warnings for unused parameter.
	UNREFERENCED_PARAMETER(layerid);
	UNREFERENCED_PARAMETER(calloutid);
	UNREFERENCED_PARAMETER(flowcontext);
}



/**
	@brief      WFP callout filter callback that checks outgoing IPv4 connections and blocks them if the remote IP is blacklisted.


	@param[in]  Values                                  Pointer to the structure containing match values for the filter condition.

	@param[in]  MetaData                                Pointer to additional metadata about the packet or event.

	@param[in]  layerdata                               Opaque pointer to layer-specific data.

	@param[in]  context                                 Pointer to context passed when the callout was added.

	@param[in]  filter                                  Pointer to the filter that triggered this callout.

	@param[in]  flowcontext                             Context value associated with the flow.

	@param[out] classifyout                             Pointer to the structure where the action result must be stored.
*/
VOID
TechniquesWindowsFilteringPlatform_FilterCallback(
	_In_        const FWPS_INCOMING_VALUES*             Values,
	_In_        const FWPS_INCOMING_METADATA_VALUES0    MetaData,
	_In_        const PVOID                             layerdata,
	_In_        const void*                             context,
	_In_        const FWPS_FILTER*                      filter,
	_In_        UINT64                                  flowcontext,
	_Out_       FWPS_CLASSIFY_OUT*                      classifyout
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing warnings for unused parameter.
	UNREFERENCED_PARAMETER(flowcontext);
	UNREFERENCED_PARAMETER(context);
	UNREFERENCED_PARAMETER(layerdata);


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if we have the right to set the classify action
	if (!(classifyout->rights & FWPS_RIGHT_ACTION_WRITE))
	{
		if (filter->flags & FWPS_FILTER_FLAG_CLEAR_ACTION_RIGHT)
		{
			classifyout->rights &= ~FWPS_RIGHT_ACTION_WRITE;
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Retrieve local and remote IPs (in UINT32 format)
	ULONG localIp = Values->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_ADDRESS].value.uint32;
	ULONG remoteIp = Values->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_ADDRESS].value.uint32;


	// ---------------------------------------------------------------------------------------------------------------------
	// Convert remote IP to string
	CHAR ipString[16] = { 0 };
	RtlStringCbPrintfA(ipString, sizeof(ipString), "%u.%u.%u.%u", (remoteIp >> 24) & 0xFF, (remoteIp >> 16) & 0xFF, (remoteIp >> 8) & 0xFF, remoteIp & 0xFF);


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the IP is blocked in the resource list
	if (UtilsResources_IsPresent(Global_WindowsFilteringPlatform_BlockedConnectionsListHead, STRUCTURE_UTILSRESOURCES_TYPE_IP, STRUCTURE_UTILSRESOURCES_ACTION_BLOCK, ipString))
	{

		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (FilterCallback) - PID %ld: LocalIP %u.%u.%u.%u | RemoteIP %u.%u.%u.%u\n", (ULONG)(MetaData.processId), (localIp >> 24) & 0xFF, (localIp >> 16) & 0xFF, (localIp >> 8) & 0xFF, localIp & 0xFF, (remoteIp >> 24) & 0xFF, (remoteIp >> 16) & 0xFF, (remoteIp >> 8) & 0xFF, remoteIp & 0xFF);
		classifyout->actionType = FWP_ACTION_BLOCK;
		classifyout->rights &= ~FWPS_RIGHT_ACTION_WRITE;
		return;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Allow connection if not blocked
	classifyout->actionType = FWP_ACTION_PERMIT;
}



/**
	@brief      Opens the Windows Filtering Platform (WFP) engine.
	@details    This function establishes a connection with the WFP engine, allowing further configuration of callouts, sublayers, and filters.


	@return     NTSTATUS code indicating success or failure.
**/
NTSTATUS
TechniquesWindowsFilteringPlatform_WfpOpenEngine()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Open WFP Engine

	// The FwpmEngineOpen function connects to the WFP engine, allowing the driver to register callouts, create filters, and manage network traffic control.

	// The FwpmEngineOpen0 function opens a session to the filter engine.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpmk/nf-fwpmk-fwpmengineopen0
	NTSTATUS status = FwpmEngineOpen(NULL, RPC_C_AUTHN_WINNT, NULL, NULL, &Global_WindowsFilteringPlatform_EngineHandle);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (WfpOpenEngine) - FwpmEngineOpen failed (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief      Registers a callout in the Windows Filtering Platform.
	@details    This function registers a callout for monitoring and controlling network traffic at a specific layer.


	@param[in]  pDeviceObject           Pointer to the device object for the driver.


	@return     NTSTATUS code indicating success or failure.
**/
NTSTATUS
TechniquesWindowsFilteringPlatform_WfpRegisterCallout(
	_In_        PDEVICE_OBJECT          pDeviceObject
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Define Callout Structure

	// The FWPS_CALLOUT0 structure defines the data that is required for a callout driver to register a callout with the filter engine.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpsk/ns-fwpsk-fwps_callout0_
	FWPS_CALLOUT Callout = { 0 };
	Callout.calloutKey = WFP_SAMPLE_ESTABLISHED_CALLOUT_V4_GUID;
	Callout.flags = 0;
	Callout.classifyFn = (FWPS_CALLOUT_CLASSIFY_FN3)TechniquesWindowsFilteringPlatform_FilterCallback;
	Callout.notifyFn = (FWPS_CALLOUT_NOTIFY_FN3)TechniquesWindowsFilteringPlatform_NotifyCallback;
	Callout.flowDeleteFn = (FWPS_CALLOUT_FLOW_DELETE_NOTIFY_FN0)TechniquesWindowsFilteringPlatform_FlowDeleteCallback;


	// ---------------------------------------------------------------------------------------------------------------------
	// Register the Callout

	// The FwpsCalloutRegister function registers the callout with the WFP engine.

	// The FwpsCalloutRegister0 function registers a callout with the filter engine.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fwpsk/nf-fwpsk-fwpscalloutregister0
	NTSTATUS status = FwpsCalloutRegister(pDeviceObject, &Callout, &Global_WindowsFilteringPlatform_RegCalloutId);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (WfpRegisterCallout) - FwpsCalloutRegister failed (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief      Adds a callout to the Windows Filtering Platform engine.
	@details    This function creates a new callout in the filtering engine, allowing it to interact with network traffic at a specified layer.


	@return     NTSTATUS code indicating success or failure.
**/
NTSTATUS
TechniquesWindowsFilteringPlatform_WfpAddCallout()
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
	NTSTATUS status = FwpmCalloutAdd(Global_WindowsFilteringPlatform_EngineHandle, &callout, NULL, &Global_WindowsFilteringPlatform_AddCalloutId);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (WfpAddCallout) - FwpmCalloutAdd failed (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief      Adds a sublayer to the Windows Filtering Platform.
	@details    This function defines a custom sublayer that will be used to group filtering rules for easier management.


	@return     NTSTATUS code indicating success or failure.
**/
NTSTATUS
TechniquesWindowsFilteringPlatform_WfpAddSublayer()
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
	NTSTATUS status = FwpmSubLayerAdd(Global_WindowsFilteringPlatform_EngineHandle, &sublayer, NULL);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (WfpAddSublayer) - FwpmSubLayerAdd failed (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief      Adds a filter to the Windows Filtering Platform.
	@details    This function creates a filter that applies to specific network traffic based on defined conditions.


	@return     NTSTATUS code indicating success or failure.
**/
NTSTATUS
TechniquesWindowsFilteringPlatform_WfpAddFilter()
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
	NTSTATUS status = FwpmFilterAdd(Global_WindowsFilteringPlatform_EngineHandle, &filter, NULL, &Global_WindowsFilteringPlatform_FilterId);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (WfpAddFilter) - FwpmFilterAdd failed (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief      Initializes the WFP driver and sets up filters.


	@param[in]  pDeviceObject           Pointer to the device object for the driver.


	@return     NTSTATUS code indicating success or failure.
**/
NTSTATUS
TechniquesWindowsFilteringPlatform_Init(
	_In_        PDEVICE_OBJECT          pDeviceObject
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Open the WFP engine to establish a session
	status = TechniquesWindowsFilteringPlatform_WfpOpenEngine();

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (Init) - TechniquesWindowsFilteringPlatform_WfpOpenEngine failed (Status 0x%08X).\n", status);
		TechniquesWindowsFilteringPlatform_Unload();
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Register a callout to define custom filtering logic
	status = TechniquesWindowsFilteringPlatform_WfpRegisterCallout(pDeviceObject);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (Init) - TechniquesWindowsFilteringPlatform_WfpRegisterCallout failed (Status 0x%08X).\n", status);
		TechniquesWindowsFilteringPlatform_Unload();
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Add the callout to WFP so it can be referenced in filters
	status = TechniquesWindowsFilteringPlatform_WfpAddCallout();

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (Init) - TechniquesWindowsFilteringPlatform_WfpAddCallout failed (Status 0x%08X).\n", status);
		TechniquesWindowsFilteringPlatform_Unload();
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Add a sublayer to categorize the filter within the WFP filtering hierarchy
	status = TechniquesWindowsFilteringPlatform_WfpAddSublayer();

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (Init) - TechniquesWindowsFilteringPlatform_WfpAddSublayer failed (Status 0x%08X).\n", status);
		TechniquesWindowsFilteringPlatform_Unload();
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Add a filter rule that uses the callout for traffic inspection
	status = TechniquesWindowsFilteringPlatform_WfpAddFilter();

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (Init) - TechniquesWindowsFilteringPlatform_WfpAddFilter failed (Status 0x%08X).\n", status);
		TechniquesWindowsFilteringPlatform_Unload();
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



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
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (Block) - Hello\n");


	// ----------------------------------------------------------------------------------------------------
	// Validate input
	if (!ipAddress || strlen(ipAddress) == 0)
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "Invalid IP address.");
		return STATUS_INVALID_PARAMETER;
	}


	// ----------------------------------------------------------------------------------------------------
	// Check if the resource is already in the tracking list
	if (UtilsResources_IsPresent(Global_WindowsFilteringPlatform_BlockedConnectionsListHead, STRUCTURE_UTILSRESOURCES_TYPE_IP, STRUCTURE_UTILSRESOURCES_ACTION_BLOCK, ipAddress))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "IP is already blocked.");
		return STATUS_ALREADY_REGISTERED;
	}


	// ----------------------------------------------------------------------------------------------------
	// Add to resource list
	if (!UtilsResources_Add(&Global_WindowsFilteringPlatform_BlockedConnectionsListHead, &Global_WindowsFilteringPlatform_BlockedConnectionsListCount, STRUCTURE_UTILSRESOURCES_TYPE_IP, STRUCTURE_UTILSRESOURCES_ACTION_BLOCK, ipAddress))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "Failed to add IP to block list.");
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Inform user-space that the IP was successfully blocked

	// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
	RtlStringCbPrintfA(responseBuffer, responseBufferSize, "IP %s blocked.", ipAddress);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (Block) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



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
)
{
	// ----------------------------------------------------------------------------------------------------
	// Variables
	CHAR ipAddress[260] = { 0 };


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (Unblock) - Hello\n");


	// ----------------------------------------------------------------------------------------------------
	// Retrieve the IP address from the list
	if (!UtilsResources_GetIdentifierByIndex(Global_WindowsFilteringPlatform_BlockedConnectionsListHead, index, ipAddress, sizeof(ipAddress)))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbPrintfA(responseBuffer, responseBufferSize, "No IP found for index %lu.", index);
		return STATUS_NOT_FOUND;
	}


	// ----------------------------------------------------------------------------------------------------
	// Remove from list
	if (!UtilsResources_Remove(&Global_WindowsFilteringPlatform_BlockedConnectionsListHead, &Global_WindowsFilteringPlatform_BlockedConnectionsListCount, index))
	{
		// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
		RtlStringCbPrintfA(responseBuffer, responseBufferSize, "Failed to remove IP entry at index %lu.", index);
		return STATUS_UNSUCCESSFUL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Confirm success to user-space

	// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
	RtlStringCbPrintfA(responseBuffer, responseBufferSize, "IP at index %lu unblocked.", index);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/WindowsFilteringPlatform.c] (Unblock) - Bye\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



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
)
{
	// ----------------------------------------------------------------------------------------------------
	// Check if the blocked list is empty
	if (Global_WindowsFilteringPlatform_BlockedConnectionsListHead == NULL)
	{
		// The RtlStringCbCopyW and RtlStringCbCopyA functions copy a byte-counted string into a buffer.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbcopya
		RtlStringCbCopyA(responseBuffer, responseBufferSize, "No blocked IPs found.");
		return STATUS_SUCCESS;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Dump blocked IPs into the response buffer
	return UtilsResources_List(Global_WindowsFilteringPlatform_BlockedConnectionsListHead, responseBuffer, responseBufferSize);
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
