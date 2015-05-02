#pragma once
#include "CCamera.h"

class CCam1stPerson :
	public CCamera
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CCam1stPerson( const CCamera * pCamera );
	CCam1stPerson();
	//~CCam1stPerson();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	CAMERA_MODE         GetCameraMode    ( ) const { return MODE_FPS; }
	void                Rotate           ( float x, float y, float z );
	void                SetCameraDetails ( const CCamera * pCamera );
	void				walk(float units);
	void				strafe(float units);


	//-------------------------------------------------------------------------
	// Private Functions for This Class.
	//-------------------------------------------------------------------------
	float				m_fPitch;
	float				m_fYaw;
	float				m_fRoll;
};
