

#include <stdio.h>
#include "defs.h"

long leafNodes;

void Perft(int depth, S_BOARD *pos) {//Program to find the perfect move the board.
	S_MOVELIST list[1];//Moves list .
	int MoveNum = 0; //Integer variable to store the move no.

	ASSERT(CheckBoard(pos)); //To check the board position.

	if(depth == 0) {//If depth is 0 then traverse through the leafnodes.
		leafNodes++;
		return;
	}

	GenerateAllMoves(pos, list);//Function call to generate all the moves available in the current position.

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		if(!MakeMove(pos, list->moves[MoveNum].move)) {
			continue;
		}

		Perft(depth - 1, pos);//Recursive call to the Perft function by decreasing the depth by 1.
		TakeMove(pos);//To take the move at a particular position.
	}

	return;
}

void PerftTest(int depth, S_BOARD *pos) {//Function to test the depth of the move ...
	S_MOVELIST list[1];
	int move;
	int MoveNum = 0;
	int start = GetTimeMs();//This function call gets time in Milliseconds.
	long cumnodes, oldnodes;//Total no. of nodes of type long.

	ASSERT(CheckBoard(pos));

	PrintBoard(pos);//Print the position of the board.
	printf("\nStarting Test To Depth:%d\n", depth);
	leafNodes = 0;
	
	GenerateAllMoves(pos, list);//To generate all the moves at a position.

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		move = list->moves[MoveNum].move;
		if(!MakeMove(pos, move))
			continue;

		cumnodes = leafNodes;
		Perft(depth - 1, pos);//Recursive call to Perft function which tests' for the different nodes of a tree. 
		TakeMove(pos);//To take a move at a position.
		oldnodes = leafNodes - cumnodes;
		printf("move %d : %s : %ld\n", MoveNum + 1, PrMove(move), oldnodes);//to print the available moves at a particular position.
	}

	printf("\nTest Complete : %ld nodes visited in %dms\n", leafNodes, GetTimeMs() - start);//To print the no.of visited nodes and their time
}
