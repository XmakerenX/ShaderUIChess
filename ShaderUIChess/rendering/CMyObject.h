#pragma once

//#include "main.h"
#include "CMyMesh.h"

struct OBJECT_PREFS //groups together all the preferences of the object
{
	D3DXVECTOR3 rotAngels;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 scale;
};

class CMyMesh; 

//-----------------------------------------------------------------------------
// Name : CObject (Class)
// Desc : Mesh container class used to store instances of meshes.
//-----------------------------------------------------------------------------
class CMyObject
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CMyObject(void);
	CMyObject( CMyMesh * pMesh ,OBJECT_PREFS* objectPref );
	virtual ~CMyObject(void);

	D3DXMATRIX				getWorldMatrix		( );
	void				    attachMesh			(CMyMesh * pMesh);
	virtual void			drawSubset			(IDirect3DDevice9* pd3dDevice, ULONG AttributeID, ID3DXEffect * effect, UINT numPass, D3DXMATRIX ViewProj);
	bool					customiseAtrributes (ULONG* newAttribMap,ULONG atrributeCount);
	CMyMesh*				getMesh				( ) const;

	bool					isObjectHidden		();
	void					setObjectHidden     (bool newStatus);
	void					setPos				(D3DXVECTOR3 newPos);
	void					setRotAngels		(D3DXVECTOR3 rotAngels);
	void					Rotate				(float x, float y, float z);
	void					setRotaionMatrix    (D3DXMATRIX rotMatrix);

	static void				setShadersHandles   (D3DXHANDLE matWorldViewProjH, D3DXHANDLE matWorldH, D3DXHANDLE matWorldTH);

	//-------------------------------------------------------------------------
	// Public Variables for This Class
	//-------------------------------------------------------------------------
	D3DXMATRIX  m_mtxWorld;             // Objects world matrix
	D3DXMATRIX  m_mtxRot;
	D3DXMATRIX  m_mtxScale;
	D3DXVECTOR3 m_rotAngles;

private:
	bool		m_useCustomAttribMap;
	bool        m_hideObject;
	ULONG	  * m_pCustomAtribbMap;
	ULONG       m_customAtrribCount;

protected:
	CMyMesh   * m_pMesh;                // Mesh we are instancing

	static D3DXHANDLE 		m_matWorldViewProjH;
	static D3DXHANDLE 		m_matWorldH;
	static D3DXHANDLE		m_matWorldTH;
};
