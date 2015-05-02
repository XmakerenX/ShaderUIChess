#pragma once
#include "CCamera.h"

class CCam3rdPerson : public CCamera
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CCam3rdPerson( const CCamera * pCamera );
	CCam3rdPerson();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	CAMERA_MODE         GetCameraMode    ( ) const { return MODE_THIRDPERSON; }
	void                Move             ( const D3DXVECTOR3& vecShift ) {};
	void                Rotate           ( float x, float y, float z )   {};
	void                Update           ( float TimeScale, float Lag );
	void                SetCameraDetails ( const CCamera * pCamera );
//	void                SetLookAt        ( const D3DXVECTOR3& vecLookAt );

};