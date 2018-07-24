

#include "defs.h"

const int KnDir[8] = { -8, -19, -21, -12, 8, 19, 21, 12 };//The squares to which a knight can possibly move from a particular position.
const int RkDir[4] = { -1, -10, 1, 10 };//The one move squares to which a rook can possible move from particular position.
const int BiDir[4] = { -9, -11, 11, 9 };//The one move squares to which a bishop can possible move from a particular position.
const int KiDir[8] = { -1, -10, 1, 10, -9, -11, 11, 9 };//The overall squares to which a king can possibly move from a particular position.

int SqAttacked(const int sq, const int side, const S_BOARD *pos) {//definition of Square attacked function()
	int pce, index, t_sq, dir;

	ASSERT(SqOnBoard(sq));//Check if the square is on the board 
	ASSERT(SideValid(side));//ASSERTs are for putting checks on various function calls.
	ASSERT(CheckBoard(pos));//Check the position on the board

	if(side == WHITE) {
		if(pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) {//For Pawn's attack.
			return TRUE;
		}
	} else {
		if(pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) {//for Black side.
			return TRUE;
		}
	}

	for(index = 0; index < 8; index++) {//For Knight's attack.
		pce = pos->pieces[sq + KnDir[index]];//Take the pce from the pieces array at the square + knight index
		if(pce != OFFBOARD && IsKn(pce) && PieceCol[pce] == side)//If the pce is not offboard
			return TRUE;
	}

	for(index = 0; index < 4; index++) {//For Rook's attack.Loop 
		dir = RkDir[index];//through all the indices take each 
		t_sq = sq + dir;//dir set target square = dir + sq 
		pce = pos->pieces[t_sq];//Take piece from the pieces
		while(pce != OFFBOARD) {//array at that target square
			if(pce != EMPTY) {//Loop and check if it's not empty
				if(IsRQ(pce) && PieceCol[pce] == side) {//If it is the Rook piece and color is of the attacking side
					return TRUE;//then return true
				}
				break;//If empty break
			}
			t_sq += dir;//update target square
			pce = pos->pieces[t_sq];//Take the pce from the pieces array at the target square 
		}
	}

	for(index = 0; index < 4; index++) {//For Bishop's attack.//Loop through all the indices 
		dir = BiDir[index];//Take each dir from the Bidir 
		t_sq = sq + dir;//t_sq is the targeted square of attack.Set the t_sq
		pce = pos->pieces[t_sq];//Take the pce at that particular t_square		
		while(pce != OFFBOARD) {//Check whether it's not offboard
			if(pce != EMPTY) {//Check whether it's not empty if empty then break
				if(IsBQ(pce) && PieceCol[pce] == side) {//If it's bishop of the attacking side
					return TRUE;
				}
				break;
			}
			t_sq += dir;//update the t_square
			pce = pos->pieces[t_sq];//take the pce from pieces array at the t_sq position
		}
	}

	for(index = 0; index < 8; index++) {//For King's attack.//Loop through all the indices 
		pce = pos->pieces[sq + KiDir[index]];//Take the pce from pieces array at sq +Kidir index 
		if(pce != OFFBOARD && IsKi(pce) && PieceCol[pce] == side) {//Check if it's not offboard etc
			return TRUE;
		}
	}

	return FALSE;//else return false
}
