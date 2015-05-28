//#include "StdAfx.h"
#include "board.h"

BOOL CALLBACK board::stDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) //gives the dlgproc 
//access to class var
{
// 	ChessMain * pWnd;//HighScoreDialog
// 	pWnd = (ChessMain *)GetWindowLong(GetParent(hwnd), GWL_USERDATA);
// 	if (pWnd)	
// 	{
// 		//return pWnd->DlgBoxProc(hwnd,Message,wParam,lParam);
// 		board * b=pWnd->getBoard();
// 		b->DlgBoxProc(hwnd,Message,wParam,lParam);
// 	}
// 	else return false;
	return false;
}

void board::bitmapTheButton(int buttonResID,int bitmapResID,HWND hwnd)
{
	HWND bHandle = GetDlgItem( hwnd, buttonResID );
	HANDLE bPawn = ::LoadImage( GetModuleHandle(NULL),
		MAKEINTRESOURCE( bitmapResID ),
		IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR );
	::SendMessage( bHandle, BM_SETIMAGE, IMAGE_BITMAP,
		(LPARAM) (DWORD) bPawn );
}

/////////////////////////////////////////
////           DlgBoxProc            ////
/////////////////////////////////////////
BOOL CALLBACK board::DlgBoxProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
// 	switch(Message)
// 	{
// 	case WM_INITDIALOG:
// 		{
// 			if(currentPawn->getColor()==-1)
// 			{
//  				bitmapTheButton(IDC_bishop,IDB_BISHOP_BLACK,hwnd);
//  				bitmapTheButton(IDC_knight,IDB_KNIGHT_BLACK,hwnd);
//  				bitmapTheButton(IDC_rook,IDB_ROOK_BLACK,hwnd);
//  				bitmapTheButton(IDC_queen,IDB_QUEEN_BLACK,hwnd);
// 			}
// 			else
// 			{
// 				bitmapTheButton(IDC_bishop,IDB_BISHOP_WHITE,hwnd);
// 				bitmapTheButton(IDC_knight,IDB_KNIGHT_WHITE,hwnd);
// 				bitmapTheButton(IDC_rook,IDB_ROOK_WHITE,hwnd);
// 				bitmapTheButton(IDC_queen,IDB_QUEEN_WHITE,hwnd);
// 			}
// 			return TRUE;
// 		}break;
// 	case WM_COMMAND:
// 		{
// 
// 			int color=currentPawn->getColor();
// 			if (color==1)
// 				color=white;
// 			else
// 				color=black;
// 
// 			switch(LOWORD(wParam))
// 			{
// 			case IDC_bishop:
// 				{
// 					//int color=currentPawn->getColor();
// 					currentSquare->promotePawn(BISHOP,color,pawnsVec[color]);
// 					InvalidateRect(m_hwnd,NULL,FALSE);
// 					EndDialog(hwnd,0);
// 					return TRUE;
// 				}break;
// 			case IDC_knight:
// 				{
// 					//int color=currentPawn->getColor();
// 					currentSquare->promotePawn(KNIGHT,color,pawnsVec[color]);
// 					InvalidateRect(m_hwnd,NULL,FALSE);
// 					EndDialog(hwnd,0);
// 					return TRUE;
// 				}break;
// 			case IDC_queen:
// 				{
// 					//int color=currentPawn->getColor();
// 					currentSquare->promotePawn(QUEEN,color,pawnsVec[color]);
// 					InvalidateRect(m_hwnd,NULL,FALSE);
// 					EndDialog(hwnd,0);
// 					return TRUE;
// 				}break;
// 			case IDC_rook:
// 				{
// 					//int color=currentPawn->getColor();
// 					currentSquare->promotePawn(ROOK,color,pawnsVec[color]);
// 					InvalidateRect(m_hwnd,NULL,FALSE);
// 					EndDialog(hwnd,0);
// 					return TRUE;
// 				}break;
// 			}
// 		}break;
// 	case WM_DESTROY:
// 		{
// 		EndDialog(hwnd, 0);
// 		}break;
// 
// 	default:
// 		return false;
// 
// 	}
 	return true;
}

//-----------------------------------------------------------------------------
// Name : board (CONSTRUCTOR)
// TODO : check if this is not useless...
//-----------------------------------------------------------------------------
// board::board()
// {
// 
// }

//-----------------------------------------------------------------------------
// Name : board (CONSTRUCTOR)
//-----------------------------------------------------------------------------
board::board(LPDIRECT3DDEVICE9 pDevice, CAssetManager& assetManager, CMyMesh *pBoardMesh,TERRAIN_PREFS boardSetting, CMyMesh * pPieceMesh[6], ULONG blkAttribID, const singal_pieceCreated::slot_type& subscriber)
	:CTerrain(pDevice, assetManager, pBoardMesh, boardSetting)
{
	connectToPieceCreated(subscriber);
	//m_pieceCreatedSig(this);

	int playerColor = WHITE;

	ZeroMemory(SBoard, sizeof(piece*) * ( boardX * boardY ) );

	startSquare.col = -1;
	startSquare.row = -1;

	ZeroMemory(&targetSqaure,sizeof(targetSqaure));

	currentPawn=NULL;
	prevPawn=NULL;

	kings[BOTTOM]=NULL;
	kings[UPPER]=NULL;

	//m_pMesh = NULL;

	currentPlayer = 1;

	pieceCount = 0;

	m_pos = boardSetting.objPrefs.pos;
	//m_pPieceObj = pPieceObj;

	for (int i = 0; i < 6; i++)
		m_pPiecesMeshs[i] = pPieceMesh[i];

	//generateBoardMesh(pDevice, pBoardMesh, boardSetting.scale);
	//if (!m_pMesh) return;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < boardX; j++)
		{ 
			createPiece(i ,j ,boardSetting.objPrefs.scale, BLACK, KING + 1, blkAttribID);
			pawnsVec[UPPER].push_back(SBoard[i][j]);
			//pawnsVec[UPPER].push_back( (piece*)pPieceObj[pieceCount++] );
		}
	}

	for (int i = boardY - 2; i < boardY; i++)
	{
		for (int j = 0; j < boardX; j++)
		{
			createPiece(i ,j ,boardSetting.objPrefs.scale, WHITE, KING + 1);
			pawnsVec[BOTTOM].push_back(SBoard[i][j]);
			//pawnsVec[BOTTOM].push_back( (piece*)pPieceObj[pieceCount++] );
		}
	}

	D3DXMatrixTranslation(&m_mtxWorld,m_pos.x,m_pos.y,m_pos.z);
	m_blkAttribID = blkAttribID;
	m_gameActive = true;
	m_unitPromotion = false;
	m_kingInThreat = false;

}

//-----------------------------------------------------------------------------
// Name : board (DESTRUCTOR)
//-----------------------------------------------------------------------------
board::~board(void)
{
	for (int i = 0; i < 6; i++)
		m_pPiecesMeshs[i] = NULL;


}

//-----------------------------------------------------------------------------
// Name : drawBoard ()
//-----------------------------------------------------------------------------
void board::drawBoard(HDC& hdc,RECT* prc,HDC& hdcBuffer)
{
// 	for (int i=0;i<boardY;i++)
// 		for (int j=0;j<boardX;j++)
// 			SBoard[j][i]->DrawBitMap(hdc,prc,hdcBuffer);
// 	for (int i=0;i<boardY;i++)
// 		for (int j=0;j<boardX;j++)
// 			SBoard[j][i]->DrawBitMap(hdc,prc,hdcBuffer);
// 	int Start=1;
// 	for (int i=0;i<boardY;i++)
// 	{
// 		for(int j=0;j<boardX;j++)
// 		{
// 			Square* currentSquare=((Square*)SBoard[j][i]);
// 			currentSquare->DrawPawn(hdc,prc,hdcBuffer);
// 		}
// 		if (Start==1)
// 			Start=0;
// 		else
// 			Start=1;
// 	}
			
}

