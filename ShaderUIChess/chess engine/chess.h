#ifndef _CHESS_H_ 
#define _CHESS_H_

const int boardX = 8;
const int boardY = 8;

enum PIECES{PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING};

typedef struct _BOARD_POINT  
{
	UINT row;	//j
	UINT col;	//i
}BOARD_POINT;

typedef struct _DIR_VEC
{
	_DIR_VEC::_DIR_VEC()
	{
		;
	}

	_DIR_VEC::_DIR_VEC(int newX, int newY)
	{
		x = newX;
		y = newY;
	}

	int x;
	int y;
}DIR_VEC;

#endif // _PIECE_H_