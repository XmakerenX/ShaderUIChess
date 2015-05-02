#pragma once

#define VERTEX_FVF      D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
#define V_FVF			D3DFVF_XYZ | D3DFVF_TEX1

//-----------------------------------------------------------------------------
// Name : CVertex (Class)
// Desc : Vertex class used to construct & store vertex components.
// Note : A simple vertex type, used to store only tex coords and pos.
//-----------------------------------------------------------------------------
class CMyVertex
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CMyVertex( float fX, float fY, float fZ, const D3DXVECTOR3 & vecNormal, float ftu = 0.0f, float ftv = 0.0f ) 
	{ x = fX; y = fY; z = fZ; Normal = vecNormal; tu = ftu; tv = ftv; }

	CMyVertex() 
	{ x = 0.0f; y = 0.0f; z = 0.0f; Normal = D3DXVECTOR3( 0, 0, 0 ); tu = 0.0f; tv = 0.0f; }

	//-------------------------------------------------------------------------
	// Public Variables for This Class
	//-------------------------------------------------------------------------
	float       x;          // Vertex Position X Component
	float       y;          // Vertex Position Y Component
	float       z;          // Vertex Position Z Component
	D3DXVECTOR3 Normal;     // Vertex normal.
	float       tu;         // Texture u coordinate
	float       tv;         // Texture v coordinate
};