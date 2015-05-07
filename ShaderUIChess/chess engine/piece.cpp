#include "piece.h"

piece::piece(void)
{
}

/////////////////////////////////////////
////           CONSTRUCTOR           ////
/////////////////////////////////////////
piece::piece(CMyMesh * pMesh,OBJECT_PREFS* objectPrefs,int playerColor)
:CMyObject(pMesh,objectPrefs)
{
	m_onStartPoint=true;
	this->color=playerColor;
}

/////////////////////////////////////////
////           DESTRUCTOR            ////
/////////////////////////////////////////
piece::~piece(void)
{

}


// void piece::setXY(int x,int y)
// {
// 	bitmap::setXY(x,y);
// 	if (pawnB!=NULL)
// 		this->pawnB->setXY(x+4,y+4);
// }

void piece::setMoved(bool newStatus)
{
	m_onStartPoint = !newStatus;
}

int piece::getColor()
{
	return color;
}

bool piece::isMoved()
{
	return !m_onStartPoint;
}

void piece::getDx(int inputArray[])
{
	inputArray[0]=dx[0];
	inputArray[1]=dx[1];
}

void piece::getDy(int inputArray[])
{
	inputArray[0]=dy[0];
	inputArray[1]=dy[1];
}


bool piece::isPieceInWay(BOARD_POINT startSquare, BOARD_POINT targetSquare, DIR_VEC dirVec, piece * sBoard[boardY][boardX])
{
	BOARD_POINT curSquare = startSquare;
	piece * curPiece;

	curSquare.col -= dirVec.y;
	curSquare.row -= dirVec.x;

	while (curSquare.col != targetSquare.col || curSquare.row != targetSquare.row)
	{
		curPiece = sBoard[curSquare.col][curSquare.row];//gets the piece on the current square
		if (curPiece != NULL)//if there is a piece return false as there should be no piece in the way to the target square
			return false;

		curSquare.col -= dirVec.y;
		curSquare.row -= dirVec.x;

	}

	return true;
}