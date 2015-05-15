#include "CEditDialogUI.h"

//-----------------------------------------------------------------------------
// Name : CEditDialogUI (constructor)
//-----------------------------------------------------------------------------
CEditDialogUI::CEditDialogUI(CTimer* timer)
{
	m_timer = timer;
	
	m_GenControlNum = 0;
	m_curControlID = IDC_GENCONTROLID + m_GenControlNum;
	m_controlInCreation = false;
	m_controlRelocate= false;
	m_pCurSelectedControl = nullptr;
}

//-----------------------------------------------------------------------------
// Name : CEditDialogUI (destructor)
//-----------------------------------------------------------------------------
CEditDialogUI::~CEditDialogUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : MsgProc ()
//-----------------------------------------------------------------------------
bool CEditDialogUI::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, CTimer* timer, bool windowed )
{
	// Send the message first for the generated dialog
	if (m_GenDialog.MsgProc(hWnd,uMsg, wParam, lParam, timer, windowed))
		return 0;
	else
	{
		switch (uMsg)
		{

		case WM_LBUTTONDOWN:
			{
				//set new control in the his location
				if (m_controlInCreation)
				{
					POINT genDailogLoc = m_GenDialog.getLocation();
					genDailogLoc.y += m_GenDialog.getCaptionHeight();
					int x = (int)LOWORD(lParam) - genDailogLoc.x;
					int y = (int)HIWORD(lParam) - genDailogLoc.y;
					m_GenDialog.getControl(m_curControlID + 1)->setLocation(x, y);
					m_GenDialog.getControl(m_curControlID + 1)->setEnabled(true);
					m_controlInCreation = false;
					m_GenControlNum++;
					m_curControlID++;
				}

				// relocate the selected control to his new location
				if (m_controlRelocate && m_pCurSelectedControl)
				{
					POINT mousePoint;
					POINT dialogPoint = m_GenDialog.getLocation();
					mousePoint.x = (int)LOWORD(lParam) - dialogPoint.x;
					mousePoint.y = (int)HIWORD(lParam) - ( dialogPoint.y + m_GenDialog.getCaptionHeight() );
					//ClientToScreen(m_hWnd,&mousePoint);

					m_pCurSelectedControl->setLocation(mousePoint.x,mousePoint.y);
					m_pCurSelectedControl->setEnabled(true);
					m_controlRelocate = false;
				}
			}break;

		case WM_MOUSEMOVE:
			{
				// move the control being created with the mouse
				if (m_controlInCreation)
				{		
					POINT genDialogLog = m_GenDialog.getLocation();
					genDialogLog.y += m_GenDialog.getCaptionHeight();
					int x = (int)LOWORD(lParam);
					int y = (int)HIWORD(lParam);
					m_GenDialog.getControl(m_curControlID + 1)->setLocation( x - genDialogLog.x, y - genDialogLog.y);
				}

				// move the control being relocated with the mouse
				if (m_controlRelocate && m_pCurSelectedControl)
				{
					POINT mousePoint;
					POINT dialogPoint = m_GenDialog.getLocation();
					dialogPoint.y += m_GenDialog.getCaptionHeight();

					mousePoint.x = (int)LOWORD(lParam);
					mousePoint.y = (int)HIWORD(lParam);
					//ClientToScreen(hWnd,&mousePoint);

					m_pCurSelectedControl->setLocation(mousePoint.x - dialogPoint.x ,mousePoint.y - dialogPoint.y );
				}
			}break;
		case WM_RBUTTONDOWN:
			{
				// if RightButton was pressed but not handled by Gendailog then no control 
				// in it was pressed , reset to defualt ui state
				getButton(IDC_CREATECONTROL)->setVisible(true);
				getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
				getButton(IDC_RELOCATEBUTTON)->setEnabled(false);
			}break;
		}

		// do Regular Dialog Message handling
		return CDialogUI::MsgProc(hWnd,uMsg,wParam,lParam,timer, windowed);
	}
}

//-----------------------------------------------------------------------------
// Name : OnRender ()
//-----------------------------------------------------------------------------
HRESULT CEditDialogUI::OnRender(float fElapsedTime, D3DXVECTOR3 vPos, LPD3DXEFFECT effect, CAssetManager& assetManger)
{
	HRESULT hr = CDialogUI::OnRender(fElapsedTime, vPos, effect, assetManger);

	if (hr == S_OK)
		return m_GenDialog.OnRender(fElapsedTime, vPos, effect, assetManger);
	else
		return hr;

}

