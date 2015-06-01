#ifndef  _CDIALOGUI_H
#define  _CDIALOGUI_H

#include <Windows.h>
#undef min
#undef max

#include <vector>
#include <string>
#include <fstream>
#include <d3dx9.h>
#include <boost/signals2/signal.hpp>
#include <boost/bind/bind.hpp>
#include "../rendering/d3d.h"
#include "../CAssetManager.h"
#include "CControlUI.h"
#include "CButtonUI.h"
#include "CStaticUI.h"
#include "CCheckBoxUI.h"
#include "CRadioButtonUI.h"
#include "CComboBoxUI.h"
#include "CListBoxUI.h"
#include "CSliderUI.h"
#include "CEditBoxUI.h"

// the callback function that used to send back GUI events to the main program
typedef VOID ( CALLBACK*PCALLBACKGUIEVENT )(HWND hWnd, UINT nEvent, int nControlID, void* pUserContext );

struct DEF_INFO
{
	DEF_INFO::DEF_INFO(std::string newControlIDText, UINT newControlID)
	{
		controlIDText = newControlIDText;
		controlID = newControlID;
	}

	std::string controlIDText;
	UINT controlID;
};

//struct that holds the default elements for a control type
struct CONTROL_GFX
{
	CONTROL_GFX::CONTROL_GFX()
	{
		nControlType = -1;
	}

	CONTROL_GFX::CONTROL_GFX(UINT newControlType, std::vector<ELEMENT_GFX> &newElementsVec)
	{
		nControlType = newControlType;
		elementsGFXvec = newElementsVec;
	}

	UINT nControlType;
	std::vector<ELEMENT_GFX>  elementsGFXvec; 
	std::vector<ELEMENT_FONT> elementsFontVec;

};

class CDialogUI
{
public:
	typedef boost::signals2::signal<void (CControlUI*)>  signal_controlClicked;

	CDialogUI(void);
	virtual ~CDialogUI(void);

	HRESULT init	 (UINT width, UINT height, int nCaptionHeight, LPCTSTR captionText, char newTexturePath[MAX_PATH], D3DXCOLOR dialogColor, HWND hWnd, CAssetManager& assetManger);
	HRESULT initDefControlElements(CAssetManager& assetManger);
	HRESULT initWoodControlElements(CAssetManager& assetManager);

