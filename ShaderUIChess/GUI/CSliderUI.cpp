#include "CSliderUI.h"
#include "CDialogUI.h"

//-----------------------------------------------------------------------------
// Name : CSliderUI(constructor) 
//-----------------------------------------------------------------------------
CSliderUI::CSliderUI(CDialogUI* pParentDialog, int ID, int x, int y, int width, int height, int min, int max, int nValue )
	:CControlUI(pParentDialog, ID, x, y, width ,height)
{
	m_type = CControlUI::SLIDER;

	m_nMin = min;
	m_nMax = max;
	m_nValue = nValue;

	m_bPressed = false;

}

//-----------------------------------------------------------------------------
// Name : CSliderUI(constructor from InputFile) 
//-----------------------------------------------------------------------------
CSliderUI::CSliderUI(std::istream& inputFile)
	:CControlUI(inputFile)
{
	m_type = CControlUI::SLIDER;

	inputFile >> m_nMin;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nMax;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

	m_nValue = (m_nMax - m_nMin) / 2;

	m_bPressed = false;
}

//-----------------------------------------------------------------------------
// Name : CSliderUI(destructor) 
//-----------------------------------------------------------------------------
CSliderUI::~CSliderUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : ContainsPoint() 
//-----------------------------------------------------------------------------
BOOL CSliderUI::ContainsPoint( POINT pt )
{
	return ( PtInRect( &m_rcBoundingBox, pt ) || PtInRect( &m_rcButton, pt ) );
}

//-----------------------------------------------------------------------------
// Name : HandleKeyboard() 
//-----------------------------------------------------------------------------
bool CSliderUI::HandleKeyboard( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return false;
}

//-----------------------------------------------------------------------------
// Name : HandleMouse() 
//-----------------------------------------------------------------------------
bool CSliderUI::HandleMouse( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

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
			if ( Dragged(mousePoint))
				return true;
		}break;

	case WM_MOUSEWHEEL:
		{
			if ( Scrolled( inputstate.nScrollAmount ) )
				return true;
		}break;
	};

	return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed() 
