// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "../../Structures/Structures00Techniques.h"



// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------
// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------



#define MACRO_UTILSTABLE_CMD_TOTAL (sizeof(Globals_Techniques_Commands)/sizeof(Globals_Techniques_Commands[0]))
#define MACRO_UTILSTABLE_COL_CMD 32
#define MACRO_UTILSTABLE_COL_DESC 64
#define MACRO_UTILSTABLE_COL_EX 45



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Prints a horizontal line used as a separator in the table.
**/
void UtilsTable_PrintLine();



/**
	@brief      Prints the header row of the command table with column titles.
**/
void UtilsTable_PrintHeader();



/**
	@brief      Prints a single command entry row with command, description, and example.
**/
void UtilsTable_PrintRow(const STRUCTURES_TECHNIQUES_COMMAND_ENTRY* e);



/**
	@brief      Prints a full table of commands belonging to the specified category.
**/
void UtilsTable_PrintTableByCategory(STRUCTURES_TECHNIQUES_COMMAND_CATEGORY cat);



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
