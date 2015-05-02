#pragma once
#include "CCamera.h"

typedef struct _RotaionLimits
{
	int maxZ;
	int minZ;
	int maxX;
	int minX;
} RotaionLimits;


class CCamSpaceCraft : public CCamera
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CCamSpaceCraft( const CCamera * pCamera );
	CCamSpaceCraft();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	CAMERA_MODE         GetCameraMode    ( ) const { return MODE_SPACECRAFT; }
	void                Rotate           ( float x, float y, float z );
	void				RotateAroundPoint(D3DXVECTOR3& point,float XDistance,float YDistance, RotaionLimits rotLimits);
	void                SetCameraDetails ( const CCamera * pCamera );
	void		        Move             ( ULONG direction, float distance);
	void				Yaw				 ( float angle);
	void				Pitch			 ( float angle);
	D3DXMATRIX			getRotateMatrix  ();

	D3DXVECTOR3		m_rightAxis;
	D3DXMATRIX		m_rotateMatrix;

};