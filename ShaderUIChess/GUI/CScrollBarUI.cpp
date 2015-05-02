#include "CScrollBarUI.h"
#include "CDialogUI.h"

const unsigned char CScrollBarUI::SCROLLBAR_MINTHUMBSIZE = 8;
const float			CScrollBarUI::SCROLLBAR_ARROWCLICK_DELAY = 0.33f;
const float			CScrollBarUI::SCROLLBAR_ARROWCLICK_REPEAT = 0.05f;

//-----------------------------------------------------------------------------
// Name : CScrollBarUI(constructor) 
//-----------------------------------------------------------------------------
CScrollBarUI::CScrollBarUI(void)
{
	m_type = SCROLLBAR;
	m_bShowThumb = false;
	m_bDrag = false;


	SetRect(&m_rcElements[UPBUTTON], 0, 0, 0, 0);
	SetRect(&m_rcElements[DOWNBUTTON], 0, 0, 0, 0);
	SetRect(&m_rcElements[TRACK], 0, 0, 0, 0);
	SetRect(&m_rcElements[THUMB], 0, 0, 0, 0);

	m_nPosition = 0;
	m_nPageSize = 1;
	m_nStart = 0;
	m_nEnd = 1;

	m_LastMouse.x = 0;
	m_LastMouse.y = 0;

	m_Arrow = CLEAR;
	m_dArrowTS = 0.0;
}

//-----------------------------------------------------------------------------
// Name : ~CScrollBarUI(destructor) 
//-----------------------------------------------------------------------------
CScrollBarUI::~CScrollBarUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : HandleKeyboard() 
// Desc : handles keyboard inputs
//-----------------------------------------------------------------------------
bool CScrollBarUI::HandleKeyboard(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
	return false;
}

//-----------------------------------------------------------------------------
// Name : HandleMouse() 
// Desc : handles mouse input
//-----------------------------------------------------------------------------
bool CScrollBarUI::HandleMouse( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer)
{
	static int ThumbOffsetY;

	m_LastMouse = mousePoint;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if ( Pressed(hWnd, mousePoint, inputstate ,timer))
				return true;
		}break;

	case WM_LBUTTONUP:
		{
			if ( Released(hWnd, mousePoint))
				return true;
		}break;

	case WM_MOUSEMOVE:
		{
			if ( Dragged(mousePoint))
				return true;
		}break;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed() 
