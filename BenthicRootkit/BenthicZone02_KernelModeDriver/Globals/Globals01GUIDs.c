// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



#define INITGUID

#include <guiddef.h>

// https://learn.microsoft.com/en-us/windows/win32/api/fwpmu/
// This header is used by Windows Filtering Platform.
#include <fwpmu.h>



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "Globals01GUIDs.h"



// START -> GUIDS -----------------------------------------------------------------------------------------------------------------------------
// START -> GUIDS -----------------------------------------------------------------------------------------------------------------------------



// Define V4 GUIDs for the callout and sublayer
// https://www.uuidgenerator.net/version4

// You define a new GUID for an item the driver exports to other system components, drivers, or applications. For example, you define a new GUID for a custom PnP event on one of its devices. Use the DEFINE_GUID macro (defined in Guiddef.h) to associate the GUID symbolic name with its value.
// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-and-exporting-new-guids
DEFINE_GUID(WFP_SAMPLE_ESTABLISHED_CALLOUT_V4_GUID, 0x23bb0568, 0x4cc2, 0x4039, 0x8c, 0x3e, 0x39, 0x23, 0xde, 0x47, 0x6e, 0x5d);
DEFINE_GUID(WFP_SAMPLE_SUB_LAYER_GUID, 0x64051bab, 0x7763, 0x44a3, 0xba, 0xd0, 0x14, 0x11, 0xe6, 0xc7, 0x79, 0x48);



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
