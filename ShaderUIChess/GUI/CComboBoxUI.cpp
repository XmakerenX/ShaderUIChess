#include "CComboBoxUI.h"
#include "CDialogUI.h"
#include <iostream>

//-----------------------------------------------------------------------------
// Name : CComboBoxUI(constructor) 
//-----------------------------------------------------------------------------
CComboBoxUI::CComboBoxUI(CDialogUI* pParentDialog, int ID, LPCTSTR strText, int x, int y, UINT width, UINT height, UINT nHotkey)
	:CButtonUI(pParentDialog, ID, strText, x, y, width, height, nHotkey)
{
	m_type = CControlUI::COMBOBOX;

	m_nDropHeight = 100;

	m_nSBWidth = 16;
	m_bOpened = false;
	m_iSelected = -1;
	m_iFocused = -1;
}

//-----------------------------------------------------------------------------
// Name : CComboBoxUI(constructor from InputFile)
//-----------------------------------------------------------------------------
CComboBoxUI::CComboBoxUI(std::istream& inputFile)
	:CButtonUI(inputFile)
{
	UINT itemsSize = 0;;
	m_type = CControlUI::COMBOBOX;

	m_bOpened = false;
	m_iSelected = -1;
	m_iFocused = -1;

	inputFile >> m_nDropHeight;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nSBWidth;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nFontHeight;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> itemsSize;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

	for (UINT i = 0; i < itemsSize; i++)
	{
		std::string strText;

		std::getline(inputFile, strText);
		strText = strText.substr(0, strText.find('|') );
		AddItem(strText.c_str(), nullptr);

		inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	}
}

//-----------------------------------------------------------------------------
// Name : CComboBoxUI(destructor) 
//-----------------------------------------------------------------------------
CComboBoxUI::~CComboBoxUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : onInit() 
// Desc : initialize the scrollbar element
//-----------------------------------------------------------------------------
bool CComboBoxUI::onInit()
{
	return m_pParentDialog->initControl(&m_ScrollBar);
}

//-----------------------------------------------------------------------------
// Name : HandleKeyboard() 
// Desc : Handles keyboard inputs for the Combo box
//-----------------------------------------------------------------------------
bool CComboBoxUI::HandleKeyboard(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return false;
}

//-----------------------------------------------------------------------------
// Name : setHotKey ()
//-----------------------------------------------------------------------------
void CComboBoxUI::OnHotkey()
{
	;
}

