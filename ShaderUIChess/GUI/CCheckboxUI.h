#ifndef  _CCHECKBOXUI_H
#define  _CCHECKBOXUI_H

#include "CButtonUI.h"

class CCheckboxUI : public CButtonUI
{
public:
	//CCheckboxUI(void);
	CCheckboxUI					(CDialogUI* pParentDialog, int ID, int x, int y, UINT width, UINT height, UINT nHotkey);
	CCheckboxUI				    (std::istream& inputFile);
	virtual ~CCheckboxUI		(void);

	virtual void	Render				( CAssetManager& assetManger);

	virtual bool    HandleKeyboard		( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool    HandleMouse			( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer );

	virtual bool    Pressed				( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer);
	virtual bool    Released			( HWND hWnd, POINT pt);

	virtual bool	SaveToFile			(std::ostream& SaveFile);

	bool			getChecked			();

protected:
	bool m_bChecked;

private:
	enum ELEMENTS{BUTTON, MOUSEOVER};
};

#endif  //_CCHECKBOXUI_H

