

#include <stdio.h>
#include "defs.h"

#define MOVE(f, t, ca, pro, fl) ((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl))//different types of moves macro
#define SQOFFBOARD(sq) (FilesBrd[(sq)] == OFFBOARD)//offboard - macro 
/*File containing move-generation functions */
const int LoopSlidePce[8] = { wB, wR, wQ, 0, bB, bR, bQ, 0 };//Major pieces on the board
const int LoopSlideIndex[2] = { 0, 4 };//Index for major pieces
const int LoopNonSlidePce[6] = { wN, wK, 0, bN, bK, 0 };//Minor pieces on the board
const int LoopNonSlideIndex[2] = { 0, 3 };//Index for minor pieces 

const int PceDir[13][8] = {//Array containing the positional value of the pieces on the board on different
	{ 0, 0, 0, 0, 0, 0, 0 },//squares.
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 }
};

const int NumDir[13] = { 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8 };//Directory containing different numbers assigned to the pieces

const int VictimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };//Scores assigned to different pieces
static int MvvLvaScores[13][13];//2-d array containing different moves //Maximum value victim and least value attacker function

void InitMvvLva(void) {//Initialize the MvvLva function
	int Attacker;
	int Victim;

	for(Attacker = wP; Attacker <= bK; ++Attacker) {//Loop through all the attacking pieces 
		for(Victim = wP; Victim <= bK; ++Victim) {//Loop through all the victim pieces 
			MvvLvaScores[Victim][Attacker] = VictimScore[Victim] + 6 - (VictimScore[Attacker] / 100);
		}//Mathematical expression for the Mvvlva array which contain the scores of different victim and attacker combination
	}
}

int MoveExists(S_BOARD *pos, const int move) {//Function to check whether the move exists on the board.
	int MoveNum = 0;
	
	S_MOVELIST list[1];//Movelist variable for the list 
	GenerateAllMoves(pos, list);//generate all the moves
	
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {//Loop through all the moves in the list 
		if(!MakeMove(pos, list->moves[MoveNum].move)) {//If the makemove returns false then continue for the next move
			continue;
		}
		TakeMove(pos);//else take the move which is made 
		if(list->moves[MoveNum].move == move) {//and store that move in the list of the moves
			return TRUE;//return true
		}
	}
	return FALSE;//return false 
}

static void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list) {//Quiet moves function in order to get rid of the horizon effect
	list->moves[list->count].move = move;//store the move sent by the caller in the list of the moves 

	if(pos->searchKillers[0][pos->ply] == move) {//Check if the move is found in the searchkillers 2-d array at 0th index 
		list->moves[list->count].score = 900000;//If found then assign the score to 9 lakh
	} else if(pos->searchKillers[1][pos->ply] == move) {//Check for the move in the 1st position 
		list->moves[list->count].score = 800000;//If found then assign the score to 8 lakh
	} else {//If not found then store the normal score in the list of the moves from the search-history array
		list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][TOSQ(move)];
	}
	list->count++;//Increment the count of the list
}

static void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list) {//loop add capture move in the board position
	list->moves[list->count].move = move;//Store the move in the list 
	list->moves[list->count].score = MvvLvaScores[CAPTURED(move)][pos->pieces[FROMSQ(move)]] + 1000000;//Store the score for the move from
	list->count++;//MvvLvaScores 2-d array + 1Million and then increment the count of the list
}

static void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list) {//Code to add EnPassant move
	list->moves[list->count].move = move;//To store the move in the list passed by the caller
	list->moves[list->count].score = 105 + 1000000;//To store the score in the list for that particular move as 105+1Million
	list->count++;//Increment the count of the list 
}

