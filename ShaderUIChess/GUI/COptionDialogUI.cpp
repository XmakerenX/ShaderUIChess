#include "COptionDialogUI.h"

const std::unordered_map<UINT,char*> COptionDialogUI::s_depthFormatsString = InitDepthFormatMap();
const std::unordered_map<UINT,char*> COptionDialogUI::s_mutliSampleString = InitMultiSampleMap();
const std::unordered_map<ULONG,char*> COptionDialogUI::s_vertexProcString = InitVertexProcMap();

COptionDialogUI::COptionDialogUI(std::vector<D3DFORMAT>& adapterFormats, std::vector<ADAPTERINFO>& adapterInfo)
	:m_adapterFormats(adapterFormats), m_adpatersInfo(adapterInfo)
{
}


COptionDialogUI::~COptionDialogUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : CreateDialogUI ()
//-----------------------------------------------------------------------------
void COptionDialogUI::CreateDialogUI()
{
	getComboBox(IDC_ADAPFORMATCOM)->setEnabled(false);
	getComboBox(IDC_RESOLUTIONCOM)->setEnabled(false);
	getComboBox(IDC_REFRATECOM)->setEnabled(false);
	getCheckBox(IDC_ASPECTCHECK)->setEnabled(false);

	getComboBox(IDC_DISPADAPCOM)->ConnectToSelectChg( boost::bind(&COptionDialogUI::AdapterSelChg, this, _1 ) );
	getComboBox(IDC_RENDERDEVCOM)->ConnectToSelectChg( boost::bind(&COptionDialogUI::DeviceTypeSelChg, this, _1 ) );
	getComboBox(IDC_RESOLUTIONCOM)->ConnectToSelectChg( boost::bind(&COptionDialogUI::ResoulationSelChg, this, _1) );
	getRadioButton(IDC_FULLSCREENRADIO)->connectToClick( boost::bind(&COptionDialogUI::FullscreenRadioClicked, this, _1 ) );

	getComboBox(IDC_APIVERCOM)->AddItem("Direct3D 9", nullptr);

	for (UINT i = 0; i < m_adpatersInfo.size(); i++)
	{
		getComboBox(IDC_DISPADAPCOM)->AddItem(m_adpatersInfo[i].adapterDescription.c_str(), (void*)m_adpatersInfo[i].adapterNum);
	}

	ChangeDisplayAdapter(0);
}

//-----------------------------------------------------------------------------
// Name : AdapterSelChg ()
//-----------------------------------------------------------------------------
void COptionDialogUI::AdapterSelChg(CComboBoxUI* pCombobox)
{
	UINT adapterIndex = (UINT)pCombobox->GetSelectedData();
	ChangeDisplayAdapter(adapterIndex);
}

//-----------------------------------------------------------------------------
// Name : ChangeDisplayAdapter ()
//-----------------------------------------------------------------------------
bool COptionDialogUI::ChangeDisplayAdapter(UINT adapterIndex, D3DDEVTYPE deviceType /* = D3DDEVTYPE_HAL */)
{
	if (adapterIndex >= m_adpatersInfo.size() )
		return false;

	ADAPTERINFO& curAdapter = m_adpatersInfo[adapterIndex];

	if (curAdapter.deviceTypes.size() == 0)
		return false;

	getComboBox(IDC_RENDERDEVCOM)->RemoveAllItems();

	for (UINT i = 0; i < curAdapter.deviceTypes.size(); i++)
		getComboBox(IDC_RENDERDEVCOM)->AddItem(curAdapter.deviceTypes[i].deviceDescription.c_str(), (void*)&curAdapter.deviceTypes[i].deviceType);

	getComboBox(IDC_RESOLUTIONCOM)->RemoveAllItems();
	for (UINT i = 0; i < curAdapter.displayModes.size(); i++)
	{
		DISPLAYMODE& curDisplayMode = curAdapter.displayModes[i];


		std::string resWidth = std::to_string( (long long)curDisplayMode.Width);
		std::string resHeight = std::to_string( (long long)curDisplayMode.Height);
		std::string resoulation = resWidth + "X" + resHeight;

		getComboBox(IDC_RESOLUTIONCOM)->AddItem(resoulation.c_str(), (void*)i);

	}

	ChangeDisplayDevice(adapterIndex, deviceType);
}