//-----------------------------------------------------------------------------
// Name : CreateDialogGUI ()
//-----------------------------------------------------------------------------
void CEditDialogUI::CreateDialogUI(CAssetManager& assetManager)
{
	RemoveAllControls();

	//-----------------------------------------------------------------------------
	// Dialog initialization
	//-----------------------------------------------------------------------------
	//setLocation(550,0);

	addStatic(IDC_CONTROLTYPESTATIC, "Control Type", 125, 0, 200, 24);

	CComboBoxUI* pComboBox;
	addComboBox(IDC_COMBOX, "", 125, 25, 200, 24, 0, &pComboBox);
	pComboBox->AddItem("Static", (void*)CControlUI::STATIC );
	pComboBox->AddItem("Button", (void*)CControlUI::BUTTON);
	pComboBox->AddItem("CheckBox", (void*)CControlUI::CHECKBOX);
	pComboBox->AddItem("RadioButton", (void*)CControlUI::RADIOBUTTON);
	pComboBox->AddItem("ComboBox", (void*)CControlUI::COMBOBOX);
	pComboBox->AddItem("Slider", (void*)CControlUI::SLIDER);
	pComboBox->AddItem("ListBox", (void*)CControlUI::LISTBOX);
	pComboBox->AddItem("EditBox", (void*)CControlUI::EDITBOX);

	pComboBox->ConnectToSelectChg( boost::bind(&CEditDialogUI::ComboboxSelChg, this, _1) );

	addStatic(IDC_WIDTHSTATIC, "Width", 125, 50, 60, 24);
	addStatic(IDC_HEIGHTSTATIC, "Height", 265, 50, 60, 24);

	addEditbox(IDC_WIDTHEDITBOX, "", 125, 75, 70, 32, m_timer);
	addEditbox(IDC_HEIGHTEDITBOX, "", 255, 75, 70, 32, m_timer);

	addStatic(IDC_IDSTATIC, "Control ID",125, 110, 200, 24);
	addEditbox(IDC_IDEDITBOX, "IDC_", 125, 134, 200, 34, m_timer);

	addStatic(IDC_CONTROLTEXT, "Control Text", 125, 168, 200, 34);
	addEditbox(IDC_TEXTEDITBOX, "", 125, 202, 200, 34, m_timer);

	//-----------------------------------------------------------------------------
	// Radio button menu initialization
	//-----------------------------------------------------------------------------
	CStaticUI* pRadioLabel = nullptr;
	CEditBoxUI* pRadioEditbox = nullptr;
	addStatic(IDC_RADIOGROUPTEXT, "Radio Button Group", 125, 236, 200, 34, &pRadioLabel);
	addEditbox(IDC_RADIOBUTTONGROUP, "0", 125, 270, 30, 34, m_timer, &pRadioEditbox);

	pRadioLabel->setVisible(false);
	pRadioEditbox->setVisible(false);

	//-----------------------------------------------------------------------------
	// Slider menu initialization
	//-----------------------------------------------------------------------------
	CStaticUI* pSliderLabel = nullptr;
	CEditBoxUI* pSliderMinEditbox = nullptr;
	CEditBoxUI* pSliderMaxEditbox = nullptr;

	addStatic(IDC_SLIDERSTATIC, "Slider Range", 125, 236, 200, 34, &pSliderLabel);
	addEditbox(IDC_SLIDERMINEDITBOX, "0", 125, 270, 50, 34, m_timer, &pSliderMinEditbox);
	addEditbox(IDC_SLIDERMAXEDITBOX, "100", 275, 270, 50, 34, m_timer, &pSliderMaxEditbox);

	pSliderLabel->setVisible(false);
	pSliderMinEditbox->setVisible(false);
	pSliderMaxEditbox->setVisible(false);

	//-----------------------------------------------------------------------------
	// listbox menu initialization
	//-----------------------------------------------------------------------------
	CListBoxUI* pListBoxNewItems = nullptr;
	CStaticUI* pListBoxStatic = nullptr;
	CEditBoxUI* pListBoxEditBox = nullptr;
	CButtonUI* pAddItem = nullptr;
	CButtonUI* pRemoveItem = nullptr;

	addListBox(IDC_LISTBOXITEMS, 125, 190, 200, 170, 0, &pListBoxNewItems);
	addStatic(IDC_LISTBOXSTATIC, "ListBox Item text", 125, 341, 200, 34, &pListBoxStatic);
	addEditbox(IDCLISTOXEDITBOX,"", 125, 380, 200, 34, m_timer, &pListBoxEditBox);
	addButton(IDC_LISTBOXITEMSADD, "Add Item", 125, 424, 85, 34, 0, &pAddItem);
	addButton(IDC_LISTBOXITEMSREMOVE, "Remove Item", 220, 424, 105, 34, 0, &pRemoveItem);

	pListBoxNewItems->setVisible(false);
	pListBoxStatic->setVisible(false);
	pListBoxEditBox->setVisible(false);
	pAddItem->setVisible(false);
	pRemoveItem->setVisible(false);

	pAddItem->connectToClick( boost::bind(&CEditDialogUI::AddListBoxItemClicked, this, _1) );
	pRemoveItem->connectToClick( boost::bind(&CEditDialogUI::RemoveListBoxItemClikced, this, _1) );

	//-----------------------------------------------------------------------------
	// Combobox menu initialization
	//-----------------------------------------------------------------------------
	CComboBoxUI* pComboboxNewItems = nullptr;
	CButtonUI* pComboAddItems = nullptr;
	CButtonUI* pComboRemoveItems = nullptr;


	addComboBox(IDC_COMBOXITEMS,"", 125, 190, 200, 95, 0, &pComboboxNewItems);
	addButton(IDC_COMBOBOXITEMSADD, "Add Items", 125, 424, 85, 34, 0, &pComboAddItems);
	addButton(IDC_COMBOBOXITEMSREMOVE, "Remove Items", 220, 424, 105, 34, 0, &pComboRemoveItems);

	pComboboxNewItems->setVisible(false);
	pComboAddItems->setVisible(false);
	pComboRemoveItems->setVisible(false);

	pComboAddItems->connectToClick( boost::bind(&CEditDialogUI::AddComboBoxItemClicked, this, _1) );
	pComboRemoveItems->connectToClick( boost::bind(&CEditDialogUI::RemoveComboBoxItemClicked, this, _1));

	//-----------------------------------------------------------------------------
	// The end of Dialog initialization and creation of create control button
	//-----------------------------------------------------------------------------
	CButtonUI* pCreateControlButton = nullptr;
	CButtonUI* pSetChangesButton = nullptr;

	addButton(IDC_CREATECONTROL, "Create Control",150, 310, 150, 25, 0, &pCreateControlButton);
	pCreateControlButton->connectToClick( boost::bind(&CEditDialogUI::CreateControlClicked, this, _1) );

	addButton(IDC_SETCHANGESBUTTON, "Set Changes", 150, 310, 150, 25,0, &pSetChangesButton);
	pSetChangesButton->connectToClick( boost::bind(&CEditDialogUI::SetChangesButtonClicked, this, _1) );
	pSetChangesButton->setVisible(false);


	//-----------------------------------------------------------------------------
	// Dialog resize menu creation
	//-----------------------------------------------------------------------------
	CStaticUI* pDialogStatic = nullptr;
	CEditBoxUI* pDialogWidth = nullptr;
	CEditBoxUI* pDialogHeight = nullptr;
	CButtonUI* pDialogSet = nullptr;

	addStatic(IDC_DIALOGSTATIC,"Dialog Size", 125, 480, 200, 60, &pDialogStatic);
	addEditbox(IDC_DIALOGWIDTH, "", 125, 530, 50, 34, m_timer, &pDialogWidth);
	addEditbox(IDC_DIALOGHEIGHT, "", 275, 530, 50, 34, m_timer, &pDialogHeight );
	addButton(IDC_DIALOGSETSIZE, "Set Size", 150, 569, 150, 25, 0,&pDialogSet);

	pDialogSet->connectToClick( boost::bind(&CEditDialogUI::SetGenDialogSize, this, _1) );

	//-----------------------------------------------------------------------------
	// Dialog resize menu creation
	//-----------------------------------------------------------------------------
	CStaticUI* pFileNameStatic = nullptr;
	CEditBoxUI* pFileNameEditBox = nullptr;
	CButtonUI* pLoadFileButton = nullptr;
	CButtonUI* pSaveFilButton = nullptr;

	addStatic(IDC_FILENAMESTATIC,"FIle Name", 125, 579, 200, 60, &pFileNameStatic);
	addEditbox(IDC_FILENAMEEDITBOX, "", 125, 624, 200, 34, m_timer, &pFileNameEditBox);
	addButton(IDC_LOADFILEBUTTON, "Load",125, 663, 70, 34, 0, &pLoadFileButton);
	addButton(IDC_SAVEFILEBUTTON, "Save", 255, 663,70, 34, 0, &pSaveFilButton);

	pLoadFileButton->connectToClick( boost::bind(&CEditDialogUI::LoadDialogButtonClicked, this, _1) );
	pSaveFilButton->connectToClick( boost::bind(&CEditDialogUI::SaveDialogButtonClicked, this, _1) );

	CEditBoxUI* pControlXEditBox = nullptr;
	CEditBoxUI* pControlYEditBox = nullptr;

	addStatic(IDC_CONTROLXSTATIC, "X", 330, 592, 50, 34);
	addStatic(IDC_CONTROLYSTATIC, "Y", 385, 592, 50, 34);

	addEditbox(IDC_CONTROLX, "", 330, 625, 50, 34, m_timer, &pControlXEditBox);
	addEditbox(IDC_CONTROLY, "", 385, 625, 50, 34, m_timer, &pControlYEditBox);

	CButtonUI* pRelocateButton = nullptr;
	addButton(IDC_RELOCATEBUTTON, "Relocate Control", 330, 553, 100, 34, 0, &pRelocateButton);
	pRelocateButton->connectToClick( boost::bind(&CEditDialogUI::RelocateControlClicked, this, _1) );
	pRelocateButton->setEnabled(false);

	CButtonUI* pDeleteButton = nullptr;
	addButton(IDC_DELETEBUTTON, "Delete Contorl", 330, 500, 100, 34, 0, &pDeleteButton);
	pDeleteButton->connectToClick( boost::bind(&CEditDialogUI::DeleteControlClicked, this, _1) );
	pDeleteButton->setEnabled(false);

	CButtonUI* pOptionsButton = nullptr;
	addButton(IDC_OPTIONSBUTTON, "Options", 450, 75, 100, 34, 0, &pOptionsButton);
	pOptionsButton->connectToClick( boost::bind(&CEditDialogUI::OptionsControlClicked, this, _1) );
	pOptionsButton->setEnabled(true);

	//-----------------------------------------------------------------------------
	// Dialog initialization of the generated Dialog
	//-----------------------------------------------------------------------------
	m_GenDialog.init(500,200, 18,"Gendialog", "dialog.png", D3DCOLOR_ARGB(200,255,255,255), m_hWnd, assetManager);
	m_GenDialog.setLocation(0, 50);
}