//-----------------------------------------------------------------------------
bool CScrollBarUI::Pressed( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer)
{
	// Check for click on up button
	if( PtInRect( &m_rcElements[UPBUTTON], pt ) )
	{
		SetCapture( hWnd);
		if( m_nPosition > m_nStart )
			--m_nPosition;

		UpdateThumbRect();
		m_Arrow = CLICKED_UP;
		//TODO: understand what the hell the time stamp here does
		m_dArrowTS = timer->getCurrentTime();
		return true;
	}

	// Check for click on down button

	if( PtInRect( &m_rcElements[DOWNBUTTON], pt ) )
	{
		SetCapture( hWnd);
		if( m_nPosition + m_nPageSize <= m_nEnd )
			++m_nPosition;

		UpdateThumbRect();
		m_Arrow = CLICKED_DOWN;
		m_dArrowTS = timer->getCurrentTime();
		return true;
	}

	// Check for click on thumb
	if( PtInRect( &m_rcElements[THUMB], pt ) )
	{
		SetCapture( hWnd);
		m_bDrag = true;
		m_thumbOffsetY = pt.y - m_rcElements[THUMB].top;
		return true;
	}

	// Check for click on track
	if( m_rcElements[THUMB].left <= pt.x && m_rcElements[THUMB].right > pt.x )
	{
		SetCapture( hWnd );
		if( m_rcElements[THUMB].top > pt.y && m_rcElements[TRACK].top <= pt.y )
		{
			Scroll( -( m_nPageSize - 1 ) );
			return true;
		}
		else if( m_rcElements[THUMB].bottom <= pt.y && m_rcElements[TRACK].bottom > pt.y )
		{
			Scroll( m_nPageSize - 1 );
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Released() 
//-----------------------------------------------------------------------------
bool CScrollBarUI::Released( HWND hWnd, POINT pt)
{
	m_bDrag = false;
	ReleaseCapture();
	UpdateThumbRect();
	m_Arrow = CLEAR;
	
	return false;// ??
}

//-----------------------------------------------------------------------------
// Name : Dragged() 
//-----------------------------------------------------------------------------
bool CScrollBarUI::Dragged( POINT pt)
{
	if( m_bDrag )
	{
		m_rcElements[THUMB].bottom += pt.y - m_thumbOffsetY - m_rcElements[THUMB].top;
		m_rcElements[THUMB].top = pt.y - m_thumbOffsetY;

		if( m_rcElements[THUMB].top < m_rcElements[TRACK].top )
			OffsetRect( &m_rcElements[THUMB], 0, m_rcElements[TRACK].top - m_rcElements[THUMB].top );

		else if( m_rcElements[THUMB].bottom > m_rcElements[TRACK].bottom )
			OffsetRect( &m_rcElements[THUMB], 0, m_rcElements[TRACK].bottom - m_rcElements[THUMB].bottom );

		// Compute first item index based on thumb position
		int rcTrackHeight = m_rcElements[TRACK].bottom - m_rcElements[TRACK].top;
		int rcThumbHeight = m_rcElements[THUMB].bottom - m_rcElements[THUMB].top;

		int nMaxFirstItem = m_nEnd - m_nStart - m_nPageSize + 1;  // Largest possible index for first item
		int nMaxThumb = rcTrackHeight - rcThumbHeight ;  // Largest possible thumb position from the top

		m_nPosition = m_nStart + ( m_rcElements[THUMB].top - m_rcElements[TRACK].top + 
			nMaxThumb / ( nMaxFirstItem * 2 ) ) * // Shift by half a row to avoid last row covered by only one pixel
			nMaxFirstItem / nMaxThumb;

		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : SaveToFile() 
// NOTE : seems to be no need to save the Scrollbar as it is automatically generated
//-----------------------------------------------------------------------------
// bool CScrollBarUI::SaveToFile(std::ostream& SaveFile)
// {
// 	CControlUI::SaveToFile(SaveFile);
// 
// 	SaveFile << 
// 
// 	return true;
// }

//-----------------------------------------------------------------------------
// Name : Render() 
// Desc : renders the scroll bar to the screen
//-----------------------------------------------------------------------------
void CScrollBarUI::Render( CAssetManager& assetManger)
{
	if (m_bVisible)
	{
		CTimer* timer = assetManger.getTimer();
		if (!timer)
			return;

		// checks if one of the arrows buttons is being held or was clicked
		if( m_Arrow != CLEAR )
		{

			//TODO: find some miracle way to bring to this function the time....
			double dCurrTime = timer->getCurrentTime();

			// check if the cursor on one of the arrow buttons if it is scrolls according to the defined delay and repeat times
			if( PtInRect( &m_rcElements[UPBUTTON], m_LastMouse ) )
			{
				switch( m_Arrow )
				{
				case CLICKED_UP:
					if( SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS )
					{
						Scroll( -1 );
						m_Arrow = HELD_UP;
						m_dArrowTS = dCurrTime;
					}
					break;
				case HELD_UP:
					if( SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS )
					{
						Scroll( -1 );
						m_dArrowTS = dCurrTime;
					}
					break;
				}
			}
			else if( PtInRect( &m_rcElements[DOWNBUTTON], m_LastMouse ) )
			{
				switch( m_Arrow )
				{
				case CLICKED_DOWN:
					if( SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS )
					{
						Scroll( 1 );
						m_Arrow = HELD_DOWN;
						m_dArrowTS = dCurrTime;
					}
					break;
				case HELD_DOWN:
					if( SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS )
					{
						Scroll( 1 );
						m_dArrowTS = dCurrTime;
					}
					break;
				}
			}
		} // ends check if arrow buttons were held or 

		// prepare to render all the elements of the scrollbar and gets a pointer to the sprite
		HRESULT hr;
		LPDIRECT3DTEXTURE9 pTexture;

		CMySprite* sprite = assetManger.getMySprite();

		if (!sprite)
			return;

		if (m_elementsGFX.size() < 4)
			return;

		// render all the scrollbar elements
		for (UINT i = 0; i < 4; i++)
		{
			//no texture was given abort rendering
			if (m_elementsGFX[i].iTexture == -1)
				return;

			//acquire a pointer to the texture we need to render the button
			pTexture = assetManger.getTexturePtr((m_elementsGFX[i].iTexture) );

			POINT dialogPos = m_pParentDialog->getLocation();
			LONG  dialogCaptionHeihgt =  m_pParentDialog->getCaptionHeight();
			dialogPos.y += dialogCaptionHeihgt;

			renderRect(m_elementsGFX[i].rcTexture, m_rcElements[i], sprite, pTexture, d3d::WHITE, TOP, dialogPos);
		}
	}
}

//-----------------------------------------------------------------------------
// Name : SetPageSize() 
// Desc : handles mouse input
//-----------------------------------------------------------------------------
void CScrollBarUI::SetPageSize( int nPageSize )
{
	m_nPageSize = nPageSize;
	Cap();
	UpdateThumbRect();
}

//-----------------------------------------------------------------------------
// Name : SetTrackRange() 
// Desc : 
//-----------------------------------------------------------------------------
void CScrollBarUI::SetTrackRange( int nStart, int nEnd )
{
	m_nStart = nStart;
	m_nEnd   = nEnd;
	Cap();
	UpdateThumbRect();
}

//-----------------------------------------------------------------------------
// Name : SetTrackPos() 
// Desc : 
//-----------------------------------------------------------------------------
void CScrollBarUI::SetTrackPos( int nPosition )
{
	m_nPosition = nPosition;
	Cap();
	UpdateThumbRect();
}

//-----------------------------------------------------------------------------
// Name : GetTrackPos() 
//-----------------------------------------------------------------------------
int CScrollBarUI::GetTrackPos()
{
	return m_nPosition;
}

//-----------------------------------------------------------------------------
// Name : GetPageSize() 
//-----------------------------------------------------------------------------
int CScrollBarUI::GetPageSize()
{
	return m_nPageSize;
}

//-----------------------------------------------------------------------------
// Name : UpdateRects() 
// Desc : 
//-----------------------------------------------------------------------------
void CScrollBarUI::UpdateRects()
{
	CControlUI::UpdateRects();

	// sets the size of the up/down buttons and makes them a square 
	SetRect( &m_rcElements[UPBUTTON], m_rcBoundingBox.left, m_rcBoundingBox.top,m_rcBoundingBox.right,
		m_rcBoundingBox.top + ( m_rcBoundingBox.right - m_rcBoundingBox.left ) );
	SetRect( &m_rcElements[DOWNBUTTON], m_rcBoundingBox.left, 
		m_rcBoundingBox.bottom - ( m_rcBoundingBox.right -m_rcBoundingBox.left ), m_rcBoundingBox.right, m_rcBoundingBox.bottom );

	SetRect( &m_rcElements[TRACK], m_rcElements[UPBUTTON].left, m_rcElements[UPBUTTON].bottom,
		m_rcElements[DOWNBUTTON].right, m_rcElements[DOWNBUTTON].top );

	// sets the size of the up/down buttons and makes them a square 
// 	SetRect(&m_rcElements[UPBUTTON], m_x, m_y, m_width ,m_y + m_width);
// 	SetRect(&m_rcElements[DOWNBUTTON], m_x, m_y + m_height - m_width, m_width, m_y + m_height);
// 
// 	SetRect(&m_rcElements[TRACK], m_rcElements[UPBUTTON].left, m_rcElements[UPBUTTON].bottom, m_rcElements[DOWNBUTTON].right,
// 		m_rcElements[DOWNBUTTON].top);

	m_rcElements[THUMB].left = m_rcElements[UPBUTTON].left;
	m_rcElements[THUMB].right = m_rcElements[UPBUTTON].right;
	
	UpdateThumbRect();
}

//-----------------------------------------------------------------------------
// Name : UpdateThumbRect() 
// Desc : 
//-----------------------------------------------------------------------------
void CScrollBarUI::UpdateThumbRect()
{
	if( m_nEnd - m_nStart > m_nPageSize )
	{
		int rcTrackHeight = m_rcElements[TRACK].bottom - m_rcElements[TRACK].top;

		int nThumbHeight = __max(rcTrackHeight * m_nPageSize / ( m_nEnd - m_nStart ), SCROLLBAR_MINTHUMBSIZE );

		int nMaxPosition = m_nEnd - m_nStart - m_nPageSize;
		m_rcElements[THUMB].top = m_rcElements[TRACK].top + ( m_nPosition - m_nStart ) * ( rcTrackHeight - nThumbHeight )
			/ nMaxPosition;
		m_rcElements[THUMB].bottom = m_rcElements[THUMB].top + nThumbHeight;
		m_bShowThumb = true;

	}
	else
	{
		// No content to scroll
		m_rcElements[THUMB].bottom = m_rcElements[THUMB].top;
		m_bShowThumb = false;
	}
}

//-----------------------------------------------------------------------------
// Name : Cap() 
// Desc : 
//-----------------------------------------------------------------------------
void CScrollBarUI::Cap()
{
	if( m_nPosition < m_nStart || m_nEnd - m_nStart <= m_nPageSize )
	{
		m_nPosition = m_nStart;
	}
	else if( m_nPosition + m_nPageSize > m_nEnd )
		m_nPosition = m_nEnd - m_nPageSize + 1;
}

//-----------------------------------------------------------------------------
// Name : Scroll 
// desc : Scroll() scrolls by nDelta items.  A positive value scrolls down, while a negative
//		  value scrolls up.
//-----------------------------------------------------------------------------
void CScrollBarUI::Scroll( int nDelta )
{
	// Perform scroll
	m_nPosition += nDelta;

	// Cap position
	Cap();

	// Update thumb position
	UpdateThumbRect();
}

//-----------------------------------------------------------------------------
// Name : ShowItem
//-----------------------------------------------------------------------------
void CScrollBarUI::ShowItem( int nIndex )
{
	// Cap the index

	if( nIndex < 0 )
		nIndex = 0;

	if( nIndex >= m_nEnd )
		nIndex = m_nEnd - 1;

	// Adjust position

	if( m_nPosition > nIndex )
		m_nPosition = nIndex;
	else if( m_nPosition + m_nPageSize <= nIndex )
		m_nPosition = nIndex - m_nPageSize + 1;

	UpdateThumbRect();
}