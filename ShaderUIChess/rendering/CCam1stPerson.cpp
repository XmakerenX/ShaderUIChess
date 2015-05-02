#include "CCam1stPerson.h"

CCam1stPerson::CCam1stPerson()
{
	m_fPitch = 0.0f;
	m_fYaw   = 0.0f;
	m_fRoll  = 0.0f;
}
//-----------------------------------------------------------------------------
// CCam1stPerson (Alternate Constructor)
// CCam1stPerson Class Constructor, extracts values from the passed camera.
//-----------------------------------------------------------------------------
CCam1stPerson::CCam1stPerson( const CCamera * pCamera )
{
	// Update the camera from the camera passed
	SetCameraDetails( pCamera );
	m_fPitch = 0.0f;
	m_fYaw   = 0.0f;
	m_fRoll  = 0.0f;
}

// CCam1stPerson::~CCam1stPerson(void)
// {
// }

//-----------------------------------------------------------------------------
// SetCameraDetails 
// Sets this camera up based upon the camera passed in.
//-----------------------------------------------------------------------------
void CCam1stPerson::SetCameraDetails( const CCamera * pCamera )
{
	// Validate Parameters
	if (!pCamera) return;

	// Reset / Clear all required values
	m_vecPos    = pCamera->GetPosition();
	m_vecRight  = pCamera->GetRight();
	m_vecLook   = pCamera->GetLook();
	m_vecUp     = pCamera->GetUp();
	m_fFOV      = pCamera->GetFOV();
	m_fNearClip = pCamera->GetNearClip();
	m_fFarClip  = pCamera->GetFarClip();
	m_Viewport  = pCamera->GetViewport();
//	m_Volume    = pCamera->GetVolumeInfo();

	// If we are switching building from a spacecraft style cam
	if ( pCamera->GetCameraMode() == MODE_SPACECRAFT )
	{
		// Flatten out the vectors
		m_vecUp      = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		m_vecRight.y = 0.0f;
		m_vecLook.y  = 0.0f;

		// Finally, normalize them
		D3DXVec3Normalize( &m_vecRight, &m_vecRight );
		D3DXVec3Normalize( &m_vecLook, &m_vecLook );

	} // End if MODE_SPACECRAFT

	// Rebuild both matrices and frustum
	m_bViewDirty    = true;
	m_bProjDirty    = true;
	m_bFrustumDirty = true;

}