//-----------------------------------------------------------------------------
// Name : GenControlRightClicked ()
//-----------------------------------------------------------------------------
void CEditDialogUI::GenControlRightClicked(CControlUI* pRClickedControl)
{
	getComboBox(IDC_COMBOX)->SetSelectedByIndex(pRClickedControl->getType());

	long long controlWidth = pRClickedControl->getWidth();
	long long controlHeight = pRClickedControl->getHeight();

	getEditBox(IDC_WIDTHEDITBOX)->SetText( std::to_string( controlWidth ).c_str() );
	getEditBox(IDC_HEIGHTEDITBOX)->SetText( std::to_string( controlHeight ).c_str() );

	const char* pControlIDText = m_GenDialog.getControlIDText(pRClickedControl->getID()); 
	getEditBox(IDC_IDEDITBOX)->SetText(pControlIDText);

	long long ControlX = pRClickedControl->getX();
	long long ControlY = pRClickedControl->getY();

	getEditBox(IDC_CONTROLX)->SetText( std::to_string(ControlX).c_str() );
	getEditBox(IDC_CONTROLY)->SetText( std::to_string(ControlY).c_str() );

	switch (pRClickedControl->getType())
	{
	case CControlUI::STATIC:
	case CControlUI::BUTTON:
	case CControlUI::EDITBOX:
	case CControlUI::CHECKBOX:
		{
			getEditBox(IDC_TEXTEDITBOX)->SetText( static_cast<CStaticUI*>(pRClickedControl)->getText() );
			SetStaticGUI(true);

		}break;
	case CControlUI::RADIOBUTTON:
		{
			getEditBox(IDC_TEXTEDITBOX)->SetText( static_cast<CStaticUI*>(pRClickedControl)->getText() );

			long long controlButtonGroup = static_cast<CRadioButtonUI*>(pRClickedControl)->getButtonGroup();
			getEditBox(IDC_RADIOBUTTONGROUP)->SetText( std::to_string(controlButtonGroup).c_str() );

			SetRadioButtonGUI(true);
		}break;
	case CControlUI::SLIDER:
		{
			int  sliderMin;
			int  sliderMax;

			static_cast<CSliderUI*>(pRClickedControl)->GetRange(sliderMin, sliderMax);

			getEditBox(IDC_SLIDERMINEDITBOX)->SetText( std::to_string( static_cast<long long>( sliderMin ) ).c_str() );
			getEditBox(IDC_SLIDERMAXEDITBOX)->SetText( std::to_string( static_cast<long long>( sliderMax ) ).c_str() );

			SetSliderGUI(true);
		}break;
	case CControlUI::LISTBOX:
		{
			getListBox(IDC_LISTBOXITEMS)->CopyItemsFrom( static_cast<CListBoxUI*>(pRClickedControl) );
			getEditBox(IDCLISTOXEDITBOX)->SetText("");

			SetListBoxGUI(true);

		}break;
	case CControlUI::COMBOBOX:
		{
			getComboBox(IDC_COMBOXITEMS)->CopyItemsFrom( static_cast<CComboBoxUI*>(pRClickedControl) );
			getEditBox(IDCLISTOXEDITBOX)->SetText("");

			SetComboBoxGUI(true);
		}break;
	}

	m_pCurSelectedControl = pRClickedControl;
	getButton(IDC_RELOCATEBUTTON)->setEnabled(true);

}


