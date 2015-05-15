#include "CGameWin.h"
// #include "GUI\CComboBox3D.h"
// #include "GUI\CListBox3D.h"

const std::unordered_map<UINT,char*> CGameWin::s_depthFormatsString = InitDepthFormatMap();
const std::unordered_map<UINT,char*> CGameWin::s_mutliSampleString = InitMultiSampleMap();
const std::unordered_map<ULONG,char*> CGameWin::s_vertexProcString = InitVertexProcMap();

//-----------------------------------------------------------------------------
// Name : CGameWin (constructor)
//-----------------------------------------------------------------------------
CGameWin::CGameWin() 
	:Width(1024),Height(742)//640 480
	, m_OptionsDialog(m_adapterFormats, m_adpatersInfo)
{
	m_gameRunning = true;

	//clearing cameras
	for (UINT i = 0; i < 2; i++)
		m_pCameras[i]		= NULL;

	m_windowed = true;
	m_bReturnCamera = false;
	m_bMoveCamera = false;

	//clearing handles to win32 and directx objects
	m_hWnd          = NULL;
	m_pD3D          = NULL;
	m_pD3DDevice    = NULL;
	m_hIcon         = NULL;
	m_hMenu         = NULL;
	m_bLostDevice   = false;

	//TODO: add something that will check device caps of textureformat...
	//m_fmtTexture = D3DFMT_DXT3;
	m_assetManger.SetTextureFormat(D3DFMT_DXT3);

	// get a pointer to the game timer
	m_timer = m_assetManger.getTimer();

	//setting render flags
	m_bPicking = false;
	m_bCamRotate = false;
	m_deviceReset= false;
	m_DrawingMethod = DRAW_ATTRIBOBJECT;

	activeMeshIndex = 4;

	//setting shader handles and effect files
	m_matWorldH = NULL;;
	m_matWorldViewProjH = NULL;
	m_matWorldInverseTH = NULL;

	m_projTextureMatrixH = NULL;

	m_vecEyeH = NULL;

	m_numActiveLightsH = NULL;

	for (UINT i = 0; i < MAX_ACTIVE_LIGHTS; i++)
	{
		m_lightPosH[i] = NULL;
		m_vecLightDirH[i] = NULL;
		m_lightAttenH[i] = NULL; 

		m_vecAmbientLightH[i] = NULL;
		m_vecDiffuseLightH[i] = NULL;
		m_vecSpecularLightH[i] = NULL;
		m_SpotPowerH[i] = NULL;
		
	}

	m_vecAmbientMtrlH = NULL;
	m_vecDiffuseMtrlH = NULL;
	m_vecSpecularMtrlH = NULL;
	m_SpecularPowerH = NULL;

	m_bHighLightH = NULL;

	m_MeshTextureH = NULL;

	m_lightTechHnadle = NULL;
	m_lightTexTechHandle = NULL;

	m_UITexture = NULL;
	m_bUIHighLight = NULL;
	m_bUITexutre = NULL;

	// effect file
	m_outlineEffect = NULL;
	m_cameraIndex = 0;
	m_numActiveLights = 0;

	m_gameBoard = nullptr;
}
//-----------------------------------------------------------------------------
// Name : CGameWin (destructor)
//-----------------------------------------------------------------------------

CGameWin::~CGameWin()
{
	//TODO: check if there nothing needed to be cleaned
	//delete m_pCamera;
	//Release();
}

//-----------------------------------------------------------------------------
// Name : InitInstance 
// Desc : initialize the game application
//-----------------------------------------------------------------------------
bool CGameWin::InitInstance(HINSTANCE hInstance, LPCTSTR lpCmdLine, int iCmdShow)
{
	CreateDisplay(hInstance,true);
	return BuildObjects();
}

//-----------------------------------------------------------------------------
// Name : WinProc 
// Desc : the window Procedure.
//-----------------------------------------------------------------------------
LRESULT CGameWin::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_MainMenu.MsgProc(hWnd, message, wParam, lParam, m_timer, m_windowed))
		return 0;

	if (m_OptionsDialog.MsgProc(hWnd, message, wParam, lParam, m_timer, m_windowed))
		return 0;

	if (m_GuiSelectPawn.MsgProc(hWnd, message, wParam, lParam, m_timer, m_windowed))
		return 0;

// 	if (m_EditDialog.MsgProc(hWnd,message,wParam,lParam, m_timer) )
// 		return 0;

	// Determine message type
	switch (message)
	{
	case WM_CREATE:
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		{
			HRESULT hr;
			if (m_pD3DDevice)
			{

				if ( wParam == SIZE_MINIMIZED )
				{
					// App is inactive
					m_bActive = false;

				} // App has been minimized
				else
				{
					// App is active
					m_bActive = true;


					if (m_pD3DDevice) 
					{
						m_deviceReset = true;
					} // End if

				} // End if !Minimized
			}
		}
		break;

	case WM_LBUTTONDOWN:
		{
			// Capture the mouse
			/*m_hWnd=hWnd;*/
			SetCapture( hWnd );
			GetCursorPos( &m_OldCursorPos );
			m_bCamRotate = true;
		}
		break;

// 	case  WM_MOUSEMOVE:
// 		{
// 
// 		}break;

	case WM_LBUTTONUP:
		{		
			// Release the mouse
			m_bCamRotate = false;
			ReleaseCapture( );
		}
		break;

	case WM_RBUTTONDOWN:
		{
			SetCapture( m_hWnd );
			m_bPicking = true;
		}break;

	case WM_RBUTTONUP:
		{
			m_bPicking = false;
			ReleaseCapture( );
		}

		// 	case WM_COMMAND:
		// 		{
		// 
		// 		
		// 
		// 
		// 
		// 
		// 		} // End Switch

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	} // End Message Switch

	return 0;
}

//-----------------------------------------------------------------------------
// Name : OptionsControlClicked ()
//-----------------------------------------------------------------------------
void CGameWin::OptionsControlClicked(CButtonUI* pButton)
{
	m_OptionsDialog.setVisible(!m_OptionsDialog.getVisible());
}

//-----------------------------------------------------------------------------
// Name : OptionDialogOKClicked ()
//-----------------------------------------------------------------------------
void CGameWin::OptionDialogOKClicked(CButtonUI* pButton)
{
	D3DPRESENT_PARAMETERS deviceParams;

	if (m_OptionsDialog.getRadioButton(IDC_WINRADIO)->getChecked())
		m_windowed = true;
	else
		m_windowed = false;

	UINT adapterIndex =  (UINT)m_OptionsDialog.getComboBox(IDC_DISPADAPCOM)->GetSelectedData();
	UINT modeIndex = (UINT)m_OptionsDialog.getComboBox(IDC_RESOLUTIONCOM)->GetSelectedData();

	if (m_windowed)
	{


// 		std::string s;
// 		DWORD n;
// 
// 		s = "new line \n";
// 		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), s.size(), &n, 0);
// 		std::stringstream out;
// 		out << rc.left;
// 		s = out.str() + '\n';
// 		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), s.size(), &n, 0);
// 
// 		std::stringstream out2;
// 		out2 << rc.top;
// 		s = out2.str() + '\n';
// 		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), s.size(), &n, 0);
// 
// 		std::stringstream out3;
// 		out3 << rc.right - rc.left;
// 		s = out3.str() + '\n';
// 		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), s.size(), &n, 0);
// 
// 		std::stringstream out4;
// 		out4 << rc.bottom - rc.top;
// 		s = out4.str() + '\n';
// 		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), s.size(), &n, 0);

		RECT rc; //= m_clientRC;
		::GetClientRect( m_hWnd, &rc );
		m_nViewX      = rc.left;
		m_nViewY      = rc.top;
		m_nViewWidth  = rc.right - rc.left;
		m_nViewHeight = rc.bottom - rc.top;

		deviceParams.BackBufferWidth = m_nViewWidth;
		deviceParams.BackBufferHeight = m_nViewHeight;
		//deviceParams.BackBufferWidth = Width;
		//deviceParams.BackBufferHeight = Height;
	}
	else
	{
		m_nViewX = 0;
		m_nViewY = 0;
		//adapterIndex = (UINT)m_OptionsDialog.getComboBox(IDC_DISPADAPCOM)->GetSelectedData();
		//modeIndex = (UINT)m_OptionsDialog.getComboBox(IDC_RESOLUTIONCOM)->GetSelectedData();
		m_nViewWidth = m_adpatersInfo[adapterIndex].displayModes[modeIndex].Width;
		m_nViewHeight = m_adpatersInfo[adapterIndex].displayModes[modeIndex].Height;

		deviceParams.BackBufferWidth = m_nViewWidth;
		deviceParams.BackBufferHeight = m_nViewHeight;

		DWORD n;
		std::string s;
		std::stringstream out;
		out << m_nViewWidth;
		s = out.str() + '\n';
		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), s.size(), &n, 0);

		std::stringstream out2;
		out2 << m_nViewHeight;
		s = out2.str() + '\n';
		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), s.size(), &n, 0);
	}

	deviceParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	deviceParams.BackBufferCount = 1;

	D3DMULTISAMPLE_TYPE multiSampleType = 
		*(static_cast<D3DMULTISAMPLE_TYPE*>(m_OptionsDialog.getComboBox(IDC_MULSAMPLECOM)->GetSelectedData()));
	deviceParams.MultiSampleType = multiSampleType;
	deviceParams.MultiSampleQuality = 0;
	deviceParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	deviceParams.hDeviceWindow = m_hWnd;
	deviceParams.Windowed = m_windowed;
	D3DDEVTYPE deviceType = *( static_cast<D3DDEVTYPE*>(m_OptionsDialog.getComboBox(IDC_RENDERDEVCOM)->GetSelectedData() ) );
	UINT deviceIndex;
	for (UINT i = 0; i < m_adpatersInfo[adapterIndex].deviceTypes.size(); i++)
	{
		if (deviceType == m_adpatersInfo[adapterIndex].deviceTypes[i].deviceType)
		{
			deviceIndex = i;
			break;
		}
		
	}

	deviceParams.EnableAutoDepthStencil = m_adpatersInfo[adapterIndex].deviceTypes[deviceIndex].bDepthEnable[m_windowed];
	deviceParams.AutoDepthStencilFormat = *(static_cast<D3DFORMAT*>(m_OptionsDialog.getComboBox(IDC_DPETHSTENCOM)->GetSelectedData()));
	deviceParams.Flags                  = 0;

	if (!m_windowed)
	{
		deviceParams.FullScreen_RefreshRateInHz = (UINT)m_OptionsDialog.getComboBox(IDC_REFRATECOM)->GetSelectedData();
	}
	else
		deviceParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	deviceParams.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	m_curD3dpp = deviceParams;
	m_deviceReset = true;
	resetDevice(m_curD3dpp);
}

