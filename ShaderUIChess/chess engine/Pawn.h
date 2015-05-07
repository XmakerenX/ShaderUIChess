#pragma once
#include "piece.h"
#include "Square.h"
class Square; 

class Pawn :
	public piece
{
public:
	Pawn(void);
	Pawn(CMyMesh * pMesh,OBJECT_PREFS* objectPrefs,int playerColor);
	~Pawn(void);
	bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
	int   getType();
	float getYpos();

	static float    Ypos;

private:
	static PIECES type;
};