//-----------------------------------------------------------------------------
// Name : CreateControlClicked ()
//-----------------------------------------------------------------------------
void CEditDialogUI::CreateControlClicked(CButtonUI* createControl)
{
	POINT cursorPoint;
	LPCTSTR pControlText;
	LPCTSTR pControlIDText;

	ULONG selectedItem = (ULONG)getComboBox(IDC_COMBOX)->GetSelectedData();
	UINT  controlWidth =  atoi( getEditBox(IDC_WIDTHEDITBOX)->GetText() );
	UINT  controlHeight = atoi( getEditBox(IDC_HEIGHTEDITBOX)->GetText() );

	pControlText = getEditBox(IDC_TEXTEDITBOX)->GetText();
	pControlIDText = getEditBox(IDC_IDEDITBOX)->GetText();

	GetCursorPos( &cursorPoint );
	ScreenToClient( m_hWnd, &cursorPoint );

	switch(selectedItem)
	{
	case CControlUI::BUTTON:
		{
			m_GenDialog.addButton(m_curControlID + 1, pControlText, cursorPoint.x,
				cursorPoint.y, controlWidth, controlHeight, 0, nullptr, pControlIDText);
			m_controlInCreation = true;
		}break;

	case CControlUI::CHECKBOX:
		{
			m_GenDialog.addCheckBox(m_curControlID + 1, cursorPoint.x, cursorPoint.y,
				controlWidth, controlHeight, 0, nullptr, pControlIDText);
			m_controlInCreation = true;
		}break;

	case CControlUI::RADIOBUTTON:
		{
			m_GenDialog.addRadioButton(m_curControlID + 1, cursorPoint.x,
				cursorPoint.y, controlWidth, controlHeight, 0, 0, nullptr, pControlIDText);
			m_controlInCreation = true;
		}break;

	case CControlUI::COMBOBOX:
		{
			m_GenDialog.addComboBox(m_curControlID + 1, pControlText, cursorPoint.x,
				cursorPoint.y, controlWidth, controlHeight, 0, nullptr, pControlIDText);

			UINT comboboxSize = getComboBox(IDC_COMBOXITEMS)->GetNumItems();

			for (UINT itemIndex = 0; itemIndex < comboboxSize; itemIndex++)
			{
				ComboBoxItem* pCurItem = getComboBox(IDC_COMBOXITEMS)->GetItem(itemIndex);
				m_GenDialog.getComboBox(m_curControlID + 1)->
					AddItem(pCurItem->strText, pCurItem->pData);
			}

			getComboBox(IDC_COMBOXITEMS)->RemoveAllItems();

			m_controlInCreation = true;
		}break;

	case CControlUI::STATIC:
		{
			m_GenDialog.addStatic(m_curControlID + 1, pControlText, cursorPoint.x,
				cursorPoint.y, controlWidth, controlHeight, nullptr, pControlIDText);
			m_controlInCreation = true;
		}break;

	case CControlUI::EDITBOX:
		{
			m_GenDialog.addEditbox(m_curControlID + 1, pControlText, cursorPoint.x,
				cursorPoint.y, controlWidth, controlHeight, m_timer, nullptr, pControlIDText);
			m_controlInCreation = true;
		}break;

	case CControlUI::LISTBOX:
		{
			m_GenDialog.addListBox(m_curControlID + 1, cursorPoint.x, cursorPoint.y,
				controlWidth, controlHeight, 0, nullptr, pControlIDText);

			UINT listboxSize = getListBox(IDC_LISTBOXITEMS)->GetSize();

			for (UINT itemIndex = 0; itemIndex < listboxSize; itemIndex++)
			{
				ListBoxItemUI* pCurItem = getListBox(IDC_LISTBOXITEMS)->GetItem(itemIndex);
				m_GenDialog.getListBox(m_curControlID + 1)->
					AddItem(pCurItem->strText, pCurItem->pData);
			}

			getListBox(IDC_LISTBOXITEMS)->RemoveAllItems();

			m_controlInCreation = true;
		}break;

	case CControlUI::SLIDER:
		{
			int minValue = atoi ( getEditBox(IDC_SLIDERMINEDITBOX)->GetText() );
			int maxValue = atoi ( getEditBox(IDC_SLIDERMAXEDITBOX)->GetText() );

			m_GenDialog.addSlider(m_curControlID + 1, cursorPoint.x, cursorPoint.y,
				controlWidth, controlHeight, minValue, maxValue, (maxValue - minValue) / 2, nullptr, pControlIDText);
			m_controlInCreation = true;
		}break;
	}

	if (m_controlInCreation)
		// Temporally disabling the control to avoid it picking up messages before it is in place.
		m_GenDialog.getControl(m_curControlID + 1)->setEnabled(false);
}

