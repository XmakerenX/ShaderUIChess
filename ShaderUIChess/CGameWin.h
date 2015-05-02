#pragma once

//Includes for this class

//external libs
#include <Windows.h>
#include <d3dx9.h>
#include <string>
#include <sstream>
#include <limits>
#include <math.h>
#include <unordered_map>
#include <boost/signals2/signal.hpp>
#include <boost/bind.hpp>

//common libs for the project
#include "main.h"
#include "CTimer.h"
//#include "resource.h"
#include "rendering/d3d.h"

//the rendering lib
#include "rendering/Cameras.h"
#include "rendering/CMyObject.h"
#include "rendering/CTerrain.h"
#include "rendering/CMyVertex.h"
#include "rendering/CLight.h"

#include "CAssetManager.h"

//#include "CSilhouetteEdges.h"

//#include "chess engine/board.h"

#include "GUI/CDialogUI.h"
#include "GUI/CEditDialogUI.h"
#include "GUI/COptionDialogUI.h"

#include "rendering/CMySprite.h"
// #include "GUI\CDialog3D.h"
// #include "GUI\CDialogResourceManager3D.h"
// #include "GUI\CStatic3D.h"

//#include "Pawnsdef.h" //pawns promotion Dilaog definitions of controls ID's
#include "settingsDef.h"

const UINT MAX_ACTIVE_LIGHTS = 4;

class CMyObject;

//-----------------------------------------------------------------------------
// Definitions, Macros & Constants
//-----------------------------------------------------------------------------
// #define IDC_ENABLE		1
// #define IDC_DISABLE		2
// #define IDC_OUTPUT		3
// #define IDC_EDITBOX1	4
#define IDC_CONTROLTYPESTATIC	 101
#define IDC_COMBOX				 102
#define IDC_WIDTHSTATIC			 103
#define IDC_HEIGHTSTATIC		 104
#define IDC_WIDTHEDITBOX		 105
#define IDC_HEIGHTEDITBOX		 106
#define IDC_IDSTATIC			 107
#define IDC_IDEDITBOX			 108
#define IDC_CONTROLTEXT			 109
#define IDC_TEXTEDITBOX			 110
#define IDC_CREATECONTROL		 111
#define IDC_SLIDERMIN			 112
#define IDC_SLIDERMAX			 113
#define IDC_SLIDERMINTEXT		 114
#define IDC_SLIDERMAXTEXT		 115
#define IDC_RADIOGROUPTEXT       116
#define IDC_RADIOBUTTONGROUP     117
#define IDC_SLIDERSTATIC		 118
#define IDC_SLIDERMINEDITBOX	 119
#define IDC_SLIDERMAXEDITBOX	 120
#define IDC_LISTBOXITEMS		 121
#define IDC_LISTBOXSTATIC		 122
#define IDCLISTOXEDITBOX	     123
#define IDC_LISTBOXITEMSADD      124
#define IDC_LISTBOXITEMSREMOVE   125
#define IDC_COMBOXITEMS			 126
#define IDC_COMBOBOXITEMSADD	 127
#define IDC_COMBOBOXITEMSREMOVE	 128
#define IDC_DIALOGSTATIC		 129
#define IDC_DIALOGWIDTH			 130
#define IDC_DIALOGHEIGHT		 131
#define IDC_DIALOGSETSIZE		 132
#define IDC_FILENAMESTATIC		 133
#define IDC_FILENAMEEDITBOX		 134
#define IDC_LOADFILEBUTTON		 135
#define IDC_SAVEFILEBUTTON		 136
#define IDC_SETCHANGESBUTTON	 137
#define IDC_CONTROLXSTATIC	     138
#define IDC_CONTROLYSTATIC		 139
#define IDC_CONTROLX			 140
#define IDC_CONTROLY			 141
#define IDC_RELOCATEBUTTON		 142
#define IDC_DELETEBUTTON		 143
#define IDC_OPTIONSBUTTON		 144


#define IDC_GENCONTROLID         200
// #define IDC_LISTBOX     6

