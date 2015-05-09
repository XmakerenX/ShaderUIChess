#include "bishop.h"

//initialization of static vars
PIECES bishop::type = BISHOP;
float  bishop::Ypos = 4.5f;

bishop::bishop(void)
{
}

bishop::~bishop(void)
{
}

bishop::bishop(CMyMesh * pMesh,OBJECT_PREFS* objectPrefs,int playerColor )
:piece(pMesh, objectPrefs , playerColor)
{
	//type=BISHOP;
	dx[0] = -1;
	dx[1] =  2;
	dy[0] = dx[0];
	dy[1] = dx[1];
}
bool bishop::validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX])
{
	if (dx == 0 || dy == 0)
		return false;

	if (abs (dx / dy) == 1 && dx % dy == 0)//check that dx and dy are 1,1 or 2,2 and etc
	{
		DIR_VEC dirVec;
		piece * targetPiece = SBoard[targetSquare.col][targetSquare.row];

		dirVec.x = dx / abs(dx);
		dirVec.y = dy / abs(dy);

		if (targetPiece != NULL) //check if there is a piece on the target square
		{
			if (targetPiece->getColor() != this->getColor()) //check if it is belong to the other player so we can eat it 
			{
				return isPieceInWay(startSqaure,targetSquare,dirVec,SBoard); //check that there was no piece in the middle of the way
			}
		}
		else //else square is empty just need to check that there was no piece in the middle of the way
		{
			return isPieceInWay(startSqaure,targetSquare,dirVec,SBoard);//check that there was no piece in the middle of the way
		}
	}
 	return false;
}

int bishop::getType()
{
	return bishop::type;
}

float bishop::getYpos()
{
	return bishop::Ypos;
}