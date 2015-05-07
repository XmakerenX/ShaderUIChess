#pragma once
#include "piece.h"
#include "Square.h"
class Square; 

class knight :
	public piece
{
public:
	knight(void);
	knight(CMyMesh * pMesh,OBJECT_PREFS* objectPrefs,int playerColor);
	~knight(void);
	bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
	int   getType();
	float getYpos();

	static float    Ypos;

private:
	static PIECES   type;
};
