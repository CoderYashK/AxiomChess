
#ifndef DEFS_H
#define DEFS_H
#include <stdio.h>
#include <stdlib.h>

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
	do {\
		if(!(n)) { \
			printf("%s ", __DATE__); \
			printf("%s: ", __TIME__); \
			printf("Assertion '%s' failed.\n", #n); \
			printf("File '%s'\n", __FILE__); \
			printf("Line %d\n", __LINE__); \
			getchar(); \
			exit(1); \
		} \
	} while(0);/*Whenever any condition in the program fails*/
#endif

typedef unsigned long long U64;//INTEGER WHICH REPRESENTS DIFFERENT SQUARES AS BITS ON THE CHESS BOARD

#define NAME "Axiom 1.0"//Name of the program.
#define BRD_SQ_NUM 120//The macro to indicate the total no. of squares on the original chess board.

#define MAXGAMEMOVES 2048//Max possible no. of moves which can be played in a game.
#define MAXPOSITIONMOVES 256//Max possible no. of different moves which can be played in a certain position.
#define MAXDEPTH 64//Depth of search of moves.
#define MAX_HASH 1024//Max size of the Hash macro

#define INF 30000//Macro for the infinity
#define ISMATE (INF - MAXDEPTH)//Is mate macro

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"//Forsyth Edward Notation for Computer Chess.

enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };//enum of different pieces on the board.
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };//enum of different files on the board.
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };//enum of different ranks on the board.

enum { WHITE, BLACK, BOTH };//enum of different colours on the board BOTH for square having no piece.
enum { UCIMODE, XBOARDMODE, CONSOLEMODE };// enum of differen GUI's

enum {
	A1 = 21, B1, C1, D1, E1, F1, G1, H1,
	A2 = 31, B2, C2, D2, E2, F2, G2, H2,
	A3 = 41, B3, C3, D3, E3, F3, G3, H3,
	A4 = 51, B4, C4, D4, E4, F4, G4, H4,
	A5 = 61, B5, C5, D5, E5, F5, G5, H5,
	A6 = 71, B6, C6, D6, E6, F6, G6, H6,
	A7 = 81, B7, C7, D7, E7, F7, G7, H7,
	A8 = 91, B8, C8, D8, E8, F8, G8, H8,
	NO_SQ, OFFBOARD
};//enum of 64 Squares "Game Board" inside the 120 Squares Board.

enum { FALSE, TRUE };//enum of TRUE and FALSE

enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };//1)WKCA = White King's Side Castling. 2)WQCA = White Queen's Side Castling.
					        //3)BKCA = Black's King's Side Castling. 4)BQCA = Black's Queen's Side Castling.
enum {  HFNONE, HFALPHA, HFBETA, HFEXACT};

typedef struct {
	int move;
	int score;
} S_MOVE;//Structure of moves and their scores.

typedef struct {
	S_MOVE moves[MAXPOSITIONMOVES];
	int count;
} S_MOVELIST;//Sturcture of list of moves.

typedef struct {
	U64 posKey;
	int move, score, depth, flags;
} S_HASHENTRY;//Structure containing positionkey, possible, good moves, depth of search and score.

typedef struct {
	S_HASHENTRY *pTable;
	int numEntries, newWrite, overWrite, hit, cut;
} S_HASHTABLE;//Structure to contain the Hashtable

typedef struct {
	int move, castlePerm, enPas, fiftyMoves;
	U64 posKey;
} S_UNDO;//Structure containing the possiblities to UNDO the moves.

typedef struct {
	int pieces[BRD_SQ_NUM];
	U64 pawns[3];

	int KingSq[2];
	int side, enPas, fiftyMove;
	int ply, hisPly, castlePerm;

	U64 posKey;

	int pceNum[13];
	int bigPce[2];
	int majPce[2];
	int minPce[2];
	int material[2];

	S_UNDO history[MAXGAMEMOVES];
	int pList[13][10];

	S_HASHTABLE HashTable[1];

	int PvArray[MAXDEPTH];
	int searchHistory[13][BRD_SQ_NUM];
	int searchKillers[2][MAXDEPTH];
} S_BOARD;//Structure to hold different types of pieces in the game and the history of the moves played.

typedef struct {
	int starttime, stoptime, depth, timeset, movestogo;
	int quit, stopped, nullCut;
	int GAME_MODE, POST_THINKING;

	long nodes;
	float fh, fhf;
} S_SEARCHINFO;//Structure to hold miscellaneous other tasks such as time, quit, mode, thinking of moves etc.

