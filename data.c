

#include "defs.h"

char PceChar[] = ".PNBRQKpnbrqk";//character array for different pieces on the board.
char SideChar[] = "wb-"; //character array for different White pieces, Black pieces and both the pieces on the board.
char RankChar[] = "12345678";//character array for the Rank nos. 
char FileChar[] = "abcdefgh"; //character array for the File alphabets.

int PieceBig[13] = { FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE  };//N, B, R, Q & K
int PieceMaj[13] = { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE  };// R & Q
int PieceMin[13] = { FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE };//N & B
int PieceVal[13] = { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000 };//Computer chess standard values.
int PieceCol[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };//Represents the colour

int PiecePawn[13] = { FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE,  TRUE, FALSE, FALSE, FALSE, FALSE, FALSE };//Represents Pawns
int PieceKnight[13] = { FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE };//Represents Knights
int PieceKing[13] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE  };//Represents Bishops
int PieceRookQueen[13] = { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE };//Represents Rooks
int PieceBishopQueen[13] = { FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE };//Represents Queens
int PieceSlides[13] = { FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE };//Represents King


int Mirror64[64] = {
	56,	57,	58,	59,	60,	61,	62,	63,
	48,	49,	50,	51,	52,	53,	54,	55,
	40,	41,	42,	43,	44,	45,	46,	47,
	32,	33,	34,	35,	36,	37,	38,	39,
	24,	25,	26,	27,	28,	29,	30,	31,
	16,	17,	18,	19,	20,	21,	22,	23,
	8,	9,	10,	11,	12,	13,	14,	15,
	0,	1,	2,	3,	4,	5,	6,	7
};//64 - Squared flipped Chess Board Array.