//-----------------------------------------------------------------------------
// Name : SaveBoardToFile ()
//-----------------------------------------------------------------------------
void board::SaveBoardToFile()
{
	std::ofstream saveFile;

	saveFile.open("board.sav");

	saveFile << m_kingInThreat << "| king in Threat" << "\n";
	saveFile << pieceCount << "| piece Count" << "\n";
	saveFile << currentPlayer << "| current player" << "\n";

	for (UINT i = 0; i < boardY; i++)
	{
		for (UINT j = 0; j < boardX; j++)
		{ 
			if (SBoard[i][j] != nullptr)
			{
				saveFile << i << "| piece i" << "\n";
				saveFile << j << "| piece j" << "\n";
				saveFile << SBoard[i][j]->getColor() << "| piece Color" << "\n";
				saveFile << SBoard[i][j]->getType() << "| piece Type" << "\n";
			}
		}
	}

	saveFile.close();
}

//-----------------------------------------------------------------------------
// Name : LoadBoardFromFile ()
//-----------------------------------------------------------------------------
bool board::LoadBoardFromFile()
{
	std::ifstream inputFile;

	// check if the files exists , if no file found abort!
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile("board.sav", &FindFileData) ;
	if(handle == INVALID_HANDLE_VALUE)
		return false;


	ZeroMemory(SBoard, sizeof(piece*) * ( boardX * boardY ) );//clearing the board

	ZeroMemory(&targetSqaure,sizeof(targetSqaure));

	currentPawn = NULL;
	prevPawn    = NULL;

	kings[BOTTOM] = NULL;
	kings[UPPER]  = NULL;

	deadPawnsVec[UPPER].clear();
	deadPawnsVec[BOTTOM].clear();


	for (UINT j =0; j < 2; j++)//clearing the pieces
	{
		for (UINT i = 0; i < pawnsVec[j].size(); i++)
		{
			std::vector<piece*>& curPieceVec = pawnsVec[j];
			delete curPieceVec[i];
			curPieceVec[i] = NULL;
		}
	}

	pawnsVec[UPPER].clear();
	pawnsVec[BOTTOM].clear();

	inputFile.open("board.sav");

	inputFile >> m_kingInThreat;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	inputFile >> pieceCount;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	inputFile >> currentPlayer;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	do 
	{
		UINT i,j;
		int color,type;

		inputFile >> i;
		inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		if (inputFile.eof() )
			break;

		inputFile >> j;
		inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		inputFile >> color;
		inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		inputFile >> type;
		inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (color != BLACK)
		{
			if (!createPiece(i,j, m_meshScale, color, type))
				return false;
			pawnsVec[BOTTOM].push_back( SBoard[i][j]);

		}
		else
		{
			if (!createPiece(i,j, m_meshScale, color, type, m_blkAttribID))
				return false;
			pawnsVec[UPPER].push_back( SBoard[i][j]);
		}

	} while (!inputFile.eof());

	inputFile.close();

	m_gameActive = true;

	return true;
}

/////////////////////////////////////////
////           getSquare             ////
/////////////////////////////////////////
// Square *(board::getSquare) (POINT &currentSquareLoc)
// {
// 	if ((currentSquareLoc.x<(8)) && (currentSquareLoc.y<(8)) && (currentSquareLoc.x>=0) && (currentSquareLoc.y>=0))
// 		return SBoard[currentSquareLoc.x][currentSquareLoc.y];	
// 	else
// 		return NULL;
// }

//-----------------------------------------------------------------------------
// Name : validateMove ()
//-----------------------------------------------------------------------------
bool board::validateMove(BOARD_POINT startLoc,BOARD_POINT newLoc)
{
	int dx = startLoc.row - newLoc.row;
	int dy = startLoc.col - newLoc.col;

	bool castlingValid = false;

	if (currentPawn != NULL)//checks the currentPawn isn't null just to be safe
	{
		if (currentPawn->validateNewPos(dx,dy,startLoc,newLoc,SBoard))
		{
			//TODO: make castling thingy here 
			D3DXVECTOR3 newCurPiecePos;

			piece * targetPiece = SBoard[newLoc.col][newLoc.row];

			if (targetPiece != NULL)
			{
				prevPawn = targetPiece;
				prevPawn->setPos(D3DXVECTOR3(900.0f,0.0f,900.0f));//sends it well off the board 
			}
			else
			{
				if (currentPawn->getType() == KING && (dx == -2 || dx == 3))//king was moved to castling pos
					if (!m_kingInThreat)//make sure the king is not in threat
					{
						if (!castling(dx, dy))//check if castling is valid 
							return false;//castling isn't valid and so is this move
						else
							castlingValid = true;
					}
					else
						return false;
			}

			if (castlingValid)
			{
				BOARD_POINT newRookSquare;
				int x = dx/abs(dx);
				int y = startSquare.col;

				x = startSquare.row - dx -x;

				newRookSquare.col = y;
				newRookSquare.row = x + 2 * (dx / abs(dx)) ;

				piece * rookPiece = SBoard[y][x]; 

				SBoard[y][x + (2 * dx)] = rookPiece;
				SBoard[y][x] = NULL;

				calcPieceBoardPos(newRookSquare,newCurPiecePos);
				newCurPiecePos.y = m_pos.y + rookPiece->getYpos();
				rookPiece->setPos(newCurPiecePos);
			}

			SBoard[newLoc.col][newLoc.row]      = currentPawn;
			SBoard[startLoc.col][startLoc.row]  = NULL;

			calcPieceBoardPos(newLoc,newCurPiecePos);
			newCurPiecePos.y = m_pos.y + currentPawn->getYpos();
			currentPawn->setPos(newCurPiecePos);

			if(isKingInThreat(currentPlayer,false))// if king is under threat the move is illegal
			{
				reverseMove();               // reverse the changes done 
				prevPawn = NULL;
				return false;
			}
			else
			{
//  				float xPos = m_pos.x + (newLoc.row * m_stepX * m_meshScale.x);//calculate pawn position in relation to the board 0,0 point
//  				float zPos = m_pos.z + ( (boardY - newLoc.col - 1) * m_stepZ * m_meshScale.z ) ;

				//TODO: add here some way to display something that will let the player choose the promotion unit
				if (prevPawn != NULL)
					killPiece(prevPawn,newLoc);

//				currentPawn->setPos(D3DXVECTOR3(xPos + 2,-7.0f,zPos + 2));
				currentPawn->setMoved(true);
				prevPawn = NULL;
				return true;
			}
		}
	}

	return false;

			
}