//-----------------------------------------------------------------------------
// Name : ChangeDisplayDevice ()
//-----------------------------------------------------------------------------
bool COptionDialogUI::ChangeDisplayDevice(UINT adapterIndex, D3DDEVTYPE deviceType)
{
	UINT deviceIndex = -1;
	if (adapterIndex < m_adpatersInfo.size() )
	{
		for (UINT i = 0; i < m_adpatersInfo[adapterIndex].deviceTypes.size(); i++)
		{
			DEVICETYPEINFO& curDeviceInfo = m_adpatersInfo[adapterIndex].deviceTypes[i];
			if (curDeviceInfo.deviceType == deviceType)
			{
				deviceIndex = i;
				break;
			}
		}
		if (deviceIndex == -1)
			return false;
	}
	else
		return false;

	DEVICETYPEINFO& curDeviceInfo = m_adpatersInfo[adapterIndex].deviceTypes[deviceIndex];

	getComboBox(IDC_BBFORMATCOM)->RemoveAllItems();
	getComboBox(IDC_BBFORMATCOM)->AddItem("X8R8G8B8",nullptr);

	getComboBox(IDC_DPETHSTENCOM)->RemoveAllItems();

	if (curDeviceInfo.bDepthEnable[DEVICETYPEINFO::WINDOWED])
	{
		for (UINT i = 0; i < curDeviceInfo.validDepths[DEVICETYPEINFO::WINDOWED].size(); i++)
		{
			std::vector<D3DFORMAT>& curValidDepths = curDeviceInfo.validDepths[DEVICETYPEINFO::WINDOWED];
			if (s_depthFormatsString.find(curValidDepths[i] ) != s_depthFormatsString.end() )
				getComboBox(IDC_DPETHSTENCOM)->AddItem( s_depthFormatsString.at(curValidDepths[i]), &curValidDepths[i]);
		}
	}
	else
		getComboBox(IDC_DPETHSTENCOM)->setEnabled(false);

	std::vector<D3DMULTISAMPLE_TYPE>& curValidMultiSampleTypes = curDeviceInfo.validMultiSampleTypes[DEVICETYPEINFO::WINDOWED];

	getComboBox(IDC_MULSAMPLECOM)->RemoveAllItems();

	for (UINT i = 0; i < curValidMultiSampleTypes.size(); i++)
	{
		if (s_mutliSampleString.find(curValidMultiSampleTypes[i] ) != s_mutliSampleString.end() )
			getComboBox(IDC_MULSAMPLECOM)->AddItem(s_mutliSampleString.at(curValidMultiSampleTypes[i]),
			&curValidMultiSampleTypes[i]);		
	}

	getComboBox(IDC_VERTEXPROCCOM)->RemoveAllItems();

	for (UINT i = 0; i < curDeviceInfo.vpTypes.size(); i++)
	{
		ULONG curVpType = curDeviceInfo.vpTypes[i];
		if ( s_vertexProcString.find(curVpType) != s_vertexProcString.end() )
			getComboBox(IDC_VERTEXPROCCOM)->AddItem(s_vertexProcString.at(curVpType), (void*)curVpType);
	}
}

//-----------------------------------------------------------------------------
// Name : DeviceTypeSelChg ()
//-----------------------------------------------------------------------------
void COptionDialogUI::DeviceTypeSelChg(CComboBoxUI* pCombobox)
{
	UINT adapterIndex = (UINT)pCombobox->GetSelectedData();
	D3DDEVTYPE deviceType = static_cast<D3DDEVTYPE>((UINT)pCombobox->GetSelectedData());
	ChangeDisplayDevice(adapterIndex, deviceType);
}

//-----------------------------------------------------------------------------
// Name : WindowedRadioClicked ()
//-----------------------------------------------------------------------------
void COptionDialogUI::WindowedRadioClicked(CButtonUI* pRadio)
{
	getComboBox(IDC_ADAPFORMATCOM)->setEnabled(false);
	getComboBox(IDC_RESOLUTIONCOM)->setEnabled(false);
	getComboBox(IDC_REFRATECOM)->setEnabled(false);
	getCheckBox(IDC_ASPECTCHECK)->setEnabled(false);

	//m_windowed = true;
}

