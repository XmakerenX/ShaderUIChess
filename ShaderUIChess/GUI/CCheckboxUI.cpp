#include "CCheckboxUI.h"
#include "CDialogUI.h"

//-----------------------------------------------------------------------------
// Name : CCheckboxUI (Constructor)
//-----------------------------------------------------------------------------
CCheckboxUI::CCheckboxUI(CDialogUI* pParentDialog, int ID, int x, int y, UINT width, UINT height, UINT nHotkey)
:CButtonUI(pParentDialog, ID, "", x, y, width, height, nHotkey)
{
	m_type = CHECKBOX;
	m_bChecked = false;
}

//-----------------------------------------------------------------------------
// Name : CCheckboxUI (constructor from InputFile)
//-----------------------------------------------------------------------------
CCheckboxUI::CCheckboxUI(std::istream& inputFile)
	:CButtonUI(inputFile)
{
	m_type = CHECKBOX;

	inputFile >> m_bChecked;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
}

//-----------------------------------------------------------------------------
// Name : CCheckboxUI (destructor)
//-----------------------------------------------------------------------------
CCheckboxUI::~CCheckboxUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : Render ()
//-----------------------------------------------------------------------------
void CCheckboxUI::Render( CAssetManager& assetManger)
{
	if (m_bVisible)
	{
		HRESULT hr;
		LPDIRECT3DTEXTURE9 pTexture;
		RECT rcWindow;

		//no texture was given abort rendering
		if (m_elementsGFX.size() < 2 || m_elementsGFX[BUTTON].iTexture == -1 || m_elementsGFX[MOUSEOVER].iTexture == -1)
			return;

		//acquire a pointer to the texture we need to render the button
		pTexture = assetManger.getTexturePtr(m_elementsGFX[0].iTexture);

		CMySprite* sprite = assetManger.getMySprite();

		//calculate the the button rendering rect
		SetRect(&rcWindow, 0, 0, m_width, m_height);
		OffsetRect(&rcWindow, m_x, m_y);

		POINT dialogPos = m_pParentDialog->getLocation();
		LONG  dialogCaptionHeihgt =  m_pParentDialog->getCaptionHeight();
		dialogPos.y += dialogCaptionHeihgt;

		if (!m_bMouseOver)
		{
			renderRect(m_elementsGFX[BUTTON].rcTexture, rcWindow, sprite, pTexture, D3DCOLOR_ARGB( 255, 200, 200, 200 ), REGLUAR, dialogPos);
			if (m_bChecked)
				renderRect(m_elementsGFX[MOUSEOVER].rcTexMouseOver, rcWindow, sprite, pTexture,  D3DCOLOR_ARGB( 255, 200, 200, 200 ), REGLUAR, dialogPos);
		}
		else
		{
			// if the button is pressed and the cursor is on it darken it to showed it is pressed
			if (m_bMouseOver && m_bPressed)
			{
				renderRect(m_elementsGFX[BUTTON].rcTexture, rcWindow, sprite, pTexture, D3DCOLOR_ARGB( 255, 150, 150, 150 ), REGLUAR, dialogPos );
				if (m_bChecked)
					renderRect(m_elementsGFX[MOUSEOVER].rcTexMouseOver, rcWindow, sprite, pTexture, D3DCOLOR_ARGB( 255, 150, 150, 150 ), REGLUAR, dialogPos);
			}
			else
				// if the button has the cursor on it high light 
				if (m_bMouseOver)
				{
					//drawButtonRect(m_controlGfx.rcTexMouseOver, rcWindow, sprite, pTexture, D3DCOLOR_ARGB( 200, 250, 250, 250 ));
					renderRect(m_elementsGFX[BUTTON].rcTexture, rcWindow, sprite, pTexture, D3DCOLOR_ARGB( 255, 255, 255, 255 ), HiGHLIGHT, dialogPos);
					if (m_bChecked)
						renderRect(m_elementsGFX[MOUSEOVER].rcTexMouseOver, rcWindow, sprite, pTexture, D3DCOLOR_ARGB( 255, 255, 255, 255 ), HiGHLIGHT, dialogPos);
				}
		}
	}
}

//-----------------------------------------------------------------------------
// Name : HandleKeyboard ()
//-----------------------------------------------------------------------------
bool CCheckboxUI::HandleKeyboard( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	//TODO: implement the HandleKeyboard and check why keyboard messages seems to not work...
	return false;
}

//-----------------------------------------------------------------------------
// Name : HandleMouse ()
//-----------------------------------------------------------------------------
bool CCheckboxUI::HandleMouse( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer )
{
	if (!m_bEnabled || !m_bVisible)
		return false;

	switch(uMsg)
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
	}

	return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed ()
//-----------------------------------------------------------------------------
bool CCheckboxUI::Pressed(  HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer)
{
	if ( ContainsPoint( pt ) )
	{
		m_bPressed = true;

		SetCapture(hWnd);

		if( !m_bHasFocus )
			m_pParentDialog->RequestFocus( this );

		return true;

	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Released ()
//-----------------------------------------------------------------------------
bool CCheckboxUI::Released( HWND hWnd, POINT pt)
{
	if (m_bPressed)
	{
		m_bPressed = false;

		ReleaseCapture();

		if (ContainsPoint(pt))
		{
			m_bChecked = !m_bChecked;
			m_clickedSig(this);
			//m_pParentDialog->SendEvent(2, true, m_ID, hWnd);
		}

		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : SaveToFile ()
//-----------------------------------------------------------------------------
bool CCheckboxUI::SaveToFile(std::ostream& SaveFile)
{
	CButtonUI::SaveToFile(SaveFile);
	SaveFile << m_bChecked << "| is CheckBox Checked" << "\n";

	return true;
}

//-----------------------------------------------------------------------------
// Name : getChecked ()
//-----------------------------------------------------------------------------
bool CCheckboxUI::getChecked()
{
	return m_bChecked;
}