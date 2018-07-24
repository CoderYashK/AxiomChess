

#include <stdio.h>
#include "defs.h"

const int PawnIsolated = -10;/*Scores assigned to different positions of pieces on the board*/
const int PawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 };
const int RookOpenFile = 10;
const int RookSemiOpenFile = 5;
const int QueenOpenFile = 5;
const int QueenSemiOpenFile = 3;
const int BishopPair = 30;


const int PawnTable[64] = {
	0,	0,	0,	0,	0,	0,	0,	0,
	10,	10,	0,	-10,-10,0,	10,	10,
	5,	0,	0,	5,	5,	0,	0,	5,
	0,	0,	10,	20,	20,	10,	0,	0,
	5,	5,	5,	10,	10,	5,	5,	5,
	10,	10,	10,	20,	20,	10,	10,	10,
	20,	20,	20,	30,	30,	20,	20,	20,
	0,	0,	0,	0,	0,	0,	0,	0
};

const int KnightTableO[64] = {
	0,	-10,0,	0,	0,	0,	-10,0,
	0,	0,	0,	5,	5,	0,	0,	0,
	0,	0,	10,	10,	10,	10,	0,	0,
	0,	0,	10,	20,	20,	10,	5,	0,
	5,	10,	15,	20,	20,	15,	10,	5,
	5,	10,	10,	20,	20,	10,	10,	5,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0
};

const int KnightTableE[64] = {
	0,	-10,0,	0,	0,	0,	-10,0,
	0,	0,	0,	5,	5,	0,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	10,	20,	20,	10,	5,	0,
	5,	10,	15,	20,	20,	15,	10,	5,
	5,	10,	15,	20,	20,	15,	10,	5,
	0,	0,	5,	20,	20,	5,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0
};

const int BishopTableO[64] = {
	0,	0,	-10,0,	0,	-10,0,	0,
	0,	0,	0,	10,	10,	0,	0,	0,
	0,	0,	10,	15,	15,	10,	0,	0,
	0,	10,	15,	20,	20,	15,	10,	0,
	0,	10,	15,	20,	20,	15,	10,	0,
	0,	0,	10,	15,	15,	10,	0,	0,
	0,	0,	0,	10,	10,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0
};

const int BishopTableE[64] = {
	0,	0,	-10,0,	0,	-10,0,	0,
	0,	0,	0,	10,	10,	0,	0,	0,
	0,	0,	20,	15,	15,	10,	0,	0,
	0,	10,	15,	20,	20,	15,	10,	0,
	0,	10,	15,	20,	20,	15,	10,	0,
	0,	0,	10,	15,	15,	20,	0,	0,
	0,	0,	0,	10,	10,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0
};

const int RookTableO[64] = {
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	25,	25,	25,	25,	25,	25,	25,	25,
	0,	0,	5,	10,	10,	5,	0,	0
};

const int RookTableE[64] = {
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	25,	25,	25,	25,	25,	25,	25,	25,
	0,	20,	5,	20,	20,	5,	0,	0
};

const int KingE[64] = {
	-50,-10,0,	0,	0,	0,	-10,-50,
	-10,0,	10,	10,	10,	10,	0,	-10,
	0,	10,	15,	15,	15,	15,	10,	0,
	0,	10,	15,	20,	20,	15,	10,	0,
	0,	10,	15,	20,	20,	15,	10,	0,
	0,	10,	15,	15,	15,	15,	10,	0,
	-10,0,	10,	10,	10,	10,	0,	-10,
	-50,-10,0,	0,	0,	0,	-10,-50
};

const int KingO[64] = {
	0,5,5,-10,-10,0,10,5,
	-30,-30,-30,-30,-30,-30,-30,-30,
	-50,-50,-50,-50,-50,-50,-50,-50,
	-70,-70,-70,-70,-70,-70,-70,-70,
	-70,-70,-70,-70,-70,-70,-70,-70,
	-70,-70,-70,-70,-70,-70,-70,-70,
	-70,-70,-70,-70,-70,-70,-70,-70,
	-70,-70,-70,-70,-70,-70,-70,-70	
};

