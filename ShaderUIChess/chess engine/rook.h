#pragma once
#include "piece.h"
#include "Square.h"
class Square; 

class rook :
	public piece
{
public:
	rook(void);
	rook(CMyMesh * pMesh,OBJECT_PREFS* objectPrefs,int playerColor);
	~rook(void);
	virtual bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
	int   getType();
	float getYpos();

	static float    Ypos;

private:
	static PIECES type;
};
