#include "CEditBoxUI.h"
#include "CDialogUI.h"
#include <iostream>

bool CEditBoxUI::s_bHideCaret;

//TODO: make scrolling and choosing a char work accurate
//TDOO: make m_nFirstVisible more accurate
//TODO: find a better and faster way to tell the text width

//-----------------------------------------------------------------------------
// Name : CEditBoxUI(constructor) 
//-----------------------------------------------------------------------------
CEditBoxUI::CEditBoxUI(CDialogUI* pParentDialog, int ID, LPCTSTR strText, int x, int y, int width, int height, CTimer* timer)
	:CControlUI(pParentDialog, ID, x, y, width, height)
{
	m_type = CControlUI::EDITBOX;

	m_Buffer = strText;

	m_nBorder = 5;  // Default border width
	m_nSpacing = 4;  // Default spacing
	m_nVisibleChars = m_Buffer.size();
	m_nFirstVisible = 0;
	m_nBackwardChars = 0;

	m_bCaretOn = true;
	m_dfBlink = GetCaretBlinkTime() * 0.001f;
	m_dfLastBlink = timer->getCurrentTime();
	s_bHideCaret = false;

	m_TextColor = D3DCOLOR_ARGB( 255, 16, 16, 16 );
	m_SelTextColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_SelBkColor = D3DCOLOR_ARGB( 255, 40, 50, 92 );
	m_CaretColor = D3DCOLOR_ARGB( 255, 0, 0, 0 );
	m_nCaret = m_nSelStart = 0;
	m_bInsertMode = true;

	m_bMouseDrag = false;

	m_assetManger = false;
}

//-----------------------------------------------------------------------------
// Name : CEditBoxUI(constructor from InputFile)
//-----------------------------------------------------------------------------
CEditBoxUI::CEditBoxUI(std::istream& inputFile, CTimer* timer)
	:CControlUI(inputFile)
{
	m_type = CControlUI::EDITBOX;

	m_dfBlink = GetCaretBlinkTime() * 0.001f;
	m_dfLastBlink = timer->getCurrentTime();
	s_bHideCaret = false;
	m_nCaret = m_nSelStart = 0;
	m_bInsertMode = true;
	m_nFirstVisible = 0;
	m_nBackwardChars = 0;

	m_bMouseDrag = false;

	std::string bufferText;
	std::getline(inputFile, bufferText);
	bufferText = bufferText.substr(0, bufferText.find('|') );
	SetText(bufferText.c_str() );
	m_nVisibleChars = m_Buffer.size();
	inputFile >> m_nBorder;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nSpacing;
	SetSpacing(m_nSpacing);
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_bCaretOn;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_TextColor;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_SelTextColor;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_SelBkColor;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_CaretColor;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

	m_assetManger = false;

}

//-----------------------------------------------------------------------------
// Name : CEditBoxUI(destructor) 
//-----------------------------------------------------------------------------
CEditBoxUI::~CEditBoxUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : HandleKeyboard() 
//-----------------------------------------------------------------------------
bool CEditBoxUI::HandleKeyboard( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	bool bHandled = false;

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_TAB:
				// We don't process Tab in case keyboard input is enabled and the user
				// wishes to Tab to other controls.
				break;

			case VK_HOME:
				PlaceCaret( 0 );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_END:
				PlaceCaret( m_Buffer.size() );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_INSERT:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control Insert. Copy to clipboard
					CopyToClipboard();
				}
				else if( GetKeyState( VK_SHIFT ) < 0 )
				{
					// Shift Insert. Paste from clipboard
					PasteFromClipboard();
				}
				else
				{
					// Toggle caret insert mode
					m_bInsertMode = !m_bInsertMode;
				}
				break;

			case VK_DELETE:
				// Check if there is a text selection.
				if( m_nCaret != m_nSelStart )
				{
					DeleteSelectionText();
					m_editboxChangedSig(this);
					//m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
				}
				else
				{
					// Deleting one character
					m_Buffer.erase(m_nCaret,1);
					m_editboxChangedSig(this);
					//if( m_Buffer.RemoveChar( m_nCaret ) )
					//	m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
				}
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_LEFT:
				{
					if( GetKeyState( VK_CONTROL ) < 0 )
					{
						// Control is down. Move the caret to a new item
						// instead of a character.
						m_nCaret--;
						//m_Buffer.GetPriorItemPos( m_nCaret, &m_nCaret );
						if ( m_nCaret < m_nFirstVisible)
						{
							if (m_nFirstVisible > 0 && m_nBackwardChars < m_nFirstVisible)
								m_nBackwardChars++;
							//m_nFirstVisible = m_nCaret;
						}
						PlaceCaret( m_nCaret );
					}
					else if( m_nCaret > 0 )
					{
						if ( m_nCaret - 1 < m_nFirstVisible)
						{
							if (m_nFirstVisible > 0 && m_nBackwardChars < m_nFirstVisible)
								m_nBackwardChars++;
							//m_nFirstVisible = m_nCaret - 1;
						}
						PlaceCaret( m_nCaret - 1 );
					}
					if( GetKeyState( VK_SHIFT ) >= 0 )
						// Shift is not down. Update selection
						// start along with the caret.
						m_nSelStart = m_nCaret;
					ResetCaretBlink();
					bHandled = true;
				}
				break;

			case VK_RIGHT:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control is down. Move the caret to a new item
					// instead of a character.
					m_nCaret++;

					if ( (m_nCaret - (m_nFirstVisible - m_nBackwardChars) ) < m_nVisibleChars )
					{
						if (m_nBackwardChars > 0)
							m_nBackwardChars--;
						//m_nCaret = m_nVisibleChars + m_nFirstVisible;
					}
					//m_Buffer.GetNextItemPos( m_nCaret, &m_nCaret );
					PlaceCaret( m_nCaret );
				}
				else if( m_nCaret < m_Buffer.size() )
				{
					if ( (m_nCaret + 1 - ( m_nFirstVisible - m_nBackwardChars ) ) > m_nVisibleChars )
					{
						if (m_nBackwardChars > 0)
							m_nBackwardChars--;
						//m_nCaret = m_nVisibleChars + m_nFirstVisible;
						PlaceCaret(m_nCaret + 1);
					}
					else
						PlaceCaret( m_nCaret + 1 );
				}
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_UP:
			case VK_DOWN:
				// Trap up and down arrows so that the dialog
				// does not switch focus to another control.
				bHandled = true;
				break;

			default:
				bHandled = wParam != VK_ESCAPE;  // Let the application handle Esc.
			}
		}
	}
	return bHandled;
}

