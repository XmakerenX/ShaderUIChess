// #include "Square.h"
// 
// /////////////////////////////////////////
// ////           CONSTRUCTOR           ////
// /////////////////////////////////////////
// Square::Square(HWND hwnd,HINSTANCE hInstance,int x,int y,int resID,COLORREF crTransparent,bool isPawn,std::vector<piece*> &pawnVector,int pID)
// {
// // 	this->pawn=NULL;
// // 	if (isPawn)
// // 	{
// // 		if ((y/SQheight)<3)
// // 		{
// // 			switch(pID)
// // 			{
// // 			case PAWN:
// // 				this->pawn=new Pawn(hwnd,hInstance,x,y,IDB_PAWN_BLACK,RGB(0,255,0),-1);
// // 			break;
// // 			case KNIGHT:
// // 				this->pawn=new knight(hwnd,hInstance,x,y,IDB_KNIGHT_BLACK,RGB(0,255,0),-1);
// // 			break;
// // 			case BISHOP:
// // 				this->pawn=new bishop(hwnd,hInstance,x,y,IDB_BISHOP_BLACK,RGB(0,255,0),-1);
// // 			break;
// // 			case ROOK:
// // 				this->pawn=new rook(hwnd,hInstance,x,y,IDB_ROOK_BLACK,RGB(0,255,0),-1);
// // 			break;
// // 			case KING:
// // 				this->pawn=new king(hwnd,hInstance,x,y,IDB_KING_BLACK,RGB(0,255,0),-1);
// // 			break;
// // 			case QUEEN:
// // 				this->pawn=new queen(hwnd,hInstance,x,y,IDB_QUEEN_BLACK,RGB(0,255,0),-1);
// // 			break;
// // 			}
// // 		}
// // 			//this->pawn=new Pawn(hwnd,hInstance,x,y,IDB_PAWN_BLACK,RGB(0,255,0),-1);
// // 		if ((y/SQheight)>4)
// // 		{
// // 			switch(pID)
// // 			{
// // 			case PAWN:
// // 				this->pawn=new Pawn(hwnd,hInstance,x,y,IDB_PAWN_WHITE,RGB(0,255,0),1);
// // 			break;
// // 			case KNIGHT:
// // 				this->pawn=new knight(hwnd,hInstance,x,y,IDB_KNIGHT_WHITE,RGB(0,255,0),1);
// // 			break;
// // 			case BISHOP:
// // 				this->pawn=new bishop(hwnd,hInstance,x,y,IDB_BISHOP_WHITE,RGB(0,255,0),1);
// // 			break;
// // 			case ROOK:
// // 				this->pawn=new rook(hwnd,hInstance,x,y,IDB_ROOK_WHITE,RGB(0,255,0),1);
// // 			break;
// // 			case KING:
// // 				this->pawn=new king(hwnd,hInstance,x,y,IDB_KING_WHITE,RGB(0,255,0),1);
// // 			break;
// // 			case QUEEN:
// // 				this->pawn=new queen(hwnd,hInstance,x,y,IDB_QUEEN_WHITE,RGB(0,255,0),1);
// // 			break;
// // 			}
// // 		}
// // 			//this->pawn=new Pawn(hwnd,hInstance,x,y,IDB_PAWN_WHITE,RGB(0,255,0),1);
// // 		pawnVector.push_back(pawn);
// // 		//this->pawn=NULL;
// // 	}
// // 	else
// // 		this->pawn=NULL;
// 	//this->pawn=NULL;
// }
// 
// Square::Square()
// {
// 
// }
// 
// /////////////////////////////////////////
// ////           DESTRUCTOR            ////
// /////////////////////////////////////////
// Square::~Square()
// {}
// 
// /////////////////////////////////////////
// ////           DrawBitMap            ////
// /////////////////////////////////////////
// void Square::DrawBitMap(HDC& hdc,RECT* prc,HDC& hdcBuffer)
// {
// //	bitmap::DrawBitMap(hdc,prc,hdcBuffer);
// // 	if (pawn!=NULL)
// // 		pawn->DrawBitMap(hdc,prc,hdcBuffer);
// }
// 
// /////////////////////////////////////////
// ////           DrawPawn              ////
// /////////////////////////////////////////
// void Square::DrawPawn(HDC& hdc,RECT* prc,HDC& hdcBuffer)
// {
// //	if (pawn!=NULL)
// //		pawn->DrawBitMap(hdc,prc,hdcBuffer);
// }
// 
// piece* Square::getPawn()
// {
// 		return this->pawn;
// }
// 
// void Square::setPawn(piece* newPawn)
// {
// 	this->pawn=newPawn;
// }
// 
// void Square::killPawn(std::vector<piece*> &pawnVector)
// {
//  	int i=0;
//  	while (i<pawnVector.size())
//  	{
//  		if (pawnVector[i]==pawn)
//  		{
//  			pawnVector.erase(pawnVector.begin()+i);
//  			i=pawnVector.size();
//  		}
//  		i++;
//  	}
// 	delete this->pawn;
// 	this->pawn=NULL;
// }
// 
// void Square::promotePawn(int pID,int color,std::vector<piece*> &pawnVector)
// {
// // 	killPawn(pawnVector);
// // 	switch(pID)
// // 	{
// // 	case KNIGHT:
// // 		{
// // 			if (color==black)
// // 				this->pawn=new knight(hwnd,m_hInstance,x,y,IDB_KNIGHT_BLACK,RGB(0,255,0),-1);
// // 			else
// // 				this->pawn=new knight(hwnd,m_hInstance,x,y,IDB_KNIGHT_WHITE,RGB(0,255,0),1);
// // 		}break;
// // 	case BISHOP:
// // 		{
// // 			if (color==black)
// // 				this->pawn=new bishop(hwnd,m_hInstance,x,y,IDB_BISHOP_BLACK,RGB(0,255,0),-1);
// // 			else
// // 				this->pawn=new bishop(hwnd,m_hInstance,x,y,IDB_BISHOP_WHITE,RGB(0,255,0),1);
// // 		}break;
// // 	case QUEEN:
// // 		{
// // 			if (color==black)
// // 				this->pawn=new queen(hwnd,m_hInstance,x,y,IDB_QUEEN_BLACK,RGB(0,255,0),-1);
// // 			else
// // 				this->pawn=new queen(hwnd,m_hInstance,x,y,IDB_QUEEN_WHITE,RGB(0,255,0),1);
// // 		}break;
// // 	case ROOK:
// // 		{
// // 			if (color==black)
// // 				this->pawn=new rook(hwnd,m_hInstance,x,y,IDB_ROOK_BLACK,RGB(0,255,0),-1);
// // 			else
// // 				this->pawn=new rook(hwnd,m_hInstance,x,y,IDB_ROOK_WHITE,RGB(0,255,0),1);
// // 		}break;
// // 	}
// // 	pawnVector.push_back(pawn);
// }