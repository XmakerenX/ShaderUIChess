#include "CCamera.h"

//-----------------------------------------------------------------------------
// CCamera Class Constructor
//-----------------------------------------------------------------------------
CCamera::CCamera(void)
{
	// Reset / Clear all required values
	//m_pPlayer         = NULL;
	m_vecRight        = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	m_vecUp           = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	m_vecLook         = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	m_vecPos          = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_fFOV            = 60.0f;
	m_fNearClip       = 1.0f;
	m_fFarClip        = 100.0f;
	m_Viewport.X      = 0;
	m_Viewport.Y      = 0;
	m_Viewport.Width  = 640;
	m_Viewport.Height = 480;
	m_Viewport.MinZ   = 0.0f;
	m_Viewport.MaxZ   = 1.0f;

	// Internal features are dirty by default
	m_bViewDirty      = true;
	m_bProjDirty      = true;
	m_bFrustumDirty   = true;

	m_fPitch         = 0.0f;   
	m_fRoll          = 0.0f;     
	m_fYaw           = 0.0f;   

//	m_pPlayer		  = NULL;

	// Set matrices to identity
	D3DXMatrixIdentity( &m_mtxView );
	D3DXMatrixIdentity( &m_mtxProj );
}

//-----------------------------------------------------------------------------
// CCamera  Alternate Class  Constructor
// extracts values from the passed camera
//-----------------------------------------------------------------------------
CCamera::CCamera( const CCamera * pCamera )
{

	// Reset / Clear all required values
//	m_pPlayer        = NULL;
	m_vecRight       = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	m_vecUp          = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	m_vecLook        = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	m_vecPos         = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_fFOV            = 60.0f;
	m_fNearClip       = 1.0f;
	m_fFarClip        = 100.0f;
	m_Viewport.X      = 0;
	m_Viewport.Y      = 0;
	m_Viewport.Width  = 640;
	m_Viewport.Height = 480;
	m_Viewport.MinZ   = 0.0f;
	m_Viewport.MaxZ   = 1.0f;

	// Internal features are dirty by default
	m_bViewDirty      = true;
	m_bProjDirty      = true;
	m_bFrustumDirty   = true;

	// Set matrices to identity
	D3DXMatrixIdentity( &m_mtxView );
	D3DXMatrixIdentity( &m_mtxProj );

}

//-----------------------------------------------------------------------------
// CCamera Class Destructor
//-----------------------------------------------------------------------------
CCamera::~CCamera(void)
{
}

//-----------------------------------------------------------------------------
//AttachToPlayer(for now not in use since there is no player object)
//Attach the camera to the specified player object
//-----------------------------------------------------------------------------
// void CCamera::AttachToPlayer( CPlayer * pPlayer )
// {
// 	// Attach ourslves to the player
// 	m_pPlayer = pPlayer;
// 
// }

//-----------------------------------------------------------------------------
//DetachFromPlayer(for now not in use since there is no player object)
//Detach the camera from the current player object
//-----------------------------------------------------------------------------
// void CCamera::DetachFromPlayer()
// {
// 	m_pPlayer = NULL;
// }

//-----------------------------------------------------------------------------
// SetViewport
// Sets the camera viewport values.
//-----------------------------------------------------------------------------
void CCamera::SetViewport( long Left, long Top, long Width, long Height, float NearClip, float FarClip, LPDIRECT3DDEVICE9 pDevice )
{
	HRESULT hr;
	// Set viewport sizes
	m_Viewport.X      = Left;
	m_Viewport.Y      = Top;
	m_Viewport.Width  = Width;
	m_Viewport.Height = Height;
	m_Viewport.MinZ   = 0.0f;
	m_Viewport.MaxZ   = 1.0f;
	m_fNearClip       = NearClip;
	m_fFarClip        = FarClip;

	m_bProjDirty      = true;
	m_bFrustumDirty   = true;

	// Update device if requested
	if ( pDevice ) 
		 hr = pDevice->SetViewport( &m_Viewport );

	hr = S_FALSE;
}

//-----------------------------------------------------------------------------
// GetProjMatrix 
// Return the current projection matrix.
//-----------------------------------------------------------------------------
const D3DXMATRIX& CCamera::GetProjMatrix()
{
	// Only update matrix if something has changed
	if ( m_bProjDirty ) 
	{     
		float fAspect = (float)m_Viewport.Width / (float)m_Viewport.Height;

		// Set the perspective projection matrix
		D3DXMatrixPerspectiveFovLH( &m_mtxProj, D3DXToRadian( m_fFOV ), fAspect, m_fNearClip, m_fFarClip );

		// Proj Matrix has been updated
		m_bProjDirty = false; 

	} // End If Projection matrix needs updating

	// Return the projection matrix.
	return m_mtxProj;
}

