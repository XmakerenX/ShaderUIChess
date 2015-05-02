#ifndef  _CGENDIALOGUI_H
#define  _CGENDIALOGUI_H

#include "CDialogUI.h"
#include "../settingsDef.h"
#include <unordered_map>

struct DEVICETYPEINFO
{
	enum MODE{WINDOWED, FULLSCREEN};

	D3DDEVTYPE deviceType;
	std::string deviceDescription;
	D3DFORMAT fomrat;

	bool bHardwareAcceleration[2];
	BOOL bDepthEnable[2];
	std::vector<D3DFORMAT> validDepths[2];
	std::vector<D3DMULTISAMPLE_TYPE> validMultiSampleTypes[2];
	std::vector<DWORD> vpTypes;
};

struct DISPLAYMODE
{
	UINT Width;
	UINT Height;
	std::vector<UINT> RefreshRates;
	D3DFORMAT Format;

	DISPLAYMODE::DISPLAYMODE(D3DDISPLAYMODE& displayMode)
	{
		this->Format = displayMode.Format;
		this->Height = displayMode.Height;
		this->RefreshRates.push_back(displayMode.RefreshRate);
		this->Width = displayMode.Width;
	}
};

struct ADAPTERINFO
{
	UINT adapterNum;
	std::string adapterDescription;

	std::vector<DEVICETYPEINFO> deviceTypes;
	std::vector<DISPLAYMODE> displayModes;

	void addDisplayMode(D3DDISPLAYMODE& displayMode)
	{
		for (UINT i = 0; i < displayModes.size(); i++)
		{
			if (displayMode.Width == displayModes[i].Width && displayMode.Height == displayModes[i].Height)
			{
				displayModes[i].RefreshRates.push_back(displayMode.RefreshRate);
				return;
			}
		}

		displayModes.push_back(DISPLAYMODE(displayMode));
	}
};

class COptionDialogUI : 
	public CDialogUI
{
public:
	COptionDialogUI(std::vector<D3DFORMAT>& adapterFormats, std::vector<ADAPTERINFO>& adapterInfo);
	virtual ~COptionDialogUI(void);

	void CreateDialogUI();

	void  AdapterSelChg				(CComboBoxUI* pCombobox);
	bool  ChangeDisplayAdapter		(UINT adapterIndex, D3DDEVTYPE deviceType = D3DDEVTYPE_HAL);
	bool  ChangeDisplayDevice		(UINT adapterIndex, D3DDEVTYPE deviceType);

	void  DeviceTypeSelChg			(CComboBoxUI* pCombobox);
	void  WindowedRadioClicked		(CButtonUI* pRadio);
	void  FullscreenRadioClicked	(CButtonUI* pRadio);
	void  ResoulationSelChg			(CComboBoxUI* pCombobox);

private:
	std::vector<D3DFORMAT>& m_adapterFormats;
	std::vector<ADAPTERINFO>& m_adpatersInfo;

	static const std::unordered_map<UINT,char*> s_depthFormatsString;
	static const std::unordered_map<UINT,char*> s_mutliSampleString;
	static const std::unordered_map<ULONG,char*> s_vertexProcString;

	static std::unordered_map<UINT,char*> InitDepthFormatMap();
	static std::unordered_map<UINT,char*> InitMultiSampleMap();
	static std::unordered_map<ULONG,char*> InitVertexProcMap();

};

#endif  //_CGENDIALOGUI_H