

#include <stdio.h>

#include "defs.h"

int CheckBoard(const S_BOARD *pos) {//Function to check the Chess Board.
	int t_pceNum[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };//Assign the intialization to the pceNum array.
	int t_bigPce[2] = { 0, 0 };//Assign the bigPce array initialization.
	int t_majPce[2] = { 0, 0 };//Assign the majPce array initialization.
	int t_minPce[2] = { 0, 0 }; //Assign the minPce array
	int t_material[2] = { 0, 0 };//Assign the material array.

	int sq64, t_piece, t_pce_num, sq120, colour, pcount;//Variables to hold different values wihch are required in further operations.

	U64 t_pawns[3] = { 0ULL, 0ULL, 0ULL };//Array containing U64 for white, black and both types of pawns.
					      //Both indicates all the pawns together		
	t_pawns[WHITE] = pos->pawns[WHITE];//Assignment of different values of pawns in respective arrays.
	t_pawns[BLACK] = pos->pawns[BLACK];
	t_pawns[BOTH] = pos->pawns[BOTH];

	for(t_piece = wP; t_piece >= bK; ++t_piece) {//Loops for assigns the t_piece and t_pce_num on particular square of 120 square chess
		for(t_pce_num = 0; t_pce_num < pos->pceNum[t_piece]; ++t_pce_num) {//board array.
			sq120 = pos->pList[t_piece][t_pce_num];
			ASSERT(pos->pieces[sq120] == t_piece);
		}
	}

	for(sq64 = 0; sq64 < 64; ++sq64) {
		sq120 = SQ120(sq64);//Taking the 120sq value from the 64sq board value.
		t_piece = pos->pieces[sq120];//To store the pieces in the respective 120board array locations.
		t_pceNum[t_piece]++;//Keep storing the piece at pceNum.
		colour = PieceCol[t_piece];//To store the Piece colour.
		if(PieceBig[t_piece] == TRUE) t_bigPce[colour]++;//Increment the ParticularPce array w.r.t each colour.
		if(PieceMin[t_piece] == TRUE) t_minPce[colour]++;
		if(PieceMaj[t_piece] == TRUE) t_majPce[colour]++;

		t_material[colour] += PieceVal[t_piece];
	}

	for(t_piece = wP; t_piece <= bK; ++t_piece)
		ASSERT(t_pceNum[t_piece] == pos->pceNum[t_piece]);//To put check for the piece on pceNum.

	pcount = CNT(t_pawns[WHITE]);//Count of the white pawns.
	ASSERT(pcount == pos->pceNum[wP]);//Check the wP count.
	pcount = CNT(t_pawns[BLACK]);//Count of the black pawns.
	ASSERT(pcount == pos->pceNum[bP]);//Check the bP count.
	pcount = CNT(t_pawns[BOTH]);//Count of the both pawns.

	ASSERT(pcount == pos->pceNum[wP] + pos->pceNum[bP]);//Check the count of both the pawns.

	while(t_pawns[WHITE]) {
		sq64 = POP(&t_pawns[WHITE]);//To store the popped White pawn bit on the 64 square board.
		ASSERT(pos->pieces[SQ120(sq64)] == wP);//To put the check on wP location on 64 square board.
	}

	while(t_pawns[BLACK]) {
		sq64 = POP(&t_pawns[BLACK]);//To store the popped Black pawn bit on the 64 square board.
		ASSERT(pos->pieces[SQ120(sq64)] == bP);//To put the check on bP location on 64 square board.
	}

	while(t_pawns[BOTH]) {
		sq64 = POP(&t_pawns[BOTH]);//To store the popped bit of both the pawns in common.
		ASSERT(pos->pieces[SQ120(sq64)] == wP || pos->pieces[SQ120(sq64)] == bP);
	}

	ASSERT(t_material[WHITE] == pos->material[WHITE] && t_material[BLACK] == pos->material[BLACK]);
	ASSERT(t_minPce[WHITE] == pos->minPce[WHITE] && t_minPce[BLACK] == pos->minPce[BLACK]);
	ASSERT(t_majPce[WHITE] == pos->majPce[WHITE] && t_majPce[BLACK] == pos->majPce[BLACK]);
	ASSERT(t_bigPce[WHITE] == pos->bigPce[WHITE] && t_bigPce[BLACK] == pos->bigPce[BLACK]);

	ASSERT(pos->side == WHITE || pos->side == BLACK);//To put the check on the sides of the of whom to play.
	ASSERT(GeneratePositionKey(pos) == pos->posKey);//To put the check on the generation of a position key.
	

	ASSERT(pos->enPas == NO_SQ ||//To put the checks on the en-passant moves.
		(RanksBrd[pos->enPas] == RANK_6 && pos->side == WHITE) ||
		(RanksBrd[pos->enPas] == RANK_3 && pos->side == BLACK));

	ASSERT(pos->pieces[pos->KingSq[WHITE]] == wK);//To put the check on the position of the play of White's King.
	ASSERT(pos->pieces[pos->KingSq[BLACK]] == bK);//To put the check on the position of the play of Black's King.

	return TRUE;
}