	virtual bool    MsgProc	 ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, CTimer* timer, bool windowed );
	void	OnMouseMove(POINT pt);

	void    SendEvent(UINT nEvent, bool bTriggeredByUser, int nControlID, HWND hWnd = NULL );
	void    SetCallback( PCALLBACKGUIEVENT pCallback);
	void    connectToControlRightClicked( const signal_controlClicked::slot_type& subscriber);

	virtual HRESULT OnRender   (float fElapsedTime, D3DXVECTOR3 vPos, LPD3DXEFFECT effect, CAssetManager& assetManger);

	void	UpdateRects();

	//-------------------------------------------------------------------------
	// Functions that handle the Dialog Controls
	//-------------------------------------------------------------------------
	bool			 initControl			(CControlUI* pControl);
	void			 UpdateControlDefText	(LPCTSTR strDefText, int controlID);

	bool			 addStatic				( int ID, LPCTSTR strText, int x, int y, int width, int height, CStaticUI** ppStaticCreated = NULL, LPCTSTR strID = "");
	bool			 addButton				( int ID, LPCTSTR strText, int x, int y, int width, int height, UINT nHotkey, CButtonUI** ppButtonCreated = NULL, LPCTSTR strID = "");
	bool			 addCheckBox			( int ID, int x, int y, int width, int height, UINT nHotkey, CCheckboxUI** ppCheckBoxCreated = NULL, LPCTSTR strID = "");
	bool			 addRadioButton			( int ID, int x, int y, int width, int height, UINT nHotkey, UINT nButtonGroup, CRadioButtonUI** ppRadioButtonCreated = NULL, LPCTSTR strID = "");
	bool			 addComboBox			( int ID, LPCTSTR strText, int x, int y, int width, int height, UINT nHotkey, CComboBoxUI** ppComboxCreated = NULL, LPCTSTR strID = "");
	bool		     addListBox				( int ID, int x, int y, int width, int height, DWORD style = 0, CListBoxUI** ppListBoxCreated = NULL, LPCTSTR strID = "");
	bool		     addSlider				( int ID, int x, int y, int width, int height, int min, int max, int nValue, CSliderUI** ppSliderCreated = NULL, LPCTSTR strID = "");
	bool			 addEditbox				( int ID, LPCTSTR strText, int x, int y, int width, int height, CTimer* timer, CEditBoxUI** ppEditBoxCreated = NULL, LPCTSTR strID = "");

	bool			 addStaticFromFile		( std::istream& InputFIle, CStaticUI** ppStaticCreated = NULL);
	bool			 addButtonFromFile		( std::istream& InputFIle, CButtonUI** ppButtonCreated = NULL);
	bool			 addCheckBoxFromFile    ( std::istream& InputFIle, CCheckboxUI** ppCheckBoxCreated = NULL);
	bool			 addRadioButtonFromFile ( std::istream& InputFIle, CRadioButtonUI** ppRadioButtonCreated = NULL);
	bool			 addComboBoxFromFile    ( std::istream& InputFIle, CComboBoxUI** ppComboxCreated = NULL);
	bool			 addListBoxFromFile     ( std::istream& InputFIle, CListBoxUI** ppListBoxCreated = NULL);
	bool			 addSliderFromFile      ( std::istream& InputFIle, CSliderUI** ppSliderCreated = NULL);
	bool			 addEditBoxFromFile     ( std::istream& InputFIle, CTimer* timer, CEditBoxUI** ppEditBoxCreated = NULL);

	void			 RemoveControl			( int ID);
	void			 RemoveAllControls		( );

	CControlUI     * getControl				( int ID );
	CControlUI     * getControl			    ( int ID, UINT nControlType );
	CStaticUI      * getStatic			    ( int ID );
	CButtonUI      * getButton		        ( int ID );
	CCheckboxUI    * getCheckBox			( int ID );
	CRadioButtonUI * getRadioButton			( int ID );
	CComboBoxUI	   * getComboBox			( int ID );
	CSliderUI	   * getSlider				( int ID );
	CEditBoxUI	   * getEditBox				( int ID );
	CListBoxUI	   * getListBox			    ( int ID );
	int				 getControlsNum			();
	const char	   * getControlIDText		( int ID);
	bool			 getVisible				();

	void		     ClearRadioButtonGruop	( UINT nButtonGroup);

	CControlUI	   * getControlAtPoint		( POINT pt);

	void			 RequestFocus			( CControlUI* pControl );

	static void WINAPI  ClearFocus();

	//-------------------------------------------------------------------------
	// Save/Load Functions
	//-------------------------------------------------------------------------
	bool				SaveDilaogToFile	(LPCTSTR FileName, ULONG curControlID);
	ULONG			    LoadDialogFromFile	(LPCTSTR FileName, CTimer* timer);

	//-------------------------------------------------------------------------
	// get and set Functions 
	//-------------------------------------------------------------------------
	void	setSize			 (UINT width, UINT height);
	void	setLocation		 (int x, int y);
	void	setVisible		 (bool bVisible);

	POINT   getLocation	     ();
	UINT	getWidth	     ();
	UINT	getHeight		 ();

	LONG	getCaptionHeight ();

protected:
	HWND m_hWnd; // a handle to the window the dialog sends the event messages through the callback function

private:
	RECT m_rcBoundingBox;
	RECT m_rcCaptionBox;

	int m_nCaptionHeight;
	char m_captionText[MAX_PATH];

	int  m_x, m_y; 
	UINT m_width;
	UINT m_height;
	UINT m_texWidth;
	UINT m_texHeight;

	//TODO: delete the  vars that aren't needed for the drag operation
	POINT m_startDragPos;
	POINT m_curDragPos;

	bool m_bVisible;
	bool m_bCaption;
	bool m_bMinimized;
	bool m_bDrag;

	char m_texturePath[MAX_PATH];
	D3DXCOLOR m_dialogColor;

	//CButtonUI m_button;
	std::vector<CControlUI*> m_Controls;

	std::vector<DEF_INFO> m_defInfo;

	static CControlUI* s_pControlFocus; // The control which has focus
	CControlUI* m_pMouseOverControl;

	//Default graphics for the controls elements stuff like : textures,Rects
	std::vector<CONTROL_GFX> m_DefControlsGFX;

	// Pointer to the callback event function that the dialog sends events to
	PCALLBACKGUIEVENT m_pCallbackEvent;
	boost::signals2::signal<void (CControlUI*)> m_controlRightClkSig;
};

#endif  //_CDIALOGUI_H