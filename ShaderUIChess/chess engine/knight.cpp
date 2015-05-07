#include "knight.h"

PIECES knight::type = KNIGHT;
float  knight::Ypos = 4.2f;

knight::knight(void)
{
}

knight::knight(CMyMesh * pMesh,OBJECT_PREFS* objectPrefs,int playerColor)
:piece(pMesh, objectPrefs, playerColor)
{
	type=KNIGHT;
	dx[0]=-2;
	dx[1]=3;
	dy[0]=-2;
	dy[1]=3;
}

knight::~knight(void)
{
}
bool knight::validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX])
{
	piece * targetPiece = SBoard[targetSquare.col][targetSquare.row];

	if (dx==0 || dy==0)
		return false;

	if ((abs(dx)+abs(dy)==3))
	{
		if (targetPiece != NULL)
		{
			if(targetPiece->getColor() != this->getColor())
				return true;
			else
				return false;
		}
		else
			return true;
		
	}
	return false;
}

int knight::getType()
{
	return knight::type;
}

float knight::getYpos()
{
	return knight::Ypos;
}