static void AddWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
	ASSERT(PieceValidEmpty(cap));//assert validity
	ASSERT(SqOnBoard(from));//assert from square's onboard
	ASSERT(SqOnBoard(to));//assert to square's onboard

	if(RanksBrd[from] == RANK_7) {//If the rank of the from square is 7
		AddCaptureMove(pos, MOVE(from, to, cap, wQ, 0), list);//Add the capture moves for the different pieces in the list 
		AddCaptureMove(pos, MOVE(from, to, cap, wR, 0), list);
		AddCaptureMove(pos, MOVE(from, to, cap, wB, 0), list);
		AddCaptureMove(pos, MOVE(from, to, cap, wN, 0), list);
	} else {
		AddCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);//else add the empty capture move
	}//White pawn capture move
}

static void AddWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
	ASSERT(SqOnBoard(from));//assert the from square 
	ASSERT(SqOnBoard(to));//assert the to square 

	if(RanksBrd[from] == RANK_7) {//If the rank of the from square is 7
		AddQuietMove(pos, MOVE(from, to, EMPTY, wQ, 0), list);//Add quiet move for all other pieces in the list
		AddQuietMove(pos, MOVE(from, to, EMPTY, wR, 0), list);
		AddQuietMove(pos, MOVE(from, to, EMPTY, wB, 0), list);
		AddQuietMove(pos, MOVE(from, to, EMPTY, wN, 0), list);
	} else {
		AddQuietMove(pos, MOVE(from, to, EMPTY, EMPTY, 0), list);//else add the empty move
	}//White pawns' move
}

static void AddBlackPawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
	ASSERT(PieceValidEmpty(cap));//assert the validity
	ASSERT(SqOnBoard(from));//assert the validity of the from square 
	ASSERT(SqOnBoard(to));//assert the validity of the to square 
	
	if(RanksBrd[from] == RANK_2) {//If the rank of the from square is 2
		AddCaptureMove(pos, MOVE(from, to, cap, bQ, 0), list);//Add capture move for all other pieces in the list 
		AddCaptureMove(pos, MOVE(from, to, cap, bR, 0), list);
		AddCaptureMove(pos, MOVE(from, to, cap, bB, 0), list);
		AddCaptureMove(pos, MOVE(from, to, cap, bN, 0), list);
	} else {
		AddCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);//else add the capture no piece move
	}
}//Black pawns' capture move

static void AddBlackPawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
	ASSERT(SqOnBoard(from));//check the validity of from square 
	ASSERT(SqOnBoard(to));//check the validity of to square 
	
	if(RanksBrd[from] == RANK_2) {//If the rank of the from square is 2
		AddQuietMove(pos, MOVE(from, to, EMPTY, bQ, 0), list);//Add moves i.e, non-capture moves in the list 
		AddQuietMove(pos, MOVE(from, to, EMPTY, bR, 0), list);
		AddQuietMove(pos, MOVE(from, to, EMPTY, bB, 0), list);
		AddQuietMove(pos, MOVE(from, to, EMPTY, bN, 0), list);
	} else {
		AddQuietMove(pos, MOVE(from, to, EMPTY, EMPTY, 0), list);//Add non-capture empty move in the list 
	}
}//Black pawns' move