//-----------------------------------------------------------------------------
// Name : AddListBoxItemClicked ()
//-----------------------------------------------------------------------------
void CEditDialogUI::AddListBoxItemClicked(CButtonUI* pAddListBoxItemButton)
{
	std::string itemText = getEditBox(IDCLISTOXEDITBOX)->GetText();
	getListBox(IDC_LISTBOXITEMS)->AddItem(itemText.c_str(), (int)0);
}

//-----------------------------------------------------------------------------
// Name : RemoveListBoxItemClikced ()
//-----------------------------------------------------------------------------
void CEditDialogUI::RemoveListBoxItemClikced(CButtonUI* pRecloateControlButton)
{
	UINT itemIndex = getListBox(IDC_LISTBOXITEMS)->GetSelectedIndex();
	getListBox(IDC_LISTBOXITEMS)->RemoveItem(itemIndex);
}

//-----------------------------------------------------------------------------
// Name : AddComboBoxItemClicked ()
//-----------------------------------------------------------------------------
void CEditDialogUI::AddComboBoxItemClicked(CButtonUI* pAddComboBoxItemButton)
{
	std::string itemText = getEditBox(IDCLISTOXEDITBOX)->GetText();
	getComboBox(IDC_COMBOXITEMS)->AddItem(itemText.c_str(), (int)0);
}

//-----------------------------------------------------------------------------
// Name : RemoveComboBoxItemClicked ()
//-----------------------------------------------------------------------------
void CEditDialogUI::RemoveComboBoxItemClicked(CButtonUI* pRemoveComboBoxItemButton)
{
	UINT itemIndex = getComboBox(IDC_COMBOXITEMS)->GetSelectedIndex();
	getComboBox(IDC_COMBOXITEMS)->RemoveItem(itemIndex);
}