//-----------------------------------------------------------------------------
// Name : HandleMouse() 
//-----------------------------------------------------------------------------
bool CEditBoxUI::HandleMouse( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if ( Pressed(hWnd, mousePoint, inputstate, timer) )
				return true;
		}break;

	case WM_LBUTTONUP:
		{
			if ( Released(hWnd, mousePoint) )
				return true;
		}break;

	case WM_MOUSEMOVE:
		{
			if ( Dragged(mousePoint) )
				return true;
		}break;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed() 
//-----------------------------------------------------------------------------
bool CEditBoxUI::Pressed( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer)
{
	if( !ContainsPoint( pt ) )
		return false;

	if( !m_bHasFocus )
		m_pParentDialog->RequestFocus( this );

	m_bMouseDrag = true;
	SetCapture( hWnd );
	// Determine the character corresponding to the coordinates.
 	int nCP, nTrail, nX1st;
// 	nCP = (pt.x - m_rcText.left) / m_elementsFonts[0].nFontWidth;
// 
// 	RECT rcFullText = {0, 0, 0, 0};
// 	RECT rcCursorText = {0, 0, 0, 0};
// 
// 	m_assetManger->getFontPtr(2)->DrawTextA(m_assetManger->getSprite(), m_Buffer.c_str(), -1
// 		, &rcFullText, DT_CALCRECT, d3d::WHITE);
// 
// 	int firstVisibleChar = ( rcFullText.right - (m_rcText.right - m_rcText.left) ) / m_elementsFonts[0].nFontWidth;
// 	nCP += m_nFirstVisible;
// 
// 	for (nCP = m_nFirstVisible; nCP < m_Buffer.size() ; nCP++)
// 	{
// 		std::string curCursorText = m_Buffer.substr(0, nCP);
// 		m_assetManger->getFontPtr(2)->DrawTextA(m_assetManger->getSprite(), curCursorText.c_str(), -1
// 			, &rcCursorText, DT_CALCRECT, d3d::WHITE);
// 
// 		if (m_nFirstVisible != 0 )
// 		{
// 			int rtExtra = rcFullText.right - rcCursorText.right;
// 			rcCursorText.right = (m_rcText.right - m_rcText.left) - rtExtra;
// 		}
// 
// 		if (rcCursorText.right < (pt.x - m_rcText.left) )
// 			continue;
// 		else 
// 			break;
// 	}
// 
// 	if ( ( pt.x - m_rcText.left ) < rcCursorText.right)
// 		nCP--; 

	nCP = CalcCaretPosByPoint(pt);

	if (nCP > m_Buffer.size())
		nCP = m_Buffer.size();

	// Cap at the NULL character.
	if(nCP >= 0 && nCP < (m_Buffer.size() + 1) )
		PlaceCaret( nCP );
	else
		PlaceCaret( 0 );
	m_nSelStart = m_nCaret;
	ResetCaretBlink();

	return true;
}

//-----------------------------------------------------------------------------
// Name : Released() 
//-----------------------------------------------------------------------------
bool CEditBoxUI::Released( HWND hWnd, POINT pt)
{
	ReleaseCapture();
	m_bMouseDrag = false;

	return false;
}

//-----------------------------------------------------------------------------
// Name : Dragged() 
//-----------------------------------------------------------------------------
bool CEditBoxUI::Dragged( POINT pt)
{
	if( m_bMouseDrag )
	{
		// Determine the character corresponding to the coordinates.
		int nCP, nTrail, nX1st;
		//nCP = (pt.x - m_rcText.left) / m_elementsFonts[0].nFontWidth;
		nCP = CalcCaretPosByPoint(pt);

		// Cap at the NULL character.
		if(nCP >= 0 && nCP < m_Buffer.size() )
		{

			if (m_nCaret < m_nFirstVisible)
				if (m_nFirstVisible > 0 && m_nBackwardChars < m_nFirstVisible)
				{
					m_nBackwardChars += m_nFirstVisible - m_nCaret;

					if (m_nBackwardChars > m_nFirstVisible)
						m_nBackwardChars = m_nFirstVisible;
				}

			if ( (m_nCaret + 1 - ( m_nFirstVisible - m_nBackwardChars ) ) > m_nVisibleChars )
				if (m_nBackwardChars > 0)
				{
					m_nBackwardChars -= m_nCaret - m_nVisibleChars;

					if (m_nBackwardChars < 0)
						m_nBackwardChars = 0;
				}
			
			PlaceCaret( nCP );

			if ( nCP > m_nCaret)
				CalcFirstVisibleCharUp();
			
			if ( nCP < m_nCaret)
				CalcFirstVisibleCharDown();
		}
		else
		{
			if (nCP < 0 || m_Buffer.size() == 0)
			{
				nCP = 0;
				//m_nFirstVisible = 0;
				//m_nBackwardChars = 0;
			}

			if (nCP > m_Buffer.size())
			{
				nCP = m_Buffer.size();
				//m_nFirstVisible = CalcFirstVisibleCharUp();
				m_nBackwardChars = 0;
			}

			PlaceCaret( nCP );

			if ( nCP > m_nCaret)
				CalcFirstVisibleCharUp();

			if ( nCP < m_nCaret)
				CalcFirstVisibleCharDown();
		}
		return true;

	}
	return false;
	//TODO: check return value
}

