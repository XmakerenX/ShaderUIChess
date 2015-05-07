#ifndef _BOARD_H_
#define _BOARD_H_

//platform includes needed std libs, win32 api, directx libs etc...
#include "../main.h"

#include <boost/signals2/signal.hpp>
#include <boost/bind/bind.hpp>

#include "../rendering/CMyVertex.h"
#include "../rendering/CMyObject.h"
#include "../rendering/CTerrain.h"

#include "chess.h"

#include "piece.h"
#include "bishop.h"
#include "king.h"
#include "knight.h"
#include "rook.h"
#include "Pawn.h"
#include "queen.h"


class Square;
class king;

// const int boardX = 8;
// const int boardY = 8;
//const char	Textures[2][MAX_PATH] = {"black.jpg","white.jpg"};//Textures that are used on the board 
const D3DXVECTOR3 vecPieceScale = D3DXVECTOR3(0.125f,0.125f,0.125f); 

enum COLORS{BLACK = -1,WHITE = 1};
enum SIDES {UPPER, BOTTOM};

class board : public CTerrain
{
public:
	typedef boost::signals2::signal<void (CMyObject*)>  singal_pieceCreated;

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	//board(void);
	//board(LPDIRECT3DDEVICE9 pDevice, CMyMesh *pBoardMesh,OBJECT_PREFS boardSetting,CMyMesh * pPieceMesh[6],CMyObject * pPieceObj[], ULONG blkAttribID);
	board(LPDIRECT3DDEVICE9 pDevice, CAssetManager& assetManager, 
		CMyMesh *pBoardMesh,TERRAIN_PREFS boardSetting,
		CMyMesh * pPieceMesh[6], ULONG blkAttribID,
		const singal_pieceCreated::slot_type& subscriber);
	~board(void);

	//HRESULT generateBoardMesh(LPDIRECT3DDEVICE9 pDevice, CMyMesh *  pMesh, D3DXVECTOR3 vecScale);
	bool		 createPiece(int i, int j, D3DXVECTOR3 vecScale, int playerColor, UINT pieceType, ULONG blkAttribID = 0);
	CMyObject*    allocPiece(UINT pieceType, OBJECT_PREFS& curObjPrefs, int playerColor, ULONG blkAttribID);

	void    connectToPieceCreated ( const singal_pieceCreated::slot_type& subscriber);

	BOARD_POINT	getPieceSquare(piece * pPiece);
	void	killPiece(piece * pPieceToKill, BOARD_POINT pieceSquare);
	void	endTurn();
	void    reverseMove();
	void    calcPieceBoardPos(BOARD_POINT boardPos,D3DXVECTOR3& rPiecePos);
	bool	resetGame();

	void	drawBoard(HDC& hdc,RECT* prc,HDC& hdcBuffer);

	//-------------------------------------------------------------------------
	// Functions that control the pawn movement logic
	//-------------------------------------------------------------------------
	void processPress(CMyObject * pickedObjected, ULONG pressedFace );
	bool PromoteUnit(PIECES type);
	//BOOL validatePress(POINT pressLoc,int currentPlayer,HWND hwnd);
	//void movePawn(POINT newLoc);
	bool validateMove(BOARD_POINT startLoc,BOARD_POINT newLoc);
	bool castling(int dx , int dy);

	//-------------------------------------------------------------------------
	// Functions that control dialog logic   --> as of now those should be made obsolete
	//-------------------------------------------------------------------------
	BOOL CALLBACK DlgBoxProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	void bitmapTheButton(int buttonResID,int bitmapResID,HWND hwnd);
	static BOOL CALLBACK stDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);


	//-------------------------------------------------------------------------
	// Functions that detect if the game has come to an end
	//-------------------------------------------------------------------------
	bool isEndGame(int curretPlayer);
	bool isDraw(int currentPlayer);


	//-------------------------------------------------------------------------
	// Functions that help detect if the king is in threat
	//-------------------------------------------------------------------------
	bool isKingInThreat(int player,bool getAllAttackers);
	bool validateKingThreat(int curretPlayer);

	//-------------------------------------------------------------------------
	// Functions that detect if pawns can move or is it stuck in his place
	//-------------------------------------------------------------------------
	bool canPawnMove(BOARD_POINT pieceSqaure,int color,int curretPlayer);

	//-------------------------------------------------------------------------
	// Get methods for this class
	//-------------------------------------------------------------------------
	//Square * getSquare (POINT &pressLoc);
	std::string getBoardStatus () const;
	bool		isBoardActive  () const;
	bool        getKingThreat  () const;
	bool        isUnitPromotion() const;

private:
	piece * SBoard[boardY][boardX];//the actual game board indicate what piece on what square

	piece * currentPawn; //the pawn that currently  being moved
	piece * prevPawn;	 //used to save the piece on the target square in order to reverse if the move is not valid

	BOARD_POINT  startSquare;//the square that was pressed
	BOARD_POINT  targetSqaure;// the square that the piece should be moved to 

// 	Square * prevSquare; 
// 	Square * highlight;
	std::vector<piece*> pawnsVec[2];
	std::vector<piece*> deadPawnsVec[2];

// 	HWND dlgBox;
// 	HWND m_hwnd;
// 	HINSTANCE m_hinstance;

	king * kings[2];
	int attackers;

	POINT attLoc;

	float m_stepX;
	float m_stepZ;

	UINT pieceCount;

	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_meshScale; //stores the scale vector that was used during the creation of the board mesh

	int currentPlayer;
	//vector<int> work;
// 	king * whiteKing;
// 	king * blackKing;
	bool m_gameActive;
	bool m_unitPromotion;
	bool m_kingInThreat;
	std::string m_curStatus;

	ULONG m_blkAttribID;
	//CMyObject ** m_pPieceObj;//stores the pointer to allocated space of the pieces 
	CMyMesh * m_pPiecesMeshs[6];

	boost::signals2::signal<void (CMyObject*)> m_pieceCreatedSig;
};
#endif // _BOARD_H_