void UpdateListsMaterial(S_BOARD *pos) {//Function used to update the List containing the material on the board.
	int piece, sq, index, colour;//variables to hold certain values..

	for(index = 0; index < BRD_SQ_NUM; ++index) {//Function to store the different pieces on the board in their respective arrays.
		sq = index;
		piece = pos->pieces[index];
		if(piece != OFFBOARD && piece != EMPTY) {
			colour = PieceCol[piece];
			if(PieceBig[piece] == TRUE) pos->bigPce[colour]++;
			if(PieceMin[piece] == TRUE) pos->minPce[colour]++;
			if(PieceMaj[piece] == TRUE) pos->majPce[colour]++;

			pos->material[colour] += PieceVal[piece];

			pos->pList[piece][pos->pceNum[piece]] = sq;
			pos->pceNum[piece]++;

			if(piece == wK) pos->KingSq[WHITE] = sq;
			if(piece == bK) pos->KingSq[BLACK] = sq;

			if(piece == wP) {
				SETBIT(pos->pawns[WHITE], SQ64(sq));//To set the Setbit for White pawns.
				SETBIT(pos->pawns[BOTH], SQ64(sq));
			} else if(piece == bP) {
				SETBIT(pos->pawns[BLACK], SQ64(sq));//To set the Setbit for Black pawns.
				SETBIT(pos->pawns[BOTH], SQ64(sq));
			}
		}
	}
}

int ParseFen(char *fen, S_BOARD *pos) {//To parse the "*fen" fen pointer to the Position of the Chess Board.  
	int rank = RANK_8;//Certain variables containing certain values to be used in the writting of this function.
	int file = FILE_A;
	int piece = 0;
	int count = 0;
	int i = 0;
	int sq64 = 0;
	int sq120 = 0;

	ASSERT(fen != NULL);//Check the Fen.
	ASSERT(pos != NULL);//Check the Position of the Board.

	ResetBoard(pos);

	while((rank >= RANK_1) && *fen) {//To assign the values of the different pieces on the board according to the 
		count = 1;//present condition on the fen function .
		switch(*fen) {
			case 'p': piece = bP; break;
			case 'r': piece = bR; break;
			case 'n': piece = bN; break;
			case 'b': piece = bB; break;
			case 'k': piece = bK; break;
			case 'q': piece = bQ; break;
			case 'P': piece = wP; break;
			case 'R': piece = wR; break;
			case 'N': piece = wN; break;
			case 'B': piece = wB; break;
			case 'K': piece = wK; break;
			case 'Q': piece = wQ; break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				piece = EMPTY;
				count = *fen - '0';
				break;

			case '/':
			case ' ':
				rank--;
				file = FILE_A;
				fen++;
				continue;

			default://Default condition is used for error printing.
				printf("FEN error!\n");
				return -1;
		}

		for(i = 0; i < count; i++) {
			sq64 = rank * 8 + file;//Formula for conversion of files and rank nos. into the sq64 notation.
			sq120 = SQ120(sq64);//To obtain the 120 sq-board representation from 64 sq-board.
			if(piece != EMPTY)
				pos->pieces[sq120] = piece;
			file++;
		}
		fen++;
	}

	ASSERT(*fen == 'w' || *fen == 'b');//Check for the side which is to play.

	pos->side = (*fen == 'w') ? WHITE : BLACK;//Ternary operator to assign the side.
	fen += 2;//To upadate the fen, as fen is a string pointer, so shifts by to locations.

	for(i = 0; i < 4; i++) {
		if(*fen == ' ')//To check for the space in the fen pointer.
			break;

		switch(*fen) {//Switch case for various castle permissions parsed by the *fen.
			case 'K': pos->castlePerm |= WKCA; break;
			case 'Q': pos->castlePerm |= WQCA; break;
			case 'k': pos->castlePerm |= BKCA; break;
			case 'q': pos->castlePerm |= BQCA; break;		
		}
		fen++;
	}
	fen++;

	ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);//To check the castlePerm bit representation-form array.

	if(*fen != '-') {//For the square which is not empty square .. ,As it is denoted by the sumbol '-'
		file = fen[0] - 'a';//conversion of the file in integer.
		rank = fen[1] - '1';//conversion of the rank in integer.

		ASSERT(file >= FILE_A && file <= FILE_H);//To put check for all the Files.
		ASSERT(rank >= RANK_1 && rank <= RANK_8);//To put check for all the Ranks.

		pos->enPas = FR2SQ(file, rank);//To get the square whether it represents an en-passant square on the board.
	}

	pos->posKey = GeneratePositionKey(pos);//To generate the position key.

	UpdateListsMaterial(pos);//Function call to update the list of the material present on the chess board.
	return 0;
}

