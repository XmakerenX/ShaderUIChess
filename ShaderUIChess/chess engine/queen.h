#pragma once
#include "bishop.h"
#include "rook.h"

class queen :
	public piece
{
public:
	queen(void);
	queen(CMyMesh * pMesh,OBJECT_PREFS* objectPrefs,int playerColor);
	bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
	~queen(void);
	int getType();
	float getYpos();

	static float    Ypos;

private:
	static PIECES type;
};
