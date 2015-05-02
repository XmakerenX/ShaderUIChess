#include "CCam3rdPerson.h"

//-----------------------------------------------------------------------------
// CCam3rdPerson () (Alternate Constructor)
// CCam3rdPerson Class Constructor, extracts values from the passed camera.
//-----------------------------------------------------------------------------
CCam3rdPerson::CCam3rdPerson( const CCamera * pCamera )
{
	// Update the camera from the camera passed
	SetCameraDetails( pCamera );
}

//-----------------------------------------------------------------------------
// SetCameraDetails ()
// Sets this camera up based upon the camera passed in.
//-----------------------------------------------------------------------------
void CCam3rdPerson::SetCameraDetails( const CCamera * pCamera )
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
	m_bViewDirty = true;
	m_bProjDirty = true;
	m_bFrustumDirty = true;

}

//-----------------------------------------------------------------------------
// Update ()
// Allow our camera to update
//-----------------------------------------------------------------------------
void CCam3rdPerson::Update( float TimeScale, float Lag )
{
// 	D3DXMATRIX  mtxRotate;
// 	D3DXVECTOR3 vecOffset, vecPosition, vecDir;
// 
// 	// Validate requirements
// //	if (!m_pPlayer) return;
// 
// 	float fTimeScale = 1.0f, Length = 0.0f;
// 	if ( Lag != 0.0f ) fTimeScale = TimeScale * (1.0f / Lag);
// 
// 	// Build a rotation matrix so that we can rotate our offset vector to it's position behind the player
// 	D3DXMatrixIdentity( &mtxRotate );
// //	D3DXVECTOR3 vecRight = m_pPlayer->GetRight(), vecUp = m_pPlayer->GetUp(), vecLook = m_pPlayer->GetLook();
// //	mtxRotate._11 = vecRight.x; mtxRotate._21 = vecUp.x; mtxRotate._31 = vecLook.x;
// //	mtxRotate._12 = vecRight.y; mtxRotate._22 = vecUp.y; mtxRotate._32 = vecLook.y;
// //	mtxRotate._13 = vecRight.z; mtxRotate._23 = vecUp.z; mtxRotate._33 = vecLook.z;
// 
// 	// Calculate our rotated offset vector
// 	D3DXVec3TransformCoord( &vecOffset, &m_pPlayer->GetCamOffset(), &mtxRotate );
// 
// 	// vecOffset now contains information to calculate where our camera position SHOULD be.
// 	vecPosition = m_pPlayer->GetPosition() + vecOffset;
// 	vecDir = vecPosition - m_vecPos;
// 	Length = D3DXVec3Length( &vecDir );
// 	D3DXVec3Normalize( &vecDir, &vecDir );
// 
// 	// Move based on camera lag
// 	float Distance = Length * fTimeScale;
// 	if ( Distance > Length ) Distance = Length;
// 
// 	// If we only have a short way to travel, move all the way
// 	if ( Length < 0.01f ) Distance = Length;
// 
// 	// Update our camera
// 	if ( Distance > 0 )
// 	{
// 		m_vecPos += vecDir * Distance;
// 
// 		// Ensure our camera is looking at the axis origin
// 		SetLookAt( m_pPlayer->GetPosition() );
// 
// 		// Our view matrix parameters have been update
// 		m_bViewDirty = true;
// 		m_bFrustumDirty = true;
// 
// 	} // End if

}

//-----------------------------------------------------------------------------
// SetLookAt () (Private)
// Set the cameras look at vector, this function will also update the
// various direction vectors so that they are correct.
//-----------------------------------------------------------------------------
//void CCam3rdPerson::SetLookAt( const D3DXVECTOR3& vecLookAt )
//{
// 	D3DXMATRIX Matrix;
// 
// 	// Generate a look at matrix
// 	D3DXMatrixLookAtLH( &Matrix, &m_vecPos, &vecLookAt, &m_pPlayer->GetUp() );
// 
// 	// Extract the vectors
// 	m_vecRight = D3DXVECTOR3( Matrix._11, Matrix._21, Matrix._31 );
// 	m_vecUp    = D3DXVECTOR3( Matrix._12, Matrix._22, Matrix._32 );
// 	m_vecLook  = D3DXVECTOR3( Matrix._13, Matrix._23, Matrix._33 );
// 
// 	// Set view matrix as dirty
// 	m_bViewDirty = true;
// 
// 	// Frustum is now dirty by definition
// 	m_bFrustumDirty = true;
//}