//-----------------------------------------------------------------------------
// Name : BeginGame 
// Desc : holds the game message loop
//-----------------------------------------------------------------------------
int CGameWin::BeginGame()
{
	MSG		msg;

	// Start main loop
	while (m_gameRunning)
	{
		// Did we receive a message, or are we idling ?
		while ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT)
			{
				m_gameRunning =  false;
				break;
			}
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 

		// Advance Game Frame.
		m_timer->frameAdvanced();
		FrameAdvance(m_timer->getTimeElapsed());
		// End If messages waiting
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Name : ShutDown 
// Desc : shut down the game and calls the release function to clear the allocated memory
//-----------------------------------------------------------------------------
bool CGameWin::ShutDown()
{
	int wtf;

	//m_GuiDilaogResManger.OnD3D9DestroyDevice();

	// Destroy Direct3D Objects
	if ( m_pD3DDevice ) 
	{
		int x;
		wtf = m_pD3DDevice->Release();
		x = 0;
	}

	if ( m_pD3D       ) 
	{
		int x;
		wtf = m_pD3D->Release();
		x = 0;
	}
	m_pD3D          = NULL;
	m_pD3DDevice    = NULL;

	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu         = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd          = NULL;

	if (m_pCameras)
	{
		for (UINT i = 0; i < 2; i++)
			if (m_pCameras[i])
				delete m_pCameras[i];
	}
	//delete m_pCamera;

	Release();

	//m_gameRunning = false;
	// Shutdown Success
	return true;
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc 
// Desc : forward the message to the proper instance of the class 
//-----------------------------------------------------------------------------
LRESULT CALLBACK CGameWin::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameWin *Destination = (CGameWin*)GetWindowLong( hWnd, GWL_USERDATA );

	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->WinProc( hWnd, Message, wParam, lParam );

	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name : InitDepthFormatMap ()
//-----------------------------------------------------------------------------
std::unordered_map<UINT,char*> CGameWin::InitDepthFormatMap()
{
	std::unordered_map<UINT,char*> m;

	m[D3DFMT_D24S8] = "D3DFMT_D24S8";
	m[D3DFMT_D16] = "D3DFMT_D16";

	return m;
}

//-----------------------------------------------------------------------------
// Name : InitMultiSampleMap ()
//-----------------------------------------------------------------------------
std::unordered_map<UINT,char*> CGameWin::InitMultiSampleMap()
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
std::unordered_map<ULONG,char*> CGameWin::InitVertexProcMap()
{
	std::unordered_map<ULONG,char*> m;

	m[D3DCREATE_HARDWARE_VERTEXPROCESSING] = "Hardware Vertex Processing";
	m[D3DCREATE_SOFTWARE_VERTEXPROCESSING] = "Software Vertex Processing";

	return m;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplay 
// Desc : creates the window for the game and the directx device
//-----------------------------------------------------------------------------
bool CGameWin::CreateDisplay(HINSTANCE hInstance,bool windowed)
{
	if ( !CreateDisplayWindow(hInstance) )
		return false;

	if ( FAILED( CreateDevice(windowed) ) )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplayWindow 
// Desc : creates a win32 window
//-----------------------------------------------------------------------------
bool CGameWin::CreateDisplayWindow(HINSTANCE hInstance)
{
	//Creating the window 
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   = StaticWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "Direct3D9App";

	if( !RegisterClass(&wc) ) 
	{
		::MessageBox(0, "RegisterClass() - FAILED", 0, 0);
		return false;
	}

	//TODO: make Adjust Window Rect work ! somehow .....
	RECT rc2;
	rc2.top = 0;
	rc2.left = 0;
	rc2.bottom = 480;
	rc2.right = 640;
	AdjustWindowRect (&rc2,WS_OVERLAPPEDWINDOW,FALSE);

// 	HDC hDc = GetDC(NULL);
// 	if (hDc)
// 	{
// 		int dpiY;
// 		dpiY = GetDeviceCaps(hDc, LOGPIXELSY);
// 		ReleaseDC(NULL, hDc);
// 	}

	m_hWnd = ::CreateWindowEx(WS_EX_CLIENTEDGE, "Direct3D9App","Direct3D9App", WS_OVERLAPPEDWINDOW|WS_VISIBLE , 0, 0, Width + 16 + 4, Height + 38 + 4, NULL, NULL, hInstance,(void*)this);

	if( !m_hWnd )
	{
		::MessageBox(NULL,"failed to create a window","lolz",MB_OK);
		return false;
	}

	::ShowWindow(m_hWnd, SW_SHOW);
	::UpdateWindow(m_hWnd);

	RECT rcd;
	GetClientRect(m_hWnd,&rcd);

	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDevice 
// Desc : creates a directx device
//-----------------------------------------------------------------------------
HRESULT CGameWin::CreateDevice(bool windowed)
{
	HRESULT hr = 0;
	D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;

	D3DFORMAT adapterFormats[6] = { D3DFMT_A1R5G5B5, D3DFMT_A2R10G10B10, D3DFMT_A8R8G8B8, D3DFMT_R5G6B5, D3DFMT_X1R5G5B5
							, D3DFMT_X8R8G8B8};
	
	D3DFORMAT format = D3DFMT_X8R8G8B8;

	D3DFORMAT depthFormats[2] = { D3DFMT_D24S8, D3DFMT_D16};

	//Creating the IDirect3D9 object.
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if( !m_pD3D )
	{
		::MessageBox(m_hWnd,"failed to create an IDirect3D9 object","lolz",MB_OK);
		return S_FALSE;
	}

	D3DDISPLAYMODE activeMode;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&activeMode);

	UINT adapterCount = m_pD3D->GetAdapterCount();

	// ---------------------------------------------
	// Enum for all Adapters
	// ---------------------------------------------
	for (UINT adapterIndex = 0; adapterIndex < adapterCount; adapterIndex++)
	{
		D3DADAPTER_IDENTIFIER9 adapterIden;
		m_pD3D->GetAdapterIdentifier(adapterIndex, 0, &adapterIden);

		ADAPTERINFO curAdapterInfo;

		curAdapterInfo.adapterNum = adapterIndex;
		curAdapterInfo.adapterDescription = adapterIden.Description;

		int modeCount = m_pD3D->GetAdapterModeCount(adapterIndex ,format);

		std::vector<D3DFORMAT> validDepths;
		std::vector<D3DMULTISAMPLE_TYPE> validMultiSampleTypes;

		// --------------------------------------------------
		// Enum DepthStencil and multiSample for HAL Device 
		// --------------------------------------------------
		DEVICETYPEINFO halDeviceType;
		halDeviceType.deviceType = D3DDEVTYPE_HAL;
		halDeviceType.deviceDescription = "HAL Device";
		halDeviceType.bHardwareAcceleration[DEVICETYPEINFO::WINDOWED] = false;
		halDeviceType.bHardwareAcceleration[DEVICETYPEINFO::FULLSCREEN] = false;

		hr = m_pD3D->CheckDeviceType(0, D3DDEVTYPE_HAL, format, format, TRUE);
		if ( SUCCEEDED(hr) )
		{
			halDeviceType.bHardwareAcceleration[DEVICETYPEINFO::WINDOWED] = true;

			halDeviceType.bDepthEnable[DEVICETYPEINFO::WINDOWED] = EnumDepthStencil( depthFormats, 2, adapterIndex, D3DDEVTYPE_HAL, format, halDeviceType.validDepths[DEVICETYPEINFO::WINDOWED]);
			EnumMultiSample(adapterIndex, D3DDEVTYPE_HAL, format, TRUE, halDeviceType.validMultiSampleTypes[DEVICETYPEINFO::WINDOWED]);
		}

		hr = m_pD3D->CheckDeviceType(0, D3DDEVTYPE_HAL, format, format, FALSE);
		if ( SUCCEEDED(hr) )
		{
			halDeviceType.bHardwareAcceleration[DEVICETYPEINFO::FULLSCREEN] = true;

			halDeviceType.bDepthEnable[DEVICETYPEINFO::FULLSCREEN] = EnumDepthStencil( depthFormats, 2, adapterIndex, D3DDEVTYPE_HAL, format, halDeviceType.validDepths[DEVICETYPEINFO::FULLSCREEN]);
			EnumMultiSample(adapterIndex, D3DDEVTYPE_HAL, format, FALSE, halDeviceType.validMultiSampleTypes[DEVICETYPEINFO::FULLSCREEN]);
		}

		// Enum all valid vertex processing types
		D3DCAPS9 caps; 
		m_pD3D->GetDeviceCaps(adapterIndex, D3DDEVTYPE_HAL, &caps);

		if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
			halDeviceType.vpTypes.push_back(D3DCREATE_HARDWARE_VERTEXPROCESSING);
		
		halDeviceType.vpTypes.push_back(D3DCREATE_SOFTWARE_VERTEXPROCESSING);

		curAdapterInfo.deviceTypes.push_back(halDeviceType);

		// --------------------------------------------------
		// Enum DepthStencil and multiSample for REF Device 
		// --------------------------------------------------
		DEVICETYPEINFO refDeviceType;
		refDeviceType.deviceType = D3DDEVTYPE_REF;
		refDeviceType.deviceDescription = "REF Device";
		refDeviceType.bHardwareAcceleration[DEVICETYPEINFO::WINDOWED] = false;
		refDeviceType.bHardwareAcceleration[DEVICETYPEINFO::FULLSCREEN] = false;

		hr = m_pD3D->CheckDeviceType(0, D3DDEVTYPE_REF, format, format, TRUE);
		if ( SUCCEEDED(hr) )
		{
			refDeviceType.bHardwareAcceleration[DEVICETYPEINFO::WINDOWED] = true;

			refDeviceType.bDepthEnable[DEVICETYPEINFO::WINDOWED] = EnumDepthStencil( depthFormats, 2, adapterIndex, D3DDEVTYPE_REF, format, refDeviceType.validDepths[DEVICETYPEINFO::WINDOWED]);
			EnumMultiSample(adapterIndex, D3DDEVTYPE_REF, format, TRUE, refDeviceType.validMultiSampleTypes[DEVICETYPEINFO::WINDOWED]);
		}

		hr = m_pD3D->CheckDeviceType(0, D3DDEVTYPE_REF, format, format, FALSE);
		if ( SUCCEEDED(hr) )
		{
			refDeviceType.bHardwareAcceleration[DEVICETYPEINFO::FULLSCREEN] = true;

			refDeviceType.bDepthEnable[DEVICETYPEINFO::FULLSCREEN] = EnumDepthStencil( depthFormats, 2, adapterIndex, D3DDEVTYPE_REF, format, refDeviceType.validDepths[DEVICETYPEINFO::FULLSCREEN]);
			EnumMultiSample(adapterIndex, D3DDEVTYPE_REF, format, FALSE, refDeviceType.validMultiSampleTypes[DEVICETYPEINFO::FULLSCREEN]);
		}
 
		// Enum all valid vertex processing types
		//D3DCAPS9 caps; 
		m_pD3D->GetDeviceCaps(adapterIndex, D3DDEVTYPE_HAL, &caps);

		if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
			refDeviceType.vpTypes.push_back(D3DCREATE_HARDWARE_VERTEXPROCESSING);

		refDeviceType.vpTypes.push_back(D3DCREATE_SOFTWARE_VERTEXPROCESSING);

		curAdapterInfo.deviceTypes.push_back(refDeviceType);

		 // ---------------------------------------------
		 // Enum for all Display Modes
		 // ---------------------------------------------
		 for (UINT modeIndex = 0; modeIndex < modeCount; modeIndex++)
		 {
			  D3DDISPLAYMODE curMode;

			  hr = m_pD3D->EnumAdapterModes(adapterIndex, format, modeIndex, &curMode);

			  if (SUCCEEDED(hr))
			  {
				  curAdapterInfo.addDisplayMode(curMode);
// 				  std::cout << "Width= "   << curMode.Width		  << std::endl;
// 				  std::cout << "Height= "  << curMode.Height	  << std::endl;
// 				  std::cout << "Format= "  << curMode.Format	  << std::endl;
// 				  std::cout << "Refresh= " << curMode.RefreshRate << std::endl;
			  }
		}

		m_adpatersInfo.push_back(curAdapterInfo);
	}	

	//Checking for hardware vp.
	//TODO add more checking for future use!!!
	D3DCAPS9 caps; 
	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DFORMAT depthstencil;
	BOOL	  depthStencilEnabled = FALSE;
	if( m_pD3D->CheckDeviceFormat( 0, deviceType, D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 ) != D3D_OK )
	{
		depthStencilEnabled = FALSE;//crash?
	} 
	else
	{
		depthStencilEnabled = TRUE;
		depthstencil = D3DFMT_D24S8;
	}

	DWORD quality;
	D3DMULTISAMPLE_TYPE multisample;
	if( m_pD3D->CheckDeviceMultiSampleType( 0, deviceType, D3DFMT_X8R8G8B8, windowed, D3DMULTISAMPLE_16_SAMPLES,
		&quality ) != D3D_OK )
	{
		;
	} else
	{
		multisample = D3DMULTISAMPLE_16_SAMPLES;
	}

	//Filling out the D3DPRESENT_PARAMETERS structure.
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = Width;
	d3dpp.BackBufferHeight           = Height;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = m_hWnd;
	d3dpp.Windowed                   = windowed;
	d3dpp.EnableAutoDepthStencil     = depthStencilEnabled; 
	d3dpp.AutoDepthStencilFormat     = depthstencil;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	//Creating device.
	hr = m_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		deviceType,         // device type
		m_hWnd,               // window associated with device
		vp,                 // vertex processing
		&d3dpp,             // present parameters
		&m_pD3DDevice);            // return created device

	if( FAILED(hr) )
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		hr = m_pD3D->CreateDevice(
			D3DADAPTER_DEFAULT,
			deviceType,
			m_hWnd,
			vp,
			&d3dpp,
			&m_pD3DDevice);

		if( FAILED(hr) )
		{
			m_pD3D->Release(); // done with d3d9 object
			::MessageBox(m_hWnd, "failed to create the d3d device", "lolz", MB_OK);
			return hr;
		}
	}

	m_curD3dpp = d3dpp;

	m_pD3DDevice->GetViewport(&m_viewPort);

	// initialize the assetManger and create needed asset on the device aka the sprite
	m_assetManger.onCreateDevice(m_pD3DDevice);

	RECT rc;
	// Retrieve the final client size of the window
	::GetClientRect( m_hWnd, &rc );
	m_nViewX      = rc.left;
	m_nViewY      = rc.top;
	m_nViewWidth  = rc.right - rc.left;
	m_nViewHeight = rc.bottom - rc.top;
	m_clientRC = rc;


	//setting cameras settings 
	//setting the camera from where the player looks
	m_pCameras[0] = new CCamSpaceCraft(NULL);
	m_pCameras[0]->SetFOV( 45.0f );
	m_pCameras[0]->SetViewport( m_nViewX, m_nViewY, m_nViewWidth, m_nViewHeight, 1.01f, 5000.0f , m_pD3DDevice);
	//m_pCameras[0]->SetPosition(D3DXVECTOR3(0.0f,0.0f,-7.0f));
	m_pCameras[0]->SetPosition(D3DXVECTOR3(9.0f,48.0f,-7.0f));
	m_pCameras[0]->SetLookAt( D3DXVECTOR3(9,0,24) );
	m_pCameras[0]->UpdateRenderView( m_pD3DDevice );
	m_pCameras[0]->UpdateRenderProj( m_pD3DDevice );

	//setting the camera for the texture projector
// 	m_pCameras[1] = new CCamSpaceCraft(NULL);
// 	m_pCameras[1]->SetFOV( 45.0f );
// 	m_pCameras[1]->SetViewport(0, 0, 100, 100, 1.0f, 1.0f, NULL);
// 	//m_pCameras[1]->SetPosition(D3DXVECTOR3(0.0f, 10.0, 16.0f) );
// 	m_pCameras[1]->SetPosition(D3DXVECTOR3(0.0f, 0.0, -7.0f) );
// 	//m_pCameras[1]->SetLookAt(D3DXVECTOR3(0.0f, 0.0f, 0.0f) );

	m_pD3DDevice->GetViewport(&m_viewPort);

	RECT rcc,rc3;
	GetClientRect(m_hWnd,&rcc);
	GetWindowRect (m_hWnd,&rc3);

	//Setting Text Font for debug text
	UINT temp;
	m_fpsFont = m_assetManger.getFont(-12, 0 ,FW_BOLD, FALSE, temp);

// 	if( FAILED( hr = D3DXCreateFont( m_pD3DDevice, -12, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
// 		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
// 		"System", &m_fpsFont ) ) )
// 		return hr;
	
	initHandlesToShader();

	//m_lightTechHnadle = m_outlineEffect->GetTechniqueByName("lightShader");
	//m_lightTexTechHandle = m_outlineEffect->GetTechniqueByName("lightTexShader");

// 	m_shadeTex = NULL;
// 	m_shadeTex = m_assetManger.getTexture("goodjob.jpg");

	//m_testSprite.init(m_pD3DDevice);
	//m_shadeTex = getTexture(this,"goodjob.png");
	//m_toonEffect->SetTexture("ShadeTex",m_shadeTex);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : EnumDepthStencil ()
//-----------------------------------------------------------------------------
BOOL CGameWin::EnumDepthStencil(D3DFORMAT depthFormats[], UINT formatsCount, UINT adapter, D3DDEVTYPE deviceType, D3DFORMAT backBufferFromat, std::vector<D3DFORMAT>& validDepths)
{
	for (UINT i = 0; i < formatsCount; i++)
	{
		HRESULT hr;

		hr = m_pD3D->CheckDeviceFormat( adapter, deviceType, backBufferFromat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, depthFormats[i] );
		if (SUCCEEDED(hr))
			validDepths.push_back(depthFormats[i]);
	}

	if (validDepths.size() > 0)
		return TRUE;
	else
		return FALSE;
}

//-----------------------------------------------------------------------------
// Name : EnumMultiSample ()
//-----------------------------------------------------------------------------
void CGameWin::EnumMultiSample(UINT adapter, D3DDEVTYPE deviceType, D3DFORMAT backBufferFormat, bool windowed, std::vector<D3DMULTISAMPLE_TYPE>& validMultiSampleTypes)
{
	HRESULT hr;

	DWORD quality;
	D3DMULTISAMPLE_TYPE multisample;

	hr =  m_pD3D->CheckDeviceMultiSampleType( adapter, deviceType, backBufferFormat, windowed, D3DMULTISAMPLE_2_SAMPLES, &quality );
	if (SUCCEEDED(hr))
		validMultiSampleTypes.push_back(D3DMULTISAMPLE_2_SAMPLES);

	hr =  m_pD3D->CheckDeviceMultiSampleType( adapter, deviceType, backBufferFormat, windowed, D3DMULTISAMPLE_4_SAMPLES, &quality );
	if (SUCCEEDED(hr))
		validMultiSampleTypes.push_back(D3DMULTISAMPLE_4_SAMPLES);

	hr =  m_pD3D->CheckDeviceMultiSampleType( adapter, deviceType, backBufferFormat, windowed, D3DMULTISAMPLE_8_SAMPLES, &quality );
	if (SUCCEEDED(hr))
		validMultiSampleTypes.push_back(D3DMULTISAMPLE_8_SAMPLES);

	hr = m_pD3D->CheckDeviceMultiSampleType( adapter, deviceType, backBufferFormat, windowed, D3DMULTISAMPLE_16_SAMPLES, &quality );
	if (SUCCEEDED(hr))
		validMultiSampleTypes.push_back(D3DMULTISAMPLE_16_SAMPLES);
}

//-----------------------------------------------------------------------------
// Name : resetDevice ()
// Desc : reset the device and reload the resource that require it
//-----------------------------------------------------------------------------
void CGameWin::resetDevice(D3DPRESENT_PARAMETERS& d3dpp)
{
	HRESULT hr;
	// App is active
	m_bActive = true;

	// width height backbuffer format multisampletype depthstencil format refresh rate 

// 	RECT rc;
// 	::GetClientRect( m_hWnd, &rc );
// 	m_nViewX      = rc.left;
// 	m_nViewY      = rc.top;
// 	m_nViewWidth  = rc.right - rc.left;
// 	m_nViewHeight = rc.bottom - rc.top;
// 
// 	D3DPRESENT_PARAMETERS d3dpp;
// 	d3dpp.BackBufferWidth            = m_nViewWidth;
// 	d3dpp.BackBufferHeight           = m_nViewHeight;
// 	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
// 	d3dpp.BackBufferCount            = 1;
// 	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
// 	d3dpp.MultiSampleQuality         = 0;
// 	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
// 	d3dpp.hDeviceWindow              = m_hWnd;
// 	d3dpp.Windowed                   = true; //TODO: set this to variable not static value
// 	d3dpp.EnableAutoDepthStencil     = true; 
// 	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
// 	d3dpp.Flags                      = 0;
// 	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
// 	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;


	// 	if (m_GuiDilaogResManger.m_pStateBlock != NULL)
	// 		m_GuiDilaogResManger.OnD3D9LostDevice();

	m_assetManger.onLostDevice();

	if (m_outlineEffect)
		m_outlineEffect->OnLostDevice();

	if (m_highLightEffect)
		m_highLightEffect->OnLostDevice();

	//m_testSprite.onLostDevice();
// 	if (m_toonEffect)
// 		m_toonEffect->OnLostDevice();

// 	if (m_fpsFont)
// 		m_fpsFont->OnLostDevice();

	if (m_pD3DDevice) 
	{
		// Reset the device
		hr = m_pD3DDevice->TestCooperativeLevel();
		hr = m_pD3DDevice->Reset(&d3dpp);

		if ( m_pCameras[m_cameraIndex] )
		{
			m_pCameras[m_cameraIndex]->SetViewport( m_nViewX, m_nViewY, m_nViewWidth, m_nViewHeight, 1.01f, 5000.0f, m_pD3DDevice );
			m_pCameras[m_cameraIndex]->SetFOV( 45.0f );
			//m_pCameras[m_cameraIndex]->SetPosition(D3DXVECTOR3(0.0f,0.0f,-7.0f));
			m_pCameras[m_cameraIndex]->SetPosition(D3DXVECTOR3(9.0f,48.0f,-7.0f));
			m_pCameras[m_cameraIndex]->SetLookAt( D3DXVECTOR3(9,0,24) );
			m_pCameras[m_cameraIndex]->UpdateRenderView( m_pD3DDevice );
			m_pCameras[m_cameraIndex]->UpdateRenderProj( m_pD3DDevice );
		}

		//	m_GuiDilaogResManger.OnD3D9ResetDevice();
		m_assetManger.onResetDevice();
		//m_fpsFont->OnResetDevice();
		m_outlineEffect->OnResetDevice();
		m_highLightEffect->OnResetDevice();

		setRenderStates();
	}

	m_deviceReset = false;
}

//-----------------------------------------------------------------------------
// Name : FrameAdvance 
// Desc : calls every time a frame need to be rendered
//        handles the rendering of all the objects in the scene
//-----------------------------------------------------------------------------
void CGameWin::FrameAdvance(float timeDelta)
{
	m_debugString.clear();

	// check if the device need to be reset
	if (m_deviceReset)
	{
		if(m_windowed)
		{
			RECT rc; //= m_clientRC;
			::GetClientRect( m_hWnd, &rc );
			m_nViewX      = rc.left;
			m_nViewY      = rc.top;
			m_nViewWidth  = rc.right - rc.left;
			m_nViewHeight = rc.bottom - rc.top;
			 
			m_curD3dpp.BackBufferWidth = m_nViewWidth;
			m_curD3dpp.BackBufferHeight = m_nViewHeight;
		}
		resetDevice(m_curD3dpp);
	}

	HRESULT hr;

	POINT cursor;
	DWORD faceCount = -1;

	//set cursor postion
	cursor.x=1;
	cursor.y=1;

	static float angle  = (3.0f * D3DX_PI) / 2.0f;
	static float height = 5.0f;
	
	DrawDebugText(); //draw debug info

	RECT rc;//rect that says where the text should be drawn

	ProcessInput(timeDelta,angle,height);

	if (m_MainMenu.getVisible())
	{
		m_pCameras[m_cameraIndex]->Move(CCamera::DIR_RIGHT, 2 * timeDelta);
		m_cameraDelta = m_pCameras[m_cameraIndex]->GetPosition().z - m_prevCameraPos.z;
		m_prevCameraPos = m_pCameras[m_cameraIndex]->GetPosition();
		//m_bReturnCamera = true;
	}
	else
		if (m_bReturnCamera)
		{
			if (m_cameraDelta > 0)
			{
				m_bMoveCamera = true;
				m_returnDir = false;
				m_bReturnCamera = false;
			}
			else
			{
				m_bMoveCamera = true;
				m_returnDir = true;
				m_bReturnCamera = false;
			}
		}

	if (m_bMoveCamera)
	{
		if (!m_returnDir)
		{
			m_pCameras[m_cameraIndex]->Move(CCamera::DIR_LEFT, 24 * timeDelta);
			m_cameraDelta = m_pCameras[m_cameraIndex]->GetPosition().z - m_prevCameraPos.z;
			m_prevCameraPos = m_pCameras[m_cameraIndex]->GetPosition();
			if (m_cameraDelta > 0)
			{
				m_pCameras[m_cameraIndex]->SetPosition(D3DXVECTOR3(9.0f,48.0f,-7.0f));
				m_pCameras[m_cameraIndex]->SetLookAt( D3DXVECTOR3(9,0,24) );
				m_bMoveCamera = false;
			}
		}
		else
		{
			m_pCameras[m_cameraIndex]->Move(CCamera::DIR_RIGHT, 24 * timeDelta);
			m_cameraDelta = m_pCameras[m_cameraIndex]->GetPosition().z - m_prevCameraPos.z;
			m_prevCameraPos = m_pCameras[m_cameraIndex]->GetPosition();
			if (m_cameraDelta > 0)
			{
				m_pCameras[m_cameraIndex]->SetPosition(D3DXVECTOR3(9.0f,48.0f,-7.0f));
				m_pCameras[m_cameraIndex]->SetLookAt( D3DXVECTOR3(9,0,24) );
				m_bMoveCamera = false;
			}
		}
	}
	

	addDebugText("cursor X:", cursor.x);
	addDebugText("Y:", cursor.y);

	if (m_bPicking)
		if (pick(cursor,faceCount))
			m_debugString += " Hit!!";
		else
			m_debugString += " miss me NANA BANANA";

	addDebugText("face index:", faceCount);

	if (m_DrawingMethod == DRAW_ATTRIBOBJECT)
		m_debugString+="\n DRAW_ATTRIBOBJECT\n";
	else
		m_debugString+="\n DRAW_OBJECTATTRIB\n";

	m_debugString += m_gameBoard->getBoardStatus();

	if (m_gameBoard->getKingThreat())
		m_debugString += "\nking is in threat!";
	else
		m_debugString += "\nking is not in threat :)";

	if (!m_gameBoard->isBoardActive())
		m_debugString += "\ngame is over!";
	else
		m_debugString += "\ngame is still on :)";

	D3DXVECTOR3 position( cosf(angle) * 7.0f, height, sinf(angle) * 7.0f );
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXVECTOR3 position2( 0, 0, -7);

	m_pCameras[0]->UpdateRenderView( m_pD3DDevice );
	
	//
	// Draw the scene:
	//
	//m_pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	m_pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, d3d::GREEN, 1.0f, 0);

	m_pD3DDevice->BeginScene();

	D3DXVECTOR3 cameraEye = m_pCameras[m_cameraIndex]->GetPosition();
	m_outlineEffect->SetVector(m_vecEyeH, &D3DXVECTOR4(cameraEye.x, cameraEye.y, cameraEye.z, 0) );
	//m_outlineEffect->SetVector("vecEye", &D3DXVECTOR4(cameraEye.x, cameraEye.y, cameraEye.z, 0) );

	UINT numPasses = 0;
 
	m_pD3DDevice->SetFVF(VERTEX_FVF);

	switch(m_DrawingMethod)
	{
	case DRAW_ATTRIBOBJECT:
		{
			for (UINT atribNum = 0; atribNum < m_assetManger.getAttribCount(); atribNum++)
			{
				setAttribute(atribNum);

				m_outlineEffect->Begin(&numPasses, 0);

				for (UINT numPass = 0; numPass < numPasses; numPass++ )
				{
					for (UINT objI = 0; objI < m_objects.size(); objI++)
					{
						D3DXMATRIX WorldViewProj;
						D3DXMATRIX ViewProj;

						WorldViewProj = m_objects[objI]->getWorldMatrix() * m_pCameras[0]->GetViewMatrix() * m_pCameras[0]->GetProjMatrix();
						ViewProj = m_pCameras[0]->GetViewMatrix() * m_pCameras[0]->GetProjMatrix();
 
						if (objI == activeMeshIndex)
							m_outlineEffect->SetBool(m_bHighLightH, TRUE);
						else
							m_outlineEffect->SetBool(m_bHighLightH, FALSE);

						m_objects[objI]->drawSubset(m_pD3DDevice, atribNum, m_outlineEffect, numPass, ViewProj);
					}
				}
				m_outlineEffect->End();
			}
		}break;

	case DRAW_OBJECTATTRIB:
		{
			for (UINT objI = 0; objI < m_objects.size(); objI++)
			{
				D3DXMATRIX WorldViewProj, ViewProj;

				WorldViewProj = m_objects[objI]->getWorldMatrix() * m_pCameras[0]->GetViewMatrix() * m_pCameras[0]->GetProjMatrix();
				ViewProj = m_pCameras[0]->GetViewMatrix() * m_pCameras[0]->GetProjMatrix();

				hr = m_outlineEffect->SetMatrix("matWorldViewProj", &WorldViewProj);
				hr = m_outlineEffect->SetMatrix("matWorld", &m_objects[objI]->getWorldMatrix());

				for (UINT attribNum = 0; attribNum < m_assetManger.getAttribCount(); attribNum++)
				{
					hr = setAttribute(attribNum);

					hr = m_outlineEffect->Begin(&numPasses, 0);

					for (UINT numPass = 0; numPass < numPasses; numPass++ )
					{
						m_objects[objI]->drawSubset(NULL, attribNum,m_outlineEffect, numPass, ViewProj);
					}

					m_outlineEffect->End();
				}
			}
		}break;
	}

	//LPD3DXSPRITE sprite = m_assetManger.getSprite();
	//CMySprite *  sprite = m_assetManger.getMySprite();
	
	D3DXHANDLE illSHaderTech = m_highLightEffect->GetTechniqueByName("illuminateShader");

	hr = m_highLightEffect->SetTechnique(illSHaderTech);

	LPD3DXFONT pFont = m_assetManger.getFontPtr(0);
	LPD3DXSPRITE sprite = m_assetManger.getSprite();
	LPD3DXSPRITE sprite2 = m_assetManger.getTopSprite();

	sprite2->Begin(D3DXSPRITE_DONOTMODIFY_RENDERSTATE);

	sprite->Begin(D3DXSPRITE_DONOTMODIFY_RENDERSTATE);

// 	RECT rcDest;
// 	SetRect(&rcDest, 550, 0, 1050, 15);
// 	pFont->DrawTextA(sprite2, "Test!", -1, &rcDest, DT_LEFT , d3d::CYAN);

	m_OptionsDialog.OnRender(timeDelta, D3DXVECTOR3(m_nViewWidth - m_nViewX - 255, 0.0f, 0.0f), m_highLightEffect, m_assetManger);
	m_GuiSelectPawn.OnRender(timeDelta, D3DXVECTOR3(m_nViewWidth - m_nViewX - 255, 0.0f, 0.0f), m_highLightEffect, m_assetManger);
	m_MainMenu.OnRender(timeDelta, D3DXVECTOR3(m_nViewWidth - m_nViewX - 255, 0.0f, 0.0f), m_highLightEffect, m_assetManger);

 	CMySprite* pMySprite = m_assetManger.getMySprite();
 	pMySprite->render(m_highLightEffect);

	sprite->End();

	pMySprite->renderTopQuads(m_highLightEffect);
	sprite2->End();


	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetFVF(VERTEX_FVF);

	SetRect( &rc, 0, 0, 30, 30 );
	m_fpsFont->DrawText( NULL,
		m_debugString.c_str(), -1, &rc,
		DT_NOCLIP, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

//	m_GuiDialog.OnRender(timeDelta);

	hr = m_pD3DDevice->EndScene();
	//TODO: add something that checks if the device was lost
	hr = m_pD3DDevice->Present(0, 0, 0, 0);

	m_pD3DDevice->SetFVF(VERTEX_FVF);
	setRenderStates();

}

//-----------------------------------------------------------------------------
// Name : DrawDebugText ()
// Desc : render to screen the debug string that include info about fps, camera pos, etc..
//-----------------------------------------------------------------------------
void CGameWin::DrawDebugText()
{
	D3DXVECTOR3 camPos;
	D3DXVECTOR3 camLook;

	camPos=m_pCameras[m_cameraIndex]->GetPosition();
	camLook=m_pCameras[m_cameraIndex]->GetLook();

	addDebugText("FPS:", m_timer->getFPS());
	addDebugText("POS:", camPos.x);
	addDebugText(NULL, camPos.y);
	addDebugText(NULL, camPos.z);
	addDebugText("LOOK: X:", camLook.x);
	addDebugText("Y:", camLook.y);
	addDebugText("Z:", camLook.z);
	addDebugText("\ncurrent active mesh:", activeMeshIndex);

}

//-----------------------------------------------------------------------------
// Name : setAttribute ()
// Desc : selects the current attribute for rendering and sends it's data to shader
//-----------------------------------------------------------------------------
HRESULT CGameWin::setAttribute(UINT attribNum)
{
	HRESULT hr;
	long  MaterialIndex, TextureIndex;
	const ATTRIBUTE_ITEM& attribComboItem = m_assetManger.getAtribItem(attribNum);

	//setup current matial info to shader
	TextureIndex  = attribComboItem.TextureIndex;

	if (TextureIndex >= 0)
	{
		hr = m_outlineEffect->SetTechnique(m_lightTexTechHandle);
		hr = m_outlineEffect->SetTexture(m_MeshTextureH, m_assetManger.getTextureItem(TextureIndex)->Texture);
		//hr = m_outlineEffect->SetTexture("MeshTexture", m_pTextureList[TextureIndex]->Texture);
	}
	else
	{
		hr = m_outlineEffect->SetTechnique(m_lightTechHnadle);
	}

	MaterialIndex = attribComboItem.MaterialIndex;

	if ( MaterialIndex >= 0 )
	{
		const OBJMATERIAL& curMatterialItem = m_assetManger.getMaterialItem(MaterialIndex);
		D3DXVECTOR4 matrialDiffuse = curMatterialItem.Diffuse; 

		D3DXVECTOR4 matrialAmbient = curMatterialItem.Ambient;
		matrialAmbient.x *= 0.6f;
		matrialAmbient.y *= 0.6f;
		matrialAmbient.z *= 0.6f;

		D3DXVECTOR4 matrialSpecular = curMatterialItem.Specular;

		m_outlineEffect->SetVector(m_vecDiffuseMtrlH, &matrialDiffuse);
		m_outlineEffect->SetVector(m_vecAmbientMtrlH, &matrialAmbient);
		m_outlineEffect->SetVector(m_vecSpecularMtrlH, &matrialSpecular);
		m_outlineEffect->SetFloat(m_SpecularPowerH, curMatterialItem.Power);
		// 		m_outlineEffect->SetVector("vecDiffuse", &matrialDiffuse);
		// 		m_outlineEffect->SetVector("vecAmbient", &matrialAmbient);
	}
	else
	{
		m_outlineEffect->SetVector(m_vecDiffuseMtrlH, &D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f) );
		m_outlineEffect->SetVector(m_vecAmbientMtrlH, &D3DXVECTOR4( 1.0f * 0.6f, 1.0f * 0.6f, 1.0f * 0.6f, 1.0f) );
		m_outlineEffect->SetVector(m_vecSpecularMtrlH, &D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f));
		m_outlineEffect->SetFloat(m_SpecularPowerH, 8.0f);
		// 		m_outlineEffect->SetVector("vecDiffuse", &D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f) );
		// 		m_outlineEffect->SetVector("vecAmbient", &D3DXVECTOR4( 1.0f * 0.6f, 1.0f * 0.6f, 1.0f * 0.6f, 1.0f) );
	}

	return hr;
}

//-----------------------------------------------------------------------------
// Name : setLight ()
// Desc : selects the given light as an active light 
//-----------------------------------------------------------------------------
void CGameWin::setLight(LIGHT_PREFS& light, ID3DXEffect * effect, UINT index)
{
	effect->SetValue (m_lightPosH[index], &light.lightPos, sizeof(D3DXVECTOR3) );
	effect->SetVector(m_vecLightDirH[index], &light.lightDir);
	effect->SetValue (m_lightAttenH[index], &light.lightAtten, sizeof(D3DXVECTOR3) );

	effect->SetVector(m_vecDiffuseLightH[index], &light.lightDiffuse);
	effect->SetVector(m_vecSpecularLightH[index], &light.lightSpecular);
	effect->SetVector(m_vecAmbientLightH[index], &light.lightAmbient);
	effect->SetFloat (m_SpotPowerH[index], light.lightSpotPower);

	//effect->SetFloat (m_SpecularPowerH, light.lightSpecularPower);
}

//-----------------------------------------------------------------------------
// Name : setRenderStates ()
// Desc : sets the render state that the game uses for rendering
//-----------------------------------------------------------------------------
void CGameWin::setRenderStates() 
{
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	//m_pD3DDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	m_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

	m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	m_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	//m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)8);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 

	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);


}
//-----------------------------------------------------------------------------
// Name : addDebugText ()
// Desc : add a string to the debug text buffer also allows to give a variable that
//		  has a conversion function to string
//-----------------------------------------------------------------------------
template <class ValueType> 
void CGameWin::addDebugText(char* Text,ValueType value )
{
	std::stringstream out;

	if (Text != NULL)
		m_debugString += Text;

	m_debugString += " ";

	out << value;
	m_debugString += out.str() + " ";


}

//-----------------------------------------------------------------------------
// Name : BuildObjects 
// Desc : creates all the object for the current scene
//-----------------------------------------------------------------------------
 bool CGameWin::BuildObjects()
{
	HRESULT hr;

	ULONG			  AttribID;

	//-----------------------------------------------------------------------------
	// load objects attributes to insure right Rendering order
	//-----------------------------------------------------------------------------
	OBJMATERIAL Material;
	ZeroMemory( &Material, sizeof(OBJMATERIAL));
	Material.Diffuse = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0f );
	Material.Ambient = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0f );

	for (UINT i = 0; i < 2; i++)
		m_assetManger.getAttributeID(Textures[i], &Material, NULL);

	OBJMATERIAL matrial;

	matrial = d3d::YELLOW_MTRL;
	m_assetManger.getAttributeID(NULL, &matrial, NULL);

	matrial = d3d::BLUE_MTRL;
	m_assetManger.getAttributeID(NULL,&matrial,NULL);

	matrial = d3d::RED_MTRL;
	m_assetManger.getAttributeID(NULL,&matrial,NULL);

	matrial = d3d::WHITE_MTRL;
	//matrial.Diffuse.w = 1.0;
	matrial.Diffuse.x = 0;
	matrial.Diffuse.y = 0;
	matrial.Diffuse.z = 0;
	matrial.Diffuse.w = 0.0f;

	matrial.Specular.x = 0;
	matrial.Specular.y = 0;
	matrial.Specular.z = 0;
	matrial.Specular.w = 0;

	//m_attribIDs[FRAME] = assetManger.getAttributeID("frame.png",&matrial,NULL);
	m_assetManger.getAttributeID("bla.png",&matrial,NULL);
	//-----------------------------------------------------------------------------
	// End of load objects attributes
	//-----------------------------------------------------------------------------

	CMyMesh * piecesMesh[6];//hold pointers to the chess pieces in order to safely send it to the board

	m_assetManger.AllocMesh(7);		//allocating space for 6 pieces meshes and 1 for the board mesh 

	//creating chess pieces mesh
	for (int i = 0; i < 6; i++)
	{
		//create an empty mesh in the data pool
		m_assetManger.AddMesh(piecesMeshesPath[i]);
		//gives the pointer to that new mesh we just created
		piecesMesh[i] =  m_assetManger.getLastLoadedMesh();
	}

	//setting board object settings
	OBJECT_PREFS boardObjSetting; 

	boardObjSetting.pos		= D3DXVECTOR3(-15.0f, -10.0f, 0.0f);
	boardObjSetting.rotAngels  = D3DXVECTOR3(0,0,0);
	boardObjSetting.scale      = D3DXVECTOR3(6.0f, 1.0f, 6.0f);

	TERRAIN_PREFS boardSettings;
	boardSettings.minBounds = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	boardSettings.maxBounds = D3DXVECTOR3(8.0f, 8.0f, 8.0f); // at the moment has no effect on the terrain
	boardSettings.numCellsWide = 8;
	boardSettings.numCellsHigh = 8;
	boardSettings.objPrefs = boardObjSetting;

	int distantFromBoard = abs( m_pCameras[0]->GetPosition().z) ;

	m_rotLimits.minX = boardObjSetting.pos.x - distantFromBoard;
	m_rotLimits.maxX = 8 * 6 + boardObjSetting.pos.x + distantFromBoard;
	m_rotLimits.minZ = boardObjSetting.pos.z - distantFromBoard;
	m_rotLimits.maxZ = 8 * 6 + boardObjSetting.pos.z + distantFromBoard;

	//allocating board mesh
	m_assetManger.AddMesh();
	CMyMesh* pBoardMesh = m_assetManger.getLastLoadedMesh();
	pBoardMesh->SetDataFormat(VERTEX_FVF,sizeof(USHORT));

	//getting attribute ID of the black matrial
	AttribID   = m_assetManger.getAttributeID(NULL,&d3d::RED_MTRL,NULL);
	
	CMyObject* pGameBoard = new board (m_pD3DDevice,m_assetManger, pBoardMesh, boardSettings, piecesMesh, AttribID,
		boost::bind(&CGameWin::addObject, this, _1 ) );

	if (pGameBoard)
		m_gameBoard = static_cast<board*>(pGameBoard);

	addObject(pGameBoard);

	CreateLights(m_outlineEffect);

	setRenderStates();

	CreateGUIObjects();
	
	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateGUIObjects ()