//-----------------------------------------------------------------------------
// Typedefs, Structures and Enumerators
//-----------------------------------------------------------------------------
enum DrawingMethod{DRAW_SIMPLE,DRAW_ATTRIBOBJECT,DRAW_OBJECTATTRIB};

const char	piecesMeshesPath[6][MAX_PATH] = {"pawn.x","knight.x","bishop.x","rook.x","queen.x","king.x"};//store the paths to the pieces meshes 

// struct DEVICETYPEINFO
// {
// 	enum MODE{WINDOWED, FULLSCREEN};
// 
// 	D3DDEVTYPE deviceType;
// 	std::string deviceDescription;
// 	D3DFORMAT fomrat;
// 
// 	bool bHardwareAcceleration[2];
// 	BOOL bDepthEnable[2];
// 	std::vector<D3DFORMAT> validDepths[2];
// 	std::vector<D3DMULTISAMPLE_TYPE> validMultiSampleTypes[2];
// 	std::vector<DWORD> vpTypes;
// };
// 
// struct DISPLAYMODE
// {
// 	UINT Width;
// 	UINT Height;
// 	std::vector<UINT> RefreshRates;
// 	D3DFORMAT Format;
// 
// 	DISPLAYMODE::DISPLAYMODE(D3DDISPLAYMODE& displayMode)
// 	{
// 		this->Format = displayMode.Format;
// 		this->Height = displayMode.Height;
// 		this->RefreshRates.push_back(displayMode.RefreshRate);
// 		this->Width = displayMode.Width;
// 	}
// };
// 
// struct ADAPTERINFO
// {
// 	UINT adapterNum;
// 	std::string adapterDescription;
// 
// 	std::vector<DEVICETYPEINFO> deviceTypes;
// 	std::vector<DISPLAYMODE> displayModes;
// 
// 	void addDisplayMode(D3DDISPLAYMODE& displayMode)
// 	{
// 		for (UINT i = 0; i < displayModes.size(); i++)
// 		{
// 			if (displayMode.Width == displayModes[i].Width && displayMode.Height == displayModes[i].Height)
// 			{
// 				displayModes[i].RefreshRates.push_back(displayMode.RefreshRate);
// 				return;
// 			}
// 		}
// 
// 		displayModes.push_back(DISPLAYMODE(displayMode));
// 	}
// };
// 
// struct MODEINFO
// {
// 	MODEINFO::MODEINFO(D3DDISPLAYMODE newMode, D3DDEVTYPE newDeviceType)
// 	{
// 		mode = newMode;
// 		deviceType = newDeviceType;
// 	}
// 
// 	D3DDISPLAYMODE mode;
// 	D3DDEVTYPE deviceType;
// };



class CGameWin
{
public:

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//------------------------------------------------------------------------
				 CGameWin();
	virtual		~CGameWin();

