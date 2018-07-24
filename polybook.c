

#include "defs.h"
#include "polykeys.h"
//To add random move of standard openings to the engine as used by most of the well known engines like stockfish, komodo and Houdini etc
typedef struct {
	U64 key;//64 bit key of the position
	unsigned short move;//16 bit move
	unsigned short weight;//16 bit score for the move
	unsigned int learn;//32 bit learn i.e, value retained 
} S_POLY_BOOK_ENTRY;//the structure holding these things

long NumEntries = 0;//No of entries

S_POLY_BOOK_ENTRY *entries = NULL;//pointer to the polybookentry

const int PolyKindOfPiece[13] = {//The elements to convert properly the numbers allocated to the pieces 
	-1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10
};

void InitPolyBook(void) {//To initalize the polybook ie, the opening book
	FILE *pFile = fopen("performance.bin", "rb");//File pointer to retain the fopen() for performance.bin
	long position, returnValue;

	EngineOptions->UseBook = FALSE;//intialize the usebook to false 

	if(pFile == NULL) {//If file pointer not returned 
		printf("Book File Not Read\n");//Print appropriate message
	} else {
		fseek(pFile, 0, SEEK_END);//Used fseek() to go to the end of the file byte by byte
		position = ftell(pFile);//To return the position of the file pointer

		if(position < sizeof(S_POLY_BOOK_ENTRY)) {//If the position is less than sizeof polybookentry
			printf("No Entries Found\n");//Print proper message
			return;
		}

		NumEntries = position / sizeof(S_POLY_BOOK_ENTRY);//To return the no. of structures found in that file
		printf("%ld Entries Found In File\n", NumEntries);//Total no. of entries found in the file 

		entries = (S_POLY_BOOK_ENTRY*)malloc(NumEntries * sizeof(S_POLY_BOOK_ENTRY)); /* NULL! *///Allocate memory for the entries pointer

		rewind(pFile);//rewind the file i.e, go to the starting position of the file
		returnValue = fread(entries, sizeof(S_POLY_BOOK_ENTRY), NumEntries, pFile);//To read the entries from the pFile
		printf("fread() %ld Entries Read in from File\n", returnValue);//No. of entries read

		if(NumEntries > 0) {//If there are non-zero no. of entries in the file then Book has something in it
			EngineOptions->UseBook = TRUE;
		}
	}

	fclose(pFile);//Close the file
}

void CleanPolyBook(void) {//Free the allocated memory
	free(entries);
}

int HasPawnForCapture(const S_BOARD *board) {
	int sqWithPawn = 0;
	int targetPce = (board->side == WHITE) ? wP : bP;//If side to play is white then white wP or bP
	if(board->enPas != NO_SQ) {//If it's not enpassant square
		if(board->side == WHITE) {//If it's white to play then sqwith current pawn in enPas - 10
			sqWithPawn = board->enPas - 10;//Similiarly for the black
		} else {
			sqWithPawn = board->enPas + 10;
		}

		if(board->pieces[sqWithPawn + 1] == targetPce) {//If there is the target pce on the either side of the pawn 
			return TRUE;//then it's pawn can be captured
		} else if(board->pieces[sqWithPawn - 1] == targetPce) {
			return TRUE;
		}
	}
	return FALSE;//else it can't be captured
}

U64 PolyKeyFromBoard(const S_BOARD *board) {//hgm nubati.net
	int sq = 0, rank = 0, file = 0;/*Value holder variables*/
	U64 finalKey = 0;
	int piece = EMPTY;
	int polyPiece = 0;
	int offset = 0;

	for(sq = 0; sq < BRD_SQ_NUM; ++sq) {//Loop through all the squares on the board
		piece = board->pieces[sq];//Retain the pieces on the particular squares on the board
		if(piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) {//Check if it's a valid piece 
			ASSERT(piece >= wP && piece <= bK);//Assert it
			polyPiece = PolyKindOfPiece[piece];//Take polyPiece from the polypiece array
			rank = RanksBrd[sq];//Take the rank from the board
			file = FilesBrd[sq];//Take the file from the board

			finalKey ^= Random64Poly[(64 * polyPiece) + (8 * rank) + file];//Formula to retain the final position key
		}
	}

	offset = 768;/*Using polykeys to set the proper offsets*/
	if(board->castlePerm & WKCA) finalKey ^= Random64Poly[offset + 0];
	if(board->castlePerm & WQCA) finalKey ^= Random64Poly[offset + 1];
	if(board->castlePerm & BKCA) finalKey ^= Random64Poly[offset + 2];
	if(board->castlePerm & BQCA) finalKey ^= Random64Poly[offset + 3];

	offset = 772;//offset for the random enpassant move on the board
	if(HasPawnForCapture(board) == TRUE) {
		file = FilesBrd[board->enPas];
		finalKey ^= Random64Poly[offset + file];
	}

	if(board->side == WHITE)//If the side to play is white
		finalKey ^= Random64Poly[780];//final key is exored with itself and the randomturn key

	return finalKey;//finally at the end of the function return the key
}
//Little endian and big endian codes are taken from the internet
unsigned short endian_swap_u16(unsigned short x) {
	x = (x >> 8) |
		(x << 8);
	return x;
}
//Endian swap code is also taken from the internet
unsigned int endian_swap_u32(unsigned int x) {
	x = (x >> 24) |
		((x << 8) & 0x00ff0000) |
		((x >> 8) & 0x0000ff00) |
		(x << 24);
	return x;
}

