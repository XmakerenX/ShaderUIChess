#ifndef  _CSLIDERUI_H
#define  _CSLIDERUI_H

#include "CControlUI.h"

class CSliderUI : public CControlUI
{
public:

	typedef boost::signals2::signal<void (CSliderUI*)>  signal_slider;
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CSliderUI				(CDialogUI* pParentDialog, int ID, int x, int y, int width, int height, int min, int max, int nValue );
	CSliderUI				(std::istream& inputFile);
	virtual ~CSliderUI		(void);

	virtual BOOL    ContainsPoint( POINT pt );

	//-------------------------------------------------------------------------
	// functions that handle user input to the control
	//-------------------------------------------------------------------------
	virtual bool    HandleKeyboard		( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam  );
	virtual bool    HandleMouse			( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer  );

	virtual bool	Pressed				( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer);
	virtual bool	Released			( HWND hWnd, POINT pt);
	virtual bool    Dragged				( POINT pt);
	virtual bool    Scrolled			( int nScrollAmount);

	void		    connectToSliderChg	( const signal_slider::slot_type& subscriber);

	//-------------------------------------------------------------------------
	// functions that handle Rendering
	//-------------------------------------------------------------------------
	virtual void    UpdateRects	();
	virtual bool    CanHaveFocus();

	virtual bool	SaveToFile	(std::ostream& SaveFile);

	virtual void    Render( CAssetManager& assetManger );

	void            SetValue( int nValue );
	int             GetValue() const;

	void            GetRange( int& nMin, int& nMax ) const;
	void            SetRange( int nMin, int nMax );

protected:
	void            SetValueInternal( int nValue, bool bFromInput );
	int             ValueFromPos( int x );

	int m_nValue;

	int m_nMin;
	int m_nMax;

	int m_nDragX;      // Mouse position at start of drag
	int m_nDragOffset; // Drag offset from the center of the button
	int m_nButtonX;

	bool m_bPressed;

	boost::signals2::signal<void (CSliderUI*)> m_sliderChangedSig;

	RECT m_rcButton;

private:
	enum ELEMENTS{TRACK, BUTTON};
};

#endif  //_CSLIDERUI_H