void ResetBoard(S_BOARD *pos) {//Function to reset the Chess Board.
	int index = 0;//integer variable to store index.

	for(index = 0; index < BRD_SQ_NUM; index++)//To Assign the OFFBOARD squares on in the pieces array.
		pos->pieces[index] = OFFBOARD;

	for(index = 0; index < 64; index++)//To assign all the squares as empty squares.
		pos->pieces[SQ120(index)] = EMPTY;

	for(index = 0; index < 2; index++) {//To assign all types of pieces' values to '0'.
		pos->bigPce[index] = 0;
		pos->majPce[index] = 0;
		pos->minPce[index] = 0;
		pos->material[index] = 0;
	}

	for(index = 0; index < 3; index++) {//To assign the pawns' array index as 0ULL as previously done.
		pos->pawns[index] = 0ULL;
	}

	for(index = 0; index < 13; index++)//To assign the pceNum array to 0.
		pos->pceNum[index] = 0;

	pos->KingSq[WHITE] = pos->KingSq[BLACK] = NO_SQ;//To make all the elements in the KingSq Array as NUll i.e, '0';

	pos->material[WHITE] = pos->material[BLACK] = 0;//To make all the elements in the material Array as Null i.e, '0';

	pos->side = BOTH;//To assign the side of the position to play as both i.e, to intialize to the starting condition.
	pos->enPas = NO_SQ;//To assign the enPas square to '0';
	pos->fiftyMove = 0;//To assign the fifty-moves counter to '0';

	pos->ply = 0;//To make ply and his-ply counters pointed by pos pointer as '0';
	pos->hisPly = 0;

	pos->castlePerm = 0;//To make the castlePerm as '0';

	pos->posKey = 0ULL;//To make the positionKey as 0ULL type.
}

void PrintBoard(const S_BOARD *pos) {//Function to PrintBoard.
	int sq, file, rank, piece;//Variables to hold the file, rank, piece etc...

	printf("\nGame Board:\n\n");//For the terminal run of the Chess Board.

	for(rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d  ", rank + 1);
		for(file = FILE_A; file <= FILE_H; file++) {
			sq = FR2SQ(file, rank);
			piece = pos->pieces[sq];
			printf("%3c", PceChar[piece]);
		}
		printf("\n");
	}

	printf("\n");
	for(file = FILE_A; file <= FILE_H; file++)
		printf("%3c", 'a' + file);

	printf("\n");
	printf("side:%c\n", SideChar[pos->side]);
	printf("enPas:%d\n", pos->enPas);
	printf("castle:%c%c%c%c\n",
		pos->castlePerm & WKCA ? 'K' : '-',
		pos->castlePerm & WQCA ? 'Q' : '-',
		pos->castlePerm & BKCA ? 'k' : '-',
		pos->castlePerm & BQCA ? 'q' : '-');

	printf("PosKey:%0llx\n", pos->posKey);//The hexa-decimal equivalent of the Position key i.e, to be generated ...
}

void MirrorBoard(S_BOARD *pos) {//Function to store the mirror board..

	int tempPiecesArray[64];
	int tempSide = pos->side^1;
	int SwapPiece[13] = { EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK };
	int tempCastlePerm = 0;
	int tempEnPas = NO_SQ;

	int sq;
	int tp;

	if (pos->castlePerm & WKCA) tempCastlePerm |= BKCA;
	if (pos->castlePerm & WQCA) tempCastlePerm |= BQCA;

	if (pos->castlePerm & BKCA) tempCastlePerm |= WKCA;
	if (pos->castlePerm & BQCA) tempCastlePerm |= WQCA;

	if (pos->enPas != NO_SQ)  {
		tempEnPas = SQ120(Mirror64[SQ64(pos->enPas)]);
	}

	for (sq = 0; sq < 64; sq++) {
		tempPiecesArray[sq] = pos->pieces[SQ120(Mirror64[sq])];
	}

	ResetBoard(pos);

	for (sq = 0; sq < 64; sq++) {
		tp = SwapPiece[tempPiecesArray[sq]];
		pos->pieces[SQ120(sq)] = tp;
	}

	pos->side = tempSide;
	pos->castlePerm = tempCastlePerm;
	pos->enPas = tempEnPas;

	pos->posKey = GeneratePositionKey(pos);

	UpdateListsMaterial(pos);

	ASSERT(CheckBoard(pos));
}