typedef struct {
	int UseBook;
} S_OPTIONS;//Structure to hold the Book options 

/* MACROS */

#define FR2SQ(f, r) ((21 + (f)) + ((r) * 10))//Macro to convert File and Rank to a particular square. 
#define SQ64(sq120) (Sq120ToSq64[(sq120)])//Macro to convert 120 square board to 64 square board.
#define SQ120(sq64) (Sq64ToSq120[(sq64)])//Macro to convert 64 square board to 120 square board.
#define POP(b) PopBit(b)//Macro to pop out a particular bit from our Standard U64. 
#define CNT(b) CountBits(b)//Macro to Count the no. of bits.
#define CLRBIT(bb, sq) ((bb) &= ClearMask[(sq)])//Macro to Clear particular bit from the U64 or the Chess Board
#define SETBIT(bb, sq) ((bb) |= SetMask[(sq)])//Macro to set a particular bit in the Chess Board. 

#define IsBQ(p) (PieceBishopQueen[(p)])//Macro to tell that the piece is either Queen or Bishop.
#define IsRQ(p) (PieceRookQueen[(p)])//Macro to tell that the piece is either Rook or Queen.
#define IsKn(p) (PieceKnight[(p)])//Macro to tell that the piece is Knight.
#define IsKi(p) (PieceKing[(p)])//Macro to tell that the piece is King.

#define MIRROR64(sq) (Mirror64[(sq)])

#define FROMSQ(m) ((m) & 0x7f)
#define TOSQ(m) (((m) >> 7) & 0x7f)
#define CAPTURED(m) (((m) >> 14) & 0xf)
#define PROMOTED(m) (((m) >> 20) & 0xf)

#define MFLAGEP   0x00040000//Macro containing the En-passant move representation of decimal in hexa-decimal system.
#define MFLAGPS   0x00080000//Macro containing the Starting-Pawn move representation of decimal in hexa-decimal system.
#define MFLAGCA   0x01000000//Macro containing the Castling move representation of decimal in hexa-decimal system.
#define MFLAGCAP  0x0007C000//Macro containing the Piece-Capture move representation of decimal in hexa-decimal system.
#define MFLAGPROM 0x00f00000//Macro containing the Piece-Promotion move representation of decimal in hexa-decimal system. 

#define NOMOVE 0//Macro containing no move played representation.

/* GLOBALS */

extern int Sq120ToSq64[BRD_SQ_NUM];//120 squares Chess board array.
extern int Sq64ToSq120[64];//64 squares Chess board array.
extern U64 SetMask[64];//To Set the bits on the board.
extern U64 ClearMask[64];//To Clear the bits on the board.
extern U64 PieceKeys[13][120];//Keys or access to different keys on the board.
extern U64 SideKey;//Different types of sides on the board representation.
extern U64 CastleKeys[16];//As the 4-different types of castle permissions are from 1 to 8 so the bits are 16 (representation).
extern char PceChar[];//Character array containing different types of pieces on the board.
extern char SideChar[];//Character array containing different sides on the board. 
extern char RankChar[];//Character array containing(representing) different Ranks on the board. 
extern char FileChar[];//Character array representing different Files on the Board.

extern int PieceBig[13];//Integer array containing different Big Pieces Such as Queens
extern int PieceMaj[13];//Integer array containing different Major pieces such as Rooks.
extern int PieceMin[13];//Integer array containing different Minor pieces sucha as Bishops and Knights.
extern int PieceVal[13];//Integer array containing different Computer-Chess values of the pieces represented using pervious arrays.
extern int PieceCol[13];//Integer array containing different Colours of the different pieces on the board.

extern int FilesBrd[BRD_SQ_NUM];//Integer array containing different Files algebraic notations on the board.
extern int RanksBrd[BRD_SQ_NUM];//Integer array containing different Ranks algebraic notations on the 64- square board.

extern int PiecePawn[13];//Max no. of Pawn pieces on the board.
extern int PieceKnight[13];//Max no. of Knight pieces on the board.
extern int PieceKing[13];//Max no. of King pieces on the board.
extern int PieceRookQueen[13];//Max no. of Rook or Queens on the board.
extern int PieceBishopQueen[13];//Max no. of Bishop or Queen pieces on the board.
extern int PieceSlides[13];//Max no. of Sliding pieces on the board i.e, Queens, Bishops and Rooks.

extern int Mirror64[64];//Array containing the squares as Mirror image on the 64 square Chess board.

extern U64 FileBBMask[8];//Array containing bits of the various Files in the Game.
extern U64 RankBBMask[8];//Array containing bits of various Ranks in the Game.