	//-------------------------------------------------------------------------
	// Public Functions for This Class
	//-------------------------------------------------------------------------
	bool         InitInstance				(HINSTANCE hInstance, LPCTSTR lpCmdLine, int iCmdShow );
	LRESULT		 WinProc					(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//void         OnGUIEvent					(HWND hWnd, UINT nEvent, int nControlID, void* pUserContext );

	void		 OptionsControlClicked		(CButtonUI* pButton);
	void		 OptionDialogOKClicked		(CButtonUI* pButton);

	int          BeginGame					( );
	bool         ShutDown					( );

private:

	//-------------------------------------------------------------------------
	// Private Static Functions For This Class
	//-------------------------------------------------------------------------
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	//static void    CALLBACK StaticOnGUIEvent(HWND hWnd, UINT nEvent, int nControlID, void* pUserContext );

	//-------------------------------------------------------------------------
	// Private  Functions that handle display window and the directx device
	//-------------------------------------------------------------------------
	bool        CreateDisplay		(HINSTANCE hInstance,bool windowed);
	bool		CreateDisplayWindow (HINSTANCE hInstance);
	HRESULT		CreateDevice		(bool windowed);

	static std::unordered_map<UINT,char*> InitDepthFormatMap();
	static std::unordered_map<UINT,char*> InitMultiSampleMap();
	static std::unordered_map<ULONG,char*> InitVertexProcMap();

	BOOL		EnumDepthStencil	(D3DFORMAT depthFormats[], UINT formatsCount, UINT adapter, D3DDEVTYPE deviceType, D3DFORMAT backBufferFromat, std::vector<D3DFORMAT>& validDepths);
	void		EnumMultiSample		(UINT adapter, D3DDEVTYPE deviceType, D3DFORMAT backBufferFormat, bool windowed, std::vector<D3DMULTISAMPLE_TYPE>& validMultiSampleTypes);
	//bool		CreateGUIObjects	();

	void		resetDevice			(D3DPRESENT_PARAMETERS& d3dpp);

	//-------------------------------------------------------------------------
	// Functions that handles rendering logic
	//-------------------------------------------------------------------------
	void        FrameAdvance		(float timeDelta);
	void	    DrawDebugText		();
	HRESULT		setAttribute		(UINT attribNum);
	void		setLight			(LIGHT_PREFS& light, ID3DXEffect * effect, UINT index);
	void        setRenderStates		( );

	//-------------------------------------------------------------------------
	// Functions that handles the addion of text to debug buffer
	//-------------------------------------------------------------------------
	template <class ValueType> void        addDebugText  (char* Text,ValueType value );

	//-------------------------------------------------------------------------
	// Functions that control creation and release of game objects
	//-------------------------------------------------------------------------
	bool        BuildObjects    ( );
	void		createObject	(CMyMesh* objMesh, OBJECT_PREFS* objectPref, ULONG* newAttribMap, ULONG atrributeCount );
	//void        ReleaseObjects  ( );

	//-------------------------------------------------------------------------
	// Functions that process user input
	//-------------------------------------------------------------------------
	void		ProcessInput    (float timeDelta,float &angle,float &height);
	HRESULT		pick		    (POINT& cursor,DWORD& faceCount);

	//-------------------------------------------------------------------------
	// Functions that handle lights creation and shader handles
	//-------------------------------------------------------------------------
	bool		initHandlesToShader();
	bool		loadEffectFile(LPCSTR fileNmae, LPD3DXEFFECT* effectHandle);

	bool		addLight(LIGHT_PREFS& light, ID3DXEffect * effect);
	void		CreateLights(ID3DXEffect * effect);

	void		Release			( );


	//-------------------------------------------------------------------------
	// Private Variables For This Class
	//-------------------------------------------------------------------------
	HWND                     m_hWnd;             // Main window HWND
	HICON                    m_hIcon;            // Window Icon
	HMENU                    m_hMenu;            // Window Menu

	//-------------------------------------------------------------------------
	// Game and rendering flags
	//-------------------------------------------------------------------------
	bool                     m_bLostDevice;      // Is the 3d device currently lost ?
	bool                     m_bActive;          // Is the application active ?

	bool					 m_gameRunning;

	bool					 m_bPicking;
	bool					 m_bCamRotate;
	bool					 m_deviceReset;

	DrawingMethod			 m_DrawingMethod;

	//-------------------------------------------------------------------------
	// Directx device and object pointers
	//-------------------------------------------------------------------------
	IDirect3D9             * m_pD3D;             // Direct3D Object
	IDirect3DDevice9	   * m_pD3DDevice;       // Direct3D Device Object
	D3DPRESENT_PARAMETERS    m_curD3dpp;         // active D3DPRESENT_PARAMETERS
	bool					 m_windowed;

	//bool m_windowed;
	//std::vector<MODEINFO> m_displayModes;
	std::vector<D3DFORMAT> m_adapterFormats;
	std::vector<ADAPTERINFO> m_adpatersInfo;

	static const std::unordered_map<UINT,char*> s_depthFormatsString;
	static const std::unordered_map<UINT,char*> s_mutliSampleString;
	static const std::unordered_map<ULONG,char*> s_vertexProcString;

	//-------------------------------------------------------------------------
	// the scene objects data
	//-------------------------------------------------------------------------
	std::vector<CMyObject*>	 m_objects;
	CTerrain			   * m_pTerrain;
	int						 activeMeshIndex;

	//	board                  * gameBoard;

	// attributes data pools
	CAssetManager			 m_assetManger;

	CEditDialogUI			 m_EditDialog;
	COptionDialogUI			 m_OptionsDialog;
	ULONG					 m_GenControlNum;
	ULONG					 m_curControlID;
	bool					 m_controlInCreation;
	bool					 m_controlRelocate;

	CControlUI			   * m_pCurSelectedControl;



	CTimer*					 m_timer;

	//-------------------------------------------------------------------------
	// cameras pointers and settings
	//-------------------------------------------------------------------------
	CCamera                * m_pCameras[2];          // A cached copy of the camera attached to the player
	UINT					 m_cameraIndex;
	D3DVIEWPORT9			 m_viewPort;
	CCamera				   * m_pProject;


	RotaionLimits			 m_rotLimits;

	POINT                    m_OldCursorPos;     // Old cursor position for tracking

	//-------------------------------------------------------------------------
	// window settings info 
	//-------------------------------------------------------------------------
	const int				 Width;
	const int				 Height;

	ULONG                    m_nViewX;
	ULONG                    m_nViewY;
	ULONG                    m_nViewWidth;
	ULONG                    m_nViewHeight;
	
	LPD3DXFONT	 		   	 m_fpsFont;         //text that shows current fps
	std::string				 m_debugString;

	//-------------------------------------------------------------------------
	// lights vector and current active light
	//-------------------------------------------------------------------------
	std::vector<LIGHT_PREFS> m_lights;
	UINT					 m_numActiveLights;

	//-------------------------------------------------------------------------
	// Handles to the shader global vars
	//-------------------------------------------------------------------------
	D3DXHANDLE				 m_matWorldH;
	D3DXHANDLE				 m_matWorldViewProjH;
	D3DXHANDLE				 m_matWorldInverseTH;

	D3DXHANDLE				 m_projTextureMatrixH;

	D3DXHANDLE				 m_vecEyeH;

	D3DXHANDLE				 m_numActiveLightsH;

	D3DXHANDLE				 m_lightPosH[MAX_ACTIVE_LIGHTS];
	D3DXHANDLE				 m_vecLightDirH[MAX_ACTIVE_LIGHTS];
	D3DXHANDLE				 m_lightAttenH[MAX_ACTIVE_LIGHTS];

	D3DXHANDLE				 m_vecAmbientLightH[MAX_ACTIVE_LIGHTS];
	D3DXHANDLE				 m_vecDiffuseLightH[MAX_ACTIVE_LIGHTS];
	D3DXHANDLE				 m_vecSpecularLightH[MAX_ACTIVE_LIGHTS];
	D3DXHANDLE				 m_SpotPowerH[MAX_ACTIVE_LIGHTS];


	D3DXHANDLE				 m_vecAmbientMtrlH;
	D3DXHANDLE				 m_vecDiffuseMtrlH;
	D3DXHANDLE				 m_vecSpecularMtrlH;
	D3DXHANDLE				 m_SpecularPowerH;

	D3DXHANDLE				 m_bHighLightH;

	D3DXHANDLE				 m_MeshTextureH;

	D3DXHANDLE				 m_lightTechHnadle;
	D3DXHANDLE				 m_lightTexTechHandle;

	D3DXHANDLE				 m_UITexture;
	D3DXHANDLE				 m_bUIHighLight;
	D3DXHANDLE				 m_bUITexutre;

	// effect file handles
	ID3DXEffect			*	 m_outlineEffect;

	ID3DXEffect			*	 m_highLightEffect;

	boost::signals2::signal<void ()> m_keyboardSig;

	RECT							 m_clientRC;
// 	CDialog3D				 m_GuiDialog;
// 	CDialog3D				 m_GuiSelectPawn;
// 	CDialogResourceManager3D m_GuiDilaogResManger;

};
