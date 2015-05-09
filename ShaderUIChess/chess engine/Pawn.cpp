#include "Pawn.h"

PIECES Pawn::type = PAWN;
float  Pawn::Ypos = 3.0f;

Pawn::Pawn(void)
{
}

Pawn::Pawn(CMyMesh * pMesh,OBJECT_PREFS* objectPrefs,int playerColor)
:piece(pMesh, objectPrefs, playerColor)
{
	//type=PAWN;
	dx[0]=-1;
	dx[1]=2;
	dy[0]=-2;
	dy[1]=3;
}

Pawn::~Pawn(void)
{
}

bool Pawn::validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX])
{
	piece * targetPiece = SBoard[targetSquare.col][targetSquare.row];
	int direction = this->getColor();// 1(white) moving up -1(black) moving down

	if ( dy == direction && dx == 0 && targetPiece == NULL) //check if movement direction is legal and that the targetPiece is Null
			return true;

	if (abs(dx) == 1 && dy == direction && targetPiece !=NULL )//check if the eating movement(diagonally) is legal and that the direction is legal 
		if(targetPiece->getColor() != this->getColor())
			return true;

	if (dy == 2*direction && dx == 0 && m_onStartPoint && targetPiece == NULL) //check if the movement of 2 squares is legal for this pawn
	{
		if (m_onStartPoint)
		{	
			int col= targetSquare.col + direction;
			int row =targetSquare.row;
			piece * middlePiece=SBoard[col][row]; //check if there is a piece in the middle of the pawn way
			if (middlePiece == NULL)
				return true;
		}
	}

 	return false;// move was not a valid one
}

int Pawn::getType()
{
	return Pawn::type;
}

float Pawn::getYpos()
{
	return Pawn::Ypos;
}