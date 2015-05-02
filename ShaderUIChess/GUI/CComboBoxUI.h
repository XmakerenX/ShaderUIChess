#ifndef  _CCOMBOBOXUI_H
#define  _CCOMBOBOXUI_H

#include "CButtonUI.h"
#include "CScrollBarUI.h"


//-------------------------------------------------------------------------
//structs for This Class.
//-------------------------------------------------------------------------
struct ComboBoxItem
{
	char strText[256];
	void* pData;

	RECT rcActive;
	bool bVisible;
};

class CComboBoxUI : public CButtonUI
{
public:
	typedef boost::signals2::signal<void (CComboBoxUI*)>  signal_comboBox;

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CComboBoxUI(CDialogUI* pParentDialog, int ID, LPCTSTR strText, int x, int y, UINT width, UINT height, UINT nHotkey);
	CComboBoxUI(std::istream& inputFile);
	virtual         ~CComboBoxUI();

	virtual bool onInit();

	//-------------------------------------------------------------------------
	//functions that handle user Input to this control
	//-------------------------------------------------------------------------
	virtual bool    HandleKeyboard		( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool    HandleMouse			( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer );
	virtual void    OnHotkey();

	virtual bool	Pressed				( HWND hWnd, POINT pt, INPUT_STATE inputState, CTimer* timer);
	virtual bool	Released			( HWND hWnd, POINT pt);
	virtual bool    Scrolled			( int nScrollAmount);
	bool			Highlight			( POINT mousePoint);

	void			ConnectToSelectChg  ( const signal_comboBox::slot_type& subscriber);

	//-------------------------------------------------------------------------
	//functions that handle control Rendering
	//-------------------------------------------------------------------------
	virtual void    Render				( CAssetManager& assetManger); 
	virtual void    UpdateRects			();

	virtual bool    CanHaveFocus		();
	virtual void    OnFocusOut			();

	virtual bool	SaveToFile			(std::ostream& SaveFile);
	void			CopyItemsFrom		(CComboBoxUI* sourceComboBox);

	//-------------------------------------------------------------------------
	//functions that handle checkBox specific properties
	//-------------------------------------------------------------------------
	HRESULT         AddItem( const char* strText, void* pData );
	void            RemoveItem( UINT index );
	void            RemoveAllItems();
	int             FindItem( const char* strText, UINT iStart = 0 );
	bool            ContainsItem( const char* strText, UINT iStart = 0 );
	void    *   	GetItemData( const char* strText );
	void    *		GetItemData( int nIndex );

	void            SetDropHeight( UINT nHeight );

	int             GetScrollBarWidth() const;
	void            SetScrollBarWidth( int nWidth );

	int             GetSelectedIndex() const;

	void*			GetSelectedData();
	ComboBoxItem*	GetSelectedItem();

	UINT            GetNumItems();
	ComboBoxItem*	GetItem( UINT index );

	HRESULT         SetSelectedByIndex( UINT index );
	HRESULT         SetSelectedByText( const char* strText );
	HRESULT         SetSelectedByData( void* pData );

protected:

	int m_iSelected;
	int m_iFocused;
	int m_nDropHeight;
	CScrollBarUI m_ScrollBar;
	int m_nSBWidth;

	UINT m_nFontHeight;

	bool m_bOpened;

	RECT m_rcText;
	RECT m_rcButton;
	RECT m_rcDropdown;
	RECT m_rcDropdownText;


	std::vector<ComboBoxItem*> m_Items;

private: 
	// the elements used to render the Combobox 
	// used to access m_elementsGFX vector and no other use
	enum ELEMENTS {MAIN, BUTTON, DROPDOWN, SELECTION }; 

	boost::signals2::signal<void (CComboBoxUI*)> m_selectionChangedSig;
};

#endif  //_CCOMBOBOXUI_H