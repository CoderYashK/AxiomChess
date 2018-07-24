
#include "defs.h"

#define INF 30000
#define MATE 29000

static void CheckUp(S_SEARCHINFO *info) {//Check up function
	if(info->timeset == TRUE && GetTimeMs() > info->stoptime)//If time is set and greater than stopping time
		info->stopped = TRUE;//Set stopped to true

	ReadInput(info);//Read the information
}

static void PickNextMove(int moveNum, S_MOVELIST *list) {//Function to pick the moves from moves' list and count moveNum
	S_MOVE temp;//Create a temporary variable for movelist
	int index = 0;//Initialize the some member variables to 0
	int bestScore = 0;
	int bestNum = moveNum;

	for(index = moveNum; index < list->count; ++index) {//Loop through the list
		if(list->moves[index].score > bestScore) {//If the score at particular index is greater than bestscore
			bestScore = list->moves[index].score;//Set the bestscore to that score 
			bestNum = index;//And bestnum to that index
		}
	}

	temp = list->moves[moveNum];//Swap the move at moveNum index and bestNum index in the moves array
	list->moves[moveNum] = list->moves[bestNum];
	list->moves[bestNum] = temp;
}

static int IsRepetition(const S_BOARD *pos) {//Function the detect repetition
	int index = 0;

	for(index = pos->hisPly - pos->fiftyMove; index < pos->hisPly; ++index) {//Loop through last fiftry moves 
		ASSERT(index >= 0 && index < MAXGAMEMOVES);//Check the validity of the index
		if(pos->posKey == pos->history[index].posKey)//If the position keys are the same 
			return TRUE;//Then detect that it's a repetition
	}

	return FALSE;//else it's not a repetition
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {//Function takes the board and searchinfo pointer info
	int index = 0;
	int index2 = 0;

	for(index = 0; index < 13; ++index)//Loop through all the indices
		for(index2 = 0; index2 < BRD_SQ_NUM; ++index2)//Loop through all the squares
			pos->searchHistory[index][index2] = 0;//Set the history array to 0

	for(index = 0; index < 2; ++index)//Loop through 2 indices
		for(index2 = 0; index2 < MAXDEPTH; ++index2)//Loop through 0 to maxdepth
			pos->searchKillers[index][index2] = 0;//Set the searchkillers array to 0

	ClearHashTable(pos->HashTable);//Clear the hashTable that is free the memory
	pos->ply = 0;//Set all the related variables to 0

	info->stopped = 0;
	info->nodes = 0;
	info->fh = 0;//Finite heuristics i.e, finite no. of searchs.
	info->fhf = 0;//Final finite heuristics i.e, finite no. of searchs.
}

static int myalgo(int depth) {//My algorithm cuts all the non-necessary search node values skips' when inCheck and otherwise increases the 
//efficiency of the engine by traversing more no. of nodes in less time
	return (512 + depth * 2);	

};	

static int mysearch(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {//Function to improve the alpha-beta function search
	int MoveNum = 0;//Initialize some variables to 0
	int Legal = 0;//Set the legal to 0
	int Score;//Declare score
	S_MOVELIST list[1];//Declare list variable 

	ASSERT(CheckBoard(pos));//Check the position on the board
	ASSERT(beta > alpha);//Check if beta is greater than alpha

	if((info->nodes & 2047) == 0)//If the nodes are not equal to 2047 
		CheckUp(info);//Then check the info

	info->nodes++;//Increment the no. of nodes

	if(IsRepetition(pos) || pos->fiftyMove >= 100)//Check for the repetition
		return 0;//If yes then return 0

	if(pos->ply > MAXDEPTH - 1)//If the max depth is reached 
		return EvalPosition(pos);//Evaluate the position at that depth

	Score = EvalPosition(pos);//Retain the score from that particular position

	ASSERT(Score > -INF && Score < INF);//Check if the score is in the proper range

	if(Score >= beta)//If the score is greater than beta
		return beta;//Do the beta cut-off

	if(Score > alpha)//If the score is greater than alpha 
		alpha = Score;//Set the score equal to alpha

	GenerateAllCaps(pos, list);//Generate all the captures 

	Score = -INF;//Set the score to -ve infinity

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {//Loop through the list 
		PickNextMove(MoveNum, list);//Keep picking the next moves from the list 

		if (!MakeMove(pos, list->moves[MoveNum].move))//If the move can't be made 
			continue;//continue

		Legal++;//Increment the legal moves
		Score = -mysearch(-beta, -alpha, pos, info);//Use the nega-max style call for the mysearch search
		TakeMove(pos);//Takemove at a particular position

		if(info->stopped == TRUE)//If the time's up
			return 0;//return 0

		if(Score > alpha) {//If the score is greater than alpha
			if(Score >= beta) {//If it's greater than beta as well
				if(Legal == 1)//If the legal is 1
					info->fhf++;//Increment fhf
				info->fh++;//else increment fh
				return beta;//Return beta
			}
			alpha = Score;//set the alpha equal to score
		}
	}

	return alpha;//Return Alpha
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {//Very important function for the engine
	S_MOVELIST list[1];//variable for the movelist 
	int InCheck;//To store in check
	int Score = -INF;//Set the score to the -infinity
	int PvMove = NOMOVE;//Pv move is set to nomove
	int MoveNum = 0;//Move num is set to 0
	int Legal = 0;//Legal moves variable is set to 0
	int OldAlpha = alpha;//OldAlpha is initialized to alpha
	int BestMove = NOMOVE;//Best move is set to no move
	int BestScore = -INF;//Best score is set to -infinity
	int ralpha;//To hold the value of the cut-off for alpha
	int v;//To store the integer value of the score 
	ASSERT(CheckBoard(pos));//Check the position on the board 
	ASSERT(beta > alpha);//Check if beta is greater than alpha
	ASSERT(depth >= 0);//Check if depth is greater than 0

	if(depth <= 0)//Depth is less than or equal to zero call the mysearch search
		return mysearch(alpha, beta, pos, info);//return mysearch returned value

	if((info->nodes & 2047) == 0)//2047 is just a random no. chosen to bitwise and with nodes it is (2 ^ 11) - 1
		CheckUp(info);//Check the information

	info->nodes++;//Increment the no. of nodes

	if((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply)//If it's repetition return zero which is a draw
		return 0;

	if(pos->ply > MAXDEPTH - 1)//If the ply = maxdepth - 1, If the maxdepth is reached 
		return EvalPosition(pos);//Evaluate the position

	InCheck = SqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos);//If the king on one side is attacked by other side

	if(InCheck == TRUE)//If it's in check then increment depth of the search
		depth++;

	if(ProbeHashEntry(pos, &PvMove, &Score, alpha, beta, depth) == TRUE) {//If the PvMove is store in the pv-table 
		pos->HashTable->cut++;//Then increment the cut variable in the hashtable
		return Score;//return the score
	}

	if(DoNull && !InCheck && pos->ply && (pos->bigPce[pos->side] > 0) && depth >= 4) {//If it's not in check it can do null, no of bigpieces
		MakeNullMove(pos);//is greater than 0 and depth is greater than or equal to say 4
		Score = -AlphaBeta(-beta, -beta + 1, depth - 4, pos, info, FALSE);//MakeNullmove, call Alphabeta TakeNullmove
		TakeNullMove(pos);
		if(info->stopped == TRUE)//If the time's up return 0
			return 0;

		if (Score >= beta && abs(Score) < ISMATE) {//If score is greater than beta or equal to beta and absolute score is less than mate
			info->nullCut++;//score increment the nullcutt that is beta cut off 
			return beta;//and return beta
		}
	}

	GenerateAllMoves(pos, list);//Generate all the moves from the list at a particular position

	Score = -INF;//Set score to -infinity

	if(PvMove != NOMOVE) {//If pvmove is not nomove
		for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {//Loop through the moves 
			if( list->moves[MoveNum].move == PvMove) {//As it's the best move 
				list->moves[MoveNum].score = 2000000;//So set the score for that move equal to 2-Million
				break;//And then break the loop
			}
		}
	}

	int FoundPv = FALSE;//Initialise the FoundPv to false, 	

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {//Loop through the list 
		PickNextMove(MoveNum, list);//Keep picking the next moves from the list 

		if(!MakeMove(pos,list->moves[MoveNum].move))//If Making move is not possible
			continue;//Then continue

		Legal++;//Increment the count of possible making moves or legal moves
		//Additional if else
		if(FoundPv == TRUE) {//When the alpha is improved if initially alpha beta are -100, 100 the call passes the search values as,
			Score = -AlphaBeta(-alpha - 1, -alpha, depth - 1, pos, info, TRUE);//99, 100 hence significantly reduces search time
			if(Score > alpha && Score < beta) {//But Score is between alpha and beta then 
				Score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, TRUE);//Call the alpha-beta function normally.
                        }
		
		} else {//So, it will run the normal nega-max alpha-beta search call, 
				Score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, TRUE);//Normal call to the alpha-beta function.
		}
		
		TakeMove(pos);//Take the move

		if(info->stopped == TRUE)//If the time's up
			return 0;//return 0

		if(Score > BestScore) {//If score is greater than the best-score
			BestScore = Score;//Set the best score equal to the Score
			BestMove = list->moves[MoveNum].move;//Best move is the move having the best score
			if(Score > alpha) {//If the score is greater than alpha
				if(Score >= beta) {//If the score is greater than beta
					if(Legal == 1)//If the move is legal
						info->fhf++;//increment fhf
					info->fh++;//else increment fh

					if(!(list->moves[MoveNum].move & MFLAGCAP)) {//If the position does not have capture move
						pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];//Add the killer moves
						pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;//Shift the moves to the 1st pos
					}

					StoreHashEntry(pos, BestMove, beta, HFBETA, depth);
					return beta;//Return the beta
				}//As, alpha gets improved set FoundPv = TRUE
				FoundPv = TRUE;
				alpha = Score;//Set the alpha equal to the current score

				if(!(list->moves[MoveNum].move & MFLAGCAP))//If the move is not the capture move 
					pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
			}//Then set the value at depth + value at the searchhistory array
		}
	}
	
			

	if(Legal == 0) {//Then set the value at depth + value at the searchhistory array
		if(InCheck) {//And the King is in check 
			return -INF + pos->ply;//Return the no. of moves left to do the mate
		} else {
			return 0;//Else it's a stalemate and return 0
		}
	}

	ASSERT(alpha >= OldAlpha);//Check if the Alpha is greater than OldAlpha

	if(alpha != OldAlpha) {//If the alpha is not equal to oldalpha, Store the exact known Hash entry in the hash table
		StoreHashEntry(pos, BestMove, BestScore, HFEXACT, depth);
	} else {//Else Store the hash entry containing alpha as it's best score
		StoreHashEntry(pos, BestMove, alpha, HFALPHA, depth);
	}

	if(!InCheck) //My sharpening code
		if(PvMove == NOMOVE) 
			if(depth < 2 * (MAXDEPTH - 1) && Score + myalgo(depth) <= alpha) //To reduce the no. of moves searched irrelevant 
				if(depth <= MAXDEPTH - 1 && Score + myalgo(3 * (MAXDEPTH - 1)) < alpha) //To increase the time of the search 
					return mysearch(alpha, beta, pos, info);
			ralpha = alpha - myalgo(depth);//raplha is the variable to hold the cut-off score
			v = mysearch(ralpha, ralpha + 1, pos, info);
			if(v <= alpha)
				return v;
						
	return alpha;//Return alpha
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
	int bestMove = NOMOVE;//Initialize
	int bestScore = -INF;//Best move is set to -infinity
	int currentDepth = 0;
	int pvMoves = 0;//Store the move in the principle variation table
	int pvNum = 0;//Store the number of moves in the principle variation table

	ClearForSearch(pos, info);//Clear the position 

	if(EngineOptions->UseBook == TRUE)//If we can use the book i.e, the polybook
		bestMove = GetBookMove(pos);//Get the best book move from that position
//Iterative deepening
	if(bestMove == NOMOVE) {//If the best move is nomove			
		for(currentDepth = 1; currentDepth <= info->depth; ++currentDepth) {//Loop through the depth 
			bestScore = AlphaBeta(-INF, INF, currentDepth, pos, info, TRUE);//Take the bestscore by calling alphabeta from the 
//current depth
			if(info->stopped == TRUE)//If the time is up then break the search
				break;

			pvMoves = GetPvLine(currentDepth, pos);//Get the pvmoves from the getpvline of the current depth and position
			bestMove = pos->PvArray[0];//Store the best move in the 0 index of the Pv array

			if(info->GAME_MODE == UCIMODE) {//If the game mode is UCI mode
				printf("info score cp %d depth %d nodes %ld time %d ",//print the information for the GUI
					bestScore, currentDepth, info->nodes, GetTimeMs() - info->starttime);
			} else if(info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE) {//If the mode is Xboard print the information
				printf("%d %d %d %ld ",
					currentDepth, bestScore, (GetTimeMs() - info->starttime) / 10, info->nodes);
			} else if(info->POST_THINKING == TRUE) {//Print the information at the current position(situation)
				printf("score:%d depth:%d nodes:%ld time:%d(ms) ",
					bestScore, currentDepth, info->nodes, GetTimeMs() - info->starttime);
			}	
	
			if(info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE) {//If the mode is uci and engine is thinking
				pvMoves = GetPvLine(currentDepth, pos);//Get the pvmoves
				printf("pv");//Print pv
				for(pvNum = 0; pvNum < pvMoves; ++pvNum)//Loop through the pvmoves
					printf(" %s", PrMove(pos->PvArray[pvNum]));//Print the prmove in the pvarray at pvnum index
				printf("\n");
			}
		}
	}

	if(info->GAME_MODE == UCIMODE) {//If the game mode is UCI 
		printf("bestmove %s\n", PrMove(bestMove));//print the best move
	} else if(info->GAME_MODE == XBOARDMODE) {//If the game mode is Xboard		
		printf("move %s\n", PrMove(bestMove));//Print the best move 
		MakeMove(pos, bestMove);//Make that best move
	} else {//Else for the terminal print the best move and the board 
		printf("\n\n***!! Axiom makes move %s !!***\n\n", PrMove(bestMove));
		MakeMove(pos, bestMove);
		PrintBoard(pos);
	}
}