//-----------------------------------------------------------------------------
bool CGameWin::CreateGUIObjects()
{
	//-----------------------------------------------------------------------------
	// initialization of Options Dialog
	//-----------------------------------------------------------------------------
	m_OptionsDialog.init(100,100, 18, "Options", "dialog.png", D3DCOLOR_ARGB(200,255,255,255), m_hWnd, m_assetManger);
	m_OptionsDialog.LoadDialogFromFile("settings.txt", m_timer);
	m_OptionsDialog.CreateDialogUI();
	m_OptionsDialog.setVisible(false);

	CButtonUI* pOptionsOKbutton = nullptr;
	pOptionsOKbutton = m_OptionsDialog.getButton(IDC_OKBUTTON);
	pOptionsOKbutton->connectToClick( boost::bind(&CGameWin::OptionDialogOKClicked, this, _1) );

	m_GuiSelectPawn.init(500,100, 18, "Select Pawn", "dialog.png", D3DCOLOR_ARGB(200,255,255,255), m_hWnd, m_assetManger);
	m_GuiSelectPawn.LoadDialogFromFile("pawns.txt", m_timer);
	m_GuiSelectPawn.setVisible(false);
	m_GuiSelectPawn.setLocation( (m_nViewWidth / 2) - m_GuiSelectPawn.getWidth() / 2, m_nViewHeight / 2 - m_GuiSelectPawn.getHeight() / 2);

	m_GuiSelectPawn.getButton(IDC_KNIGHT)->connectToClick( boost::bind(&CGameWin::PromoteUnitToKnight, this, _1) );
	m_GuiSelectPawn.getButton(IDC_BISHOP)->connectToClick( boost::bind(&CGameWin::PromoteUnitToBishop, this, _1) );
	m_GuiSelectPawn.getButton(IDC_ROOK)->connectToClick( boost::bind(&CGameWin::PromoteUnitToRook, this, _1) );
	m_GuiSelectPawn.getButton(IDC_QUEEN)->connectToClick( boost::bind(&CGameWin::PromoteUnitToQueen, this, _1) );

	m_MainMenu.init(200,320, 18, "Main Menu", "dialog.png", D3DCOLOR_ARGB(200,255,255,255), m_hWnd, m_assetManger);
	m_MainMenu.LoadDialogFromFile("MainMenu.txt", m_timer);
	m_MainMenu.setLocation( (m_nViewWidth / 2) - m_MainMenu.getWidth() / 2, m_nViewHeight / 2 - m_MainMenu.getHeight() / 2);

	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile("board.sav", &FindFileData) ;
	if(handle == INVALID_HANDLE_VALUE)
		m_MainMenu.getButton(IDC_Continue)->setEnabled(false);

	m_MainMenu.getButton(IDC_NEWGAME)->connectToClick( boost::bind(&CGameWin::NewGameClicked, this, _1) );
	m_MainMenu.getButton(IDC_Continue)->connectToClick( boost::bind(&CGameWin::ContinueClicked, this, _1) );
	m_MainMenu.getButton(IDC_OPTIONS)->connectToClick( boost::bind(&CGameWin::OptionsClicked, this, _1 ) );
	m_MainMenu.getButton(IDC_EXIT)->connectToClick( boost::bind(&CGameWin::ExitClicked, this, _1) );
	

	return true;
}