//-----------------------------------------------------------------------------
// Rotate 
// Rotate the camera about the players local axis.
//-----------------------------------------------------------------------------
void CCam1stPerson::Rotate( float x, float y, float z )
{
	D3DXMATRIX mtxRotate;

	// Validate requirements
	//if (!m_pPlayer) return;


	// Update & Clamp pitch / roll / Yaw values
	if ( x )
	{
		// Make sure we don't overstep our pitch boundaries
		m_fPitch += x;
		if ( m_fPitch >  89.0f ) { x -= (m_fPitch - 89.0f); m_fPitch = 89.0f; }
		if ( m_fPitch < -89.0f ) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }

	} // End if any Pitch

	if ( y )
	{
		// Ensure yaw (in degrees) wraps around between 0 and 360
		m_fYaw += y;
		if ( m_fYaw >  360.0f ) m_fYaw -= 360.0f;
		if ( m_fYaw <  0.0f   ) m_fYaw += 360.0f;

	} // End if any yaw

	// Roll is purely a statistical value, no player rotation actually occurs
	if ( z ) 
	{
		// Make sure we don't overstep our roll boundaries
		m_fRoll += z;
		if ( m_fRoll >  20.0f ) { z -= (m_fRoll - 20.0f); m_fRoll = 20.0f; }
		if ( m_fRoll < -20.0f ) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }

	} // End if any roll



	if ( x != 0 ) 
	{
		// Build rotation matrix
		D3DXMatrixRotationAxis( &mtxRotate, &m_vecRight, D3DXToRadian( x ) );

		// rotate _up and _look around _right vector
		D3DXVec3TransformCoord(&m_vecUp,&m_vecUp, &mtxRotate);
		D3DXVec3TransformCoord(&m_vecLook,&m_vecLook, &mtxRotate);

// 		// Update our vectors
// 		D3DXVec3TransformNormal( &m_vecLook, &m_vecLook, &mtxRotate );
// 		D3DXVec3TransformNormal( &m_vecUp, &m_vecUp, &mtxRotate );
// 		D3DXVec3TransformNormal( &m_vecRight, &m_vecRight, &mtxRotate );

	} // End if Pitch

	if ( y != 0 ) 
	{
		// Build rotation matrix
		//D3DXMatrixRotationAxis( &mtxRotate, &m_vecUp, D3DXToRadian( y ) );
		D3DXMatrixRotationY(&mtxRotate, D3DXToRadian( y ));

		D3DXVec3TransformCoord(&m_vecRight,&m_vecRight, &mtxRotate);
		D3DXVec3TransformCoord(&m_vecLook,&m_vecLook, &mtxRotate);

		// Update our vectors
// 		D3DXVec3TransformNormal( &m_vecLook, &m_vecLook, &mtxRotate );
// 		D3DXVec3TransformNormal( &m_vecUp, &m_vecUp, &mtxRotate );
// 		D3DXVec3TransformNormal( &m_vecRight, &m_vecRight, &mtxRotate );

	} // End if Yaw

	if ( z != 0 ) 
	{
		// Build rotation matrix
		D3DXMatrixRotationAxis( &mtxRotate, &m_vecLook, D3DXToRadian( z ) );

		// Adjust camera position
/*		m_vecPos -= m_vecPos;*/
		D3DXVec3TransformCoord ( &m_vecPos, &m_vecPos, &mtxRotate );
/*		m_vecPos += m_vecPos;*/

		D3DXVec3TransformCoord(&m_vecRight,&m_vecRight, &mtxRotate);
		D3DXVec3TransformCoord(&m_vecUp,&m_vecUp, &mtxRotate);

		// Update our vectors
// 		D3DXVec3TransformNormal( &m_vecLook, &m_vecLook, &mtxRotate );
// 		D3DXVec3TransformNormal( &m_vecUp, &m_vecUp, &mtxRotate );
// 		D3DXVec3TransformNormal( &m_vecRight, &m_vecRight, &mtxRotate );

	} // End if Roll

// 	// Now rotate our axis
// 	if ( y )
// 	{
// 		// Build rotation matrix
// 		D3DXMatrixRotationAxis( &mtxRotate, &m_vecUp, D3DXToRadian( y ) );
// 
// 		// Update our vectors
// 		D3DXVec3TransformNormal( &m_vecLook, &m_vecLook, &mtxRotate );
// 		D3DXVec3TransformNormal( &m_vecRight, &m_vecRight, &mtxRotate );
// 
// 	} // End if any yaw

	// Set view matrix as dirty
	m_bViewDirty = true;

	// Frustum is now dirty by definition
	m_bFrustumDirty = true;
}

//-----------------------------------------------------------------------------
// walk 
// moves camera in xz plain
//-----------------------------------------------------------------------------
void CCam1stPerson::walk(float units)
{
	m_vecPos+= D3DXVECTOR3(m_vecLook.x, 0.0f, m_vecLook.z) * units;
	m_bViewDirty = true;
	m_bFrustumDirty = true;
}

//-----------------------------------------------------------------------------
// strafe 
// strafe camera in xz plain
//-----------------------------------------------------------------------------
void CCam1stPerson::strafe(float units)
{
	m_vecPos+= D3DXVECTOR3(m_vecRight.x, 0.0f, m_vecRight.z) * units;
	m_bViewDirty = true;
	m_bFrustumDirty = true;
}