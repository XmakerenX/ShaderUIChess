// #ifndef _SQYARE_H_
// #define _SQYARE_H_
// 
// #include <vector>
// 
// #include "piece.h"
// #include "../resource.h"
// #include "Pawn.h"
// #include "bishop.h"
// #include "king.h"
// #include "knight.h"
// #include "rook.h"
// #include "queen.h"
// 
// class piece;
// class Pawn;
// class bishop;
// class king;
// class knight;
// class rook;
// 
// const int SQheight=64;
// const int SQlength=64;
// 
// class Square
// {
// public :
// 	Square(HWND hwnd,HINSTANCE hInstance,int x,int y,int resID,COLORREF crTransparent,bool isPawn,std::vector<piece*> &pawnVector,int pID);
// 	Square();
// 	~Square();
// 	void DrawBitMap(HDC& hdc,RECT* prc,HDC& hdcBuffer);
// 	void DrawPawn(HDC& hdc,RECT* prc,HDC& hdcBuffer);
// 	piece* getPawn();
// 	void setPawn(piece * newPawn);
// 	void killPawn(std::vector<piece*> &pawnVector);
// 	void promotePawn(int pID,int color,std::vector<piece*> &pawnVector);
// private:
// 	piece * pawn;
// };
// 
// #endif // __SQYARE_H__H_