//-----------------------------------------------------------------------------
// Name : createObject ()
// Desc : creates a object that will be added to the current scene
//-----------------------------------------------------------------------------
void CGameWin::createObject(CMyMesh* objMesh, OBJECT_PREFS* objectPref, ULONG* newAttribMap, ULONG atrributeCount )
{
	CMyObject* newObject;
	newObject = new CMyObject(objMesh,objectPref);
	//m_objects[m_nObjectCount]->attachMesh(objMesh);
	//AttribID = 0;

	if (newAttribMap && atrributeCount > 0)
		newObject->customiseAtrributes(newAttribMap, atrributeCount);

	m_objects.push_back(newObject);
}

//-----------------------------------------------------------------------------
// Name : addObject ()
//-----------------------------------------------------------------------------
void CGameWin::addObject(CMyObject* newObject)
{
	m_objects.push_back(newObject);
}

//-----------------------------------------------------------------------------
// Name : ProcessInput 
// Desc : process mouse and keyboard input for the game
//-----------------------------------------------------------------------------
void CGameWin::ProcessInput(float timeDelta,float &angle,float &height)
{
	//process keyboards input
	static UCHAR keysBuffer[ 256 ];
	float rotAngle = 0.0;
	D3DXMATRIX rotaion;

	ULONG direction =0;

	// Retrieve keyboard state
	if ( !GetKeyboardState( keysBuffer ) ) return;

	if (keysBuffer[VK_UP] & 0xF0 )
	{
		direction |= CCamera::DIR_FORWARD;
	}

	if (keysBuffer[VK_DOWN] & 0xF0)
	{
		direction |= CCamera::DIR_BACKWARD;
	}

	if (keysBuffer[VK_LEFT] & 0xF0)
	{
		direction |= CCamera::DIR_LEFT;
	}

	if (keysBuffer[VK_RIGHT] & 0xF0)
	{
		direction |= CCamera::DIR_RIGHT;
	}

	if (keysBuffer[VK_ESCAPE] & 0xF0)
	{
		m_MainMenu.setVisible(!m_MainMenu.getVisible()); 
		m_bReturnCamera = !m_MainMenu.getVisible();
	}

	m_pCameras[m_cameraIndex]->Move(direction, 2 * timeDelta);

	if (keysBuffer['E'] & 0xF0)
	{
		m_objects[activeMeshIndex]->Rotate( (D3DX_PI /8) * m_timer->getTimeElapsed(), 0.0f, 0.0f);
		//m_objects[activeMeshIndex]->Rotate( (D3DX_PI /8) * m_timer.getTimeElapsed(), (D3DX_PI /8) * m_timer.getTimeElapsed(), 0.0f);
		//m_objects[activeMeshIndex]->m_rotAngle += (D3DX_PI /8) * m_timer.getTimeElapsed();
	}

	if (keysBuffer['Q'] & 0xF0)
	{
		m_objects[activeMeshIndex]->Rotate( -(D3DX_PI /8) * m_timer->getTimeElapsed(), 0.0f, 0.0f);
		//m_objects[activeMeshIndex]->Rotate( 0.0f, (D3DX_PI /8) * m_timer.getTimeElapsed(), 0.0f);
		//m_objects[activeMeshIndex]->m_rotAngle += -(D3DX_PI /8) * m_timer.getTimeElapsed();
	}

	if (keysBuffer['D'] & 0xF0)
	{
		m_objects[activeMeshIndex]->m_mtxWorld._41+=0.001f;
	}

	if (keysBuffer['A'] & 0xF0)
	{
		m_objects[activeMeshIndex]->m_mtxWorld._41-=0.001f;
	}

	if (keysBuffer['W'] & 0xF0)
	{
		m_objects[activeMeshIndex]->m_mtxWorld._42+=0.001f;
	}

	if (keysBuffer['S'] & 0xF0)
	{
		m_objects[activeMeshIndex]->m_mtxWorld._42-=0.001f;
	}

	if (keysBuffer['Z'] & 0xF0)
	{
		m_objects[activeMeshIndex]->m_mtxWorld._43+=0.001f;
	}

	if (keysBuffer['X'] & 0xF0)
	{
		m_objects[activeMeshIndex]->m_mtxWorld._43-=0.001f;
	}

	if (keysBuffer[VK_NUMPAD4] & 0xF0)
	{
		((CCamSpaceCraft*)m_pCameras[m_cameraIndex])->Yaw( -2 * timeDelta);
	}

	if (keysBuffer[VK_NUMPAD6] & 0xF0)
	{
		((CCamSpaceCraft*)m_pCameras[m_cameraIndex])->Yaw( 2* timeDelta);
	}

	if (keysBuffer[VK_NUMPAD8] & 0xF0)
	{
		((CCamSpaceCraft*)m_pCameras[m_cameraIndex])->Pitch( -2 * timeDelta);
	}

	if (keysBuffer[VK_NUMPAD2] & 0xF0)
	{
		((CCamSpaceCraft*)m_pCameras[m_cameraIndex])->Pitch( 2* timeDelta);
	}

	if (keysBuffer['M'] & 0xF0) //lets you advance to the next drawing method and loop it back to DRAW_SIMPLE if you pass the last method
	{
		if (m_DrawingMethod == DRAW_ATTRIBOBJECT)
			m_DrawingMethod = DRAW_OBJECTATTRIB;
		else
			m_DrawingMethod = DRAW_ATTRIBOBJECT;
	}

	if (( keysBuffer['C'] & 0xF0 ))
	{
		float buttonPressTime;
		static float buttonPrevPressTime;
		float buttonPressDelta;

		buttonPressTime = m_timer->getCurrentTime();
		buttonPressDelta = buttonPressTime - buttonPrevPressTime;

		if (buttonPressDelta > 0.017f)
		{	
			if (m_cameraIndex == 0)
			{
				m_cameraIndex = 1;
				//m_pCameras[1]->SetViewport(0, 0, 1024, 768, 1.01f, 5000.0f, m_pD3DDevice);
				//m_pCameras[1]->SetViewport( m_nViewX, m_nViewY, m_nViewWidth, m_nViewHeight, 1.01f, 5000.0f , m_pD3DDevice);
				m_pCameras[1]->UpdateRenderView( NULL );
				m_pCameras[1]->UpdateRenderProj( NULL );
			}
			else	
			{
				m_cameraIndex = 0;
				m_pCameras[0]->SetViewport( m_nViewX, m_nViewY, m_nViewWidth, m_nViewHeight, 1.01f, 5000.0f , m_pD3DDevice);
				m_pCameras[0]->UpdateRenderView( m_pD3DDevice );
				m_pCameras[0]->UpdateRenderProj( m_pD3DDevice );
			}
		}
		buttonPrevPressTime = buttonPressTime;
	}

// 	if (keysBuffer['R'] & 0xF0)
// 		if (gameBoard != NULL)
// 			gameBoard->resetGame();

	//processing mouse input 
	POINT        CursorPos;
	float        X = 0.0f, Y = 0.0f;

	// Now process the mouse (if the button is pressed)
	//if (false)
	if ( GetCapture() == m_hWnd && m_bCamRotate)
	{
		// Hide the mouse pointer
		SetCursor( NULL );

		// Retrieve the cursor position
		GetCursorPos( &CursorPos );

		// Calculate mouse rotational values
		X = (float)(CursorPos.x - m_OldCursorPos.x) / 3.0f;
		Y = (float)(CursorPos.y - m_OldCursorPos.y) / 3.0f;

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );

	} // End if Captured

	if ( X != 0.0f || Y != 0.0f )
	{
		// Rotate our camera
		if ( X || Y ) 
		{
			//( (CCamSpaceCraft*)m_pCamera )->Rotate(Y, X, 0.0f);
			( (CCamSpaceCraft*)m_pCameras[m_cameraIndex] )->RotateAroundPoint(D3DXVECTOR3(9,0,24), X, Y, m_rotLimits);
			//((CCamSpaceCraft*)m_pCameras[m_cameraIndex])->Rotate( Y, X, 0.0f );

		} // End if any rotation
	}

	m_pCameras[0]->UpdateRenderView(m_pD3DDevice);

