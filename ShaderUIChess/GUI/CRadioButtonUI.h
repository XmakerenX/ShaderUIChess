#ifndef  _CRADIOBUTTONUI_H
#define  _CRADIOBUTTONUI_H

#include "CCheckboxUI.h"

class CRadioButtonUI : public CCheckboxUI
{
public:
	CRadioButtonUI		(CDialogUI* pParentDialog, int ID, int x, int y, UINT width, UINT height, UINT nHotkey, UINT nButtonGruop);
	CRadioButtonUI		(std::istream& inputFile);
	virtual ~CRadioButtonUI(void);

	virtual bool    HandleKeyboard		( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool    HandleMouse			( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer );

	virtual bool	Pressed				( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer);
	virtual bool	Released			( HWND hWnd, POINT pt);

	virtual bool	SaveToFile			(std::ostream& SaveFile);

	UINT getButtonGroup();

	void setChecked(bool bChecked);
	void setButtonGroup(UINT buttonGroup);

protected:
	UINT m_nButtonGroup;
};

#endif  //_CRADIOBUTTONUI_H