void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list) {//Function to generate all possible moves on the board
	int pce = EMPTY;/*Declare some variables which will be used in this function */
	int side = pos->side;
	int sq = 0, t_sq = 0;
	int pceNum = 0;

	int dir = 0;
	int index = 0;
	int pceIndex = 0;

	ASSERT(CheckBoard(pos));//Assert the position on the board
	list->count = 0;//set the count of the list to 0

	if(side == WHITE) {//If the side to move is white
		for(pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {//Loop through white pawns
			sq = pos->pList[wP][pceNum];//Take the square from the plist 2-d array 
			ASSERT(SqOnBoard(sq));//assert it

			if(pos->pieces[sq + 10] == EMPTY) {//If the position on the square in the next rank is empty 
				AddWhitePawnMove(pos, sq, sq + 10, list);//Then add white pawn move to the list at that square
				if(RanksBrd[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {//If rank is the intial rank and the check the pawn
					AddQuietMove(pos, MOVE(sq, (sq + 20), EMPTY, EMPTY, MFLAGPS), list);
				}//to be moved to squares ahead is true if it's true then add quiet move in that position 
			}
//For the next 4 lines check if the either capture is possible, if possible add the capture move in the list
			if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq + 9, pos->pieces[sq + 9], list);
			}
			if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq + 11, pos->pieces[sq + 11], list);
			}

			if(pos->enPas != NO_SQ) {//If the enpassant square is not no square then add that move in the list 
				if(sq + 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
				}
				if(sq + 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
				}
			}
		}

		if(pos->castlePerm & WKCA) {//If white can castle the king side then add quiet move of castling
			if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {//If the f1 and g1 squares are empty
				if(!SqAttacked(E1, BLACK, pos) && !SqAttacked(F1, BLACK, pos)) {//If the King is not attacked on any of these 
					AddQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), list);//squares by the black then add quiet move
				}
			}
		}		

		if(pos->castlePerm & WQCA) {//If white can castle the queen side then add quiet move 
			if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {//If the d1,c1,b1squares are empty
				if(!SqAttacked(E1, BLACK, pos) && !SqAttacked(D1, BLACK, pos)) {//If the King is not attacked on any of these 
					AddQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), list);//squares by the black then add quiet move
				}//for the castling
			}
		}
	} else {//Else do the vice versa for all the black pawns with some minor changes in the code
		for(pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
			sq = pos->pList[bP][pceNum];
			ASSERT(SqOnBoard(sq));

			if(pos->pieces[sq - 10] == EMPTY) {
				AddBlackPawnMove(pos, sq, sq - 10, list);
				if(RanksBrd[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
					AddQuietMove(pos, MOVE(sq, (sq - 20), EMPTY, EMPTY, MFLAGPS), list);
				}
			}

			if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq - 9, pos->pieces[sq - 9], list);
			}
			if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq - 11, pos->pieces[sq - 11], list);
			}

			if(pos->enPas != NO_SQ) {
				if(sq - 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
				}
				if(sq - 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
				}
			}
		}	

		if(pos->castlePerm & BKCA) {
			if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
				if(!SqAttacked(E8, WHITE, pos) && !SqAttacked(F8, WHITE, pos)) {
					AddQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}

		if(pos->castlePerm & BQCA) {
			if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
				if((!SqAttacked(E8, WHITE, pos)) && (!SqAttacked(D8, WHITE, pos))) {
					AddQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}
	}

	pceIndex = LoopSlideIndex[side];//set pceindex to the sliding piece 
	pce = LoopSlidePce[pceIndex++];//take the pce from the slidepce array
	while(pce != 0) {//While the pce is not equal to 0
		ASSERT(PieceValid(pce));//check it's validity

		for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {//Loop through all the pieces
			sq = pos->pList[pce][pceNum];//take the square from the plist 2-d array
			ASSERT(SqOnBoard(sq));//Assert it on the board

			for(index = 0; index < NumDir[pce]; ++index) {//Loop through all the pieces
				dir = PceDir[pce][index];//take directed piece square no.
				t_sq = sq + dir;//add the sq + dir to get the targeted square 

				while(!SQOFFBOARD(t_sq)) {//While the targeted square is not offboard

					if(pos->pieces[t_sq] != EMPTY) {//if there is no piece at the targeted square
						if(PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {//If there is a piece of opponent of the 
							AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);//targeted square
						}//Then add the capture move on the board 
						break;//then break the loop
					}
					AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);//add empty non-capture move in the list 
					t_sq += dir;//targeted square += directed square
				}
			}
		}
		pce = LoopSlidePce[pceIndex++];//Take the piece from the Slidepce array
	}

	pceIndex = LoopNonSlideIndex[side];//Take the piece index from non-slideindex 
	pce = LoopNonSlidePce[pceIndex++];//retain the piece at that particular position 
	while(pce != 0) {//While the piece is not equal to 0
		ASSERT(PieceValid(pce));//assert the square onboard

		for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {//Loop through all the pieces 
			sq = pos->pList[pce][pceNum];//Take the directed piece value
			ASSERT(SqOnBoard(sq));//set the target square as directed + sq

			for(index = 0; index < NumDir[pce]; ++index) {//Loop through all the pieces 
				dir = PceDir[pce][index];//Take the directed piece value
				t_sq = sq + dir;//set the target square as directed + sq

				if(SQOFFBOARD(t_sq))//If the square is offboard continue for the next square in the loop
					continue;

				if(pos->pieces[t_sq] != EMPTY) {//If the target square is not empty
					if(PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {//And there is no opponent's piece on that square
						AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
					}
					continue;
				}//add empty move on that target square 
				AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);//Add quiet move
			}
		}
		pce = LoopNonSlidePce[pceIndex++];//Take the pce from nonslidepce array
	}
}