//-----------------------------------------------------------------------------
// Name : SetChangesButtonClicked ()
//-----------------------------------------------------------------------------
void CEditDialogUI::SetChangesButtonClicked(CButtonUI* pSetChangesButton)
{
	UINT controlWidth = atoi( getEditBox(IDC_WIDTHEDITBOX)->GetText() );
	UINT controlHeight = atoi( getEditBox(IDC_HEIGHTEDITBOX)->GetText() );

	int controlX = atoi( getEditBox(IDC_CONTROLX)->GetText() );
	int controlY = atoi( getEditBox(IDC_CONTROLY)->GetText() );

	m_pCurSelectedControl->setLocation(controlX, controlY);
	m_pCurSelectedControl->setSize(controlWidth, controlHeight);

	ULONG selectedItem = (ULONG)getComboBox(IDC_COMBOX)->GetSelectedData();

	switch(selectedItem)
	{
	case CControlUI::STATIC:
	case CControlUI::BUTTON:
	case CControlUI::EDITBOX:
	case CControlUI::CHECKBOX:
		{
			LPCTSTR pControlText = getEditBox(IDC_TEXTEDITBOX)->GetText();
			static_cast<CStaticUI*>(m_pCurSelectedControl)->setText(pControlText);
		}break;

	case CControlUI::RADIOBUTTON:
		{
			LPCTSTR pControlText = getEditBox(IDC_TEXTEDITBOX)->GetText();
			static_cast<CRadioButtonUI*>(m_pCurSelectedControl)->setText(pControlText);

			UINT buttonGroup = atoi( getEditBox(IDC_RADIOBUTTONGROUP)->GetText() ) ;
			static_cast<CRadioButtonUI*>(m_pCurSelectedControl)->setButtonGroup(buttonGroup);
		}break;

	case CControlUI::SLIDER:
		{
			int sliderMin = atoi( getEditBox(IDC_SLIDERMINEDITBOX)->GetText() );
			int sliderMax = atoi( getEditBox(IDC_SLIDERMAXEDITBOX)->GetText() );

			static_cast<CSliderUI*>(m_pCurSelectedControl)->SetRange(sliderMin, sliderMax);
		}break;

	case CControlUI::LISTBOX:
		{
			static_cast<CListBoxUI*>(m_pCurSelectedControl)->CopyItemsFrom( getListBox(IDC_LISTBOXITEMS) );
		}break;

	case CControlUI::COMBOBOX:
		{
			static_cast<CComboBoxUI*>(m_pCurSelectedControl)->CopyItemsFrom( getComboBox(IDC_COMBOXITEMS) );
		}break;
	}

	LPCTSTR pControlIDText = getEditBox(IDC_IDEDITBOX)->GetText();

	m_GenDialog.UpdateControlDefText( pControlIDText, m_pCurSelectedControl->getID());
}

//-----------------------------------------------------------------------------
// Name : SaveDialogButtonClicked ()
//-----------------------------------------------------------------------------
void CEditDialogUI::SaveDialogButtonClicked(CButtonUI* pSaveButton)
{
	m_GenDialog.SaveDilaogToFile( getEditBox(IDC_FILENAMEEDITBOX)->GetText(), m_curControlID );
}

//-----------------------------------------------------------------------------
// Name : LoadDialogButtonClicked ()
//-----------------------------------------------------------------------------
void CEditDialogUI::LoadDialogButtonClicked(CButtonUI* pLoadButton)
{
	m_curControlID = m_GenDialog.LoadDialogFromFile( getEditBox(IDC_FILENAMEEDITBOX)->GetText() , m_timer);
	m_GenControlNum = m_GenDialog.getControlsNum();
}

//-----------------------------------------------------------------------------
// Name : SetGenDialogSize ()
//-----------------------------------------------------------------------------
void CEditDialogUI::SetGenDialogSize(CButtonUI* pDialogSetButton)
{
	UINT dialogWidth = atoi( getEditBox(IDC_DIALOGWIDTH)->GetText() );
	UINT dialogHeight = atoi( getEditBox(IDC_DIALOGHEIGHT)->GetText() );

	m_GenDialog.setSize(dialogWidth, dialogHeight);
}

//-----------------------------------------------------------------------------
// Name : RelocateControlClicked ()
//-----------------------------------------------------------------------------
void CEditDialogUI::RelocateControlClicked(CButtonUI* pRecloateControlButton)
{
	POINT mousePoint; 
	POINT dialogPoint = m_GenDialog.getLocation();
	dialogPoint.y +=  m_GenDialog.getCaptionHeight();
	mousePoint.x = m_pCurSelectedControl->getX() + dialogPoint.x;
	mousePoint.y = m_pCurSelectedControl->getY() + dialogPoint.y;
	ClientToScreen(m_hWnd,&mousePoint);

	SetCursorPos(mousePoint.x, mousePoint.y);
	// Temporally disabled the control to prevent it form processing messages till it is placed.
	m_pCurSelectedControl->setEnabled(false);
	m_controlRelocate = true;
}

//-----------------------------------------------------------------------------
// Name : DeleteControlClicked ()
//-----------------------------------------------------------------------------
void CEditDialogUI::DeleteControlClicked(CButtonUI* pDeleteButton)
{
	m_GenDialog.RemoveControl(m_pCurSelectedControl->getID());
	m_pCurSelectedControl = nullptr;

	getButton(IDC_CREATECONTROL)->setVisible(true);
	getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
	getButton(IDC_RELOCATEBUTTON)->setEnabled(false);
	getButton(IDC_DELETEBUTTON)->setEnabled(false);
}

//-----------------------------------------------------------------------------
// Name : OptionsControlClicked ()
//-----------------------------------------------------------------------------
void CEditDialogUI::OptionsControlClicked(CButtonUI* pOptionsButton)
{
	setVisible(true);
}

