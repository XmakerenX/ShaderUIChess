#include "CListBoxUI.h"
#include "CDialogUI.h"

//-----------------------------------------------------------------------------
// Name : CListBoxUI (constructor)
//-----------------------------------------------------------------------------
CListBoxUI::CListBoxUI(CDialogUI* pParentDialog, int ID, int x, int y, int width, int height, DWORD dwStyle)
	:CControlUI(pParentDialog, ID, x, y, width, height)
{
	m_type = CControlUI::LISTBOX;

	m_dwStyle = dwStyle;
	m_nSBWidth = 16;
	m_nSelected = -1;
	m_nSelStart = 0;
	m_bDrag = false;
	m_nBorder = 6;
	m_nMargin = 5;
	m_nTextHeight = 0;
}

//-----------------------------------------------------------------------------
// Name : CListBoxUI (constructor from InputFile)
//-----------------------------------------------------------------------------
CListBoxUI::CListBoxUI(std::istream& inputFile)
	:CControlUI(inputFile)
{
	UINT itemsSize = 0;
	m_type = CControlUI::LISTBOX;

	m_nSelected = -1;
	m_nSelStart = 0;
	m_bDrag = false;

	inputFile >> m_nSBWidth;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nBorder;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nMargin;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nTextHeight;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_dwStyle;
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
// Name : CListBoxUI (destructor)
//-----------------------------------------------------------------------------
CListBoxUI::~CListBoxUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : onInit()
//-----------------------------------------------------------------------------
bool CListBoxUI::onInit()
{
	return m_pParentDialog->initControl( &m_ScrollBar );
}

//-----------------------------------------------------------------------------
// Name : HandleKeyboard()
//-----------------------------------------------------------------------------
bool CListBoxUI::HandleKeyboard( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
	return false;
}

//-----------------------------------------------------------------------------
// Name : HandleMouse
//-----------------------------------------------------------------------------
bool CListBoxUI::HandleMouse( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer  )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	// First acquire focus
	if( WM_LBUTTONDOWN == uMsg )
		if (ContainsPoint(mousePoint))
			if( !m_bHasFocus )
				m_pParentDialog->RequestFocus( this );

	// Let the scroll bar handle it first.
	if( m_ScrollBar.HandleMouse( hWnd, uMsg, mousePoint, inputstate, timer ) )
		return true;

	switch( uMsg )
	{

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if ( Pressed(hWnd, mousePoint, inputstate, timer))
				return true;
		}break;

	case WM_LBUTTONUP:
		{
			if ( Released(hWnd, mousePoint))
				return true;
		}break;

	case WM_MOUSEMOVE:
		{
			if (Dragged(mousePoint))
				return true;
		}break;

	case WM_MOUSEWHEEL:
		{
			//TODO: add here way to calc or pas the value for scrolled!
			if ( Scrolled( inputstate.nScrollAmount ))
				return true;
		}break;
	}

	return false;
}
//-----------------------------------------------------------------------------
// Name : Pressed
//-----------------------------------------------------------------------------
bool CListBoxUI::Pressed( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer)
{
	// Check for clicks in the text area
	if( m_Items.size() > 0 && PtInRect( &m_rcSelection, pt ) )
	{
		// Compute the index of the clicked item

		int nClicked = -1;
		int temp = m_rcBoundingBox.top;

		if( m_nTextHeight )
		{
			nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
		}
		else
			nClicked = -1;

		// Only proceed if the click falls on top of an item.

		if( nClicked >= m_ScrollBar.GetTrackPos() &&
			nClicked < ( int )m_Items.size() &&
			nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
		{
			SetCapture( hWnd );
			m_bDrag = true;

			// If this is a double click, fire off an event and exit
			// since the first click would have taken care of the selection
			// updating.
			//if( uMsg == WM_LBUTTONDBLCLK )
			if ( inputState.bDoubleClick )
			{
				//m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_DBLCLK, true, this );
				return true;
			}

			m_nSelected = nClicked;
			if( !( inputState.bShift ) )
				m_nSelStart = m_nSelected;

			// If this is a multi-selection listbox, update per-item
			// selection data.

			if( m_dwStyle & MULTISELECTION )
			{
				// Determine behavior based on the state of Shift and Ctrl

				ListBoxItemUI* pSelItem = m_Items[m_nSelected];
				if( inputState.bCtrl && !inputState.bShift )
				{
					// Control click. Reverse the selection of this item.

					pSelItem->bSelected = !pSelItem->bSelected;
				}
				else if( !inputState.bCtrl && inputState.bShift)
				{
					// Shift click. Set the selection for all items
					// from last selected item to the current item.
					// Clear everything else.

					int nBegin = __min( m_nSelStart, m_nSelected );
					int nEnd = __max( m_nSelStart, m_nSelected );

					for( int i = 0; i < nBegin; ++i )
					{
						ListBoxItemUI* pItem = m_Items[i];
						pItem->bSelected = false;
					}

					for( int i = nEnd + 1; i < ( int )m_Items.size(); ++i )
					{
						ListBoxItemUI* pItem = m_Items[i];
						pItem->bSelected = false;
					}

					for( int i = nBegin; i <= nEnd; ++i )
					{
						ListBoxItemUI* pItem = m_Items[i];
						pItem->bSelected = true;
					}
				}
				else if( inputState.bCtrl && inputState.bShift )
				{
					// Control-Shift-click.

					// The behavior is:
					//   Set all items from m_nSelStart to m_nSelected to
					//     the same state as m_nSelStart, not including m_nSelected.
					//   Set m_nSelected to selected.

					int nBegin = __min( m_nSelStart, m_nSelected );
					int nEnd = __max( m_nSelStart, m_nSelected );

					// The two ends do not need to be set here.

					bool bLastSelected = m_Items[m_nSelStart]->bSelected;
					for( int i = nBegin + 1; i < nEnd; ++i )
					{
						ListBoxItemUI* pItem = m_Items[ i ];
						pItem->bSelected = bLastSelected;
					}

					pSelItem->bSelected = true;

					// Restore m_nSelected to the previous value
					// This matches the Windows behavior

					m_nSelected = m_nSelStart;
				}
				else
				{
					// Simple click.  Clear all items and select the clicked
					// item.


					for( int i = 0; i < ( int )m_Items.size(); ++i )
					{
						ListBoxItemUI* pItem = m_Items[i];
						pItem->bSelected = false;
					}

					pSelItem->bSelected = true;
				}
			}  // End of multi-selection case

			//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
		}

		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Released
//-----------------------------------------------------------------------------
bool CListBoxUI::Released( HWND hWnd, POINT pt)
{
	ReleaseCapture();
	m_bDrag = false;

	if( m_nSelected != -1 )
	{
		// Set all items between m_nSelStart and m_nSelected to
		// the same state as m_nSelStart
		int nEnd = __max( m_nSelStart, m_nSelected );

		for( int n = __min( m_nSelStart, m_nSelected ) + 1; n < nEnd; ++n )
			m_Items[n]->bSelected = m_Items[m_nSelStart]->bSelected;
		m_Items[m_nSelected]->bSelected = m_Items[m_nSelStart]->bSelected;

		// If m_nSelStart and m_nSelected are not the same,
		// the user has dragged the mouse to make a selection.
		// Notify the application of this.
		if( m_nSelStart != m_nSelected );
		//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );

		//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION_END, true, this );
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Dragged
//-----------------------------------------------------------------------------
bool CListBoxUI::Dragged(POINT pt)
{
	if( m_bDrag )
	{
		// Compute the index of the item below cursor

		int nItem;
		if( m_nTextHeight )
			nItem = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
		else
			nItem = -1;

		// Only proceed if the cursor is on top of an item.

		if( nItem >= ( int )m_ScrollBar.GetTrackPos() &&
			nItem < ( int )m_Items.size() &&
			nItem < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
		{
			m_nSelected = nItem;
			//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
		}
		else if( nItem < ( int )m_ScrollBar.GetTrackPos() )
		{
			// User drags the mouse above window top
			m_ScrollBar.Scroll( -1 );
			m_nSelected = m_ScrollBar.GetTrackPos();
			//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
		}
		else if( nItem >= m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
		{
			// User drags the mouse below window bottom
			m_ScrollBar.Scroll( 1 );
			m_nSelected = __min( ( int )m_Items.size(), m_ScrollBar.GetTrackPos() +
				m_ScrollBar.GetPageSize() ) - 1;
			//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Name : Scrolled
//-----------------------------------------------------------------------------
bool CListBoxUI::Scrolled( int nScrollAmount)
{
	if (m_bMouseOver)
	{	
		UINT uLines;
		SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
		//int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
		m_ScrollBar.Scroll( -nScrollAmount );
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : ConnectToItemDBLCK
//-----------------------------------------------------------------------------
void CListBoxUI::ConnectToItemDBLCK(const signal_listbox::slot_type& subscriber)
{
	m_itemDBLCLKSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : ConnectToListboxSel
//-----------------------------------------------------------------------------
void CListBoxUI::ConnectToListboxSel(const signal_listbox::slot_type& subscriber)
{
	m_listboxSelSig.connect(subscriber);
}


//-----------------------------------------------------------------------------
// Name : Render
//-----------------------------------------------------------------------------
void CListBoxUI::Render( CAssetManager& assetManger )
{
	LPDIRECT3DTEXTURE9 pTexture;

	if( m_bVisible == false )
		return;

	CMySprite* sprite = assetManger.getMySprite();
	FONT_ITEM font = assetManger.getFontItem(m_elementsFonts[0].fontIndex);
	
	POINT dialogPos = m_pParentDialog->getLocation();
	LONG  dialogCaptionHeihgt =  m_pParentDialog->getCaptionHeight();
	dialogPos.y += dialogCaptionHeihgt;

	pTexture = assetManger.getTexturePtr(m_elementsGFX[0].iTexture);
	renderRect(m_elementsGFX[0].rcTexture, m_rcBoundingBox, sprite, pTexture, D3DCOLOR_ARGB( 255, 255, 255, 255 ), REGLUAR, dialogPos);
	//m_pDialog->DrawSprite( pElement, &m_rcBoundingBox, DXUT_FAR_BUTTON_DEPTH );

	// Render the text
	if( m_Items.size() > 0 )
	{
		// Find out the height of a single line of text
		RECT rc = m_rcText;
		RECT rcSel = m_rcSelection;
		rc.bottom = rc.top + m_elementsFonts[0].nFontHeight;

		// Update the line height formation
		m_nTextHeight = rc.bottom - rc.top;

// 		static bool bSBInit;
// 		if( !bSBInit )
// 		{
// 			// Update the page size of the scroll bar
// 			if( m_nTextHeight )
// 				m_ScrollBar.SetPageSize( RectHeight( m_rcText ) / m_nTextHeight );
// 			else
// 				m_ScrollBar.SetPageSize( RectHeight( m_rcText ) );
// 			bSBInit = true;
// 		}

		rc.right = m_rcText.right;
		for( int i = m_ScrollBar.GetTrackPos(); i < ( int )m_Items.size(); ++i )
		{
			if( rc.bottom > m_rcText.bottom )
				break;

			ListBoxItemUI* pItem = m_Items[i];

			// Determine if we need to render this item with the
			// selected element.
			bool bSelectedStyle = false;

			if( !( m_dwStyle & MULTISELECTION ) && i == m_nSelected )
				bSelectedStyle = true;
			else if( m_dwStyle & MULTISELECTION )
			{
				if( m_bDrag &&
					( ( i >= m_nSelected && i < m_nSelStart ) ||
					( i <= m_nSelected && i > m_nSelStart ) ) )
					bSelectedStyle = m_Items[m_nSelStart]->bSelected;
				else if( pItem->bSelected )
					bSelectedStyle = true;
			}

			if( bSelectedStyle )
			{
				rcSel.top = rc.top; rcSel.bottom = rc.bottom;
				renderRect(m_elementsGFX[1].rcTexture, rcSel, sprite, pTexture, D3DCOLOR_ARGB( 255, 255, 255, 255 ), REGLUAR, dialogPos);
				//m_pDialog->DrawSprite( pSelElement, &rcSel, DXUT_NEAR_BUTTON_DEPTH );
				//m_pDialog->DrawText( pItem->strText, pSelElement, &rc );
				RenderText(pItem->strText, rc, font.pFont,  DT_LEFT | DT_VCENTER, assetManger.getSprite(), d3d::WHITE, dialogPos);
			}
			else
				//m_pDialog->DrawText( pItem->strText, pElement, &rc );
				RenderText(pItem->strText, rc, font.pFont,  DT_LEFT | DT_VCENTER, assetManger.getSprite(), d3d::BLACK, dialogPos);

			OffsetRect( &rc, 0, m_nTextHeight );
		}
	}

	// Render the scroll bar
	m_ScrollBar.Render(assetManger);
}

//-----------------------------------------------------------------------------
// Name : UpdateRects
//-----------------------------------------------------------------------------
void CListBoxUI::UpdateRects()
{
	CControlUI::UpdateRects();

	m_rcSelection = m_rcBoundingBox;
	m_rcSelection.right -= m_nSBWidth;
	InflateRect( &m_rcSelection, -m_nBorder, -m_nBorder );
	m_rcText = m_rcSelection;
	InflateRect( &m_rcText, -m_nMargin, 0 );

	// Update the scrollbar's rects
	m_ScrollBar.setLocation( m_rcBoundingBox.right - m_nSBWidth, m_rcBoundingBox.top );
	m_ScrollBar.setSize( m_nSBWidth, m_height );

	if(m_elementsFonts.size() > 0 )
	{
		m_ScrollBar.SetPageSize( ( m_rcText.bottom - m_rcText.top ) /  m_elementsFonts[0].nFontHeight );

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_ScrollBar.ShowItem( m_nSelected );
	}
}

//-----------------------------------------------------------------------------
// Name : GetStyle
//-----------------------------------------------------------------------------
DWORD CListBoxUI::GetStyle() const
{
	return m_dwStyle;
}

//-----------------------------------------------------------------------------
// Name : SetStyle
//-----------------------------------------------------------------------------
void CListBoxUI::SetStyle( DWORD dwStyle )
{
	m_dwStyle = dwStyle;
}

//-----------------------------------------------------------------------------
// Name : GetSize
//-----------------------------------------------------------------------------
int CListBoxUI::GetSize() const 
{
	return m_Items.size();
}

//-----------------------------------------------------------------------------
// Name : GetScrollBarWidth
//-----------------------------------------------------------------------------
int CListBoxUI::GetScrollBarWidth() const
{
	return m_nSBWidth;
}

//-----------------------------------------------------------------------------
// Name : SetScrollBarWidth
//-----------------------------------------------------------------------------
void CListBoxUI::SetScrollBarWidth( int nWidth )
{
	m_nSBWidth = nWidth;
	UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : SetBorder
//-----------------------------------------------------------------------------
void CListBoxUI::SetBorder( int nBorder, int nMargin )
{
	m_nBorder = nBorder;
	m_nMargin = nMargin;
}

//-----------------------------------------------------------------------------
// Name : AddItem
//-----------------------------------------------------------------------------
HRESULT CListBoxUI::AddItem( const char* strText, void* pData)
{
	//TODO: add a check that will catch the exception in a case push_back fails! 
	ListBoxItemUI* pNewItem = new ListBoxItemUI();
	if (!pNewItem)
		return S_FALSE;

	strcpy_s( pNewItem->strText, 256, strText );
	pNewItem->pData = pData;
	SetRect(&pNewItem->rcActive, 0, 0, 0, 0);
	pNewItem->bSelected = false;

	m_Items.push_back(pNewItem);

	m_ScrollBar.SetTrackRange( 0, m_Items.size());

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : InsertItem
//-----------------------------------------------------------------------------
HRESULT CListBoxUI::InsertItem( int nIndex, const char* strText, void* pData )
{
	ListBoxItemUI* pNewItem = new ListBoxItemUI();
	if (!pNewItem)
		return S_FALSE;

	strcpy_s( pNewItem->strText, 256, strText );
	pNewItem->pData = pData;
	SetRect(&pNewItem->rcActive, 0, 0, 0, 0);
	pNewItem->bSelected = false;

	//TODO: add something that will catch the exception
	m_Items.insert(m_Items.begin() + nIndex, pNewItem );

	m_ScrollBar.SetTrackRange( 0, m_Items.size() );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : RemoveItem
//-----------------------------------------------------------------------------
void CListBoxUI::RemoveItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= ( int )m_Items.size() )
		return;

	//ListBoxItem3D item = m_Items[nIndex];
	m_Items.erase(m_Items.begin() + nIndex);

	m_ScrollBar.SetTrackRange( 0, m_Items.size() );
	if( m_nSelected >= ( int )m_Items.size() )
		m_nSelected = m_Items.size() - 1;

	m_pParentDialog->SendEvent(5, false, m_ID, NULL);
	//m_pParentDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
}

//TODO: decide if I really need this function as I don't really use data here....
//-----------------------------------------------------------------------------
// Name : RemoveItemByData
//-----------------------------------------------------------------------------
void CListBoxUI::RemoveItemByData( void* pData )
{
}

//-----------------------------------------------------------------------------
// Name : RemoveAllItems
//-----------------------------------------------------------------------------
void CListBoxUI::RemoveAllItems()
{
	m_Items.clear();
	m_ScrollBar.SetTrackRange( 0, 1 );
}

//-----------------------------------------------------------------------------
// Name : GetItem
//-----------------------------------------------------------------------------
ListBoxItemUI* CListBoxUI::GetItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= ( int )m_Items.size() )
		return NULL;

	return m_Items[nIndex];
}

//-----------------------------------------------------------------------------
// Name : GetSelectedIndex
// Desc : For single-selection listbox, returns the index of the selected item.
// For multi-selection, returns the first selected item after the nPreviousSelected position.
// To search for the first selected item, the app passes -1 for nPreviousSelected.  For
// subsequent searches, the app passes the returned index back to GetSelectedIndex as.
// nPreviousSelected.
// Returns -1 on error or if no item is selected.
//-----------------------------------------------------------------------------
int CListBoxUI::GetSelectedIndex( int nPreviousSelected/*  = -1 */)
{
	if( nPreviousSelected < -1 )
		return -1;

	if( m_dwStyle & MULTISELECTION )
	{
		// Multiple selection enabled. Search for the next item with the selected flag.
		for( int i = nPreviousSelected + 1; i < ( int )m_Items.size(); ++i )
		{
			ListBoxItemUI* pItem = m_Items[i];

			if( pItem->bSelected )
				return i;
		}

		return -1;
	}
	else
	{
		// Single selection
		return m_nSelected;
	}
}

//-----------------------------------------------------------------------------
// Name : GetSelectedItem
//-----------------------------------------------------------------------------
ListBoxItemUI* CListBoxUI::GetSelectedItem( int nPreviousSelected/* = -1 */)
{
	return GetItem( GetSelectedIndex( nPreviousSelected ) );
}

//-----------------------------------------------------------------------------
// Name : SelectItem
//-----------------------------------------------------------------------------
void CListBoxUI::SelectItem( int nNewIndex )
{
	// If no item exists, do nothing.
	if( m_Items.size() == 0 )
		return;

	int nOldSelected = m_nSelected;

	// Adjust m_nSelected
	m_nSelected = nNewIndex;

	// Perform capping
	if( m_nSelected < 0 )
		m_nSelected = 0;
	if( m_nSelected >= ( int )m_Items.size() )
		m_nSelected = m_Items.size() - 1;

	if( nOldSelected != m_nSelected )
	{
		if( m_dwStyle & MULTISELECTION )
		{
			m_Items[m_nSelected]->bSelected = true;
		}

		// Update selection start
		m_nSelStart = m_nSelected;

		// Adjust scroll bar
		m_ScrollBar.ShowItem( m_nSelected );
	}

	m_pParentDialog->SendEvent(5, false, m_ID, false);
	//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
}

//-----------------------------------------------------------------------------
// Name : CanHaveFocus
//-----------------------------------------------------------------------------
bool CListBoxUI::CanHaveFocus()
{
	//return true;
	return ( m_bVisible && m_bEnabled );
}

//-----------------------------------------------------------------------------
// Name : SaveToFile
//-----------------------------------------------------------------------------
bool CListBoxUI::SaveToFile(std::ostream& SaveFile)
{
	CControlUI::SaveToFile(SaveFile);

	SaveFile << m_nSBWidth << "| ListBox SBWidth" << "\n";
	SaveFile << m_nBorder << "| ListBox Border" << "\n";
	SaveFile << m_nMargin << "| ListBox Margin" << "\n";
	SaveFile << m_nTextHeight << "| ListBox Text Height" << "\n";
	SaveFile << m_dwStyle << "| ListBox Style" << "\n";

	SaveFile << m_Items.size() << "| ListBox Item size" << "\n";

	for (UINT i = 0; i < m_Items.size(); i++)
	{
		SaveFile << m_Items[i]->strText << "| ListBox Item " << i << " Text" << "\n";
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name : CopyItemsFrom
//-----------------------------------------------------------------------------
void CListBoxUI::CopyItemsFrom(CListBoxUI* sourceListBox)
{
	// clears the items vector
	RemoveAllItems();

	for (UINT i = 0; i < sourceListBox->GetSize(); i++)
	{
		AddItem( sourceListBox->GetItem(i)->strText, sourceListBox->GetItem(i)->pData );
	}
}