//-----------------------------------------------------------------------------
// GetViewMatrix 
// Return the current view matrix.
//-----------------------------------------------------------------------------
const D3DXMATRIX& CCamera::GetViewMatrix()
{
	// Only update matrix if something has changed
	if ( m_bViewDirty ) 
	{
		// Because many rotations will cause floating point errors, the axis will eventually become
		// non-perpendicular to one other causing all hell to break loose. Therefore, we must
		// perform base vector regeneration to ensure that all vectors remain unit length and
		// perpendicular to one another. This need not be done on EVERY call to rotate (i.e. you
		// could do this once every 50 calls for instance).
		D3DXVec3Normalize( &m_vecLook, &m_vecLook );
		D3DXVec3Cross( &m_vecRight, &m_vecUp, &m_vecLook );
		D3DXVec3Normalize( &m_vecRight, &m_vecRight );
		D3DXVec3Cross( &m_vecUp, &m_vecLook, &m_vecRight );
		D3DXVec3Normalize( &m_vecUp, &m_vecUp );

		// Set view matrix values
		m_mtxView._11 = m_vecRight.x; m_mtxView._12 = m_vecUp.x; m_mtxView._13 = m_vecLook.x;
		m_mtxView._21 = m_vecRight.y; m_mtxView._22 = m_vecUp.y; m_mtxView._23 = m_vecLook.y;
		m_mtxView._31 = m_vecRight.z; m_mtxView._32 = m_vecUp.z; m_mtxView._33 = m_vecLook.z;
		m_mtxView._41 =- D3DXVec3Dot( &m_vecPos, &m_vecRight );
		m_mtxView._42 =- D3DXVec3Dot( &m_vecPos, &m_vecUp    );
		m_mtxView._43 =- D3DXVec3Dot( &m_vecPos, &m_vecLook  );

		// View Matrix has been updated
		m_bViewDirty = false;

	} // End If View Dirty

	// Return the view matrix.
	return m_mtxView;
}

//-----------------------------------------------------------------------------
// UpdateRenderView 
// Update the render device with the updated view matrix etc.
//-----------------------------------------------------------------------------
void CCamera::UpdateRenderView( LPDIRECT3DDEVICE9 pD3DDevice )
{  
	HRESULT hr;
	// Validate Parameters
	if (!pD3DDevice) return;

	// Update the device with this matrix.
	hr = pD3DDevice->SetTransform( D3DTS_VIEW, &GetViewMatrix() );
}

//-----------------------------------------------------------------------------
// UpdateRenderProj 
// Update the render device with the updated projection matrix etc.
//-----------------------------------------------------------------------------
void CCamera::UpdateRenderProj( LPDIRECT3DDEVICE9 pD3DDevice )
{  
	HRESULT hr;
	// Validate Parameters
	if (!pD3DDevice) return;

	// Update the device with this matrix.
	hr = pD3DDevice->SetTransform( D3DTS_PROJECTION, &GetProjMatrix() );
}

//-----------------------------------------------------------------------------
// CalcFrustumPlanes (Private)
// Calculate the 6 frustum planes based on the current values.
//-----------------------------------------------------------------------------
void CCamera::CalcFrustumPlanes()
{
	// Only update planes if something has changed
	if ( !m_bFrustumDirty ) return;

	// Build a combined view & projection matrix
	D3DXMATRIX m = GetViewMatrix() * GetProjMatrix();

	// Left clipping plane
	m_Frustum[0].a = -(m._14 + m._11);
	m_Frustum[0].b = -(m._24 + m._21);
	m_Frustum[0].c = -(m._34 + m._31);
	m_Frustum[0].d = -(m._44 + m._41);

	// Right clipping plane
	m_Frustum[1].a = -(m._14 - m._11);
	m_Frustum[1].b = -(m._24 - m._21);
	m_Frustum[1].c = -(m._34 - m._31);
	m_Frustum[1].d = -(m._44 - m._41);

	// Top clipping plane
	m_Frustum[2].a = -(m._14 - m._12);
	m_Frustum[2].b = -(m._24 - m._22);
	m_Frustum[2].c = -(m._34 - m._32);
	m_Frustum[2].d = -(m._44 - m._42);

	// Bottom clipping plane
	m_Frustum[3].a = -(m._14 + m._12);
	m_Frustum[3].b = -(m._24 + m._22);
	m_Frustum[3].c = -(m._34 + m._32);
	m_Frustum[3].d = -(m._44 + m._42);

	// Near clipping plane
	m_Frustum[4].a = -(m._13);
	m_Frustum[4].b = -(m._23);
	m_Frustum[4].c = -(m._33);
	m_Frustum[4].d = -(m._43);

	// Far clipping plane
	m_Frustum[5].a = -(m._14 - m._13);
	m_Frustum[5].b = -(m._24 - m._23);
	m_Frustum[5].c = -(m._34 - m._33);
	m_Frustum[5].d = -(m._44 - m._43);

	// Normalize the m_Frustum
	for ( ULONG i = 0; i < 6; i++ ) D3DXPlaneNormalize( &m_Frustum[i], &m_Frustum[i] );

	// Frustum is no longer dirty
	m_bFrustumDirty = false;
}