//-----------------------------------------------------------------------------
// Name : ComboboxSelChg ()
//-----------------------------------------------------------------------------
void CEditDialogUI::ComboboxSelChg(CComboBoxUI* pCombobox)
{
	ComboBoxItem* pSelectedItem =  pCombobox->GetSelectedItem();

	UINT selectedItem = (UINT)pSelectedItem->pData;

	switch(selectedItem)
	{
	case CControlUI::STATIC:
	case CControlUI::BUTTON:
	case CControlUI::EDITBOX:
	case CControlUI::CHECKBOX:
		{
			SetStaticGUI();
		}break;
	case CControlUI::RADIOBUTTON:
		{
			SetRadioButtonGUI();
		}break;
	case CControlUI::SLIDER:
		{
			SetSliderGUI();
		}break;
	case CControlUI::LISTBOX:
		{
			SetListBoxGUI();
		}break;

	case CControlUI::COMBOBOX:
		{
			SetComboBoxGUI();
		}
	}
}

//-----------------------------------------------------------------------------
// Name : ComboboxSelChg ()
//-----------------------------------------------------------------------------
void CEditDialogUI::SetStaticGUI(bool ControlSelected /* = false */)
{
	getStatic(IDC_RADIOGROUPTEXT)->setVisible(false);
	getEditBox(IDC_RADIOBUTTONGROUP)->setVisible(false);
	getStatic(IDC_SLIDERSTATIC)->setVisible(false);
	getEditBox(IDC_SLIDERMINEDITBOX)->setVisible(false);
	getEditBox(IDC_SLIDERMAXEDITBOX)->setVisible(false);

	getListBox(IDC_LISTBOXITEMS)->setVisible(false);
	getStatic(IDC_LISTBOXSTATIC)->setVisible(false);
	getEditBox(IDCLISTOXEDITBOX)->setVisible(false);
	getButton(IDC_LISTBOXITEMSADD)->setVisible(false);
	getButton(IDC_LISTBOXITEMSREMOVE)->setVisible(false);

	getComboBox(IDC_COMBOXITEMS)->setVisible(false);
	getButton(IDC_COMBOBOXITEMSADD)->setVisible(false);
	getButton(IDC_COMBOBOXITEMSREMOVE)->setVisible(false);

	getStatic(IDC_CONTROLTEXT)->setVisible(true);
	getEditBox(IDC_TEXTEDITBOX)->setVisible(true);

	getButton(IDC_CREATECONTROL)->setLocation(150, 310);
	getButton(IDC_SETCHANGESBUTTON)->setLocation(150, 310);

	if (!ControlSelected)
	{
		getButton(IDC_CREATECONTROL)->setVisible(true);
		getButton(IDC_SETCHANGESBUTTON)->setVisible(false);

	}
	else
	{
		getButton(IDC_SETCHANGESBUTTON)->setVisible(true);
		getButton(IDC_CREATECONTROL)->setVisible(false);
		getButton(IDC_DELETEBUTTON)->setEnabled(true);
	}
}

//-----------------------------------------------------------------------------
// Name : SetRadioButtonGUI ()
//-----------------------------------------------------------------------------
void CEditDialogUI::SetRadioButtonGUI(bool ControlSelected /* = false */)
{
	getStatic(IDC_SLIDERSTATIC)->setVisible(false);
	getEditBox(IDC_SLIDERMINEDITBOX)->setVisible(false);
	getEditBox(IDC_SLIDERMAXEDITBOX)->setVisible(false);

	getListBox(IDC_LISTBOXITEMS)->setVisible(false);
	getStatic(IDC_LISTBOXSTATIC)->setVisible(false);
	getEditBox(IDCLISTOXEDITBOX)->setVisible(false);
	getButton(IDC_LISTBOXITEMSADD)->setVisible(false);
	getButton(IDC_LISTBOXITEMSREMOVE)->setVisible(false);

	getComboBox(IDC_COMBOXITEMS)->setVisible(false);
	getButton(IDC_COMBOBOXITEMSADD)->setVisible(false);
	getButton(IDC_COMBOBOXITEMSREMOVE)->setVisible(false);

	getStatic(IDC_RADIOGROUPTEXT)->setVisible(true);
	getEditBox(IDC_RADIOBUTTONGROUP)->setVisible(true);

	getStatic(IDC_CONTROLTEXT)->setVisible(true);
	getEditBox(IDC_TEXTEDITBOX)->setVisible(true);

	getButton(IDC_CREATECONTROL)->setLocation(150, 310);
	getButton(IDC_SETCHANGESBUTTON)->setLocation(150, 310);

	if (!ControlSelected)
	{
		getButton(IDC_CREATECONTROL)->setVisible(true);
		getButton(IDC_SETCHANGESBUTTON)->setVisible(false);

	}
	else
	{
		getButton(IDC_SETCHANGESBUTTON)->setVisible(true);
		getButton(IDC_CREATECONTROL)->setVisible(false);
		getButton(IDC_DELETEBUTTON)->setEnabled(true);
	}
}