int MaterialDraw(const S_BOARD *pos) {/*Draw material condition on the board*/
	if(!pos->pceNum[wR] && !pos->pceNum[bR] && !pos->pceNum[wQ] && !pos->pceNum[bQ]) {
		if(!pos->pceNum[bB] && !pos->pceNum[wB]) {
			if(pos->pceNum[wN] < 3 && pos->pceNum[bN] < 3)
				return TRUE;
		} else if (!pos->pceNum[wN] && !pos->pceNum[bN]) {
			if (abs(pos->pceNum[wB] - pos->pceNum[bB]) < 2)
				return TRUE;
		} else if ((pos->pceNum[wN] < 3 && !pos->pceNum[wB]) || (pos->pceNum[wB] == 1 && !pos->pceNum[wN])) {
			if ((pos->pceNum[bN] < 3 && !pos->pceNum[bB]) || (pos->pceNum[bB] == 1 && !pos->pceNum[bN]))
				return TRUE;
		}
	} else if (!pos->pceNum[wQ] && !pos->pceNum[bQ]) {
		if (pos->pceNum[wR] == 1 && pos->pceNum[bR] == 1) {
			if ((pos->pceNum[wN] + pos->pceNum[wB]) < 2 && (pos->pceNum[bN] + pos->pceNum[bB]) < 2)
				return TRUE;
		} else if (pos->pceNum[wR] == 1 && !pos->pceNum[bR]) {
			if ((pos->pceNum[wN] + pos->pceNum[wB] == 0) && (((pos->pceNum[bN] + pos->pceNum[bB]) == 1) || ((pos->pceNum[bN] + pos->pceNum[bB]) == 2)))
				return TRUE;
		} else if (pos->pceNum[bR] == 1 && !pos->pceNum[wR]) {
			if ((pos->pceNum[bN] + pos->pceNum[bB] == 0) && (((pos->pceNum[wN] + pos->pceNum[wB]) == 1) || ((pos->pceNum[wN] + pos->pceNum[wB]) == 2)))
				return TRUE;
		}
	}
	return FALSE;
}

#define ENDGAME_MAT (PieceVal[wR] + 2 * PieceVal[wN] + 2 * PieceVal[wP])/*End-Game material Macro*/

int EvalPosition(const S_BOARD *pos) {/*Function to evaluate the position*/
	int pce;
	int pceNum;
	int sq;
	int score = pos->material[WHITE] - pos->material[BLACK];

	if(!pos->pceNum[wP] && !pos->pceNum[bP] && MaterialDraw(pos) == TRUE)
		return 0;

	pce = wP;
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += PawnTable[SQ64(sq)];

		if((IsolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0)
			score += PawnIsolated;

		if((WhitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0)
			score += PawnPassed[RanksBrd[sq]];
	}

	pce = bP;
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= PawnTable[MIRROR64(SQ64(sq))];

		if((IsolatedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0)
			score -= PawnIsolated;

		if((BlackPassedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0)
			score -= PawnPassed[7 - RanksBrd[sq]];
	}

	pce = wN;
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += KnightTableO[SQ64(sq)];
		if(pos->material[WHITE] <= ENDGAME_MAT) {
			score += KnightTableE[SQ64(sq)];			
		}
	}

	pce = bN;
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= KnightTableO[MIRROR64(SQ64(sq))];
		if(pos->material[WHITE] <= ENDGAME_MAT) {
			score -= KnightTableE[SQ64(sq)];			
		}
	}

	pce = wB;
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += BishopTableO[SQ64(sq)];
		if(pos->material[WHITE] <= ENDGAME_MAT) {
			score += BishopTableE[SQ64(sq)];			
		}
	}

	pce = bB;
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= BishopTableO[MIRROR64(SQ64(sq))];
		if(pos->material[BLACK] <= ENDGAME_MAT) {
			score -= BishopTableE[SQ64(sq)];			
		}
	}

	pce = wR;
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += RookTableO[SQ64(sq)];

		if(pos->material[WHITE] <= ENDGAME_MAT) {
			score += RookTableE[SQ64(sq)];			
		}
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score += RookOpenFile;
		} else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
			score += RookSemiOpenFile;
		}
	}

	pce = bR;
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= RookTableO[MIRROR64(SQ64(sq))];

		if(pos->material[BLACK] <= ENDGAME_MAT) {
			score -= RookTableE[SQ64(sq)];			
		}
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score -= RookOpenFile;
		} else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
			score -= RookSemiOpenFile;
		}
		
	}

	pce = wQ;
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));

		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score += QueenOpenFile;
		} else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
			score += QueenSemiOpenFile;
		}
	}

	pce = bQ;
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));

		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score -= QueenOpenFile;
		} else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
			score -= QueenSemiOpenFile;
		}
	}

	pce = wK;
	sq = pos->pList[pce][0];
	if(pos->material[BLACK] <= ENDGAME_MAT) {
		score += KingE[SQ64(sq)];
	} else {
		score += KingO[SQ64(sq)];
	}

	pce = bK;
	sq = pos->pList[pce][0];
	if(pos->material[WHITE] <= ENDGAME_MAT) {
		score -= KingE[MIRROR64(SQ64(sq))];
	} else {
		score -= KingO[MIRROR64(SQ64(sq))];
	}

	if(pos->pceNum[wB] >= 2) score += BishopPair;
	if(pos->pceNum[bB] >= 2) score -= BishopPair;

	if(pos->side == WHITE) {/*Because white needs to have maximum and black needs to have minimum score after evaluation*/
		return score;
	} else {
		return -score;
	}
}