extern U64 BlackPassedMask[64];//Array of masks for detecting different black passed pawns on the different squares of a chess board
extern U64 WhitePassedMask[64];//Array of masks for detecting different white passed pawns on the different squares of a chess board
extern U64 IsolatedMask[64];//Array of masks for detecting different isolated pawns on the different squares of a chess board 

S_OPTIONS EngineOptions[1];//Option variable

/* FUNCTIONS */

/* init.c */
extern void AllInit(void);//To intialize the Chess Board to the starting Position.

/* bitboard.c */
extern int PopBit(U64 *bb);//To pop a particular bit from the board.
extern int CountBits(U64 bb);//To Count no. of bits from the chess board.
extern void PrintBitBoard(U64 bb);//To Print the chess board on the console.

/* hashkeys.c */
extern U64 GeneratePositionKey(const S_BOARD *pos);//Position Key. Forsyth Edward Notation Key, for a particular position.

/* board.c */
extern void ResetBoard(S_BOARD *pos);//To Reset the board.
extern int ParseFen(char *fen, S_BOARD *pos);//To parse the FEN key the board.
extern void PrintBoard(const S_BOARD *pos);//To print the Board.
extern void UpdateListsMaterial(S_BOARD *pos);//To update the list containing differernt material.
extern int CheckBoard(const S_BOARD *pos);//To check the board.
extern void MirrorBoard(S_BOARD *pos);//To flip the board.

/* attack.c */
extern int SqAttacked(const int sq, const int side, const S_BOARD *pos);//To check whether a square is attacked.

/* io.c */
extern char *PrMove(const int move);//To print the move played on the board.
extern char *PrSq(const int sq);//To print the sequence of the moves played. 
extern int ParseMove(char *ptrChar, S_BOARD *pos);//To parse the moves to the board.
extern void PrintMoveList(const S_MOVELIST *list);//To print the list of the moves.

/* validate.c */
extern int SqOnBoard(const int sq);//To check whether a square on the board is valid to be played on.
extern int SideValid(const int side);//To which side is to play the move.
extern int FileRankValid(const int fr);//To check whether a particular file and rank is valid to be played on.
extern int PieceValidEmpty(const int pce);//To check whether the valid piece to be played is empty.
extern int PieceValid(const int pce);//To check whether the piece to be played is valid or not. 
extern void MirrorEvalTest(S_BOARD *pos);//To check whether the flipped board is correct or not.


/* movegen.c */
extern void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list);//Genereates all possible moves in the current position.
extern int MoveExists(S_BOARD *pos, const int move);//Shows whether the particular move exists or not.
extern void InitMvvLva(void);//Maximum value victim least value attacker function()
extern void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list);//Generates all the capture moves.

/* makemove.c */
extern int MakeMove(S_BOARD *pos, int move);//To make a move.
extern void TakeMove(S_BOARD *pos);//To accept a move.
extern void MakeNullMove(S_BOARD *pos);//To make a null move.
extern void TakeNullMove(S_BOARD *pos);//To accept null move.

/* perft.c */
extern void PerftTest(int depth, S_BOARD *pos);//To make the perfect or the best possible move.
//We'll have to make use of tree here and to obtain the best move from the leaf nodes.
/* search.c */
extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);//To search or a particular position on the board.

/* misc.c */
extern int GetTimeMs(void);//To get the time in milliseconds
extern void ReadInput(S_SEARCHINFO *info);//To read the input or the information of moves fed to the game.

/* pventry.c */
extern void InitHashTable(S_HASHTABLE *table, const int MB);//Take the hash-table pointer and allocates a mb size of memory for it
extern void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth);//Stores a hash-entry
extern int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth);//Removes the hash-entry
extern int ProbePvMove(const S_BOARD *pos);//Retains the principle variation of moves' line
extern int GetPvLine(const int depth, S_BOARD *pos);//Get's the principle variation line
extern void ClearHashTable(S_HASHTABLE *table);//Clear the hash-table

/* evaluate.c */
extern int EvalPosition(const S_BOARD *pos);//Evaluate the position.

/* uci.c */
extern void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info);//Operates on the uci loop

/* xboard.c */
extern void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info);//Operates on the Xboard loop
extern void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info);//Operates on the Console loop

/* polybook.c */
extern int GetBookMove(S_BOARD *board);//Get's the move from the book-move
extern void InitPolyBook(void);//Initialize the book
extern void CleanPolyBook(void);//Clean the book

#endif