//-----------------------------------------------------------------------------
// Name : SetSliderGUI ()
//-----------------------------------------------------------------------------
void CEditDialogUI::SetSliderGUI(bool ControlSelected /* = false */)
{
	getStatic(IDC_RADIOGROUPTEXT)->setVisible(false);
	getEditBox(IDC_RADIOBUTTONGROUP)->setVisible(false);

	getListBox(IDC_LISTBOXITEMS)->setVisible(false);
	getStatic(IDC_LISTBOXSTATIC)->setVisible(false);
	getEditBox(IDCLISTOXEDITBOX)->setVisible(false);
	getButton(IDC_LISTBOXITEMSADD)->setVisible(false);
	getButton(IDC_LISTBOXITEMSREMOVE)->setVisible(false);

	getComboBox(IDC_COMBOXITEMS)->setVisible(false);
	getButton(IDC_COMBOBOXITEMSADD)->setVisible(false);
	getButton(IDC_COMBOBOXITEMSREMOVE)->setVisible(false);

	getStatic(IDC_CONTROLTEXT)->setVisible(true);
	getEditBox(IDC_TEXTEDITBOX)->setVisible(true);

	getStatic(IDC_SLIDERSTATIC)->setVisible(true);
	getEditBox(IDC_SLIDERMINEDITBOX)->setVisible(true);
	getEditBox(IDC_SLIDERMAXEDITBOX)->setVisible(true);

	getButton(IDC_CREATECONTROL)->setLocation(150, 310);
	getButton(IDC_SETCHANGESBUTTON)->setLocation(150, 310);

	if (!ControlSelected)
	{
		getButton(IDC_CREATECONTROL)->setVisible(true);
		getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
	}
	else
	{
		getButton(IDC_SETCHANGESBUTTON)->setVisible(true);
		getButton(IDC_CREATECONTROL)->setVisible(false);
		getButton(IDC_DELETEBUTTON)->setEnabled(true);
	}
}

//-----------------------------------------------------------------------------
// Name : SetListBoxGUI ()
//-----------------------------------------------------------------------------
void CEditDialogUI::SetListBoxGUI(bool ControlSelected /* = false */)
{
	getStatic(IDC_CONTROLTEXT)->setVisible(false);
	getEditBox(IDC_TEXTEDITBOX)->setVisible(false);

	getStatic(IDC_RADIOGROUPTEXT)->setVisible(false);
	getEditBox(IDC_RADIOBUTTONGROUP)->setVisible(false);

	getStatic(IDC_SLIDERSTATIC)->setVisible(false);
	getEditBox(IDC_SLIDERMINEDITBOX)->setVisible(false);
	getEditBox(IDC_SLIDERMAXEDITBOX)->setVisible(false);

	getComboBox(IDC_COMBOXITEMS)->setVisible(false);
	getButton(IDC_COMBOBOXITEMSADD)->setVisible(false);
	getButton(IDC_COMBOBOXITEMSREMOVE)->setVisible(false);

	getListBox(IDC_LISTBOXITEMS)->setVisible(true);
	getStatic(IDC_LISTBOXSTATIC)->setVisible(true);
	getStatic(IDC_LISTBOXSTATIC)->setText("ListBox Item text");
	getEditBox(IDCLISTOXEDITBOX)->setVisible(true);
	getButton(IDC_LISTBOXITEMSADD)->setVisible(true);
	getButton(IDC_LISTBOXITEMSREMOVE)->setVisible(true);

	getButton(IDC_CREATECONTROL)->setLocation(150, 468);
	getButton(IDC_SETCHANGESBUTTON)->setLocation(150, 468);

	if (!ControlSelected)
	{
		getButton(IDC_CREATECONTROL)->setVisible(true);
		getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
	}
	else
	{
		getButton(IDC_SETCHANGESBUTTON)->setVisible(true);
		getButton(IDC_CREATECONTROL)->setVisible(false);
		getButton(IDC_DELETEBUTTON)->setEnabled(true);
	}
}

//-----------------------------------------------------------------------------
// Name : SetComboBoxGUI ()
//-----------------------------------------------------------------------------
void CEditDialogUI::SetComboBoxGUI(bool ControlSelected /* = false */)
{
	getStatic(IDC_CONTROLTEXT)->setVisible(false);
	getEditBox(IDC_TEXTEDITBOX)->setVisible(false);

	getStatic(IDC_RADIOGROUPTEXT)->setVisible(false);
	getEditBox(IDC_RADIOBUTTONGROUP)->setVisible(false);

	getStatic(IDC_SLIDERSTATIC)->setVisible(false);
	getEditBox(IDC_SLIDERMINEDITBOX)->setVisible(false);
	getEditBox(IDC_SLIDERMAXEDITBOX)->setVisible(false);

	getListBox(IDC_LISTBOXITEMS)->setVisible(false);
	getButton(IDC_LISTBOXITEMSADD)->setVisible(false);
	getButton(IDC_LISTBOXITEMSREMOVE)->setVisible(false);

	getStatic(IDC_LISTBOXSTATIC)->setVisible(true);
	getStatic(IDC_LISTBOXSTATIC)->setText("Combobox Item text");
	getEditBox(IDCLISTOXEDITBOX)->setVisible(true);
	getComboBox(IDC_COMBOXITEMS)->setVisible(true);
	getButton(IDC_COMBOBOXITEMSADD)->setVisible(true);
	getButton(IDC_COMBOBOXITEMSREMOVE)->setVisible(true);

	getButton(IDC_CREATECONTROL)->setLocation(150, 468);
	getButton(IDC_SETCHANGESBUTTON)->setLocation(150, 468);


	if (!ControlSelected)
	{
		getButton(IDC_CREATECONTROL)->setVisible(true);
		getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
	}
	else
	{
		getButton(IDC_SETCHANGESBUTTON)->setVisible(true);
		getButton(IDC_CREATECONTROL)->setVisible(false);
		getButton(IDC_DELETEBUTTON)->setEnabled(true);
	}
}