//-----------------------------------------------------------------------------
// Name : FullscreenRadioClicked ()
//-----------------------------------------------------------------------------
void COptionDialogUI::FullscreenRadioClicked(CButtonUI* pRadio)
{
	getComboBox(IDC_ADAPFORMATCOM)->setEnabled(true);
	getComboBox(IDC_RESOLUTIONCOM)->setEnabled(true);
	getComboBox(IDC_REFRATECOM)->setEnabled(true);
	getCheckBox(IDC_ASPECTCHECK)->setEnabled(true);

	//m_windowed = false;

	UINT dislpayAdapterIndex = (UINT)getComboBox(IDC_DISPADAPCOM)->GetSelectedData();

	ADAPTERINFO& curAdatperInfo = m_adpatersInfo[dislpayAdapterIndex];
	if (curAdatperInfo.displayModes.size() == 0)
		return;

	DISPLAYMODE& selDisplayMode = curAdatperInfo.displayModes[0];

	getComboBox(IDC_REFRATECOM)->RemoveAllItems();
	for (UINT j = 0; j < selDisplayMode.RefreshRates.size(); j++)
	{
		std::string refreshRate = std::to_string((long long) selDisplayMode.RefreshRates[j]);
		getComboBox(IDC_REFRATECOM)->AddItem(refreshRate.c_str(),
			(void*)selDisplayMode.RefreshRates[j]);
	}
}

//-----------------------------------------------------------------------------
// Name : ResoulationSelChg ()
//-----------------------------------------------------------------------------
void COptionDialogUI::ResoulationSelChg(CComboBoxUI* pCombobox)
{
	UINT dislpayAdapterIndex = (UINT)getComboBox(IDC_DISPADAPCOM)
		->GetSelectedData();

	UINT modeIndex = (UINT)pCombobox->GetSelectedData();

	DISPLAYMODE curDisplayMode = m_adpatersInfo[dislpayAdapterIndex].displayModes[modeIndex];
	getComboBox(IDC_REFRATECOM)->RemoveAllItems();

	for (UINT i = 0; i < curDisplayMode.RefreshRates.size(); i++)
	{
		std::string refreshRate = std::to_string((long long) curDisplayMode.RefreshRates[i]);
		getComboBox(IDC_REFRATECOM)->AddItem(refreshRate.c_str(),
			(void*)curDisplayMode.RefreshRates[i]);
	}
}


//-----------------------------------------------------------------------------
// Name : InitDepthFormatMap ()
//-----------------------------------------------------------------------------
std::unordered_map<UINT,char*> COptionDialogUI::InitDepthFormatMap()
{
	std::unordered_map<UINT,char*> m;

	m[D3DFMT_D24S8] = "D3DFMT_D24S8";
	m[D3DFMT_D16] = "D3DFMT_D16";

	return m;
}

//-----------------------------------------------------------------------------
// Name : InitMultiSampleMap ()
//-----------------------------------------------------------------------------
std::unordered_map<UINT,char*> COptionDialogUI::InitMultiSampleMap()
{
	std::unordered_map<UINT,char*> m;

	m[D3DMULTISAMPLE_NONE]       = "None";
	m[D3DMULTISAMPLE_2_SAMPLES]  = "x2";
	m[D3DMULTISAMPLE_4_SAMPLES]  = "x4";
	m[D3DMULTISAMPLE_8_SAMPLES]  = "x8";
	m[D3DMULTISAMPLE_16_SAMPLES] = "x16";

	return m;
}

//-----------------------------------------------------------------------------
// Name : InitVertexProcMap ()
//-----------------------------------------------------------------------------
std::unordered_map<ULONG,char*> COptionDialogUI::InitVertexProcMap()
{
	std::unordered_map<ULONG,char*> m;

	m[D3DCREATE_HARDWARE_VERTEXPROCESSING] = "Hardware Vertex Processing";
	m[D3DCREATE_SOFTWARE_VERTEXPROCESSING] = "Software Vertex Processing";

	return m;
}