//-----------------------------------------------------------------------------
// Name : processPress ()
//-----------------------------------------------------------------------------
void board::processPress(CMyObject * pickedObjected, ULONG pressedFace )
{
	BOARD_POINT pressedSqaure;

	if (m_gameActive && !m_unitPromotion) // we only handle presses if game is still active and there is no units waiting for promotion
	{
		// retrieve the pressed square coordinates 
		if (this == pickedObjected)				 //check if the press was on the board mesh itself
		{
			ULONG squareNum;
			UINT  tempy;

			if (pressedFace != 0)
				squareNum   = pressedFace / 2; //finding the square number as every 2 faces make up a single square
			else
				squareNum = 0;

			//retrieve the square coordinates from face number that was picked on the board mes
			if (squareNum != 0)
				pressedSqaure.row = squareNum - ( (squareNum / boardY) * boardY);
			else
				pressedSqaure.row = 0;
			pressedSqaure.col = boardY - 1 - (squareNum / boardY); 

		}
		else//if the pressed was not on the board than get the object position in the world and test if it is on the board
		{	
			//retrieve object position in the world
			D3DXMATRIX objWorldMat = pickedObjected->m_mtxWorld;
			D3DXVECTOR3 objectPos = D3DXVECTOR3(objWorldMat._41,objWorldMat._42,objWorldMat._43);

			//retrieve the square coordinates from the object position 
			pressedSqaure.row = (objectPos .x - m_pos.x) / (m_stepX * m_meshScale.x); // ( object posX - board posX ) / (stepX * sclaeX) = row coordinate of the square 
			pressedSqaure.col = ( (m_pos.z - objectPos.z) / (m_stepZ * m_meshScale.z ) ) + (boardY - 1) + 0.5;
		}
		if (pressedSqaure.row == startSquare.row && pressedSqaure.col == startSquare.col)//the square was already selected
			return;

		std::stringstream out,out2,out3,out4;

		out3<<startSquare.col;
		out4<<startSquare.row;
		out<<pressedSqaure.col;
		out2<<pressedSqaure.row;

		m_curStatus = "the pressed square is "+out.str()+","+out2.str();

		if (pressedSqaure.row < boardX && pressedSqaure.col < boardY ) //check if the object is in the board bounds if not we have nothing to do with this object
		{
			if (startSquare.row < boardX && startSquare.col < boardY)//was there a valid press before ?
			{
				targetSqaure = pressedSqaure;
				if (!validateMove(startSquare,pressedSqaure)) // process press and check if the move is valid ?
				{
					//not valid therefore sets startSquare to invalid value to reset piece movement process
					startSquare.row = -1;
					startSquare.col = -1;

					m_attribSquares[ATTACK].clear();
					m_attribSquares[MOVE].clear();
					m_selectSquare = -1;

					m_curStatus +="\n move was invalid start:"+out3.str()+","+out4.str()+" target:"+out.str()+","+out2.str();
				}
				else
				{
					m_attribSquares[ATTACK].clear();
					m_attribSquares[MOVE].clear();
					m_selectSquare = -1;

					if (currentPawn->getType() == PAWN)
					{
						if (targetSqaure.col == 0 || targetSqaure.col == 7)
							m_unitPromotion = true;
					}
					//the move was valid now ends current player turn 
					if (!m_unitPromotion)//checks if pawn need to be promoted if yes wait till the main program tell us user selection and then proceed to endTurn
						endTurn();

					m_curStatus +="\n move was valid";
				}
			}
			else // no valid press exist so set the current pawn and startSquare point 
			{
				currentPawn = SBoard[pressedSqaure.col][pressedSqaure.row]; //retrieve current pawn from the board

				if (currentPawn == NULL)
					return;

				if (currentPawn->getColor() == currentPlayer) 
				{
					startSquare = pressedSqaure;
					m_curStatus += "\n press is Valid";

					m_attribSquares[ATTACK].clear();
					m_attribSquares[MOVE].clear();
					m_selectSquare = startSquare.row + (7 - startSquare.col) * m_numCellsWide;
					markPawnMoves(startSquare, currentPawn->getColor(), currentPlayer);

				}

			}

		}
	}

}

/////////////////////////////////////////
////            movePawn             ////
/////////////////////////////////////////
// void board::movePawn(POINT newLoc)
// {
// 	POINT squareLoc;
// 	squareLoc.x=newLoc.x/SQlength;
// 	squareLoc.y=newLoc.y/SQheight;
// 	targetSqaure=this->getSquare(squareLoc);
// 	if (prevSquare!=NULL)//removing highlight for previous Square
// 		prevSquare->HighLightBitmap(0);
// 	if (targetSqaure!=NULL)//highlighting current Square if it is without a pawn
// 		if (targetSqaure->getPawn()==NULL)
// 			targetSqaure->HighLightBitmap(IDB_SquareHighLighted);
// 	
// 
//  		currentPawn->setXY(newLoc.x-16,newLoc.y-16);
// 	prevSquare=targetSqaure;
// }


//-----------------------------------------------------------------------------
// Name : isKingInThreat ()
//-----------------------------------------------------------------------------
bool board::isKingInThreat(int player,bool getAllAttackers)
{
	int pieceSide,kingSide,maxThreat;

	m_kingInThreat = false;

	if (player == 1)
		kingSide = BOTTOM;
	else
		kingSide = UPPER;

	if (player == 1)
		pieceSide = UPPER;
	else
		pieceSide = BOTTOM;

	if (getAllAttackers)
		maxThreat = 2;
	else
		maxThreat = 1;

	UINT i = 0;
	int threat = 0;

	std::vector<piece*> pawnTemp=pawnsVec[pieceSide];
	while (i < pawnsVec[pieceSide].size() && threat < maxThreat)
	{
		piece * pCurPiece = pawnTemp[i];

		BOARD_POINT curPieceSquare = getPieceSquare(pCurPiece);
		BOARD_POINT curKingSquare  = getPieceSquare(kings[kingSide]);

		if ( (curPieceSquare.row  > 8 || curPieceSquare.row < 0) || ( curPieceSquare.col > 8 || curPieceSquare.col < 0))
		{
			i++;
			continue;//piece is on invalid location so ignore it ... **piece will be on an invalid location only if it is about to be killed
		}

 		int dx = curPieceSquare.row - curKingSquare.row;
 		int dy = curPieceSquare.col - curKingSquare.col;

		if(pCurPiece->validateNewPos(dx,dy,curPieceSquare,curKingSquare,SBoard))
		{
			threat++;
			if (getAllAttackers)
			{
				attLoc.x = dx;
				attLoc.y = dy;
			}
			//return true;
		}
		i++;
	}
	if (getAllAttackers)
		attackers=threat;

	if (threat > 0)
	{
		m_kingInThreat = true;
		return true;
	}
	else
		return false;

	return false;
}

//-----------------------------------------------------------------------------
// Name : reverseMove ()
//-----------------------------------------------------------------------------
void board::reverseMove()
{
	D3DXVECTOR3 currentPieceStartPos, currentPieceCurPos;

	//reset changes 
	SBoard[targetSqaure.col][targetSqaure.row]      = prevPawn;
	SBoard[startSquare.col][startSquare.row]	    = currentPawn;

	//reset the pieces to their old positions
	calcPieceBoardPos(startSquare,currentPieceStartPos);
	currentPieceStartPos.y = m_pos.y + currentPawn->getYpos();
	currentPawn->setPos(currentPieceStartPos);

	if (prevPawn != NULL)
	{
		calcPieceBoardPos(targetSqaure,currentPieceCurPos);
		currentPieceCurPos.y = m_pos.y + prevPawn->getYpos();
		prevPawn->setPos(currentPieceCurPos);
	}

}