//-----------------------------------------------------------------------------
// Name : connectToEditboxChg() 
//-----------------------------------------------------------------------------
void CEditBoxUI::connectToEditboxChg(const signal_editbox::slot_type& subscriber)
{
	m_editboxChangedSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : MsgProc() 
//-----------------------------------------------------------------------------
bool CEditBoxUI::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	switch( uMsg )
	{
		// Make sure that while editing, the keyup and keydown messages associated with 
		// WM_CHAR messages don't go to any non-focused controls or cameras
		// TODO: Fix the problem that this case flashes the delete button messages..
		// possible solution is to just put the delete thingy here instead in Handlekeyboard
	case WM_KEYUP:
	case WM_KEYDOWN:
		return false;

	case WM_CHAR:
		{
			switch( ( char )wParam )
			{
				// Backspace
			case VK_BACK:
				{
					// If there's a selection, treat this
					// like a delete key.
					if( m_nCaret != m_nSelStart )
					{
						DeleteSelectionText();
						m_editboxChangedSig(this);
						//m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
					}
					else if( m_nCaret > 0 )
					{
						// Move the caret, then delete the char.
						PlaceCaret( m_nCaret - 1 );
						m_nSelStart = m_nCaret;
						m_Buffer.erase(m_nCaret,1);

						if (m_nFirstVisible > 0)
							CalcFirstVisibleCharDown();

						m_nVisibleChars = m_Buffer.size() - m_nFirstVisible;

						m_editboxChangedSig(this);
						//m_Buffer.RemoveChar( m_nCaret );
						//m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
					}
					ResetCaretBlink();
					break;
				}

			case 24:        // Ctrl-X Cut
			case VK_CANCEL: // Ctrl-C Copy
				{
					CopyToClipboard();

					// If the key is Ctrl-X, delete the selection too.
					if( ( char )wParam == 24 )
					{
						DeleteSelectionText();
						m_editboxChangedSig(this);
						//m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
					}

					break;
				}

				// Ctrl-V Paste
			case 22:
				{
					PasteFromClipboard();
					m_editboxChangedSig(this);
					//m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
					break;
				}

				// Ctrl-A Select All
			case 1:
				if( m_nSelStart == m_nCaret )
				{
					m_nSelStart = 0;
					PlaceCaret( m_Buffer.size() );
				}
				break;

			case VK_RETURN:
				// Invoke the callback when the user presses Enter.
				//m_pDialog->SendEvent( EVENT_EDITBOX_STRING, true, this );
				break;

				// Junk characters we don't want in the string
			case 26:  // Ctrl Z
			case 2:   // Ctrl B
			case 14:  // Ctrl N
			case 19:  // Ctrl S
			case 4:   // Ctrl D
			case 6:   // Ctrl F
			case 7:   // Ctrl G
			case 10:  // Ctrl J
			case 11:  // Ctrl K
			case 12:  // Ctrl L
			case 17:  // Ctrl Q
			case 23:  // Ctrl W
			case 5:   // Ctrl E
			case 18:  // Ctrl R
			case 20:  // Ctrl T
			case 25:  // Ctrl Y
			case 21:  // Ctrl U
			case 9:   // Ctrl I
			case 15:  // Ctrl O
			case 16:  // Ctrl P
			case 27:  // Ctrl [
			case 29:  // Ctrl ]
			case 28:  // Ctrl \ 
				break;

			default:
				{
					// If there's a selection and the user
					// starts to type, the selection should
					// be deleted.
					if( m_nCaret != m_nSelStart )
						DeleteSelectionText();

					// If we are in overwrite mode and there is already
					// a char at the caret's position, simply replace it.
					// Otherwise, we insert the char as normal.
					if( !m_bInsertMode && m_nCaret < m_Buffer.size() )
					{
						m_Buffer[m_nCaret] = ( char )wParam;
						PlaceCaret( m_nCaret + 1 );
						m_nSelStart = m_nCaret;

						LPD3DXFONT pFont = m_assetManger->getFontPtr(2);
						RECT rt = {0,0,0,0};
						

						//if (visibleText.size() > 0 && visibleText[visibleText.size() - 1] == ' ')
						//	visibleText[visibleText.size() - 1] = ';';

/*						int textEdge =  m_rcText.left + rt.right;*/

						CalcFirstVisibleCharUp();

// 						std::string visibleText = m_Buffer.substr(m_nFirstVisible, m_Buffer.size() - m_nFirstVisible);
// 						m_nVisibleChars = visibleText.size();
// 
// 						pFont->DrawTextA(m_assetManger->getSprite(), visibleText.c_str(), -1, &rt, DT_CALCRECT, d3d::WHITE);
					}
					else
					{
						// Insert the char
						m_Buffer.insert(m_nCaret, 1, (char)wParam);
						//if( m_Buffer.InsertChar( m_nCaret, ( char )wParam ) )
						//{
							PlaceCaret( m_nCaret + 1 );
							m_nSelStart = m_nCaret;
						//}
							LPD3DXFONT pFont = m_assetManger->getFontPtr(2);
							RECT rt = {0,0,0,0};

							CalcFirstVisibleCharUp();

// 							std::string visibleText = m_Buffer.substr(m_nFirstVisible, m_Buffer.size() - m_nFirstVisible);
// 							m_nVisibleChars = visibleText.size();
// 
// 							if (visibleText.size() > 0 && visibleText[visibleText.size() - 1] == ' ')
// 								visibleText[visibleText.size() - 1] = ';';
// 
// 							pFont->DrawTextA(m_assetManger->getSprite(), visibleText.c_str(), -1, &rt, DT_CALCRECT, d3d::WHITE);
// 
// 							int textEdge =  m_rcText.left + rt.right;

					}
					ResetCaretBlink();
					m_editboxChangedSig(this);
					//m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
				}
			}
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : UpdateRects() 
//-----------------------------------------------------------------------------
void CEditBoxUI::UpdateRects()
{
	CControlUI::UpdateRects();

	// Update the text rectangle
	m_rcText = m_rcBoundingBox;
	// First inflate by m_nBorder to compute render rects
	InflateRect( &m_rcText, -m_nBorder, -m_nBorder );

	// Update the render rectangles
	m_rcRender[0] = m_rcText;
	SetRect( &m_rcRender[1], m_rcBoundingBox.left, m_rcBoundingBox.top, m_rcText.left, m_rcText.top );
	SetRect( &m_rcRender[2], m_rcText.left, m_rcBoundingBox.top, m_rcText.right, m_rcText.top );
	SetRect( &m_rcRender[3], m_rcText.right, m_rcBoundingBox.top, m_rcBoundingBox.right, m_rcText.top );
	SetRect( &m_rcRender[4], m_rcBoundingBox.left, m_rcText.top, m_rcText.left, m_rcText.bottom );
	SetRect( &m_rcRender[5], m_rcText.right, m_rcText.top, m_rcBoundingBox.right, m_rcText.bottom );
	SetRect( &m_rcRender[6], m_rcBoundingBox.left, m_rcText.bottom, m_rcText.left, m_rcBoundingBox.bottom );
	SetRect( &m_rcRender[7], m_rcText.left, m_rcText.bottom, m_rcText.right, m_rcBoundingBox.bottom );
	SetRect( &m_rcRender[8], m_rcText.right, m_rcText.bottom, m_rcBoundingBox.right, m_rcBoundingBox.bottom );

	// Inflate further by m_nSpacing
	InflateRect( &m_rcText, -m_nSpacing, -m_nSpacing );
}

//-----------------------------------------------------------------------------
// Name : Render() 
//-----------------------------------------------------------------------------
void CEditBoxUI::Render( CAssetManager& assetManger )
{
	if( m_bVisible == false )
		return;

	if (m_assetManger == NULL)
		m_assetManger = &assetManger;

	//std::cout << "m_nBackwardChars = " << m_nBackwardChars << std::endl;
	//std::cout << "m_nCaret : " << m_nCaret << std::endl;

	HRESULT hr;
	LPDIRECT3DTEXTURE9 pTexture;
	// acquire a pointer to the sprite to draw to
	CMySprite* sprite = assetManger.getMySprite();

	// acquire the pointer to the font of the editbox text
	LPD3DXFONT pFont = assetManger.getFontPtr(m_elementsFonts[0].fontIndex);

	POINT dialogPos =  m_pParentDialog->getLocation();
	LONG  dialogCaptionHeihgt =  m_pParentDialog->getCaptionHeight();
	dialogPos.y += dialogCaptionHeihgt;

	int nSelStartX = 0, nCaretX = 0;  // Left and right X cordinates of the selection region

	// Render the control graphics
	for(UINT i = 0; i < m_elementsGFX.size(); ++i )
	{
		pTexture = assetManger.getTexturePtr(m_elementsGFX[i].iTexture);
		renderRect(m_elementsGFX[i].rcTexture, m_rcRender[i], sprite, pTexture, d3d::WHITE, REGLUAR, dialogPos);
		//renderRect(m_elementsGFX[i].rcTexture, m_rcRender[i], sprite, NULL, d3d::BLACK, false);
	}

	//
	// Compute the X coordinates of the first visible character.
	//
	FONT_ITEM fontItem = assetManger.getFontItem(m_elementsFonts[0].fontIndex);

	int nXFirst = m_rcText.left + fontItem.width;
	// compute the x coordinates of the caret
	nCaretX = m_rcText.left + fontItem.width + m_nCaret * fontItem.width;

	if (nCaretX > m_rcText.right)
		      ;//m_nFirstVisible = (nCaretX - m_rcText.right) / fontItem.avgWidth;

	nCaretX = m_rcText.right - fontItem.weight;

	//
	// Compute the X coordinates of the selection rectangle
	//

	if (m_nCaret != m_nSelStart)
		nSelStartX = nXFirst + m_nSelStart * fontItem.width;
	else
		nSelStartX = nCaretX;

	//
	// Render the selection rectangle
	//
	RECT rcSelection;  // Make this available for rendering selected text
	if( m_nCaret != m_nSelStart )
	{
		int nSelLeftX = nCaretX, nSelRightX = nSelStartX;
		// Swap if left is bigger than right
		if( nSelLeftX > nSelRightX )
		{
			int nTemp = nSelLeftX;
			nSelLeftX = nSelRightX;
			nSelRightX = nTemp;
		}
		
		int nFirstToRender = __max( m_nFirstVisible - m_nBackwardChars, __min( m_nSelStart, m_nCaret ) );
		int nNumChatToRender = __max( m_nSelStart, m_nCaret ) - nFirstToRender;

		std::string temp = m_Buffer.substr( nFirstToRender, nNumChatToRender /*+ m_nBackwardChars*/);
		if (temp.size() > 0 && temp[0] == ' ')
			temp[0] = ';';
		if (temp.size() > 0 && temp[temp.size() -1] == ' ')
			temp [temp.size() - 1] = ';';
		
		RECT rcSelectionBound = {0, 0, 0, 0};
		RECT rcSelectX = {0, 0, 0, 0};
		pFont->DrawTextA(assetManger.getSprite(), temp.c_str(), -1, &rcSelectionBound, DT_CALCRECT, d3d::WHITE);

		temp = m_Buffer.substr(0, nFirstToRender);
		if (temp.size() > 0 && temp[0] == ' ')
			temp[0] = ';';
		if (temp.size() > 0 && temp[temp.size() -1] == ' ')
			temp [temp.size() - 1] = ';';

		pFont->DrawTextA(assetManger.getSprite(), temp.c_str(), -1, &rcSelectX, DT_CALCRECT, d3d::WHITE);

		SetRect( &rcSelection, nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom );
		OffsetRect( &rcSelection, m_rcText.left - nXFirst, 0 );
		IntersectRect( &rcSelection, &m_rcText, &rcSelection );

		RECT rtFullText = {0, 0, 0, 0};
		pFont->DrawTextA(assetManger.getSprite(), m_Buffer.c_str(), -1, &rtFullText, DT_CALCRECT, d3d::WHITE);
 
 		if (m_nFirstVisible - m_nBackwardChars != 0 )
 		{
 			int rtExtra = rtFullText.right - rcSelectX.right;
 			rcSelectX.right = (m_rcText.right - m_rcText.left) - rtExtra;
 		}

		nSelLeftX = m_rcText.left + rcSelectX.right + rcSelectionBound.left;
		nSelRightX = m_rcText.left + rcSelectX.right + rcSelectionBound.right;
		SetRect( &rcSelection, nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom );

// 		IDirect3DDevice9* pd3dDevice = m_pDialog->GetManager()->GetD3D9Device();
// 		if( pd3dDevice )
// 			pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

		RECT rc;
		renderRect(rc, rcSelection, sprite, NULL, m_SelBkColor, TOP, dialogPos);

// 		m_pDialog->DrawRect( &rcSelection, m_SelBkColor );
// 		if( pd3dDevice )
// 			pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	}

	//
	// Render the text
	//
	// Element 0 for text
	RECT rt = {0, 0, 0, 0};
	RECT rtFullText = {0, 0, 0, 0};
	//std::string temp = m_Buffer.substr(m_nFirstVisible, m_nCaret);
	std::string temp = m_Buffer.substr(0, m_nCaret);
	std::string fullText = m_Buffer.substr(0, m_Buffer.size() - m_nBackwardChars);
	//std::string temp = m_Buffer.substr(m_nFirstVisible, m_Buffer.size() - m_nFirstVisible);

	std::string textToRender;

	if (m_nBackwardChars == 0)
		textToRender = m_Buffer.substr(m_nFirstVisible, m_Buffer.size() - m_nFirstVisible );
	else
		textToRender = m_Buffer.substr(m_nFirstVisible - m_nBackwardChars, m_Buffer.size() - (m_nFirstVisible /*- m_nBackwardChars * 2*/) );

// 	if (m_nFirstVisible > 0)
// 		RenderText(temp.c_str(), m_rcText, pFont,  DT_LEFT | DT_VCENTER, assetManger.getSprite(), m_TextColor, dialogPos);
// 	else
// 		RenderText(m_Buffer.c_str(), m_rcText, pFont,  DT_LEFT | DT_VCENTER, assetManger.getSprite(), m_TextColor, dialogPos);

	if (m_nFirstVisible - m_nBackwardChars == 0)
		RenderText(textToRender.c_str(), m_rcText, pFont,  DT_LEFT | DT_VCENTER , assetManger.getSprite(), m_TextColor, dialogPos);
	else
		RenderText(textToRender.c_str(), m_rcText, pFont,  DT_RIGHT | DT_VCENTER , assetManger.getSprite(), m_TextColor, dialogPos);

	if (temp.size() > 0 && temp[temp.size() - 1] == ' ')
		temp[temp.size() - 1] = ';';

	pFont->DrawTextA(assetManger.getSprite(), temp.c_str(), -1, &rt, DT_CALCRECT, d3d::WHITE);
	//pFont->DrawTextA(assetManger.getSprite(), m_Buffer.c_str(), -1, &rtFullText, DT_CALCRECT, d3d::WHITE);
	pFont->DrawTextA(assetManger.getSprite(), fullText.c_str(), -1, &rtFullText, DT_CALCRECT, d3d::WHITE);

	if (m_nFirstVisible - m_nBackwardChars != 0)
	{
		int rtExtra = rtFullText.right - rt.right;
		rt.right = (m_rcText.right - m_rcText.left) - rtExtra;
	}

	// Render the selected text
	if( m_nCaret != m_nSelStart )
	{
		//int nFirstToRender = __max( m_nFirstVisible, __min( m_nSelStart, m_nCaret ) );
		int nFirstToRender = __min(m_nSelStart, m_nCaret);
		int nNumChatToRender = __max( m_nSelStart, m_nCaret ) - nFirstToRender;

		temp = m_Buffer.substr( nFirstToRender, nNumChatToRender  + m_nBackwardChars);
		RenderText(temp.c_str(), rcSelection, pFont,  DT_LEFT | DT_VCENTER, assetManger.getTopSprite(), m_SelTextColor, dialogPos);
	}

	//
	// Blink the caret
	//
	if( assetManger.getTimer()->getCurrentTime() - m_dfLastBlink >= m_dfBlink )
	{
		m_bCaretOn = !m_bCaretOn;
		m_dfLastBlink = assetManger.getTimer()->getCurrentTime();
	}

	//
	// Render the caret if this control has the focus
	//
	if( m_bHasFocus && m_bCaretOn && !s_bHideCaret )
	{
		// Start the rectangle with insert mode caret
		RECT rcCaret = { m_rcText.left + rt.right - 1, m_rcText.top,
			m_rcText.left + rt.right + 1, m_rcText.bottom };

// 		RECT rcCaret = { m_rcText.left - nXFirst + nCaretX - 1, m_rcText.top,
// 			m_rcText.left - nXFirst + nCaretX + 1, m_rcText.bottom };

		// If we are in overwrite mode, adjust the caret rectangle
		// to fill the entire character.
		if( !m_bInsertMode )
		{
			// Obtain the right edge X coord of the current character
			int nRightEdgeX;
			nRightEdgeX = nXFirst + fontItem.width * ( m_nCaret + 1 );
			//m_Buffer.CPtoX( m_nCaret, TRUE, &nRightEdgeX );
			rcCaret.right = m_rcText.left - nXFirst + nRightEdgeX;
		}

		RECT rc;
		//SetRect(&rcCaret, 133, 84, 135, 98);
		//OffsetRect(&rcCaret, 0, 40);
		//SetRect(&rcCaret, 0, 0, 50, 50);
		renderRect(rc, rcCaret, sprite, NULL, m_CaretColor, TOP, dialogPos);
	}
}

//-----------------------------------------------------------------------------
// Name : SetText() 
//-----------------------------------------------------------------------------
void CEditBoxUI::SetText( LPCTSTR strText, bool bSelected /* = false */ )
{
	assert( strText != NULL );

	m_Buffer = strText;
	m_nFirstVisible = 0;
	m_nBackwardChars = 0;
	m_nVisibleChars = m_Buffer.size();
	// Move the caret to the end of the text
	PlaceCaret( m_Buffer.size() );

	if (bSelected)
		m_nSelStart = 0;
	else
		m_nSelStart = m_nCaret;
}

//-----------------------------------------------------------------------------
// Name : GetText() 
//-----------------------------------------------------------------------------
LPCTSTR CEditBoxUI::GetText()
{
	return m_Buffer.c_str();
}

//-----------------------------------------------------------------------------
// Name : GetTextLength() 
//-----------------------------------------------------------------------------
int CEditBoxUI::GetTextLength()
{
	return m_Buffer.size();
}

//-----------------------------------------------------------------------------
// Name : GetTextCopy() 
//-----------------------------------------------------------------------------
HRESULT CEditBoxUI::GetTextCopy(LPSTR strDest, UINT bufferCount )
{
	assert( strDest );

	strcpy_s( strDest, bufferCount, m_Buffer.c_str() );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : ClearText() 
//-----------------------------------------------------------------------------
void CEditBoxUI::ClearText()
{
	m_Buffer.clear();
	m_nFirstVisible = 0;
	m_nBackwardChars = 0;
	m_nVisibleChars = m_Buffer.size();
	PlaceCaret( 0 );
	m_nSelStart = 0;
}

//-----------------------------------------------------------------------------
// Name : SetTextColor() 
//-----------------------------------------------------------------------------
void CEditBoxUI::SetTextColor( D3DCOLOR Color )
{
	m_TextColor = Color;
}

//-----------------------------------------------------------------------------
// Name : SetSelectedTextColor() 
//-----------------------------------------------------------------------------
void CEditBoxUI::SetSelectedTextColor( D3DCOLOR Color )
{
	m_SelTextColor = Color;
}

//-----------------------------------------------------------------------------
// Name : SetSelectedBackColor() 
//-----------------------------------------------------------------------------
void CEditBoxUI::SetSelectedBackColor( D3DCOLOR Color )
{
	m_SelBkColor = Color;
}

//-----------------------------------------------------------------------------
// Name : SetCaretColor() 
//-----------------------------------------------------------------------------
void CEditBoxUI::SetCaretColor( D3DCOLOR Color )
{
	m_CaretColor = Color;
}

//-----------------------------------------------------------------------------
// Name : SetBorderWidth() 
//-----------------------------------------------------------------------------
void CEditBoxUI::SetBorderWidth( int nBorder )
{
	m_nBorder = nBorder;
}

//-----------------------------------------------------------------------------
// Name : SetSpacing() 
//-----------------------------------------------------------------------------
void CEditBoxUI::SetSpacing( int nSpacing )
{
	m_nSpacing = nSpacing;
	UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : ParseFloatArray() 
//-----------------------------------------------------------------------------
// void CEditBoxUI::ParseFloatArray( float* pNumbers, int nCount )
// {
// 	int nWritten = 0;  // Number of floats written
// 	const char* pToken, *pEnd;
// 	char strToken[60];
// 
// 	pToken = m_Buffer.GetBuffer();
// 	while( nWritten < nCount && *pToken != L'\0' )
// 	{
// 		// Skip leading spaces
// 		while( *pToken == L' ' )
// 			++pToken;
// 
// 		if( *pToken == L'\0' )
// 			break;
// 
// 		// Locate the end of number
// 		pEnd = pToken;
// 		//TODO: replace this crap with something that is logical and elegant 
// 		while( IN_FLOAT_CHARSET( *pEnd ) )
// 			++pEnd;
// 
// 		// Copy the token to our buffer
// 		int nTokenLen = __min( sizeof( wszToken ) / sizeof( wszToken[0] ) - 1, int( pEnd - pToken ) );
// 		strcpy_s( wszToken, nTokenLen, pToken );
// 		*pNumbers = ( float )strtod( wszToken, NULL );
// 		++nWritten;
// 		++pNumbers;
// 		pToken = pEnd;
// 	}
//}

//-----------------------------------------------------------------------------
// Name : SetTextFloatArray() 
//-----------------------------------------------------------------------------
void CEditBoxUI::SetTextFloatArray( const float* pNumbers, int nCount )
{
	char strBuffer[512] = {0};
	char strTmp[64];

	if( pNumbers == NULL )
		return;

	for( int i = 0; i < nCount; ++i )
	{
		sprintf_s( strTmp, 64, "%.4f ", pNumbers[i] );
		strcat_s( strBuffer, 512, strTmp );
	}

	// Don't want the last space
	if( nCount > 0 && strlen( strBuffer ) > 0 )
		strBuffer[strlen( strBuffer ) - 1] = 0;

	SetText( strBuffer );
}

//-----------------------------------------------------------------------------
// Name : CalcCaretPosByPoint() 
//-----------------------------------------------------------------------------
int CEditBoxUI::CalcCaretPosByPoint( POINT pt )
{
	// Determine the character corresponding to the coordinates.
	int nCP;

	RECT rcFullText = {0, 0, 0, 0};
	RECT rcCursorText = {0, 0, 0, 0};

	m_assetManger->getFontPtr(2)->DrawTextA(m_assetManger->getSprite(), m_Buffer.c_str(), -1
		, &rcFullText, DT_CALCRECT, d3d::WHITE);

	for (nCP = m_nFirstVisible - m_nBackwardChars; nCP <= m_Buffer.size() ; nCP++)
	{
		std  ::string curCursorText = m_Buffer.substr(0, nCP);
		m_assetManger->getFontPtr(2)->DrawTextA(m_assetManger->getSprite(), curCursorText.c_str(), -1
			, &rcCursorText, DT_CALCRECT, d3d::WHITE);

		if (m_nFirstVisible - m_nBackwardChars != 0 )
		{
			int rtExtra = rcFullText.right - rcCursorText.right;
			rcCursorText.right = (m_rcText.right - m_rcText.left) - rtExtra;
		}

		if (rcCursorText.right < (pt.x - m_rcText.left) )
			continue;
		else 
			break;
	}

	if ( ( pt.x - m_rcText.left ) < rcCursorText.right)
		nCP--; 

	return nCP;
}

//-----------------------------------------------------------------------------
// Name : PlaceCaret() 
//-----------------------------------------------------------------------------
void CEditBoxUI::PlaceCaret( int nCP )
{
	assert( nCP >= 0 && nCP <= m_Buffer.size() );
// 	if ( nCP < m_nFirstVisible)
// 		m_nFirstVisible = nCP;
// 	if ( (nCP - m_nFirstVisible) < m_nVisibleChars )
// 		nCP = m_nVisibleChars + m_nFirstVisible;

	m_nCaret = nCP;
}

//-----------------------------------------------------------------------------
// Name : DeleteSelectionText() 
//-----------------------------------------------------------------------------
void CEditBoxUI::DeleteSelectionText()
{
	int nFirst = __min( m_nCaret, m_nSelStart );
	int nLast = __max( m_nCaret, m_nSelStart );
	// Update caret and selection
	PlaceCaret( nFirst );
	m_nSelStart = m_nCaret;
	CalcFirstVisibleCharDown();
	m_nVisibleChars = m_Buffer.size() - m_nFirstVisible;

	// Remove the characters
	m_Buffer.erase(nFirst, nLast - nFirst);
	//for( int i = nFirst; i < nLast; ++i )
	//	m_Buffer.RemoveChar( nFirst );
}

//-----------------------------------------------------------------------------
// Name : ResetCaretBlink() 
//-----------------------------------------------------------------------------
void CEditBoxUI::ResetCaretBlink()
{
	//TODO: try to use the timer here instead of this ugly thing
	__int64 currTime;
	__int64 prefFreq;
	float   timeScale;

	m_bCaretOn = true;

	QueryPerformanceCounter((LARGE_INTEGER *)&currTime);
	QueryPerformanceFrequency((LARGE_INTEGER *)&prefFreq);

	timeScale			= 1.0f / prefFreq;

	m_dfLastBlink = currTime * timeScale;
}

//-----------------------------------------------------------------------------
// Name : CopyToClipboard() 
//-----------------------------------------------------------------------------
void CEditBoxUI::CopyToClipboard()
{
	// Copy the selection text to the clipboard
	if( m_nCaret != m_nSelStart && OpenClipboard( NULL ) )
	{
		EmptyClipboard();

		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, sizeof( char ) * ( m_Buffer.size() + 1 ) );
		if( hBlock )
		{
			char* pwszText = ( char* )GlobalLock( hBlock );
			if( pwszText )
			{
				int nFirst = __min( m_nCaret, m_nSelStart );
				int nLast = __max( m_nCaret, m_nSelStart );
				if( nLast - nFirst > 0 )
					CopyMemory( pwszText, m_Buffer.c_str() + nFirst, ( nLast - nFirst ) * sizeof( char ) );
				pwszText[nLast - nFirst] = L'\0';  // Terminate it
				GlobalUnlock( hBlock );
			}
			SetClipboardData( CF_UNICODETEXT, hBlock );
		}
		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if( hBlock )
			GlobalFree( hBlock );
	}
}

//-----------------------------------------------------------------------------
// Name : PasteFromClipboard() 
//-----------------------------------------------------------------------------
void CEditBoxUI::PasteFromClipboard()
{
	DeleteSelectionText();

	if( OpenClipboard( NULL ) )
	{
		HANDLE handle = GetClipboardData( CF_UNICODETEXT );
		if( handle )
		{
			// Convert the ANSI string to Unicode, then
			// insert to our buffer.
			char* pStrText = ( char* )GlobalLock( handle );
			if( pStrText )
			{
				// Copy all characters up to null.
				m_Buffer.insert(m_nCaret, pStrText);
				//if( m_Buffer.InsertString( m_nCaret, pStrText ) )
				PlaceCaret( m_nCaret + lstrlen( pStrText ) );
				m_nSelStart = m_nCaret;
				GlobalUnlock( handle );
			}
		}
		CloseClipboard();
	}
}

//-----------------------------------------------------------------------------
// Name : CanHaveFocus() 
//-----------------------------------------------------------------------------
bool CEditBoxUI::CanHaveFocus()
{
	//return true;
	return ( m_bVisible && m_bEnabled );
}

//-----------------------------------------------------------------------------
// Name : OnFocusIn 
//-----------------------------------------------------------------------------
void CEditBoxUI::OnFocusIn()
{
	CControlUI::OnFocusIn();

	ResetCaretBlink();
}

//-----------------------------------------------------------------------------
// Name : OnFocusOut 
//-----------------------------------------------------------------------------
void CEditBoxUI::OnFocusOut()
{
	CControlUI::OnFocusOut();

	m_nSelStart = m_nCaret;
}

//-----------------------------------------------------------------------------
// Name : SaveToFile 
//-----------------------------------------------------------------------------
bool CEditBoxUI::SaveToFile(std::ostream& SaveFile)
{
	CControlUI::SaveToFile(SaveFile);
	
	//TODO: make all the editbox options saveable
	SaveFile << m_Buffer << "| EditBox text " << "\n";
	SaveFile << m_nBorder << "| EditBox Border Number" << "\n";
	SaveFile << m_nSpacing << "| EditBox Spacing" << "\n";
	SaveFile << m_bCaretOn << "| is EditBox Caret On" << "\n";
 	SaveFile << m_TextColor << "| EditBox Text Color" << "\n";
 	SaveFile << m_SelTextColor << "| EditBox Selection Color" << "\n";
 	SaveFile << m_SelBkColor << "| EditBox Background Color" << "\n";
 	SaveFile << m_CaretColor << "| EditBox Caret Color" << "\n";

	return true;
}

//-----------------------------------------------------------------------------
// Name : CalcFirstVisibleCharUp 
//-----------------------------------------------------------------------------
int CEditBoxUI::CalcFirstVisibleCharUp()
{
	LPD3DXFONT pFont = m_assetManger->getFontPtr(2);
	RECT rt = {0,0,0,0};

	std::string visibleText = m_Buffer.substr(m_nFirstVisible, m_Buffer.size() - m_nFirstVisible);
	m_nVisibleChars = visibleText.size();

	//if (visibleText.size() > 0 && visibleText[visibleText.size() - 1] == ' ')
	//	visibleText[visibleText.size() - 1] = ';';

	pFont->DrawTextA(m_assetManger->getSprite(), visibleText.c_str(), -1, &rt, DT_CALCRECT, d3d::WHITE);

	int textEdge =  m_rcText.left + rt.right;

	if (textEdge > m_rcText.right )
	{
		m_nFirstVisible++;
		//return m_nFirstVisible;
		CalcFirstVisibleCharUp();
	}
	else
		return m_nFirstVisible;
}

//-----------------------------------------------------------------------------
// Name : CalcFirstVisibleCharDown 
//-----------------------------------------------------------------------------
int CEditBoxUI::CalcFirstVisibleCharDown()
{
	LPD3DXFONT pFont = m_assetManger->getFontPtr(2);
	RECT rt = {0,0,0,0};

	std::string visibleText = m_Buffer.substr(m_nFirstVisible, m_Buffer.size() - m_nFirstVisible);
	m_nVisibleChars = visibleText.size();

	//if (visibleText.size() > 0 && visibleText[visibleText.size() - 1] == ' ')
	//	visibleText[visibleText.size() - 1] = ';';

	pFont->DrawTextA(m_assetManger->getSprite(), visibleText.c_str(), -1, &rt, DT_CALCRECT, d3d::WHITE);

	int textEdge =  m_rcText.left + rt.right;

	if (textEdge < m_rcText.right  && m_nFirstVisible > 0)
	{
		m_nFirstVisible--;
		//return m_nFirstVisible;
		CalcFirstVisibleCharDown();
	}
	else
	{
		if (m_nBackwardChars > m_nFirstVisible)
			m_nBackwardChars = m_nFirstVisible;

		return m_nFirstVisible;
	}
}