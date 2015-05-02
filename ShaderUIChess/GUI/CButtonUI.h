#ifndef  _CBUTTONUI_H
#define  _CBUTTONUI_H

#include <windows.h>
#include <d3dx9.h>
#include "../rendering/d3d.h"
#include "../CAssetManager.h"
#include "../CTimer.h"
#include "CStaticUI.h"

class CButtonUI : public CStaticUI
{
public:
	typedef boost::signals2::signal<void (CButtonUI*)>  signal_clicked;

	//CButtonUI					(void);
	CButtonUI					(CDialogUI* pParentDialog, int ID, LPCTSTR strText, int x, int y, UINT width, UINT height, UINT nHotkey);
	CButtonUI					(std::istream& inputFile);
	virtual ~CButtonUI			(void);

	virtual void	Render				( CAssetManager& assetManger);

	virtual bool    HandleKeyboard		( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool    HandleMouse			( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer );

	virtual bool	Pressed				( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer);
	virtual bool    Released			( HWND hWnd, POINT pt);

	virtual bool    CanHaveFocus		();


	void	setHotKey					( UINT nHotKey);

	void    connectToClick				( const signal_clicked::slot_type& subscriber);
	
	virtual bool SaveToFile				(std::ostream& SaveFile);

	//void	drawButtonRect		(RECT& rcTexture, RECT& rcWindow, LPD3DXSPRITE sprite, LPDIRECT3DTEXTURE9 pTexture, D3DCOLOR color);
	//void	drawButtonRect		(RECT& rcTexture, RECT& rcWindow, CMySprite* sprite, LPDIRECT3DTEXTURE9 pTexture, D3DCOLOR color, bool bHighLight);

protected:
	UINT	    m_nHotkey;

	bool	    m_bPressed;

	boost::signals2::signal<void (CButtonUI*)> m_clickedSig;



private:
	enum ELEMENTS{BUTTON, MOUSEOVER};
};

#endif  //_CBUTTONUI_H