//-----------------------------------------------------------------------------
// Name : isEndGame ()
//-----------------------------------------------------------------------------
bool board::isEndGame(int curretPlayer)
{
	int side;
	BOARD_POINT kingSquare;

	if (curretPlayer == WHITE)
		side = BOTTOM;
	else
		side = UPPER;

	kingSquare = getPieceSquare(kings[side]);

	if (attackers == 1)
	{
		//----------------------------------------------------------------
		//check if we can eat the piece that is attacking               
		//----------------------------------------------------------------
		targetSqaure.row = kingSquare.row + attLoc.x;//getting the position of the pawn that is attacking the king
		targetSqaure.col = kingSquare.col + attLoc.y;

		std::vector<piece*> pawnTemp = pawnsVec[side];     //getting pawns under the king color
		UINT i=0;

		while (i<pawnsVec[side].size())//scanning to check if a pawn can eat the attacking pawn
		{
			piece* curPawn=pawnTemp[i];//getting the pawn that we checking for right now
			BOARD_POINT curPieceSquare,curAttPieceSquare;

			curPieceSquare = getPieceSquare(curPawn);
			startSquare = curPieceSquare;// getting the square he is current at

			curAttPieceSquare.row = kingSquare.row + attLoc.x;//getting the location of the attacker on the board
			curAttPieceSquare.col = kingSquare.col + attLoc.y;

			int dx=curPieceSquare.row - curAttPieceSquare.row;//calculating dx dy of curPawn from 
			int dy=curPieceSquare.col - curAttPieceSquare.col;//Attacker

			if(curPawn->validateNewPos(dx,dy,startSquare,targetSqaure,SBoard))
			{
				//if the move is valid checking to see that the king is not threated from
				//such move if the king is threated than keep scanning 
				//also after the move was done always reverse it in the end
				if  (!validateKingThreat(curretPlayer))
					return false;
			}
			i++;
		}
		//----------------------------------------------------------------
		//END of check if we can eat the attacker                       
		//----------------------------------------------------------------


		//----------------------------------------------------------------
		//checking if a pawn can block the attacker path                
		//----------------------------------------------------------------
		targetSqaure.row = kingSquare.row + attLoc.x;
		targetSqaure.col = kingSquare.col + attLoc.y;

		int AtkPawnType = SBoard[targetSqaure.col][targetSqaure.row]->getType();
		int dxxx,dyyy;

		if (attLoc.x != 0)
			dxxx = attLoc.x / abs(attLoc.x);
		else
			dxxx = 0;

		if (attLoc.y != 0)
			dyyy = attLoc.y / abs(attLoc.y);
		else
			dyyy=0;

		targetSqaure.row = kingSquare.row + dxxx;
		targetSqaure.col = kingSquare.col + dyyy;

		if(AtkPawnType != KNIGHT && AtkPawnType != PAWN && AtkPawnType != KING) //knight king and pawn cannot be blocked
		{
			i=0;
			while (i < pawnsVec[side].size())//scanning to check if a pawn can eat the attacking pawn
			{
				int j = kingSquare.row + dxxx;
				int k = kingSquare.col + dyyy;

				while (j != (kingSquare.row + attLoc.x) || k != (kingSquare.col + attLoc.y))
				{

					//getting the location of the attacker on the board
					targetSqaure.row = j;
					targetSqaure.col = k;

					piece* curPawn=pawnTemp[i];//getting the pawn that we checking for right now

					startSquare = getPieceSquare(curPawn); // getting the square he is current at

					int dx = startSquare.row - targetSqaure.row;//calculating dx dy of curPawn from 
					int dy = startSquare.col - targetSqaure.col;//Attacker

					if(curPawn->validateNewPos(dx,dy,startSquare,targetSqaure,SBoard))
					{
						//if the move is valid checking to see that the king is not threated from
						//such move if the king is threated than keep scanning 
						//also after the move was done always reverse it in the end
						if  (!validateKingThreat(curretPlayer))
							return false;
					}
					j+=dxxx;
					k+=dyyy;
				}
				i++;
			}
		}
		//----------------------------------------------------------------
		//END of checking if a pawn can block the attacker path         
		//----------------------------------------------------------------

	}

	//----------------------------------------------------------------
	//check for if the king can flee to other square from the attack
	//----------------------------------------------------------------
 	startSquare = kingSquare;//getting king square

	for (int dy = -1; dy < 2; dy++)//scanning all possible squares that the king can move to 
	{					  
		for (int dx =- 1; dx < 2; dx++)
		{
			if (dx !=0 || dy != 0)
			{
				BOARD_POINT kingFleeLoc; 
				kingFleeLoc.row = kingSquare.row - dx;//getting current flee square x,y
				kingFleeLoc.col = kingSquare.col - dy;

				if ( (kingFleeLoc.row < 8 && kingFleeLoc.row >=0) && (kingFleeLoc.col < 8 && kingFleeLoc.col >=0 ) )//checking that we are still in board range
				{
					targetSqaure = kingFleeLoc;//getting the flee square

					if(kings[side]->validateNewPos(dx,dy,startSquare,targetSqaure,SBoard))
					{
						if (!validateKingThreat(curretPlayer))//checking that the king
							return false;					  //can do the move without being threaten

					}
				}
			}
		}
	}
	//------------------------------------------------------------------------
	//END of check for if the king can flee to other square from the attack 
	//------------------------------------------------------------------------

 	return true;
}

//-----------------------------------------------------------------------------
// Name : validateKingThreat ()
//-----------------------------------------------------------------------------
bool board::validateKingThreat(int curretPlayer)
{
	D3DXVECTOR3 curPieceNewPos;

	prevPawn    = SBoard[targetSqaure.col][targetSqaure.row];
	currentPawn = SBoard[startSquare.col][startSquare.row];

	if (prevPawn != NULL)
		prevPawn->setPos(D3DXVECTOR3(900.0f,0.0f,900.0f));//sends the pawn far far away

	SBoard[targetSqaure.col][targetSqaure.row] = currentPawn;
	SBoard[startSquare.col][startSquare.row]   = NULL;
	
	calcPieceBoardPos(targetSqaure,curPieceNewPos);
	curPieceNewPos.y = m_pos.y + currentPawn->getYpos();
	currentPawn->setPos(curPieceNewPos);

	if (!isKingInThreat(currentPlayer,false))
	{
		reverseMove();
		prevPawn = NULL;
		return false;
	}
	else
	{
		reverseMove();
		prevPawn = NULL;
		return true;
	}

	prevPawn = NULL;
	return true;//default value .... because I really don't know X_X
}

