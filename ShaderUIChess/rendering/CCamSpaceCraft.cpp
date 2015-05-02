#include "CCamSpaceCraft.h"
#include <string>
#include <sstream>

//-----------------------------------------------------------------------------
// CCamSpaceCraft () (Alternate Constructor)
// CCamSpaceCraft Class Constructor, extracts values from the passed camera.
//-----------------------------------------------------------------------------
CCamSpaceCraft::CCamSpaceCraft( const CCamera * pCamera )
{
	// Update the camera from the camera passed
	SetCameraDetails( pCamera );
}

//-----------------------------------------------------------------------------
// SetCameraDetails ()
// Sets this camera up based upon the camera passed in.
//-----------------------------------------------------------------------------
void CCamSpaceCraft::SetCameraDetails( const CCamera * pCamera )
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

	D3DXMatrixIdentity(&m_rotateMatrix);

	// Rebuild both matrices and frustum
	m_bViewDirty = true;
	m_bProjDirty = true;
	m_bFrustumDirty = true;

}

//-----------------------------------------------------------------------------
// Rotate ()
// Rotate the camera about the cameras local axis.
// Just performs a standard three vector rotation. This is essentially 
//    how we perform 'anti-gimble-lock' space craft rotations.
//-----------------------------------------------------------------------------
void CCamSpaceCraft::Rotate( float x, float y, float z )
{
	D3DXMATRIX mtxRotate;
	D3DXVECTOR3 upTemp;//save the up vector before the x roation in order for the y rotaion to work correctly
	D3DXVECTOR3 lookTemp;

	upTemp = m_vecUp;
	lookTemp = m_vecLook;

	if ( x != 0 ) 
	{
		m_fPitch += x;

		if ( m_fPitch >  89.0f )
		{
			x -= (m_fPitch - 89.0f); 
			m_fPitch = 89.0f; 
		}
		
		if ( m_fPitch < -89.0f )
		{
			x -= (m_fPitch + 89.0f);
			m_fPitch = -89.0f; 
		}

		// Build rotation matrix
		D3DXMatrixRotationAxis( &mtxRotate, &m_vecRight, D3DXToRadian( x ) );

		m_rotateMatrix = mtxRotate;

		D3DXVec3TransformNormal( &m_vecLook, &m_vecLook, &mtxRotate );
		D3DXVec3TransformNormal( &m_vecUp, &m_vecUp, &mtxRotate );
		D3DXVec3TransformNormal( &m_vecRight, &m_vecRight, &mtxRotate );


	} // End if Pitch

	if ( y != 0 ) 
	{
		m_fYaw += y;

		if ( m_fYaw >  360.0f )
		{
			y -= 360.0f;
			m_fYaw = 360.0f;
			//m_fYaw -= 360.0f;
			//y = m_fYaw;
		}
		if ( m_fYaw <  0.0f   )
		{
			y += 360.0f;
			m_fYaw = 0.0f;
			//m_fYaw += 360.0f;
			//y = m_fYaw;
		}

		// Build rotation matrix
		D3DXMatrixRotationAxis( &mtxRotate, &upTemp, D3DXToRadian( y ) );
		m_rotateMatrix *= mtxRotate;

		D3DXVec3TransformNormal( &m_vecLook, &m_vecLook, &mtxRotate );
		D3DXVec3TransformNormal( &m_vecUp, &m_vecUp, &mtxRotate );
		D3DXVec3TransformNormal( &m_vecRight, &m_vecRight, &mtxRotate );


	} // End if Yaw

	if ( z != 0 ) 
	{
		m_fRoll += z;
		if ( m_fRoll >  20.0f ) 
		{
			z -= (m_fRoll - 20.0f); 
			m_fRoll = 20.0f; 
		}
		if ( m_fRoll < -20.0f )
		{
			z -= (m_fRoll + 20.0f);
			m_fRoll = -20.0f; 
		}

		// Build rotation matrix
		D3DXMatrixRotationAxis( &mtxRotate, &m_vecLook, D3DXToRadian( z ) );
		m_rotateMatrix *= mtxRotate;

		D3DXVec3TransformNormal( &m_vecUp, &m_vecUp, &mtxRotate );
		D3DXVec3TransformNormal( &m_vecRight, &m_vecRight, &mtxRotate );


	} // End if Roll

	m_vecUp = D3DXVECTOR3(0,1,0);

	D3DXVec3Normalize( &m_vecLook, &m_vecLook );
	D3DXVec3Cross( &m_vecRight, &m_vecUp, &m_vecLook );
	D3DXVec3Normalize( &m_vecRight, &m_vecRight );
	D3DXVec3Cross( &m_vecUp, &m_vecLook, &m_vecRight );
	D3DXVec3Normalize( &m_vecUp, &m_vecUp );

	// Set view matrix as dirty
	m_bViewDirty = true;

	// Frustum is now dirty by definition
	m_bFrustumDirty = true;
}