//-----------------------------------------------------------------------------
// BoundsInFrustum ()
// Determine whether or not the box passed is within the frustum.
//-----------------------------------------------------------------------------
bool CCamera::BoundsInFrustum( const D3DXVECTOR3 & Min, const D3DXVECTOR3 & Max )
{
	// First calculate the frustum planes
	CalcFrustumPlanes();

	ULONG i;
	D3DXVECTOR3 NearPoint, FarPoint, Normal;

	// Loop through all the planes
	for ( i = 0; i < 6; i++ )
	{
		// Store the plane normal
		Normal = D3DXVECTOR3( m_Frustum[i].a, m_Frustum[i].b, m_Frustum[i].c );

		if ( Normal.x > 0.0f )
		{
			if ( Normal.y > 0.0f )
			{
				if ( Normal.z > 0.0f ) 
				{
					NearPoint.x = Min.x; NearPoint.y = Min.y; NearPoint.z = Min.z;

				} // End if Normal.z > 0
				else 
				{
					NearPoint.x = Min.x; NearPoint.y = Min.y; NearPoint.z = Max.z; 

				} // End if Normal.z <= 0

			} // End if Normal.y > 0
			else
			{
				if ( Normal.z > 0.0f ) 
				{
					NearPoint.x = Min.x; NearPoint.y = Max.y; NearPoint.z = Min.z;

				} // End if Normal.z > 0
				else 
				{
					NearPoint.x = Min.x; NearPoint.y = Max.y; NearPoint.z = Max.z; 

				} // End if Normal.z <= 0

			} // End if Normal.y <= 0

		} // End if Normal.x > 0
		else
		{
			if ( Normal.y > 0.0f )
			{
				if ( Normal.z > 0.0f ) 
				{
					NearPoint.x = Max.x; NearPoint.y = Min.y; NearPoint.z = Min.z;

				} // End if Normal.z > 0
				else 
				{
					NearPoint.x = Max.x; NearPoint.y = Min.y; NearPoint.z = Max.z; 

				} // End if Normal.z <= 0

			} // End if Normal.y > 0
			else
			{
				if ( Normal.z > 0.0f ) 
				{
					NearPoint.x = Max.x; NearPoint.y = Max.y; NearPoint.z = Min.z;

				} // End if Normal.z > 0
				else 
				{
					NearPoint.x = Max.x; NearPoint.y = Max.y; NearPoint.z = Max.z; 

				} // End if Normal.z <= 0

			} // End if Normal.y <= 0

		} // End if Normal.x <= 0

		// Near extreme point is outside, and thus the
		// AABB is totally outside the frustum ?
		if ( D3DXVec3Dot( &Normal, &NearPoint ) + m_Frustum[i].d > 0.0f ) return false;

	} // Next Plane

	// Is within the frustum
	return true;
}

//-----------------------------------------------------------------------------
// SetVolumeInfo ()
// Set the players collision volume information
//-----------------------------------------------------------------------------
// void CCamera::SetVolumeInfo( const VOLUME_INFO& Volume )
// {
// 	m_Volume = Volume;
// }

//-----------------------------------------------------------------------------
// GetVolumeInfo ()
// Retrieve the players collision volume information
//-----------------------------------------------------------------------------
// const VOLUME_INFO& CCamera::GetVolumeInfo( ) const
// {
// 	return m_Volume;
//}

void CCamera::SetLookAt( const D3DXVECTOR3& vecLookAt )
{
	D3DXMATRIX Matrix;
	D3DXVECTOR3 up,up2;

	D3DXVec3Cross(&up,&vecLookAt,&m_vecRight);
	up2 = up;
// 	if (vecLookAt == D3DXVECTOR3(0.0f, -1.0f, 0.0f))
// 		up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	// Generate a look at matrix
	D3DXMatrixLookAtLH( &Matrix, &m_vecPos, &vecLookAt, &up );

	// Extract the vectors
	m_vecRight = D3DXVECTOR3( Matrix._11, Matrix._21, Matrix._31 );

	//m_vecUp    = D3DXVECTOR3( Matrix._12, Matrix._22, Matrix._32 );
	m_vecUp = up2;
	m_vecLook  = D3DXVECTOR3( Matrix._13, Matrix._23, Matrix._33 );

	// Set view matrix as dirty
	m_bViewDirty = true;

	// Frustum is now dirty by definition
	m_bFrustumDirty = true;
}