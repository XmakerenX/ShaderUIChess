#ifndef  _CSCROLLBARUI_H
#define  _CSCROLLBARUI_H

#include "CControlUI.h"

class CScrollBarUI :
	public CControlUI
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CScrollBarUI(void);
	virtual ~CScrollBarUI(void);

	//-------------------------------------------------------------------------
	// functions that handle user Input to control
	//-------------------------------------------------------------------------
	virtual bool    HandleKeyboard( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool    HandleMouse	  ( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer);
	
	virtual bool    Pressed		  ( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer);
	virtual bool	Released	  ( HWND hWnd, POINT pt);
	virtual bool    Dragged		  ( POINT pt);

	//virtual bool	SaveToFile	  (std::ostream& SaveFile);

	// TODO: add support for controls to have access to other messages than keyboard and mouse inputs..
	// with new slots and signal system could be no need for MsgProc...
	//virtual bool    MsgProc		  ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual void    Render( CAssetManager& assetManger); 

	//-------------------------------------------------------------------------
	// functions that handle control specific properties
	//-------------------------------------------------------------------------
	void            SetTrackRange( int nStart, int nEnd );
	void            SetTrackPos( int nPosition );

	int             GetTrackPos();
	int             GetPageSize();

	void            SetPageSize( int nPageSize );

	void            Scroll( int nDelta );    // Scroll by nDelta items (plus or minus)
	void            ShowItem( int nIndex );  // Ensure that item nIndex is displayed, scroll if necessary

private:
	// ARROWSTATE indicates the state of the arrow buttons.
	// CLEAR            No arrow is down.
	// CLICKED_UP       Up arrow is clicked.
	// CLICKED_DOWN     Down arrow is clicked.
	// HELD_UP          Up arrow is held down for sustained period.
	// HELD_DOWN        Down arrow is held down for sustained period.
	enum ARROWSTATE { CLEAR, CLICKED_UP, CLICKED_DOWN, HELD_UP, HELD_DOWN};

	enum ELEMENTS {TRACK, UPBUTTON, DOWNBUTTON, THUMB };

protected:

	virtual void    UpdateRects();
	void            UpdateThumbRect();
	void            Cap();  // Clips position at boundaries. Ensures it stays within legal range.

	bool m_bShowThumb;	 // would only be true if there is more items than the size of a single page
	bool m_bDrag;		 // true only while the thumb is being dragged
	RECT m_rcElements[4];// the Rects for the scrollbar elements
	//RECT m_rcUpButton;	 // the Rect of the up arrow
	//RECT m_rcDownButton; // the Rect of the down arrow
	//RECT m_rcTrack;		 // the Rect of the Scrollbar background
	//RECT m_rcThumb;		 // the Rect of the Thumb
	int m_nPosition;	 // Position of the first displayed item currently being shown
	int m_nPageSize;	 // How many items are displayable in one page
	int m_nStart;		 // First item index
	int m_nEnd;			 // The index after the last item
	int m_thumbOffsetY;
	POINT m_LastMouse;	 // Last mouse position
	ARROWSTATE m_Arrow;  // State of the arrows
	double m_dArrowTS;   // Timestamp of last arrow event.

	// Minimum scroll bar thumb size
	static const unsigned char  SCROLLBAR_MINTHUMBSIZE;
	// Delay and repeat period when clicking on the scroll bar arrows
	static const float SCROLLBAR_ARROWCLICK_DELAY;  		   
	static const float SCROLLBAR_ARROWCLICK_REPEAT;       


};
#endif  //_CSCROLLBARUI_H