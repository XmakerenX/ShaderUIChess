#ifndef  _CLISTBOXUI_H
#define  _CLISTBOXUI_H

#include "CControlUI.h"
#include "CScrollBarUI.h"

//-------------------------------------------------------------------------
// Enumerators and Structures
//-------------------------------------------------------------------------
struct ListBoxItemUI
{
	char strText[256];
	void* pData;

	RECT rcActive;
	bool bSelected;
};

class CListBoxUI : public CControlUI
{
public:

	typedef boost::signals2::signal<void (CListBoxUI*)>  signal_listbox;
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CListBoxUI					(CDialogUI* pParentDialog, int ID, int x, int y, int width, int height, DWORD dwStyle);
	CListBoxUI					(std::istream& inputFile);

	virtual ~CListBoxUI			(void);

	virtual bool onInit();

	//-------------------------------------------------------------------------
	// functions that handle user Input
	//-------------------------------------------------------------------------
	virtual bool    HandleKeyboard		( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam  );
	virtual bool    HandleMouse			( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer );

	virtual bool	Pressed				( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer);
	virtual bool    Released			( HWND hWnd, POINT pt);
	virtual bool    Dragged				( POINT pt);
	virtual bool    Scrolled			( int nScrollAmount);

	void		    ConnectToItemDBLCK	( const signal_listbox::slot_type& subscriber);
	void            ConnectToListboxSel ( const signal_listbox::slot_type& subscriber);

	virtual void    Render				( CAssetManager& assetManger );
	virtual void    UpdateRects			();

	virtual bool    CanHaveFocus		();

	virtual bool	SaveToFile			(std::ostream& SaveFile);
	void			CopyItemsFrom		(CListBoxUI* sourceListBox);

	DWORD           GetStyle			() const;
	void            SetStyle			( DWORD dwStyle );
	int             GetSize				() const;

	int             GetScrollBarWidth	() const;
	void            SetScrollBarWidth	( int nWidth );
	void            SetBorder			( int nBorder, int nMargin );

	//-------------------------------------------------------------------------
	// functions that ListBox actions
	//-------------------------------------------------------------------------
	HRESULT         AddItem				( const char* strText, void* pData );
	HRESULT         InsertItem			( int nIndex, const char* strText, void* pData );
	void            RemoveItem			( int nIndex );
	void            RemoveItemByData	( void* pData );
	void            RemoveAllItems		();

	ListBoxItemUI*  GetItem				( int nIndex );
	int             GetSelectedIndex	( int nPreviousSelected = -1 );
	ListBoxItemUI*  GetSelectedItem		( int nPreviousSelected = -1 );
	void            SelectItem			( int nNewIndex );

	enum STYLE
	{
		MULTISELECTION = 1
	};

protected:
	RECT m_rcText;      // Text rendering bound
	RECT m_rcSelection; // Selection box bound

	CScrollBarUI m_ScrollBar;
	int m_nSBWidth; //Scrollbar width

	int m_nBorder;
	int m_nMargin;
	int m_nTextHeight;  // Height of a single line of text
	DWORD m_dwStyle;    // List box style
	int m_nSelected;    // Index of the selected item for single selection list box
	int m_nSelStart;    // Index of the item where selection starts (for handling multi-selection)
	bool m_bDrag;       // Whether the user is dragging the mouse to select

	std::vector<ListBoxItemUI*> m_Items;

	boost::signals2::signal<void (CListBoxUI*)> m_itemDBLCLKSig;
	boost::signals2::signal<void (CListBoxUI*)> m_listboxSelSig;
};

#endif  //_CLISTBOXUI_H