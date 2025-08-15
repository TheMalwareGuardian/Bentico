// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://en.wikibooks.org/wiki/C_Programming/stdio.h
// The C programming language provides many standard library functions for file input and output. These functions make up the bulk of the C standard library header <stdio.h>. The I/O functionality of C is fairly low-level by modern standards; C abstracts all file operations into operations on streams of bytes, which may be "input streams" or "output streams". Unlike some earlier programming languages, C has no direct support for random-access data files; to read from a record in the middle of a file, the programmer must create a stream, seek to the middle of the file, and then read bytes in sequence from the stream. 
#include <stdio.h>



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "Utils00Table.h"
#include "../../Structures/Structures00Techniques.h"
#include "../../Globals/Globals01Techniques.h"



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Prints a horizontal line used as a separator in the table.
**/
void UtilsTable_PrintLine()
{
	// ---------------------------------------------------------------------------------------------------------------------
	printf("    ");
	for (int i = 0; i < MACRO_UTILSTABLE_COL_CMD + MACRO_UTILSTABLE_COL_DESC + MACRO_UTILSTABLE_COL_EX + 10; ++i)
	{
		printf("=");
	}
	printf("\n");
}



/**
	@brief      Prints the header row of the command table with column titles.
**/
void UtilsTable_PrintHeader()
{
	// ---------------------------------------------------------------------------------------------------------------------
	UtilsTable_PrintLine();
	printf("    | %-*s | %-*s | %-*s |\n", MACRO_UTILSTABLE_COL_CMD, "Command", MACRO_UTILSTABLE_COL_DESC, "Description", MACRO_UTILSTABLE_COL_EX, "Example");
	UtilsTable_PrintLine();
}



/**
	@brief      Prints a single command entry row with command, description, and example.
**/
void UtilsTable_PrintRow(const STRUCTURES_TECHNIQUES_COMMAND_ENTRY* e)
{
	// ---------------------------------------------------------------------------------------------------------------------
	printf("    | * %-*s | %-*s | %-*s |\n", MACRO_UTILSTABLE_COL_CMD-2, e->command, MACRO_UTILSTABLE_COL_DESC, e->description, MACRO_UTILSTABLE_COL_EX, e->example ? e->example : "-");
}



/**
	@brief      Prints a full table of commands belonging to the specified category.
**/
void UtilsTable_PrintTableByCategory(STRUCTURES_TECHNIQUES_COMMAND_CATEGORY cat)
{
	// ---------------------------------------------------------------------------------------------------------------------
	printf(" \033[0;32m[+]\033[0m\n");
	printf("    \033[38;5;214m%s\033[0m\n", Globals_Techniques_CategoryNames[cat]);
	UtilsTable_PrintHeader();
	for (size_t i = 0; i < MACRO_UTILSTABLE_CMD_TOTAL; ++i)
	{
		if (Globals_Techniques_Commands[i].category == cat)
		{
			UtilsTable_PrintRow(&Globals_Techniques_Commands[i]);
		}
	}
	UtilsTable_PrintLine();
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
