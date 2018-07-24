
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

int main(int argc, char **argv) {
	S_BOARD pos[1];//Declare the variable for the board 
	S_SEARCHINFO info[1];//Declare the variable for info 
	char line[256];//character array for taking the input
	int ArgNum = 0;//Set the argument no. to 0

	pos->HashTable->pTable = NULL;//Set the pTable pointer to NULL

	AllInit();//Intiailize everything 
	InitHashTable(pos->HashTable, 64);//Initialize the hash-table
	memset(pos->searchKillers, 0, 2 * MAXDEPTH * sizeof(int));//Set the memory with searchKillers variables which is stored in pos pointer

	setbuf(stdin, NULL);//Set the standard input to NULL
	setbuf(stdout, NULL);//Set the standard output to NULL

	for(ArgNum = 0; ArgNum < argc; ++ArgNum) {//Loop through arguments 
		if(strncmp(argv[ArgNum], "NoBook", 6) == 0) {//If the Argument is NoBook
			EngineOptions->UseBook = FALSE;//Don't use the Book from the PolyBooks
		}
	}
	
	printf("Welcome to Axiom! Type 'Axiom' for console mode...\n");//Set the Welcome command

	while (TRUE) {//Set the infinite while loop
		memset(&line[0], 0, sizeof(line));//Set the memory with the line that is the character array

		fflush(stdout);//flush all the output from the screen
		if (!fgets(line, 256, stdin))//If we can't get a string from the file 
			continue;//continue
		if (line[0] == '\n')//Set newline at the oth index of the line
			continue;//continue
		if (!strncmp(line, "uci",3)) {//If The command passed is uci			
			Uci_Loop(pos, info);//Call the Uci_Loop
			if(info->quit == TRUE) break;//If the quit is True then break the loop
			continue;//continue
		} else if (!strncmp(line, "xboard", 6))	{//else if the command is xboard 
			XBoard_Loop(pos, info);//Call the Xboard_Loop
			if(info->quit == TRUE) break;//If the quit is set to true then break
			continue;//continue
		} else if (!strncmp(line, "Axiom", 5))	{//If the command is AxiomChess
			Console_Loop(pos, info);//Call the Console_loop
			if(info->quit == TRUE) break;//If the quit is true then break
			continue;//continue
		} else if(!strncmp(line, "quit", 4))	{//If the command is quit
			break;//then break
		}
	}
	
	free(pos->HashTable->pTable);
	CleanPolyBook();
	return 0;
}