//-----------------------------------------------------------------------------
// Name : canPawnMove ()
//-----------------------------------------------------------------------------
bool board::canPawnMove(BOARD_POINT pieceSqaure,int color,int curretPlayer)
{
	int dxValues[2];
	int dyValues[2];

	startSquare = pieceSqaure;//getting the current Pawn we are checking for  square
	currentPawn = SBoard[startSquare.col][startSquare.row];

	currentPawn->getDx(dxValues);//getting the values of dx should run between in order to cover all possible moves
	currentPawn->getDy(dyValues);//getting the values of dy should run between in order to cover all possible moves

	for (int dy = dyValues[0]; dy < dyValues[1]; dy++)//scanning all possible squares that the pawn can move to 
	{					  
		for (int dx = dxValues[0]; dx < dxValues[1]; dx++)
		{
			if (dx != 0 || dy != 0)
			{
				BOARD_POINT possibleMoveLoc; 
				possibleMoveLoc.row = startSquare.row - dx;//getting current possible square x,y that the pawn can move to
				possibleMoveLoc.col = startSquare.col - dy;

				if ( (possibleMoveLoc.row < 8 && possibleMoveLoc.row >=0) && (possibleMoveLoc.col < 8 && possibleMoveLoc.col >=0 ) )//checking that we are still in board range
				{
					targetSqaure = possibleMoveLoc;//getting the  current possible square
					if(currentPawn->validateNewPos(dx,dy,startSquare,targetSqaure,SBoard))
					{
						if (!validateKingThreat(curretPlayer))//checking that the king
							return true;		//can do the move without being threaten
					}
				}
			}
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : markPawnMoves ()
//-----------------------------------------------------------------------------
void board::markPawnMoves(BOARD_POINT pieceSqaure,int color,int curretPlayer)
{
	int dxValues[2];
	int dyValues[2];

	startSquare = pieceSqaure;//getting the current Pawn we are checking for  square
	currentPawn = SBoard[startSquare.col][startSquare.row];

	currentPawn->getDx(dxValues);//getting the values of dx should run between in order to cover all possible moves
	currentPawn->getDy(dyValues);//getting the values of dy should run between in order to cover all possible moves

	switch (currentPawn->getType())
	{
	case PAWN:
	case KING:
	case KNIGHT:
		{
			for (int dy = dyValues[0]; dy < dyValues[1]; dy++)//scanning all possible squares that the pawn can move to 
			{					  
				for (int dx = dxValues[0]; dx < dxValues[1]; dx++)
				{
					if (dx != 0 || dy != 0)
					{
						BOARD_POINT possibleMoveLoc; 
						possibleMoveLoc.row = startSquare.row - dx;//getting current possible square x,y that the pawn can move to
						possibleMoveLoc.col = startSquare.col - dy;

						if ( (possibleMoveLoc.row < 8 && possibleMoveLoc.row >=0) && (possibleMoveLoc.col < 8 && possibleMoveLoc.col >=0 ) )//checking that we are still in board range
						{
							targetSqaure = possibleMoveLoc;//getting the  current possible square
							if(currentPawn->validateNewPos(dx,dy,startSquare,targetSqaure,SBoard))
							{
								if (!validateKingThreat(curretPlayer))//checking that the king
								{
									if (SBoard[targetSqaure.col][targetSqaure.row] != nullptr)
									{
										if (SBoard[targetSqaure.col][targetSqaure.row]->getColor() != currentPlayer)
											m_attribSquares[ATTACK].push_back(targetSqaure.row + ( (7 - targetSqaure.col ) * m_numCellsWide));
										else
											m_attribSquares[MOVE].push_back(targetSqaure.row + ( (7 - targetSqaure.col) * m_numCellsWide));
									}
									else
										m_attribSquares[MOVE].push_back(targetSqaure.row + ( ( 7 - targetSqaure.col) * m_numCellsWide));
								}
							}
						}
					}
				}
			}

			if (currentPawn->getType() == KING)// && (dx == -2 || dx == 3))//king was moved to castling pos
				if (!m_kingInThreat)//make sure the king is not in threat
				{
					BOARD_POINT possibleMoveLoc; 
					possibleMoveLoc.row = startSquare.row - (-2);//getting current possible square x,y that the pawn can move to
					possibleMoveLoc.col = startSquare.col - 0;

					targetSqaure = possibleMoveLoc;

					if (currentPawn->validateNewPos(-2, 0, startSquare, targetSqaure, SBoard) )
						if (castling(-2, 0))//check if castling is valid
							m_attribSquares[MOVE].push_back(targetSqaure.row + ( ( 7 - targetSqaure.col) * m_numCellsWide) );

					possibleMoveLoc.row = startSquare.row - 3;//getting current possible square x,y that the pawn can move to
					possibleMoveLoc.col = startSquare.col - 0;

					targetSqaure = possibleMoveLoc;

					if (currentPawn->validateNewPos(3, 0, startSquare, targetSqaure, SBoard) )
						if (castling(3, 0))//check if castling is valid
							m_attribSquares[MOVE].push_back(targetSqaure.row + ( ( 7 - targetSqaure.col) * m_numCellsWide) );
				}

		}break;

	case BISHOP:
		{
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,1));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,-1));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,-1));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,1));
		}break;

	case ROOK:
		{
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,0));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(0,1));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,0));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(0,-1));
		}break;

	case QUEEN:
		{
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,1));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,-1));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,-1));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,1));

			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,0));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(0,1));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,0));
			ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(0,-1));
		}break;

	}


}


//-----------------------------------------------------------------------------
// Name : ScanPawnMoves ()
//-----------------------------------------------------------------------------
void board::ScanPawnMoves(BOARD_POINT pieceSqaure,int color,int curretPlayer,DIR_VEC dir)
{
	BOARD_POINT possibleMoveLoc;
	possibleMoveLoc = pieceSqaure;//getting the current Pawn we are checking for  square
	startSquare = pieceSqaure;


	possibleMoveLoc.row = possibleMoveLoc.row + dir.x;
	possibleMoveLoc.col = possibleMoveLoc.col + dir.y;

	while( (possibleMoveLoc.row < 8 && possibleMoveLoc.row >=0) && (possibleMoveLoc.col < 8 && possibleMoveLoc.col >=0 ) )
	{
		targetSqaure = possibleMoveLoc;//getting the  current possible square

		int dx = startSquare.row - targetSqaure.row;
		int dy = startSquare.col - targetSqaure.col;

		if(currentPawn->validateNewPos(dx,dy,startSquare,targetSqaure,SBoard))
		{
			if (!validateKingThreat(curretPlayer))//checking that the king
			{
				if (SBoard[targetSqaure.col][targetSqaure.row] != nullptr)
				{
					if (SBoard[targetSqaure.col][targetSqaure.row]->getColor() != currentPlayer)
					{
						m_attribSquares[ATTACK].push_back(targetSqaure.row + ( (7 - targetSqaure.col ) * m_numCellsWide));
						break;
					}
				}
				else
					m_attribSquares[MOVE].push_back(targetSqaure.row + ( ( 7 - targetSqaure.col) * m_numCellsWide));
			}
		}

		possibleMoveLoc.row = possibleMoveLoc.row + dir.x;
		possibleMoveLoc.col = possibleMoveLoc.col + dir.y;
	}
}

//-----------------------------------------------------------------------------
// Name : isDraw ()
//-----------------------------------------------------------------------------
bool board::isDraw(int currentPlayer)
{
	int side;
	if (currentPlayer == 1)
		side = BOTTOM;
	else
		side = UPPER;

	UINT i=0;
	std::vector<piece*> pawnTemp=pawnsVec[side];

	while (i < pawnsVec[side].size())
	{
		BOARD_POINT curPieceSquare = getPieceSquare(pawnTemp[i]);
		if(canPawnMove(curPieceSquare,side,currentPlayer))
			return false;
		i++;
	}
 	return true;

}

