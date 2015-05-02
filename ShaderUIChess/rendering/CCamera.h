#pragma once
//-----------------------------------------------------------------------------
// CCamera Specific Includes
//-----------------------------------------------------------------------------
#include "../main.h"

class CCamera
{
public:

	//-------------------------------------------------------------------------
	// Enumerators and Structures
	//-------------------------------------------------------------------------
	enum CAMERA_MODE {
		MODE_FPS         = 1,
		MODE_THIRDPERSON = 2,
		MODE_SPACECRAFT  = 3,

		MODE_FORCE_32BIT = 0x7FFFFFFF
	};

	enum DIRECTION { 
		DIR_FORWARD     = 1, 
		DIR_BACKWARD    = 2, 
		DIR_LEFT        = 4, 
		DIR_RIGHT       = 8, 
		DIR_UP          = 16, 
		DIR_DOWN        = 32,

		DIR_FORCE_32BIT = 0x7FFFFFFF
	};

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CCamera( const CCamera * pCamera );
	CCamera(void);
	virtual ~CCamera(void);


	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void                SetFOV           ( float FOV ) { m_fFOV = FOV; m_bProjDirty = true; }
	void                SetViewport      ( long Left, long Top, long Width, long Height, float NearClip, float FarClip, LPDIRECT3DDEVICE9 pDevice = NULL );
	void				SetLookAt		 ( const D3DXVECTOR3& vecLookAt );

	void                UpdateRenderView ( LPDIRECT3DDEVICE9 pD3DDevice );
	void                UpdateRenderProj ( LPDIRECT3DDEVICE9 pD3DDevice );
	const D3DXMATRIX&   GetProjMatrix    ( );    
	float               GetFOV           ( ) const { return m_fFOV;  }
	float               GetNearClip      ( ) const { return m_fNearClip; }
	float               GetFarClip       ( ) const { return m_fFarClip; }
	const D3DVIEWPORT9& GetViewport      ( ) const { return m_Viewport; }
//	CPlayer *           GetPlayer        ( ) const { return m_pPlayer;  }

	const D3DXVECTOR3&  GetPosition      ( ) const { return m_vecPos;   }
	const D3DXVECTOR3&  GetLook          ( ) const { return m_vecLook;  }
	const D3DXVECTOR3&  GetUp            ( ) const { return m_vecUp;    }
	const D3DXVECTOR3&  GetRight         ( ) const { return m_vecRight; }
	const D3DXMATRIX&   GetViewMatrix    ( );

// 	void                SetVolumeInfo    ( const VOLUME_INFO& Volume );
// 	const VOLUME_INFO&  GetVolumeInfo    ( ) const;


	//-------------------------------------------------------------------------
	// Public Virtual Functions for This Class.
	//-------------------------------------------------------------------------
	//virtual void        AttachToPlayer   ( CPlayer * pPlayer );
	//virtual void        DetachFromPlayer ( );
	virtual void        SetPosition      ( const D3DXVECTOR3& Position ) { m_vecPos = Position; m_bViewDirty = true; m_bFrustumDirty = true; }
	virtual void        Move             ( ULONG direction, float distance) = 0; // { m_vecPos += vecShift; m_bViewDirty = true; m_bFrustumDirty = true; }
	virtual void        Rotate           ( float x, float y, float z )   {}
	virtual void        Update           ( float TimeScale, float Lag )  {}
	virtual void        SetCameraDetails ( const CCamera * pCamera )     {}

	virtual CAMERA_MODE GetCameraMode    ( ) const = 0;

	bool                BoundsInFrustum  ( const D3DXVECTOR3 & Min, const D3DXVECTOR3 & Max );

protected:
	//-------------------------------------------------------------------------
	// Protected Functions for This Class.
	//-------------------------------------------------------------------------
	void                CalcFrustumPlanes( );

	//-------------------------------------------------------------------------
	// Protected Variables for This Class.
	//-------------------------------------------------------------------------
//	CPlayer       * m_pPlayer;              // The player object we are attached to
//	VOLUME_INFO     m_Volume;               // Stores information about cameras collision volume
	D3DXMATRIX      m_mtxView;              // Cached view matrix
	D3DXMATRIX      m_mtxProj;              // Cached projection matrix
	D3DXPLANE       m_Frustum[6];           // The 6 planes of our frustum.

	bool            m_bViewDirty;           // View matrix dirty ?
	bool            m_bProjDirty;           // Proj matrix dirty ?
	bool            m_bFrustumDirty;        // Are the frustum planes dirty ?

	// Perspective Projection parameters
	float           m_fFOV;                 // FOV Angle.
	float           m_fNearClip;            // Near Clip Plane Distance
	float           m_fFarClip;             // Far Clip Plane Distance
	D3DVIEWPORT9    m_Viewport;             // The viewport details into which we are rendering.

	// Cameras current position & orientation
	D3DXVECTOR3     m_vecPos;               // Camera Position
	D3DXVECTOR3     m_vecUp;                // Camera Up Vector
	D3DXVECTOR3     m_vecLook;              // Camera Look Vector
	D3DXVECTOR3     m_vecRight;             // Camera Right Vector

	float           m_fPitch;               // pitch 
	float           m_fRoll;                // roll
	float           m_fYaw;                 // yaw
};
