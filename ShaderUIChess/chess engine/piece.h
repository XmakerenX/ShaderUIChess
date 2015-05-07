//#pragma once
#ifndef _PIECE_H_
#define _PIECE_H_

#include "../rendering/CMyObject.h"
#include "chess.h"

class Square;
class piece :public CMyObject // the base class to all the chess pieces
{
public:

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	piece(void);
	piece( CMyMesh * pMesh,OBJECT_PREFS* objectPrefs,int playerColor );
	virtual ~piece(void);

	//-------------------------------------------------------------------------
	// Pure Virtual functions of this class
	//-------------------------------------------------------------------------
	virtual bool  validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX])=0; 
	virtual int   getType() = 0; //returns the piece type PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING
	virtual float getYpos() = 0; //returns the piece Y pos in relation to the board

	//-------------------------------------------------------------------------
	// misc Functions
	//-------------------------------------------------------------------------
	bool isPieceInWay(BOARD_POINT startSquare, BOARD_POINT targetSquare, DIR_VEC dirVec, piece * sBoard[boardY][boardX]);
	bool isMoved();

	//-------------------------------------------------------------------------
	// Set methods for this class.
	//-------------------------------------------------------------------------
	void setMoved(bool newStatus);
	//void setXY(int x,int y);

	//-------------------------------------------------------------------------
	// Get methods for this class
	//-------------------------------------------------------------------------
	int getColor();
	void getDx(int inputArray[]);
	void getDy(int inputArray[]);

protected:
	int color;
	bool m_onStartPoint;
	int dx[2];
	int dy[2];
};
#endif // _PIECE_H_