// 	D3DXVECTOR3 lookVecotr = m_pCameras[1]->GetLook();
// 	D3DXVECTOR3 rightVector = m_pCameras[1]->GetRight();
// 	D3DXVECTOR3 upVector = m_pCameras[1]->GetUp();
// 
// 	D3DXVECTOR3 angles;
// 	angles.z = acos( D3DXVec3Dot(&lookVecotr, &D3DXVECTOR3(0.0f, 0.0f, 1.0f ) ) );
// 	angles.x = acos( D3DXVec3Dot(&rightVector, &D3DXVECTOR3(1.0f, 0.0f, 0.0f) ) );
// 	angles.y = acos( D3DXVec3Dot(&upVector, &D3DXVECTOR3(0.0f, 1.0f, 0.0f) ) );
// 
// 
// 	m_objects[0]->setRotaionMatrix( ( (CCamSpaceCraft*)m_pCameras[m_cameraIndex])->getRotateMatrix() );
	//m_objects[0]->setRotAngels(angles);
}

//-----------------------------------------------------------------------------
// Name : pick() 
// Desc : finds on what polygon the cursor is on and sets accordingly the 
//        current active object
//-----------------------------------------------------------------------------
HRESULT CGameWin::pick(POINT& cursor,DWORD& faceCount)
{
	POINT ptCursor;
	const D3DXMATRIX* pmatProj = &m_pCameras[m_cameraIndex]->GetProjMatrix();
	DWORD activeFaceCount;

	activeMeshIndex = -1;

	GetCursorPos( &ptCursor );
	cursor = ptCursor;

	if (m_windowed)
		ScreenToClient( m_hWnd, &ptCursor );

	cursor = ptCursor;

	// Compute the vector of the Pick ray in screen space
	D3DXVECTOR3 v;
	v.x = ( ( ( 2.0f * ptCursor.x ) / m_pCameras[m_cameraIndex]->GetViewport().Width ) - 1 ) / pmatProj->_11;
	v.y = -( ( ( 2.0f * ptCursor.y ) / m_pCameras[m_cameraIndex]->GetViewport().Height ) - 1 ) / pmatProj->_22;
	v.z = 1.0f;

	D3DXMATRIX m,view;
	D3DXVECTOR3 rayOrigin,rayDir;
	float minDistance = 30000.0;
	HRESULT hRet;

	view = m_pCameras[m_cameraIndex]->GetViewMatrix();

	D3DXMatrixInverse( &m, NULL, &view );

	// Transform the screen space pick ray into 3D space
	rayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	rayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	rayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	//D3DXVec3Normalize(&rayDir,&rayDir);
	rayOrigin.x = m._41;
	rayOrigin.y = m._42;
	rayOrigin.z = m._43;

  	for (ULONG i=0 ; i < m_objects.size() ; i++)
  	{	
		if (!m_objects[i]->isObjectHidden())
		{
			//getting our mesh object 
			CMyMesh* curMesh = m_objects[i]->getMesh();
			// Use inverse of matrix
			D3DXMATRIX matInverse;

			D3DXMatrixInverse(&matInverse,NULL,&m_objects[i]->getWorldMatrix());


			// Transform ray origin and direction by inv matrix
			D3DXVECTOR3 rayObjOrigin,rayObjDirection;

			D3DXVec3TransformCoord(&rayObjOrigin,&rayOrigin,&matInverse);
			
			D3DXVec3TransformNormal(&rayObjDirection,&rayDir,&matInverse);

			BOOL hasHit;
			float distanceToCollision;

			hRet = D3DXIntersect(curMesh->GetMesh(), &rayObjOrigin, &rayObjDirection, &hasHit, &faceCount, NULL, NULL, &distanceToCollision, NULL, NULL);
			
			if (FAILED(hRet) ) return hRet;

			if (hasHit)
 				if (distanceToCollision < minDistance)
 				{
 					minDistance = distanceToCollision;
 					activeMeshIndex = i;
					activeFaceCount = faceCount;
 				}
		}
 	}
	if (activeMeshIndex > -1)
	{
 		m_gameBoard->processPress(m_objects[activeMeshIndex],activeFaceCount);
 		if (m_gameBoard->isUnitPromotion())
 			m_GuiSelectPawn.setVisible(true);
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : PromoteUnitToKnight() 
//-----------------------------------------------------------------------------
void CGameWin::PromoteUnitToKnight(CButtonUI* pButton)
{
	m_gameBoard->PromoteUnit(KNIGHT);
	m_GuiSelectPawn.setVisible(false);
}

//-----------------------------------------------------------------------------
// Name : PromoteUnitToBishop() 
//-----------------------------------------------------------------------------
void CGameWin::PromoteUnitToBishop(CButtonUI* pButton)
{
	m_gameBoard->PromoteUnit(BISHOP);
	m_GuiSelectPawn.setVisible(false);
}

//-----------------------------------------------------------------------------
// Name : PromoteUnitToRook() 
//-----------------------------------------------------------------------------
void CGameWin::PromoteUnitToRook(CButtonUI* pButton)
{
	m_gameBoard->PromoteUnit(ROOK);
	m_GuiSelectPawn.setVisible(false);
}

//-----------------------------------------------------------------------------
// Name : PromoteUnitToQueen() 
//-----------------------------------------------------------------------------
void CGameWin::PromoteUnitToQueen(CButtonUI* pButton)
{
	m_gameBoard->PromoteUnit(QUEEN);
	m_GuiSelectPawn.setVisible(false);
}

//-----------------------------------------------------------------------------
// Name : NewGameClicked() 
//-----------------------------------------------------------------------------
void CGameWin::NewGameClicked(CButtonUI* pButton)
{
	if (m_gameBoard != NULL)
	{
		m_objects.clear();
		m_MainMenu.setVisible(false);
		m_bReturnCamera = true;
		m_gameBoard->resetGame();
		addObject(m_gameBoard);
	}
}

//-----------------------------------------------------------------------------
// Name : ContinueClicked() 
//-----------------------------------------------------------------------------
void CGameWin::ContinueClicked(CButtonUI* pButton)
{
	m_objects.clear();
	m_gameBoard->LoadBoardFromFile();
	m_MainMenu.setVisible(false);
	m_bReturnCamera = true;
	addObject(m_gameBoard);
}

//-----------------------------------------------------------------------------
// Name : OptionsClicked() 
//-----------------------------------------------------------------------------
void CGameWin::OptionsClicked(CButtonUI* pButton)
{
	m_OptionsDialog.setVisible(true);
	m_MainMenu.setVisible(false);
	m_bReturnCamera = true;
}

//-----------------------------------------------------------------------------
// Name : ExitClicked() 
//-----------------------------------------------------------------------------
void CGameWin::ExitClicked(CButtonUI* pButton)
{
	m_gameBoard->SaveBoardToFile();
	m_MainMenu.setVisible(false);
	m_bReturnCamera = true;

	SendMessage(m_hWnd, WM_CLOSE, 0, 0);
}

//-----------------------------------------------------------------------------
// Name : StaticOnGUIEvent 
//-----------------------------------------------------------------------------
// void CALLBACK CGameWin::StaticOnGUIEvent(HWND hWnd, UINT nEvent, int nControlID, void* pUserContext )
// {
// 	// Obtain the correct destination for this message
// 	CGameWin* Destination = (CGameWin*)GetWindowLong( hWnd, GWL_USERDATA );
// 
// 	// If the hWnd has a related class, pass it through
// 	if (Destination) return Destination->OnGUIEvent(hWnd,nEvent, nControlID,  pUserContext);
// }

//-----------------------------------------------------------------------------
// Name : OnGUIEvent 
//-----------------------------------------------------------------------------
// void CGameWin::OnGUIEvent(HWND hWnd, UINT nEvent, int nControlID, void* pUserContext )
// {
// 	switch(nControlID)
//  	{
// 	case IDC_CREATECONTROL:
// 		{
// 			POINT cursorPoint;
// 			LPCTSTR pControlText;
// 			
// 			ULONG selectedItem = (ULONG)m_EditDialog.getComboBox(IDC_COMBOX)->GetSelectedData();
// 			UINT  controlWidth =  atoi( m_EditDialog.getEditBox(IDC_WIDTHEDITBOX)->GetText() );
// 			UINT  controlHeight = atoi( m_EditDialog.getEditBox(IDC_HEIGHTEDITBOX)->GetText() );
// 
// 			pControlText = m_EditDialog.getEditBox(IDC_TEXTEDITBOX)->GetText();
// 
// 			GetCursorPos( &cursorPoint );
// 			ScreenToClient( m_hWnd, &cursorPoint );
// 
// 			switch(selectedItem)
// 			{
// 			case CControlUI::BUTTON:
// 				{
// 					m_GenDialog.addButton(m_curControlID + 1, pControlText, cursorPoint.x,
// 						cursorPoint.y, controlWidth, controlWidth, 0);
// 					m_controlInCreation = true;
// 				}break;
// 
// 			case CControlUI::CHECKBOX:
// 				{
// 					m_GenDialog.addCheckBox(m_curControlID + 1, cursorPoint.x, cursorPoint.y,
// 						controlWidth, controlHeight, 0);
// 					m_controlInCreation = true;
// 				}break;
// 
// 			case CControlUI::RADIOBUTTON:
// 				{
// 					m_GenDialog.addRadioButton(m_curControlID + 1, cursorPoint.x,
// 						cursorPoint.y, controlWidth, controlHeight, 0, 0);
// 					m_controlInCreation = true;
// 				}break;
// 
// 			case CControlUI::COMBOBOX:
// 				{
// 					m_GenDialog.addComboBox(m_curControlID + 1, pControlText, cursorPoint.x,
// 						cursorPoint.y, controlWidth, controlHeight, 0);
// 					m_controlInCreation = true;
// 				}break;
// 
// 			case CControlUI::STATIC:
// 				{
// 					m_GenDialog.addStatic(m_curControlID + 1, pControlText, cursorPoint.x,
// 						cursorPoint.y, controlWidth, controlHeight);
// 					m_controlInCreation = true;
// 				}break;
// 
// 			case CControlUI::EDITBOX:
// 				{
// 					m_GenDialog.addEditbox(m_curControlID + 1, pControlText, cursorPoint.x,
// 						cursorPoint.y, controlWidth, controlHeight, m_timer);
// 					m_controlInCreation = true;
// 				}break;
// 
// 			case CControlUI::LISTBOX:
// 				{
// 					m_GenDialog.addListBox(m_curControlID + 1, cursorPoint.x, cursorPoint.y,
// 						controlWidth, controlHeight);
// 					m_controlInCreation = true;
// 				}break;
// 
// 			case CControlUI::SLIDER:
// 				{
// 					m_GenDialog.addSlider(m_curControlID + 1, cursorPoint.x, cursorPoint.y,
// 						controlWidth, controlHeight, 0, 0, 0);
// 					m_controlInCreation = true;
// 				}break;
// 			}
// 
// 			//MessageBox(m_hWnd,"lolz button pressed","lolz lolz lolz",MB_OK);
// 		}break;
// 
// 	case IDC_COMBOX: 
// 		{
// 			switch (nEvent)
// 			{
// 			case EVENT_COMBOBOX_SELECTION_CHANGED: 
// 				{
// 					CComboBoxUI* pSelectionBox =  m_EditDialog.getComboBox(IDC_COMBOX);
// 					ComboBoxItem* pSelectedItem =  pSelectionBox->GetSelectedItem();
// 
// 					UINT selectedItem = (UINT)pSelectedItem->pData;
// 
// 					switch(selectedItem)
// 					{
// 					case CControlUI::STATIC:
// 						{
// 							m_EditDialog.getStatic(IDC_RADIOGROUPTEXT)->setVisible(false);
// 							m_EditDialog.getComboBox(IDC_RADIOBUTTONGROUP)->setVisible(false);
// 						}break;
// 					case CControlUI::RADIOBUTTON:
// 						{
// 							m_EditDialog.getStatic(IDC_RADIOGROUPTEXT)->setVisible(true);
// 							m_EditDialog.getComboBox(IDC_RADIOBUTTONGROUP)->setVisible(true);
// 						}break;
// 					}
// 				}break;
// 			}
// 
// 		}break;
// 	case IDC_KNIGHT:
// 		{
// 			gameBoard->PromoteUnit(KNIGHT);
// 			m_GuiSelectPawn.SetVisible(false);
// 		}break;
// 
// 	case IDC_BISHOP:
// 		{
// 			gameBoard->PromoteUnit(BISHOP);
// 			m_GuiSelectPawn.SetVisible(false);
// 		}break;
// 
// 	case IDC_ROOK:
// 		{
// 			gameBoard->PromoteUnit(ROOK);
// 			m_GuiSelectPawn.SetVisible(false);
// 		}break;
// 
// 	case IDC_QUEEN:
// 		{
// 			gameBoard->PromoteUnit(QUEEN);
// 			m_GuiSelectPawn.SetVisible(false);
// 		}break;
// 
 	//}
//}

//-----------------------------------------------------------------------------
// Name : initHandlesToShader ()
// Desc : get all the handles to the variables in the shader in order to pass it 
//		  data about textures ,materials and lights
//-----------------------------------------------------------------------------
bool CGameWin::initHandlesToShader()
{
	//HRESULT hr;
	//loading the effect file for the scene
	if (!loadEffectFile("light4.fx",&m_outlineEffect))
		return false;

	if (!loadEffectFile("liuminate.fx",&m_highLightEffect))
		return false;

	//get handles to techniques
	m_lightTechHnadle = m_outlineEffect->GetTechniqueByName("lightShader");
	m_lightTexTechHandle = m_outlineEffect->GetTechniqueByName("lightTexShader");
	//m_texProjHandle = m_texProjEffect->GetTechniqueByName("texProjShader");

	m_matWorldH			= m_outlineEffect->GetParameterByName(NULL, "matWorld");
	m_matWorldViewProjH = m_outlineEffect->GetParameterByName(NULL, "matWorldViewProj");
	m_matWorldInverseTH = m_outlineEffect->GetParameterByName(NULL, "matWorldInverseT");

	m_projTextureMatrixH = m_outlineEffect->GetParameterByName(NULL, "projTextureMatrix");

	m_vecEyeH			 = m_outlineEffect->GetParameterByName(NULL, "vecEye");

	m_numActiveLightsH	 = m_outlineEffect->GetParameterByName(NULL, "numActiveLights");

	D3DXHANDLE lights = NULL;
	for (int i = 0; i < MAX_ACTIVE_LIGHTS; i++)
	{
		lights					 = m_outlineEffect->GetParameterElement("lights", i);

		m_lightPosH[i]			 = m_outlineEffect->GetParameterByName(lights, "pos");
		m_vecLightDirH[i]		 = m_outlineEffect->GetParameterByName(lights, "direction");
		m_lightAttenH[i]		 = m_outlineEffect->GetParameterByName(lights, "Attenuation012");

		m_vecAmbientLightH[i]	 = m_outlineEffect->GetParameterByName(lights, "ambient");
		m_vecDiffuseLightH[i]    = m_outlineEffect->GetParameterByName(lights, "diffuse");
		m_vecSpecularLightH[i]   = m_outlineEffect->GetParameterByName(lights, "specular");
		m_SpotPowerH[i]          = m_outlineEffect->GetParameterByName(lights, "spotPower");
	}

	m_vecAmbientMtrlH    = m_outlineEffect->GetParameterByName(NULL, "vecAmbientMtrl");
	m_vecDiffuseMtrlH    = m_outlineEffect->GetParameterByName(NULL, "vecDiffuseMtrl");
	m_vecSpecularMtrlH	 = m_outlineEffect->GetParameterByName(NULL, "vecSpecularMtrl");
	m_SpecularPowerH	 = m_outlineEffect->GetParameterByName(NULL, "SpecularPower");
	

	m_bHighLightH		 = m_outlineEffect->GetParameterByName(NULL, "bHighLight");

	m_MeshTextureH		 = m_outlineEffect->GetParameterByName(NULL, "MeshTexture");

	m_UITexture			 = m_highLightEffect->GetParameterByName(NULL, "UITexture");
	m_bUIHighLight		 = m_highLightEffect->GetParameterByName(NULL, "bHighLight");
	m_bUITexutre		 = m_highLightEffect->GetParameterByName(NULL, "bTexture");

	CMyObject::setShadersHandles(m_matWorldViewProjH, m_matWorldH, m_matWorldInverseTH);
	CMySprite::setShadersHandles(m_UITexture, m_bUIHighLight, m_bUITexutre);

	return true;
}

//-----------------------------------------------------------------------------
// Name : loadEffectFile ()
// Desc : loads an effect file from file
//-----------------------------------------------------------------------------
bool CGameWin::loadEffectFile(LPCSTR fileNmae, LPD3DXEFFECT* effectHandle)
{
	HRESULT hr;
	//loading the effect file for the scene
	ID3DXBuffer* errorBuffer = 0;

	hr = D3DXCreateEffectFromFile(
		m_pD3DDevice,       // associated device
		fileNmae, // effect filename
		0,                // no preprocessor definitions
		0,                // no ID3DXInclude interface
		D3DXSHADER_DEBUG, // compile flags
		0,                // don't share parameters
		effectHandle,      // return effect
		&errorBuffer);    // return error messages

	if( errorBuffer )
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(errorBuffer);
	}

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	//D3DXHANDLE a = NULL;
	//effect->GetParameterByName(a,"matWorld");

	return true;
}

//-----------------------------------------------------------------------------
// Name : addLight ()
// Desc : add a new light to the current scene
//-----------------------------------------------------------------------------
bool CGameWin::addLight(LIGHT_PREFS& light, ID3DXEffect * effect)
{
	if (m_numActiveLights < MAX_ACTIVE_LIGHTS)
	{
		m_lights.push_back(light);

		setLight(light, effect, m_numActiveLights);
		m_numActiveLights++;
		m_outlineEffect->SetInt(m_numActiveLightsH, m_numActiveLights);
		return true;
	}
	else
		return false; // can't add any more lights to this scene

}

//-----------------------------------------------------------------------------
// Name : CreateLights ()
// Desc : creates all the needed lights for the current scene
//-----------------------------------------------------------------------------
void CGameWin::CreateLights(ID3DXEffect * effect)
{
	LIGHT_PREFS light;
 	light = d3d::InitDirectionalLight(D3DXVECTOR4(1.0, -0.0, 0.25f, 0.0), d3d::WHITE );
 
 	addLight(light,effect);
}

//-----------------------------------------------------------------------------
// Name : Release ()
// Desc : Release all active resources
//-----------------------------------------------------------------------------
void CGameWin::Release( )
{
	ULONG i;
	// 	if (m_pCamera)
	// 		delete m_pCamera;

	for (i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i] != NULL)
		{
			delete m_objects[i];
			m_objects[i] = NULL;
		}
	}

	// Clear out the skybox mesh
	//m_SkyBoxMesh.Release();

	m_assetManger.onDestoryDevice();

	m_fpsFont = NULL;
// 	if (m_fpsFont)
// 		m_fpsFont->Release();

	// Release Direct3D Objects
	if ( m_pD3DDevice ) 
		m_pD3DDevice->Release();

	if (m_outlineEffect)
		m_outlineEffect->Release();

// 	if (m_toonEffect)
// 		m_toonEffect->Release();

	// Free strdup()'d string data
	//if ( m_strDataPath ) free( m_strDataPath );

	// Clear Variables
	m_pD3DDevice       = NULL;
	//m_bHardwareTnL     = false;
	//m_strDataPath      = NULL;

	//m_nReservedLights  = 0;
	//m_nLightLimit      = 0;
}