//-----------------------------------------------------------------------------
// Name : generateBoardMesh ()
//-----------------------------------------------------------------------------
// HRESULT board::generateBoardMesh(LPDIRECT3DDEVICE9 pDevice, CMyMesh *  pMesh, D3DXVECTOR3 vecScale)
// {
// 	HRESULT hRet;
// 
// 	bool		ManageAttribs = pMesh->isMenageAtrributes();
// 	USHORT		numVertsX	  = boardX + 1;
// 	USHORT		numVertsZ	  = boardY + 1;
// 
// 	//calculate the step size from one vertex to the other along both the x and the z axis for now 1 unit as we pass a vector to scale it
// 	m_stepX = 1.0f;
// 	m_stepZ = 1.0f;
// 
// 	//preparing the vertex buffer for the terrain mesh
// 	CMyVertex* pVertices = new CMyVertex[numVertsX * numVertsZ];
// 	if (!pVertices) return E_OUTOFMEMORY;
// 
// 	ZeroMemory( pVertices,sizeof(CMyVertex) * (numVertsX * numVertsZ) );
// 
// 	// Set the start position
// 	D3DXVECTOR3 pos(0, 0, 0);
// 
// 	int VertexCount=0;
// 	float Tu = 0.0f;// tu and tv are texture coordinates
// 	float Tv = 0.0f;
// 
// 	// Loop across and up
// 	for (int z=0 ; z<numVertsZ ; z++)
// 	{
// 		Tu    = 0.0f;
// 		pos.x = 0;
// 
// 		for (int x=0 ; x<numVertsX ; x++)
// 		{
// 			// Create the verts
// 			pVertices[VertexCount].x	  = pos.x * vecScale.x;
// 			pVertices[VertexCount].y	  = pos.y * vecScale.y;
// 			pVertices[VertexCount].z	  = pos.z * vecScale.z;
// 			pVertices[VertexCount].Normal = D3DXVECTOR3(0.0f , 1.0f ,0.0f);
// 			pVertices[VertexCount].tu	  = Tu;
// 			pVertices[VertexCount].tv	  = Tv;
// 
// 			Tu++;
// 
// 			// Increment x across
// 			pos.x += m_stepX;
// 			VertexCount++;
// 		}
// 
// 		Tv++;
// 		// Increment Z
// 		pos.z += m_stepZ;
// 	}
// 	//end of  preparing the vertex buffer
// 
// 	//preparing the index buffer 
// 	int count = 0;
// 	USHORT vIndex = 0;
// 
// 
// 	//creating the attribute data 
// 	MESH_ATTRIB_DATA* pAttribData;
// 	ULONG attribMap[2];
// 
// 	//creating a pure white material
// 	D3DMATERIAL9 Material;
// 	ZeroMemory( &Material, sizeof(D3DMATERIAL9));
// 	Material.Diffuse = D3DXCOLOR( 1.0, 1.0, 1.0, 1.0f );
// 	Material.Ambient = D3DXCOLOR( 1.0, 1.0, 1.0, 1.0f );
// 
// 	// if we manage the attribute load them using the callback function and than save the reference to them in the mesh
// 	if (ManageAttribs)
// 	{	
// 		if ( pMesh->AddAttributeData(2) < 0)
// 			return E_OUTOFMEMORY;
// 
// 		pAttribData = pMesh->GetAttributeData();
// 
// 		for (int i = 0; i < 2; i++)
// 		{
// 			pAttribData[i].Texture = pMesh->getTexture(Textures[i]);
// 			pAttribData[i].Material = Material;
// 			pAttribData[i].Effect = NULL;
// 		}
// 	}
// 
// 	else
// 	{
// 		//loading attribute and creating an attribute map for them 
// 		for (int i = 0; i < 2; i++)
// 		{
// 			attribMap[i] = pMesh->getAttributes(Textures[i],&Material,NULL);
// 		}
// 
// 		pMesh->setAttribMap(attribMap,2);//setting the attribute map to the mesh
// 	}
// 
// 	//the max number of indices in the terrain is numCellsHigh * numCellsWide * how many indices per cell.  which in our case for now is 6 ...
// 	USHORT* pIndices = new USHORT[6];
// 
// 	for (int z=0 ; z < boardY ; z++)
// 	{
// 		for (int x=0 ; x < boardX ; x++)
// 		{
// 			ULONG attribID;
// 			// first triangle
// 			pIndices[count++] = vIndex;
// 			pIndices[count++] = vIndex+numVertsX;
// 			pIndices[count++] = vIndex+numVertsX+1;
// 
// 			// second triangle
// 			pIndices[count++] = vIndex;
// 			pIndices[count++] = vIndex+numVertsX+1;
// 			pIndices[count++] = vIndex+1;
// 
// 			if (vIndex % 2 == 0)
// 					attribID = 0;	
// 			else
// 					attribID = 1;
// 			
// 
// 			if ( pMesh->AddFace(2,pIndices,attribID) < 0)
// 				return E_OUTOFMEMORY;
// 
// 			delete []pIndices;
// 			count = 0;
// 			pIndices = NULL;
// 			pIndices = new USHORT[6];
// 
// 			vIndex++;
// 		}
// 		vIndex++;
// 	}
// 	//end of preparing the index buffer 
// 
// 
// 
// 	if ( pMesh->AddVertex(VertexCount,pVertices) < 0)
// 		return E_OUTOFMEMORY;
// 
// 
// 	hRet = pMesh->BuildMesh(D3DXMESH_MANAGED,pDevice);
// 
// 	delete []pVertices;
// 	delete []pIndices;
// 
// 	m_meshScale = vecScale;
// 	m_pMesh		= pMesh; //board mesh successfully created so store in the object data
// 	return hRet;	
// }

//-----------------------------------------------------------------------------
// Name : createPiece ()
//-----------------------------------------------------------------------------
bool board::createPiece(int i, int j, D3DXVECTOR3 vecScale, int playerColor, UINT pieceType, ULONG blkAttribID /* = 0 */)
{
	OBJECT_PREFS curObjPrefs;
	CMyObject* pCurPiece = nullptr;

	float xPos = m_pos.x + (j * m_stepX * vecScale.x);//calculate pawn position in relation to the board 0,0 point
	float zPos = m_pos.z + ( (boardY - i - 1) * m_stepZ * vecScale.z ) ;

	curObjPrefs.pos = D3DXVECTOR3(xPos + 3, 0 , zPos + 3);
	curObjPrefs.rotAngels = D3DXVECTOR3(0,0,0);
	curObjPrefs.scale = vecPieceScale;

	if (pieceType <= KING) //check if  a valid type was giving and create the piece for it 
						   //If the type is not valid create the default piece that should be in that location
	{
		pCurPiece = allocPiece(pieceType, curObjPrefs, playerColor, blkAttribID);

		if (!pCurPiece)
			return false;

		if (playerColor == BLACK)
			pCurPiece->customiseAtrributes(&blkAttribID,1);//if the piece should be black change it's color from the default (white is the default!)

		SBoard[i][j] = (piece*)pCurPiece;
		m_pieceCreatedSig(pCurPiece);
		return true;
	}

	if (i == 1 || i == 6)
	{
		pCurPiece = allocPiece(PAWN, curObjPrefs, playerColor, blkAttribID);
// 		curObjPrefs.pos.y = m_pos.y + Pawn::Ypos;
// 		pPieceObj[pieceIndex] = new Pawn(m_pPiecesMeshs[PAWN],&curObjPrefs,playerColor);
	}

	if (i == 0 || i == 7)
	{	
		if (j == 0 || j == 7)
		{
			pCurPiece = allocPiece(ROOK, curObjPrefs, playerColor, blkAttribID);
// 			curObjPrefs.pos.y	  = m_pos.y + rook::Ypos;
// 			pPieceObj[pieceIndex] = new rook(m_pPiecesMeshs[ROOK], &curObjPrefs, playerColor);
		}

		if (j == 1 || j == 6)
		{
			pCurPiece = allocPiece(KNIGHT, curObjPrefs, playerColor, blkAttribID);
// 			curObjPrefs.pos.y     = m_pos.y + knight::Ypos;
// 			pPieceObj[pieceIndex] = new knight(m_pPiecesMeshs[KNIGHT], &curObjPrefs, playerColor);
		}

		if (j == 2 || j == 5 )
		{
			pCurPiece = allocPiece(BISHOP, curObjPrefs, playerColor, blkAttribID);
// 			curObjPrefs.pos.y     = m_pos.y + bishop::Ypos;
// 			pPieceObj[pieceIndex] = new bishop(m_pPiecesMeshs[BISHOP], &curObjPrefs, playerColor);
		}

		if (j == 3)
		{
			pCurPiece = allocPiece(QUEEN, curObjPrefs, playerColor, blkAttribID);
// 			curObjPrefs.pos.y     = m_pos.y + queen::Ypos;
// 			pPieceObj[pieceIndex] = new queen(m_pPiecesMeshs[QUEEN], &curObjPrefs, playerColor);
		}

		if (j == 4)
		{
			pCurPiece = allocPiece(KING, curObjPrefs, playerColor, blkAttribID);
// 			curObjPrefs.pos.y     = m_pos.y + king::Ypos;
// 			pPieceObj[pieceIndex] = new king(m_pPiecesMeshs[KING], &curObjPrefs,playerColor);
// 
// 			if (!pPieceObj[pieceIndex])// check here too  because I am lazy to make this part pretty so fuck it
// 				return false;
// 
// 			if (playerColor == BLACK)
// 				kings[UPPER] =  (king*)pPieceObj[pieceIndex];
// 			else
// 				kings[BOTTOM] = (king*)pPieceObj[pieceIndex];
		}
	}

	if (!pCurPiece)
		return false;

	if (playerColor == BLACK)
		pCurPiece->customiseAtrributes(&blkAttribID,1);//if the piece should be black change it's color from the default (white is the default!)

	SBoard[i][j] = (piece*)pCurPiece;
	m_pieceCreatedSig(pCurPiece);
	return true;
}