U64 endian_swap_u64(U64 x) {
	x = (x >> 56) |
		((x << 40) & 0x00ff000000000000ULL) |
		((x << 24) & 0x0000ff0000000000ULL) |
		((x << 8)  & 0x000000ff00000000ULL) |
		((x >> 8)  & 0x00000000ff000000ULL) |
		((x >> 24) & 0x0000000000ff0000ULL) |
		((x >> 40) & 0x000000000000ff00ULL) |
		(x << 56);
	return x;
}

int ConvertPolyMoveToInternalMove(unsigned short polyMove, S_BOARD *board) {
	int ff = (polyMove >> 6) & 7;
	int fr = (polyMove >> 9) & 7;
	int tf = (polyMove >> 0) & 7;
	int tr = (polyMove >> 3) & 7;
	int pp = (polyMove >> 12) & 7;
	char promChar = 'q';

	char moveString[6];
	if(pp == 0) {
		sprintf(moveString, "%c%c%c%c",
			FileChar[ff],
			RankChar[fr],
			FileChar[tf],
			RankChar[tr]);
	} else {
		switch(pp) {
			case 1: promChar = 'n';
			case 2: promChar = 'b';
			case 3: promChar = 'r';
		}

		sprintf(moveString, "%c%c%c%c%c",
			FileChar[ff],
			RankChar[fr],
			FileChar[tf],
			RankChar[tr],
			promChar);
	}

	return ParseMove(moveString, board);
}

static int PickMove(int *moves, unsigned short *weight, unsigned int cumWeight, int count) {//function to pick a move 
	int i, pick;//integer variables used

	pick = rand() % cumWeight;//returns a random value from the rand() function which is moduloed by cumWeight

	for(i = 0; i < count; i++) {//Loop through all the count
		if(pick < weight[i])//If pick is less than i then return the base address of the moves array
			return moves[i];
		pick -= weight[i];//update the pick 
		cumWeight -= weight[i];//update the cumWeight
	}
	return weight[count - 1];//Return value at the count -1 index of the weight
}

int GetBookMove(S_BOARD *board) {//To get the book move
	S_POLY_BOOK_ENTRY *entry;//pointer to the polybook entry
	unsigned short move;//short for the move
	const int MAXBOOKMOVES = 32;//max book moves are set to 32 as I don't think there can be more than that at a particular instant
	int bookMoves[32]; /* !! */
	unsigned short weight[32];
	unsigned int cumWeight = 0;
	int tempMove = NOMOVE;//tempmove is set to nomove
	int count = 0;//count is set to 0

	U64 polyKey = PolyKeyFromBoard(board);//polykey is retained from the board 

	for(entry = entries; entry < entries + NumEntries; entry++) {//Loop through all the entries 
		if(polyKey == endian_swap_u64(entry->key)) {//if poly key is the key we were looking for 
			move = endian_swap_u16(entry->move);//store the appropriate move in the move variable
			tempMove = ConvertPolyMoveToInternalMove(move, board);//take a temporary move from the convert polymove to internal move
			if(tempMove != NOMOVE) {//If the tempmove is not equal to no move
				weight[count] = entry->weight;//weight at count is set to entry's weight
				cumWeight += weight[count];//cumWeight is then updated
				bookMoves[count++] = tempMove;//tempmove is stored in the book move
				if(count > MAXBOOKMOVES)//If count exceeds the maxbookmoves then break the for loop
					break;
			}
		}
	}

	if(count != 0) {//If count not equal to 0 then return the PickMove
		return PickMove(bookMoves, weight, cumWeight, count);
	} else {
		return NOMOVE;//else return nomove
	}
}