//-----------------------------------------------------------------------------
// Name : HandleMouse() 
// Desc : Handles mouse input for the Combo box
//-----------------------------------------------------------------------------
bool CComboBoxUI::HandleMouse( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer )
{
	if (!m_bEnabled || !m_bVisible)
		return false;

	// Let the scroll bar handle it first.
	if( m_ScrollBar.HandleMouse( hWnd,uMsg, mousePoint, inputstate, timer) )
		return true;

	switch(uMsg)
	{
	case WM_MOUSEMOVE:
		{
			if ( Highlight(mousePoint) )
				return true;
		}break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if ( Pressed(hWnd, mousePoint, inputstate, timer) )
				return true;
			// Release focus if appropriate
// 			if( !m_pParentDialog->m_bKeyboardInput )
// 			{
// 				m_pParentDialog->ClearFocus();
// 			}

		}break;

	case WM_LBUTTONUP:
		{
			if ( Released(hWnd, mousePoint) )
				return true;
		}break;

	case WM_MOUSEWHEEL:
		{
			//TODO: pass zdelta or pass the number needed to calc it!
			if ( Scrolled( inputstate.nScrollAmount) )
				return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed() 
//-----------------------------------------------------------------------------
bool CComboBoxUI::Pressed( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer)
{
	if( ContainsPoint( pt ) )
	{
		// Pressed while inside the control
		m_bPressed = true;
		SetCapture( hWnd);

		if( !m_bHasFocus )
			m_pParentDialog->RequestFocus( this );

		// Toggle dropdown
		if( m_bHasFocus )
		{
			m_bOpened = !m_bOpened;

			if( !m_bOpened )
			{
				//TODO: check what this suppose to do ?
				// 						if( !m_pParentDialog->m_bKeyboardInput )
				// 							m_pDialog->ClearFocus();
			}
		}

		return true;
	}

	// Perhaps this click is within the dropdown
	if( m_bOpened && PtInRect( &m_rcDropdown, pt ) )
	{
		// Determine which item has been selected
		for( UINT i = m_ScrollBar.GetTrackPos(); i < m_Items.size(); i++ )
		{
			ComboBoxItem* pItem = m_Items[i];
			if( pItem->bVisible && PtInRect( &pItem->rcActive, pt ) )
			{
				m_iFocused = m_iSelected = i;
				m_selectionChangedSig( this);
				//m_pParentDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, m_ID, hWnd );
				m_bOpened = false;

				m_bMouseOver = false;

				// 						if( !m_pDialog->m_bKeyboardInput )
				// 							m_pDialog->ClearFocus();

				break;
			}
		}

		return true;
	}

	// Mouse click not on main control or in dropdown, fire an event if needed
	if( m_bOpened )
	{
		m_iFocused = m_iSelected;
		m_selectionChangedSig( this );
		//m_pParentDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, m_ID, hWnd);
		m_bOpened = false;
	}

	// Make sure the control is no longer in a pressed state
	m_bPressed = false;

	// Release focus if appropriate
	// 			if( !m_pParentDialog->m_bKeyboardInput )
	// 			{
	// 				m_pParentDialog->ClearFocus();
	// 			}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Released() 
//-----------------------------------------------------------------------------
bool CComboBoxUI::Released( HWND hWnd, POINT pt)
{
	if( m_bPressed && ContainsPoint( pt ) )
	{
		// Button click
		m_bPressed = false;
		ReleaseCapture();
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Scrolled() 
//-----------------------------------------------------------------------------
bool CComboBoxUI::Scrolled( int nScrollAmount)
{
	//int zDelta = ( short )HIWORD( wParam ) / WHEEL_DELTA;
	//TODO: make sure nScrollAmount 
	UINT uLines;
	if( m_bOpened )
	{
		m_ScrollBar.Scroll( -nScrollAmount /** uLines*/ );
	}
	else
	{
		if (m_bMouseOver)
		{
			SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
			if( (nScrollAmount / uLines) > 0 )
			{
				if( m_iFocused > 0 )
				{
					m_iFocused--;
					m_iSelected = m_iFocused;

					if( !m_bOpened )
						m_selectionChangedSig( this );
						//m_pParentDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
				}
			}
			else
			{
				if( m_iFocused + 1 < ( int )GetNumItems() )
				{
					m_iFocused++;
					m_iSelected = m_iFocused;

					if( !m_bOpened )
						m_selectionChangedSig( this );
						//m_pParentDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
				}
			}
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Name : Highlight() 
//-----------------------------------------------------------------------------
bool CComboBoxUI::Highlight(POINT mousePoint)
{
	if( m_bOpened && PtInRect( &m_rcDropdown, mousePoint ) )
	{
		// Determine which item has been selected
		for( UINT i = 0; i < m_Items.size(); i++ )
		{
			ComboBoxItem* pItem = m_Items[i];
			if( pItem->bVisible && PtInRect( &pItem->rcActive, mousePoint ) )
			{
				m_iFocused = i;
			}
		}
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : ConnectToSelectChg() 
//-----------------------------------------------------------------------------
void CComboBoxUI::ConnectToSelectChg( const signal_comboBox::slot_type& subscriber)
{
	m_selectionChangedSig.connect(subscriber);
}


//-----------------------------------------------------------------------------
// Name : Render() 
//-----------------------------------------------------------------------------
void    CComboBoxUI::Render( CAssetManager& assetManger)
{
	if (m_bVisible)
	{
		LPDIRECT3DTEXTURE9 pTexture;

		// check that there is actual fonts 
		//int test = m_elementsFonts.size();
		if (m_elementsFonts.size() == 0)
			return;	// if the function returns here initDefalutElements wasn't called or Elements were not defined

		// acquire the sprite and font to use for rendering
		FONT_ITEM font = assetManger.getFontItem(m_elementsFonts[0].fontIndex);
		CMySprite* sprite = assetManger.getMySprite();

		POINT dialogPos = m_pParentDialog->getLocation();
		LONG  dialogCaptionHeihgt =  m_pParentDialog->getCaptionHeight();
		dialogPos.y += dialogCaptionHeihgt;

		//if Combobox is open render the scrollbar
		if (m_bOpened)
		{
			m_ScrollBar.Render(assetManger);

			// acquire a pointer for the dropdown texture
			pTexture = assetManger.getTexturePtr(m_elementsGFX[DROPDOWN].iTexture);
			renderRect(m_elementsGFX[DROPDOWN].rcTexture, m_rcDropdown, sprite, pTexture, d3d::WHITE, TOP, dialogPos);

		}

		int curY = m_rcDropdownText.top;
		int nRemainingHeight =  m_rcDropdownText.bottom - m_rcDropdownText.top;

		// render all item within the dropdown box;
		for (UINT i = m_ScrollBar.GetTrackPos(); i < m_Items.size(); i++)
		{
			ComboBoxItem* pItem = m_Items[i];

			// Make sure there's room left in the dropdown
			nRemainingHeight -= font.height;
			if( nRemainingHeight < 0 )
			{
				pItem->bVisible = false;
				continue;
			}

			SetRect( &pItem->rcActive, m_rcDropdownText.left, curY, m_rcDropdownText.right, curY + font.height );
			curY += font.height;

			pItem->bVisible = true;

			if( m_bOpened )
			{
				if( ( int )i == m_iFocused )
				{
					RECT rc;

					//acquire a pointer to the texture we need to render the button
					LPDIRECT3DTEXTURE9 pTexture = assetManger.getTexturePtr((m_elementsGFX[SELECTION].iTexture) );

					SetRect( &rc, m_rcDropdown.left, pItem->rcActive.top - 2, m_rcDropdown.right, pItem->rcActive.bottom + 2 );
					renderRect(m_elementsGFX[SELECTION].rcTexture,rc , sprite, pTexture, d3d::WHITE, TOP, dialogPos);
					RenderText(pItem->strText, rc, font.pFont, DT_CENTER | DT_VCENTER,assetManger.getTopSprite(), d3d::WHITE, dialogPos);
					//m_pDialog->DrawSprite( pSelectionElement, &rc, DXUT_NEAR_BUTTON_DEPTH );
					//m_pDialog->DrawText( pItem->strText, pSelectionElement, &pItem->rcActive );
				}
				else
				{
					RenderText(pItem->strText, pItem->rcActive, font.pFont,  DT_CENTER | DT_VCENTER, assetManger.getTopSprite(), d3d::BLACK, dialogPos);
				}
			}
		}
		// end of render dropDownButton

		pTexture = assetManger.getTexturePtr(m_elementsGFX[BUTTON].iTexture);
		LPDIRECT3DTEXTURE9 pTexture2 = assetManger.getTexturePtr(m_elementsGFX[MAIN].iTexture);

		//if the button is not pressed or doesn't have the cursor on it render it normally
		if (!m_bMouseOver && !m_bOpened)
		{
			renderRect(m_elementsGFX[BUTTON].rcTexture, m_rcButton, sprite, pTexture, D3DCOLOR_ARGB( 255, 200, 200, 200 ), REGLUAR, dialogPos );
			renderRect(m_elementsGFX[MAIN].rcTexture, m_rcText, sprite, pTexture2, D3DCOLOR_ARGB( 255, 200, 200, 200 ), REGLUAR, dialogPos );
		}
		else
		{
			// if the button is pressed and the cursor is on it darken it to showed it is pressed
			if (m_bMouseOver && m_bPressed)
			{
				renderRect(m_elementsGFX[BUTTON].rcTexMouseOver, m_rcButton, sprite, pTexture, D3DCOLOR_ARGB( 255, 150, 150, 150 ), REGLUAR, dialogPos );
				renderRect(m_elementsGFX[MAIN].rcTexMouseOver, m_rcText, sprite, pTexture2, D3DCOLOR_ARGB( 255, 150, 150, 150 ), REGLUAR, dialogPos );
			}
			else
				// if the button has the cursor on it high light 
				if (m_bMouseOver || m_bOpened)
				{
					//drawButtonRect(m_controlGfx.rcTexMouseOver, rcWindow, sprite, pTexture, D3DCOLOR_ARGB( 200, 250, 250, 250 ));
					renderRect(m_elementsGFX[BUTTON].rcTexMouseOver, m_rcButton, sprite, pTexture, D3DCOLOR_ARGB( 255, 255, 255, 255 ), HiGHLIGHT, dialogPos);
					renderRect(m_elementsGFX[MAIN].rcTexMouseOver, m_rcText, sprite, pTexture2, D3DCOLOR_ARGB( 255, 255, 255, 255 ), HiGHLIGHT, dialogPos);
				}
		}

		if( m_iSelected >= 0 && m_iSelected < ( int )m_Items.size() )
		{                                      
			ComboBoxItem* pItem = m_Items[m_iSelected];
			if( pItem != NULL )
			{
				if (!m_bMouseOver && !m_bOpened)
					RenderText(pItem->strText, m_rcText, font.pFont,  DT_CENTER | DT_VCENTER, assetManger.getSprite(), d3d::WHITE, dialogPos);
				else
					RenderText(pItem->strText, m_rcText, font.pFont,  DT_CENTER | DT_VCENTER, assetManger.getSprite(), d3d::BLACK, dialogPos);
				//m_pDialog->DrawText( pItem->strText, pElement, &m_rcText );

			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name : UpdateRects() 
//-----------------------------------------------------------------------------
void CComboBoxUI::UpdateRects()
{
	CButtonUI::UpdateRects();

	m_rcButton = m_rcBoundingBox;
	m_rcButton.left = m_rcButton.right - (m_rcButton.bottom - m_rcButton.top);

	m_rcText = m_rcBoundingBox;
	m_rcText.right = m_rcButton.left;

	m_rcDropdown = m_rcText;
	OffsetRect( &m_rcDropdown, 0, ( int )( 0.90f * (m_rcText.bottom - m_rcText.top) ) );
	m_rcDropdown.bottom += m_nDropHeight;
	m_rcDropdown.right -= m_nSBWidth;

	m_rcDropdownText = m_rcDropdown;
	m_rcDropdownText.left += ( int )( 0.1f * (m_rcDropdown.right - m_rcDropdown.left) );
	m_rcDropdownText.right -= ( int )( 0.1f * (m_rcDropdown.right - m_rcDropdown.left) );
	m_rcDropdownText.top += ( int )( 0.1f * (m_rcDropdown.bottom - m_rcDropdown.top) );
	m_rcDropdownText.bottom -= ( int )( 0.1f * (m_rcDropdown.bottom - m_rcDropdown.top) );

	// Update the scrollbar's rects
	m_ScrollBar.setLocation( m_rcDropdown.right, m_rcDropdown.top + 2 );
	m_ScrollBar.setSize( m_nSBWidth, m_rcDropdown.bottom - m_rcDropdown.top - 2 );

	
	m_ScrollBar.SetPageSize( (m_rcDropdownText.bottom - m_rcDropdownText.top)  / m_elementsFonts[0].nFontHeight);

	// The selected item may have been scrolled off the page.
	// Ensure that it is in page again.
	m_ScrollBar.ShowItem( m_iSelected );
	
}

//-----------------------------------------------------------------------------
// Name : AddItem() 
// Desc : add an item to the combobox
//-----------------------------------------------------------------------------
//TODO: sort the objects by something else than pData or change it from void* to some kind of ENUM
HRESULT CComboBoxUI::AddItem( const char* strText, void* pData )
{
	// Validate parameters
	if( strText == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new item and set the data
	ComboBoxItem* newItem = new ComboBoxItem;

	ZeroMemory( newItem, sizeof( ComboBoxItem ) );
	strcpy_s( newItem->strText, 256, strText );
	newItem->pData = pData;

	//TODO: add something that will catch the exception thrown by push_back in case of lack of memory 
	m_Items.push_back(newItem);
	//m_Items.Add( pItem );

	// Update the scroll bar with new range
	m_ScrollBar.SetTrackRange( 0, m_Items.size() );

	// If this is the only item in the list, it's selected
	if( GetNumItems() == 1 )
	{
		m_iSelected = 0;
		m_iFocused = 0;
		//m_pParentDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );
		m_selectionChangedSig(this);
		//m_pParentDialog->SendEvent( 8, false, m_ID, NULL );
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : RemoveItem() 
// Desc : remove an item at the given index
//-----------------------------------------------------------------------------
void CComboBoxUI::RemoveItem( UINT index )
{
	ComboBoxItem* pItem = m_Items[index];

	if (pItem)
	{
		delete pItem;
		pItem = NULL;
	}

	m_Items.erase(m_Items.begin() + index);
	//m_Items.Remove( index );

	m_ScrollBar.SetTrackRange( 0, m_Items.size() );
	if( m_iSelected >= m_Items.size() )
		m_iSelected = m_Items.size() - 1;
}

//-----------------------------------------------------------------------------
// Name : RemoveAllItems() 
// Desc : removes all the items from the combobox
//-----------------------------------------------------------------------------
void CComboBoxUI::RemoveAllItems()
{
	for( UINT i = 0; i < m_Items.size(); i++ )
	{
		ComboBoxItem* pItem = m_Items[i];
		if (pItem)
		{
			delete pItem;
			pItem = NULL;
		}
	}

	m_Items.clear();
	m_ScrollBar.SetTrackRange( 0, 1 );
	m_iFocused = m_iSelected = -1;
}

//-----------------------------------------------------------------------------
// Name : FindItem() 
// Desc : finds an item that match the given text and returns the item index
// Note : iStart can be given if there is need to start the check from a certain index
//-----------------------------------------------------------------------------
int CComboBoxUI::FindItem( const char* strText, UINT iStart/* = 0 */)
{
	if( strText == NULL )
		return -1;

	for( UINT i = iStart; i < m_Items.size(); i++ )
	{
		ComboBoxItem* pItem = m_Items[i];

		if( strcmp( pItem->strText, strText ) == 0 )
		{
			return i;
		}
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Name : ContainsItem() 
// Desc : check is there is a Item that match the given text
// Note : iStart can be given if there is need to start the check from a certain index
//-----------------------------------------------------------------------------
bool CComboBoxUI::ContainsItem( const char* strText, UINT iStart /*= 0 */)
{
		return ( FindItem( strText, iStart ) != -1 );
}

//-----------------------------------------------------------------------------
// Name : GetItemData() 
// Desc : returns the item data 
// Note : the search for the item is being done based on the item text
//-----------------------------------------------------------------------------
void* CComboBoxUI::GetItemData( const char* strText )
{
	int index = FindItem( strText );
	if( index == -1 )
	{
		return NULL;
	}

	ComboBoxItem* pItem = m_Items[index];
	if( pItem == NULL )
	{
		//TODO: add some way to pop up errors Messages
		//DXTRACE_ERR( L"CGrowableArray::GetAt", E_FAIL );
		return NULL;
	}

	return pItem->pData;
}

//-----------------------------------------------------------------------------
// Name : GetItemData() 
// Desc : returns the item data 
// Note : the search for the item is being done based on the item index
//-----------------------------------------------------------------------------
void* CComboBoxUI::GetItemData( int nIndex )
{
	if( nIndex < 0 || nIndex >= m_Items.size() )
		return NULL;

	return m_Items[nIndex]->pData;
}

//-----------------------------------------------------------------------------
// Name : SetDropHeight() 
//-----------------------------------------------------------------------------
void CComboBoxUI::SetDropHeight( UINT nHeight )
{
	m_nDropHeight = nHeight; 
	UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : GetScrollBarWidth() 
//-----------------------------------------------------------------------------
int CComboBoxUI::GetScrollBarWidth() const
{
	return m_nSBWidth;
}

//-----------------------------------------------------------------------------
// Name : SetScrollBarWidth() 
//-----------------------------------------------------------------------------
void CComboBoxUI::SetScrollBarWidth( int nWidth )
{
	m_nSBWidth = nWidth; 
	UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : GetSelectedIndex() 
//-----------------------------------------------------------------------------
int CComboBoxUI::GetSelectedIndex() const
{
	return m_iSelected;
}

//-----------------------------------------------------------------------------
// Name : GetSelectedData() 
// Desc : returns the data of the current selected Item
//-----------------------------------------------------------------------------
void* CComboBoxUI::GetSelectedData()
{
	if( m_iSelected < 0 )
		return NULL;

	ComboBoxItem* pItem = m_Items[m_iSelected];
	return pItem->pData;
}

//-----------------------------------------------------------------------------
// Name : GetSelectedItem() 
// Desc : returns the current selected Item
//-----------------------------------------------------------------------------
ComboBoxItem* CComboBoxUI::GetSelectedItem()
{
	if( m_iSelected < 0 )
		return NULL;

	return m_Items[m_iSelected];
}

//-----------------------------------------------------------------------------
// Name : GetNumItems() 
//-----------------------------------------------------------------------------
UINT CComboBoxUI::GetNumItems()
{
	return m_Items.size();
}

//-----------------------------------------------------------------------------
// Name : GetItem() 
//-----------------------------------------------------------------------------
ComboBoxItem* CComboBoxUI::GetItem( UINT index )
{
	return m_Items[index];
}

//-----------------------------------------------------------------------------
// Name : SetSelectedByIndex() 
//-----------------------------------------------------------------------------
HRESULT CComboBoxUI::SetSelectedByIndex( UINT index )
{
	if( index >= GetNumItems() )
		return E_INVALIDARG;

	m_iFocused = m_iSelected = index;
	//m_pParentDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, NULL );
	m_pParentDialog->SendEvent( 9, false, NULL );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : SetSelectedByText() 
//-----------------------------------------------------------------------------
HRESULT CComboBoxUI::SetSelectedByText( const char* strText )
{
	if( strText == NULL )
		return E_INVALIDARG;

	int index = FindItem( strText );
	if( index == -1 )
		return E_FAIL;

	m_iFocused = m_iSelected = index;
	//m_pParentDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, NULL );
	m_pParentDialog->SendEvent( 9, false, NULL );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : SetSelectedByData() 
//-----------------------------------------------------------------------------
HRESULT CComboBoxUI::SetSelectedByData( void* pData )
{
	for( UINT i = 0; i < m_Items.size(); i++ )
	{
		ComboBoxItem* pItem = m_Items[i];

		if( pItem->pData == pData )
		{
			m_iFocused = m_iSelected = i;
			//m_pParentDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, NULL );
			m_pParentDialog->SendEvent( 9, false, NULL );
			return S_OK;
		}
	}

	return E_FAIL;
}

//-----------------------------------------------------------------------------
// Name : CanHaveFocus() 
//-----------------------------------------------------------------------------
bool CComboBoxUI::CanHaveFocus()
{
	//return true;
	return ( m_bVisible && m_bEnabled );
}

//-----------------------------------------------------------------------------
// Name : OnFocusOut() 
//-----------------------------------------------------------------------------
void CComboBoxUI::OnFocusOut()
{
	CButtonUI::OnFocusOut();

	m_bOpened = false;
}

//-----------------------------------------------------------------------------
// Name : SaveToFile() 
//-----------------------------------------------------------------------------
bool CComboBoxUI::SaveToFile(std::ostream& SaveFile)
{
	CButtonUI::SaveToFile(SaveFile);

	SaveFile << m_nDropHeight << "| ComboBox Drop Height" << "\n";
	SaveFile << m_nSBWidth << "| ComboBox SBWidth" << "\n";
	SaveFile << m_nFontHeight << "| ComboBox Font Height" << "\n";

	SaveFile << m_Items.size() << "| ComboBox Items Size" << "\n";

	for (UINT i = 0; i < m_Items.size(); i++)
	{
		SaveFile << m_Items[i]->strText << "| ComboBox Item "<< i <<" Text" << "\n";
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name : CopyItemsFrom
//-----------------------------------------------------------------------------
void CComboBoxUI::CopyItemsFrom(CComboBoxUI* sourceComboBox)
{
	// clears the items vector
	RemoveAllItems();

	for (UINT i = 0; i < sourceComboBox->GetNumItems(); i++)
	{
		AddItem( sourceComboBox->GetItem(i)->strText, sourceComboBox->GetItem(i)->pData );
	}
}