//-----------------------------------------------------------------------------
// Name : allocPiece ()
//-----------------------------------------------------------------------------
CMyObject* board::allocPiece(UINT pieceType, OBJECT_PREFS& curObjPrefs, int playerColor, ULONG blkAttribID)
{
	CMyObject* pPieceObj = nullptr;
	switch (pieceType)
	{

	case PAWN:
		{
			curObjPrefs.pos.y = m_pos.y/* + Pawn::Ypos*/;
			pPieceObj = new Pawn(m_pPiecesMeshs[PAWN], &curObjPrefs, playerColor);
		}break;

	case KNIGHT:
		{
			curObjPrefs.pos.y     = m_pos.y/* + knight::Ypos*/;
			pPieceObj = new knight(m_pPiecesMeshs[KNIGHT], &curObjPrefs, playerColor);
			pPieceObj->setRotAngels(D3DXVECTOR3(0, D3DX_PI / 2, 0));
		}break;

	case BISHOP:
		{
			curObjPrefs.pos.y     = m_pos.y/* + bishop::Ypos*/;
			pPieceObj = new bishop(m_pPiecesMeshs[BISHOP], &curObjPrefs, playerColor);
		}break;

	case ROOK:
		{
			// manual fix for rook scale...
			curObjPrefs.scale.x *= 0.269;
			curObjPrefs.scale.y *= 0.422;
			curObjPrefs.scale.z *= 0.269;

// 			curObjPrefs.scale.x = 3.717f;
// 			curObjPrefs.scale.y = 2.369f;
// 			curObjPrefs.scale.y = 3.717f;

			curObjPrefs.pos.y	  = m_pos.y + rook::Ypos;
			pPieceObj = new rook(m_pPiecesMeshs[ROOK], &curObjPrefs, playerColor);
		}break;

	case QUEEN:
		{
			curObjPrefs.pos.y     = m_pos.y/* + queen::Ypos*/;
			pPieceObj = new queen(m_pPiecesMeshs[QUEEN], &curObjPrefs, playerColor);
		}break;

	case KING:
		{
			curObjPrefs.pos.y     = m_pos.y/* - king::Ypos + 6*/;
			pPieceObj = new king(m_pPiecesMeshs[KING], &curObjPrefs, playerColor);


			if (!pPieceObj)// check here too  because I am lazy to make this part pretty so fuck it
				return nullptr;

			if (playerColor == BLACK)
				kings[UPPER] =  (king*)pPieceObj;
			else
				kings[BOTTOM] = (king*)pPieceObj;

		}break;

	}

	return pPieceObj;
// 	if (!pPieceObj[pieceIndex])
// 		return false;
// 
// 	if (playerColor == BLACK)
// 		pPieceObj[pieceIndex]->customiseAtrributes(&blkAttribID,1);//if the piece should be black change it's color from the default (white is the default!)
// 
// 	SBoard[i][j] = (piece*)pPieceObj[pieceIndex];
// 
// 	return true;

}