void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list) {//Function to generate all the capture moves
	int pce = EMPTY;/*Some variables to hold some values which will be required in this function*/
	int side = pos->side;//Initialize all the useful variables to 0
	int sq = 0, t_sq = 0;
	int pceNum = 0;

	int dir = 0;
	int index = 0;
	int pceIndex = 0;

	ASSERT(CheckBoard(pos));//check the position on the board validity
	list->count = 0;

	if(side == WHITE) {//If the side to play is white
		for(pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {//Loop through all pawns
			sq = pos->pList[wP][pceNum];//Take the square from the plist array for that particular pawn
			ASSERT(SqOnBoard(sq));//assert it

			if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK) {//Check onboard condition of the square and on 
				AddWhitePawnCapMove(pos, sq, sq + 9, pos->pieces[sq + 9], list);//either sides of that pawn check the 
			}//availability of the black's piece if yes then add capture move on that particular square 
			if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq + 11, pos->pieces[sq + 11], list);
			}

			if(pos->enPas != NO_SQ) {//Check availability of the enpassant square 
				if(sq + 9 == pos->enPas) {//check the avaliablility on the enpassant square on the either side of the pawn
					AddEnPassantMove(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);//if yes then add the enpassant
				}//move
				if(sq + 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
				}
			}
		}

	} else {//And vice-versa for the black
		for(pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
			sq = pos->pList[bP][pceNum];
			ASSERT(SqOnBoard(sq));

			if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq - 9, pos->pieces[sq - 9], list);
			}
			if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq - 11, pos->pieces[sq - 11], list);
			}

			if(pos->enPas != NO_SQ) {
				if(sq - 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
				}
				if(sq - 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
				}
			}
		}	

	}

	pceIndex = LoopSlideIndex[side];
	pce = LoopSlidePce[pceIndex++];
	while(pce != 0) {
		ASSERT(PieceValid(pce));

		for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
			sq = pos->pList[pce][pceNum];
			ASSERT(SqOnBoard(sq));

			for(index = 0; index < NumDir[pce]; ++index) {
				dir = PceDir[pce][index];
				t_sq = sq + dir;

				while(!SQOFFBOARD(t_sq)) {//Check if it's not offboard

					if(pos->pieces[t_sq] != EMPTY) {//If it's not empty
						if(PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
							AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
						}
						break;
					}//If there's opponents piece on that particular target square then add a capture move
					t_sq += dir;
				}
			}
		}
		pce = LoopSlidePce[pceIndex++];
	}

	pceIndex = LoopNonSlideIndex[side];//Take the pceindex from the slide index array
	pce = LoopNonSlidePce[pceIndex++];//retain the pce on that particular position
	while(pce != 0) {//If it's not equal to 0
		ASSERT(PieceValid(pce));//Assert it's validity

		for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {//Loop through all the pieces
			sq = pos->pList[pce][pceNum];//Take the square from plist at that particular pce
			ASSERT(SqOnBoard(sq));//assert it

			for(index = 0; index < NumDir[pce]; ++index) {//Loop through all the pieces 
				dir = PceDir[pce][index];//take the directed pce index 
				t_sq = sq + dir;//Set the target square equal to direct

				if(SQOFFBOARD(t_sq))
					continue;

				if(pos->pieces[t_sq] != EMPTY) {//If it's not empty
					if(PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
						AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
					}
				}
			}
		}
		pce = LoopNonSlidePce[pceIndex++];
	}
}
