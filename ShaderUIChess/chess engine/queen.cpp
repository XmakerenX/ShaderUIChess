#include "queen.h"

PIECES queen::type = QUEEN;
float  queen::Ypos = 5.5f;

queen::queen(void)
{

}

queen::queen(CMyMesh * pMesh,OBJECT_PREFS* objectPrefs,int playerColor)
:piece(pMesh, objectPrefs, playerColor)
{
	//type=QUEEN;
	dx[0]=-1;
	dx[1]=2;
	dy[0]=dx[0];
	dy[1]=dx[1];
}

queen::~queen(void)
{
}

bool queen::validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX])
{
	if (dx == 0 && dy == 0)
		return false;

	if ( (dx * dy == 0) || ( abs(dx / dy) == 1 && dx % dy == 0 ) )
	{
		DIR_VEC dirVec;
		piece * targetPiece = SBoard[targetSquare.col][targetSquare.row];

		if (dx != 0)
			dirVec.x = dx / abs(dx);
		else
			dirVec.x = 0;

		if (dy != 0)
			dirVec.y = dy / abs(dy);
		else
			dirVec.y = 0;

		if (targetPiece != NULL)//check if there is a piece on the target square
		{
			if (targetPiece->getColor() != this->getColor()) //check if it is belong to the other player so we can eat it 
			{
				return isPieceInWay(startSqaure,targetSquare,dirVec,SBoard);//check that there was no piece in the middle of the way
			}
		}
		else//else square is empty just need to check that there was no piece in the middle of the way
		{
			return isPieceInWay(startSqaure,targetSquare,dirVec,SBoard);//check that there was no piece in the middle of the way
		}
	}
	return false;
}

int queen::getType()
{
	return queen::type;
}

float queen::getYpos()
{
	return queen::Ypos;
}