//-----------------------------------------------------------------------------
// Name : connectToPieceCreated ()
//-----------------------------------------------------------------------------
void board::connectToPieceCreated(const singal_pieceCreated::slot_type& subscriber)
{
	m_pieceCreatedSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : endTurn ()
//-----------------------------------------------------------------------------
void board::endTurn()
{
	currentPlayer *=-1;

	if (isKingInThreat(currentPlayer,true))
	{
		if(isEndGame(currentPlayer))
		{
			m_gameActive= false;
			m_curStatus = "CheckMate";
		}
	}
	else
		if (isDraw(currentPlayer))
		{
			m_gameActive = false;
			m_curStatus = "Draw";
		}

    startSquare.row = -1;
	startSquare.col = -1;
}

//-----------------------------------------------------------------------------
// Name : killPiece ()
//-----------------------------------------------------------------------------
void board::killPiece(piece * pPieceToKill, BOARD_POINT pieceSquare)
{
	int side;

	if (pPieceToKill->getType() == KING)
	{
		int x;
		x=5;
		return;
	}
//	SBoard[pieceSquare.col][pieceSquare.row] = NULL;//remove the piece from the board

	if (currentPlayer == BLACK)
		side = BOTTOM;
	else
		side = UPPER;

	std::vector<piece*> &curVector = pawnsVec[side];

	for (UINT i = 0; i < curVector.size(); i++)
	{
		if (pPieceToKill == curVector[i])
		{
			//adding to a dead pawns vector and than removing it from the active vec as we don't want to completely lose the pointer to the object
			pPieceToKill->setObjectHidden(true);
			deadPawnsVec[side].push_back(pPieceToKill);
			curVector.erase(curVector.begin()+i);
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Name : getPieceSquare ()
//-----------------------------------------------------------------------------
BOARD_POINT board::getPieceSquare(piece * pPiece)
{
	BOARD_POINT boardSquare;

	D3DXMATRIX objWorldMat = pPiece->m_mtxWorld;
	D3DXVECTOR3 objectPos = D3DXVECTOR3(objWorldMat._41,objWorldMat._42,objWorldMat._43);

	boardSquare.row = (objectPos .x - m_pos.x) / (m_stepX * m_meshScale.x); // ( object posX - board posX ) / (stepX * sclaeX) = row coordinate of the square 
	boardSquare.col = ( (m_pos.z - objectPos.z) / (m_stepZ * m_meshScale.z ) ) + (boardY - 1) + 0.5;

	return boardSquare;
}

//-----------------------------------------------------------------------------
// Name : getBoardStatus ()
//-----------------------------------------------------------------------------
std::string board::getBoardStatus() const
{
	return m_curStatus;
}

//-----------------------------------------------------------------------------
// Name : isBoardActive ()
//-----------------------------------------------------------------------------
bool board::isBoardActive () const
{
	return m_gameActive;
}

//-----------------------------------------------------------------------------
// Name : isUnitPromotion ()
//-----------------------------------------------------------------------------
bool board::isUnitPromotion()  const
{
	return m_unitPromotion;
}

//-----------------------------------------------------------------------------
// Name : getKingThreat ()
//-----------------------------------------------------------------------------
bool board::getKingThreat () const
{
	return m_kingInThreat;
}

//-----------------------------------------------------------------------------
// Name : calcPieceBoardPos ()
//-----------------------------------------------------------------------------
void board::calcPieceBoardPos(BOARD_POINT boardPos,D3DXVECTOR3& rPiecePos)
{
	float xPos = m_pos.x + (boardPos.row * m_stepX * m_meshScale.x);//calculate pawn position in relation to the board 0,0 point
	float zPos = m_pos.z + ( (boardY - boardPos.col - 1) * m_stepZ * m_meshScale.z );
	rPiecePos = D3DXVECTOR3(xPos + 3, 0 , zPos + 3);
}

//-----------------------------------------------------------------------------
// Name : resetGame ()
//-----------------------------------------------------------------------------
bool board::resetGame()
{
	ZeroMemory(SBoard, sizeof(piece*) * ( boardX * boardY ) );//clearing the board

	ZeroMemory(&targetSqaure,sizeof(targetSqaure));

	currentPawn = NULL;
	prevPawn    = NULL;

	kings[BOTTOM] = NULL;
	kings[UPPER]  = NULL;

	deadPawnsVec[UPPER].clear();
	deadPawnsVec[BOTTOM].clear();


	for (UINT j =0; j < 2; j++)//clearing the pieces
	{
		for (UINT i = 0; i < pawnsVec[j].size(); i++)
		{
			std::vector<piece*>& curPieceVec = pawnsVec[j];
			delete curPieceVec[i];
			curPieceVec[i] = NULL;
		}
	}

	pawnsVec[UPPER].clear();
	pawnsVec[BOTTOM].clear();

	pieceCount = 0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < boardX; j++)
		{
			std::vector<piece*>& curPieceVec = pawnsVec[UPPER];
			if (createPiece(i ,j ,m_meshScale , BLACK, KING + 1,m_blkAttribID))
			{
				//pawnsVec[UPPER].push_back((piece*)m_pPieceObj[pieceCount++]);
				pawnsVec[UPPER].push_back( SBoard[i][j]);
			}
			else
				return false;
			//pawnsVec[UPPER].push_back( (piece*)pPieceObj[pieceCount++] );
		}
	}

	for (int i = boardY - 2; i < boardY; i++)
	{
		for (int j = 0; j < boardX; j++)
		{
			std::vector<piece*>& curPieceVec = pawnsVec[BOTTOM];
			if (createPiece(i ,j ,m_meshScale , WHITE, KING + 1))
			{
				pawnsVec[BOTTOM].push_back( SBoard[i][j]);
				//pawnsVec[BOTTOM].push_back( (piece*)m_pPieceObj[pieceCount++] );
			}
			else
				return false;
			//pawnsVec[BOTTOM].push_back( (piece*)pPieceObj[pieceCount++] );
		}
	}

	currentPlayer = 1;
	m_gameActive = true;
	m_kingInThreat = false;

	return true;
}

//-----------------------------------------------------------------------------
// Name : PromoteUnit ()
//-----------------------------------------------------------------------------
bool board::PromoteUnit(PIECES type)
{
	SIDES curSide;
	int pieceIndex = -1;

	if (currentPawn->getColor() == 1)
		curSide = BOTTOM;
	else
		curSide = UPPER;

	std::vector<piece*>& curPieceVec = pawnsVec[curSide];

	if (currentPawn)
	{
		for (UINT i = 0; i < curPieceVec.size(); i++)
		{
			if (curPieceVec[i] == currentPawn)
			{
				pieceIndex = i;
				curPieceVec.erase(curPieceVec.begin() + pieceIndex);//remove the piece from the vector
				break;
			}
		}

		if (pieceIndex != -1)
		{
			//pieceIndex++;//adding one to pieceIndex as the board stands in index 0 
			if (currentPawn->getColor() == WHITE)
				pieceIndex += 16; 

			//pieceIndex = 0;

			//delete m_pPieceObj[pieceIndex];
			//m_pPieceObj[pieceIndex] = NULL;
			currentPawn = NULL;

			if (currentPlayer == WHITE)
				createPiece(targetSqaure.col , targetSqaure.row , m_meshScale, currentPlayer, type);
			else
				createPiece(targetSqaure.col , targetSqaure.row , m_meshScale, currentPlayer, type, m_blkAttribID);

			curPieceVec.push_back( SBoard[targetSqaure.col][targetSqaure.row] );
			//curPieceVec.push_back( (piece*)m_pPieceObj[pieceIndex] );
			m_unitPromotion = false;
			endTurn();//the new promoted piece is now part of the board so we can end the player turn

// 			delete m_pPieceObj[pieceIndex]
// 			 pawnsVec[curSide][pieceIndex] = NULL;

			// pawnsVec[curSide]->erase(pawnsVec[curSide]->begin() + pawnIndex);
		}

	}
	else 
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Name : castling ()
//-----------------------------------------------------------------------------
bool board::castling(int dx , int dy)
{
	D3DXVECTOR3 newCurPiecePos;


	BOARD_POINT targetSqaureBack  = targetSqaure; // backing up the target Square
	BOARD_POINT newRookSquare;
	int x = dx/abs(dx);
	int y = startSquare.col;

	x = startSquare.row - dx -x;

	newRookSquare.col = y;
	newRookSquare.row = x + 2 * (dx / abs(dx)) ;

	piece * rookPiece = SBoard[y][x]; 
	if (rookPiece != NULL)
	{
		if (rookPiece->getType() == ROOK && !rookPiece->isMoved())
		{
			dx = dx / abs(dx);
			for (int i = startSquare.row - dx; i != x; i -= dx)
			{
				targetSqaure.col = y;
				targetSqaure.row = i;
				if ( validateKingThreat(currentPlayer) )
					return false;
			}

// 			SBoard[y][x + (2 * dx)] = rookPiece;
// 			SBoard[y][x] = NULL;
// 
// 			calcPieceBoardPos(newRookSquare,newCurPiecePos);
// 			newCurPiecePos.y = m_pos.y + rookPiece->getYpos();
// 			rookPiece->setPos(newCurPiecePos);
			return true;

		}
	}
	
		
	return false;
// 	if ( dx==-2 || dx==3 )
// 	{
// 		BOARD_POINT * temp = currentSquare;
// 
// 		int x = dx/abs(dx);
// 		int y = startSquare->getY()/SQheight;
// 
// 		x=startSquare->getX()/SQlength-dx-x;
// 		BOARD_POINT * rookSquare=SBoard[x][y];
// 		prevPawn=rookSquare->getPawn();
// 		dx=dx/abs(dx);
// 		if (prevPawn->getType()==ROOK)
// 		{
// 			if (!currentPawn->isMoved() && !prevPawn->isMoved())
// 			{
// 				for (int i=currentPawn->getX()/SQlength-dx;i<x;i-=dx)
// 				{
// 					currentSquare=SBoard[i][y];
// 					if(validateMove(currentPlayer))
// 						return false;
// 				}
// 				Square * rooknewSquare=SBoard[x+2*dx][y];
// 				rooknewSquare->setPawn(rookSquare->getPawn());
// 				rookSquare->setPawn(NULL);
// 				rooknewSquare->getPawn()->setXY(rooknewSquare->getX(),rooknewSquare->getY());
// 			}
// 			else
// 				return false;
// 		}
// 		else
// 			return false;
// 		currentSquare=temp;
// 	}
// 	else 
// 		if (currentPawn->getType()==KING && (dx==-2 || dx==3))
// 			return false;
}