//-----------------------------------------------------------------------------
bool CSliderUI::Pressed( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer)
{
	if( PtInRect( &m_rcButton, pt ) )
	{
		// Pressed while inside the control
		m_bPressed = true;
		SetCapture( hWnd );

		m_nDragX = pt.x;
		//m_nDragY = pt.y;
		m_nDragOffset = m_nButtonX - m_nDragX;

		//m_nDragValue = m_nValue;

		if( !m_bHasFocus )
			m_pParentDialog->RequestFocus( this );

		return true;
	}

	if( PtInRect( &m_rcBoundingBox, pt ) )
	{
		m_nDragX = pt.x;
		m_nDragOffset = 0;
		m_bPressed = true;

		if( !m_bHasFocus )
			m_pParentDialog->RequestFocus( this );

		if( pt.x > m_nButtonX + m_x )
		{
			SetValueInternal( m_nValue + 1, true );
			return true;
		}

		if( pt.x < m_nButtonX + m_x )
		{
			SetValueInternal( m_nValue - 1, true );
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Released() 
//-----------------------------------------------------------------------------
bool CSliderUI::Released( HWND hWnd, POINT pt)
{
	if( m_bPressed )
	{
		m_bPressed = false;
		ReleaseCapture();

		m_sliderChangedSig(this);
		//m_pDialog->SendEvent( EVENT_SLIDER_VALUE_CHANGED, true, this );

		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Dragged() 
//-----------------------------------------------------------------------------
bool CSliderUI::Dragged( POINT pt)
{
	if( m_bPressed )
	{
		SetValueInternal( ValueFromPos( m_x + pt.x + m_nDragOffset ), true );
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Scrolled() 
//-----------------------------------------------------------------------------
 bool CSliderUI::Scrolled( int nScrollAmount)
{
	if (m_bMouseOver)
	{
		//int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA;
		SetValueInternal( m_nValue - nScrollAmount, true );
		return true;
	}
	return false;
}

 //-----------------------------------------------------------------------------
 // Name : connectToSliderChg() 
 //-----------------------------------------------------------------------------
 void CSliderUI::connectToSliderChg( const signal_slider::slot_type& subscriber)
 {
	 m_sliderChangedSig.connect(subscriber);
 }

//-----------------------------------------------------------------------------
// Name : UpdateRects() 
//-----------------------------------------------------------------------------
void CSliderUI::UpdateRects()
{
	CControlUI::UpdateRects();

	m_rcButton = m_rcBoundingBox;
	m_rcButton.right = m_rcButton.left + ( m_rcButton.bottom - m_rcButton.top );
	OffsetRect( &m_rcButton, - ( m_rcButton.right - m_rcButton.left ) / 2, 0 );

	m_nButtonX = ( int )( ( m_nValue - m_nMin ) * ( float )( m_rcBoundingBox.right - m_rcBoundingBox.left ) / ( m_nMax - m_nMin ) );
	OffsetRect( &m_rcButton, m_nButtonX, 0 );
}

//-----------------------------------------------------------------------------
// Name : SaveToFile() 
//-----------------------------------------------------------------------------
bool CSliderUI::SaveToFile(std::ostream& SaveFile)
{
	CControlUI::SaveToFile(SaveFile);

	SaveFile << m_nMin << "| Slider Minimum Value" << "\n";
	SaveFile << m_nMax << "| Slider Maximum Value" << "\n";

	return true;
}

//-----------------------------------------------------------------------------
// Name : Render() 
//-----------------------------------------------------------------------------
void CSliderUI::Render( CAssetManager& assetManger )
{
	if (!m_bVisible)
		return;

	HRESULT hr;
	LPDIRECT3DTEXTURE9 pTexTrack, pTexButton;

	//no texture was given abort rendering
	if (m_elementsGFX.size() < 2 ||m_elementsGFX[TRACK].iTexture == -1 || m_elementsGFX[BUTTON].iTexture == -1)
		return;

	// acquire the sprite needed for rendering
	CMySprite* sprite = assetManger.getMySprite();

	//acquire a pointers for the textures of the Track and the button
	pTexTrack = assetManger.getTexturePtr(m_elementsGFX[TRACK].iTexture);
	pTexButton = assetManger.getTexturePtr(m_elementsGFX[BUTTON].iTexture);

	POINT dialogPos = m_pParentDialog->getLocation();

	if (m_bMouseOver)
	{
		renderRect(m_elementsGFX[TRACK].rcTexture, m_rcBoundingBox, sprite, pTexTrack, d3d::WHITE,HiGHLIGHT, dialogPos);
		renderRect(m_elementsGFX[BUTTON].rcTexture, m_rcButton, sprite, pTexButton, d3d::WHITE, HiGHLIGHT,dialogPos);
	}
	else
	{
		renderRect(m_elementsGFX[TRACK].rcTexture, m_rcBoundingBox, sprite, pTexTrack, D3DCOLOR_ARGB( 255, 200, 200, 200 ), REGLUAR, dialogPos);
		renderRect(m_elementsGFX[BUTTON].rcTexture, m_rcButton, sprite, pTexButton, D3DCOLOR_ARGB( 255, 200, 200, 200 ), REGLUAR, dialogPos);
	}
}

//-----------------------------------------------------------------------------
// Name : SetValue() 
//-----------------------------------------------------------------------------
void CSliderUI::SetValue( int nValue )
{
	SetValueInternal( nValue, false );
}

//-----------------------------------------------------------------------------
// Name : GetValue() 
//-----------------------------------------------------------------------------
int CSliderUI::GetValue() const
{
	return m_nValue;
}

//-----------------------------------------------------------------------------
// Name : GetRange() 
//-----------------------------------------------------------------------------
void CSliderUI::GetRange( int& nMin, int& nMax ) const
{
	nMin = m_nMin;
	nMax = m_nMax;
}

//-----------------------------------------------------------------------------
// Name : SetRange() 
//-----------------------------------------------------------------------------
void CSliderUI::SetRange( int nMin, int nMax )
{
	m_nMin = nMin;
	m_nMax = nMax;

	SetValueInternal( m_nValue, false );
}

//-----------------------------------------------------------------------------
// Name : SetValueInternal()
//-----------------------------------------------------------------------------
void CSliderUI::SetValueInternal( int nValue, bool bFromInput )
{
	// Clamp to range
	nValue = __max( m_nMin, nValue );
	nValue = __min( m_nMax, nValue );

	if( nValue == m_nValue )
		return;

	m_nValue = nValue;
	UpdateRects();

	m_sliderChangedSig(this);
	//m_pDialog->SendEvent( EVENT_SLIDER_VALUE_CHANGED, bFromInput, this );
}

//-----------------------------------------------------------------------------
// Name : ValueFromPos()
//-----------------------------------------------------------------------------
int CSliderUI::ValueFromPos( int x )
{
	float fValuePerPixel = ( float )( m_nMax - m_nMin ) / ( m_rcBoundingBox.right - m_rcBoundingBox.left );
	return ( int )( 0.5f + m_nMin + fValuePerPixel * ( x - m_rcBoundingBox.left ) );
}

//-----------------------------------------------------------------------------
// Name : CanHaveFocus()
//-----------------------------------------------------------------------------
bool CSliderUI::CanHaveFocus()
{
	//return  true;
	return ( m_bVisible && m_bEnabled );
}