void CCamSpaceCraft::RotateAroundPoint(D3DXVECTOR3& point,float XDistance,float YDistance, RotaionLimits rotLimits)
{
	std::string st;
	std::stringstream out;

	D3DXMATRIX Matrix;
	D3DXVECTOR3 vecUp = m_vecUp;
	

	out << XDistance;
	st = out.str() + "\n";
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), st.c_str(), st.size(), NULL, NULL);

	//capping the XDistance Value to prevent going too much to the right
// 	if (XDistance > 2)
// 		XDistance = 2;
// 
// 	if (XDistance < -2)
// 		XDistance = -2;

	m_vecPos += m_vecRight * XDistance;
	m_vecPos += m_vecUp    * YDistance;

	if (m_vecPos.z < rotLimits.minZ)
		m_vecPos.z = static_cast<float>(rotLimits.minZ );

	if (m_vecPos.z > rotLimits.maxZ)
		m_vecPos.z = static_cast<float>(rotLimits.maxZ);

	if (m_vecPos.x < rotLimits.minX)
		m_vecPos.x = static_cast<float>(rotLimits.minX);

	if (m_vecPos.x > rotLimits.maxX)
		m_vecPos.x = static_cast<float>(rotLimits.maxX);

	 D3DXMatrixLookAtLH( &Matrix, &m_vecPos, &point, &vecUp );

	// Extract the vectors
	m_vecRight = D3DXVECTOR3( Matrix._11, Matrix._21, Matrix._31 ); 
	//m_vecUp    = D3DXVECTOR3( Matrix._12, Matrix._22, Matrix._32 );
	m_vecUp = D3DXVECTOR3 (0,1,0);
	m_vecLook  = D3DXVECTOR3( Matrix._13, Matrix._23, Matrix._33 );

	m_bViewDirty = true;
	m_bFrustumDirty = true;
}

void CCamSpaceCraft::Move(ULONG direction, float distance)
{
	//m_rightAxis = D3DXVECTOR3(0.0f ,0.0f ,1.0f);
	m_rightAxis = m_vecRight;
	m_rightAxis.y = 0;

	if (direction & CCamera::DIR_FORWARD)
		m_vecPos += m_vecLook * distance;

	if (direction & CCamera::DIR_BACKWARD)
		m_vecPos += m_vecLook * (-distance);

	if (direction & CCamera::DIR_RIGHT)
		m_vecPos += m_rightAxis * distance;

	if (direction & CCamera::DIR_LEFT)
		m_vecPos += m_rightAxis * (-distance) ;

	if (direction & CCamera::DIR_UP)
		m_vecPos += m_vecUp * distance;
	
	if (direction & CCamera::DIR_DOWN)
		m_vecPos += m_vecUp * (-distance);


	D3DXMATRIX Matrix;
	D3DXVECTOR3 vecUp = m_vecUp;
	 // Generate a look at matrix
// 	 D3DXMatrixLookAtLH( &Matrix, &m_vecPos, &D3DXVECTOR3(9,0,24), &vecUp );
// 	
// 	// Extract the vectors
// 	m_vecRight = D3DXVECTOR3( Matrix._11, Matrix._21, Matrix._31 ); 
// 	//m_vecUp    = D3DXVECTOR3( Matrix._12, Matrix._22, Matrix._32 );
// 	m_vecUp = D3DXVECTOR3 (0,1,0);
// 	m_vecLook  = D3DXVECTOR3( Matrix._13, Matrix._23, Matrix._33 );

	m_bViewDirty = true;
	m_bFrustumDirty = true;

	
}

void CCamSpaceCraft::Yaw( float angle)
{
	D3DXMATRIX yawMatrix;

	m_fYaw+=angle;
	if (m_fYaw > 2 * D3DX_PI)
		m_fYaw -= 2 * D3DX_PI;

	if (m_fYaw < 0.0f)
		m_fYaw += 2 * D3DX_PI;

	D3DXMatrixRotationAxis(&yawMatrix, &m_vecUp, angle);
	// To apply yaw we rotate the m_look & m_right vectors about the m_up vector (using our yaw matrix)
	D3DXVec3TransformCoord(&m_vecLook, &m_vecLook, &yawMatrix); 
	D3DXVec3TransformCoord(&m_vecRight, &m_vecRight, &yawMatrix); 

	m_vecUp = D3DXVECTOR3(0,1,0);
}

void CCamSpaceCraft::Pitch( float angle)
{
	D3DXMATRIX pitchMatrix;

	m_fPitch+=angle;
	if (m_fPitch > 2 * D3DX_PI)
		m_fPitch -= 2 * D3DX_PI;

	if (m_fPitch < 0.0f)
		m_fPitch += 2 * D3DX_PI;

	D3DXMatrixRotationAxis(&pitchMatrix, &m_vecRight, angle);
	// To apply pitch we rotate the m_look and m_up vectors about the m_right vector (using our pitch matrix)
	D3DXVec3TransformCoord(&m_vecLook, &m_vecLook, &pitchMatrix); 
	D3DXVec3TransformCoord(&m_vecUp, &m_vecUp, &pitchMatrix); 

	m_vecUp = D3DXVECTOR3(0,1,0);
}

D3DXMATRIX CCamSpaceCraft::getRotateMatrix()
{
	return m_rotateMatrix;
}