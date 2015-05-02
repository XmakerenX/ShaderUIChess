#include "CMyObject.h"

D3DXHANDLE CMyObject::m_matWorldViewProjH = NULL;
D3DXHANDLE CMyObject::m_matWorldH = NULL;
D3DXHANDLE CMyObject::m_matWorldTH = NULL;


//-----------------------------------------------------------------------------
// Name : CObject () (Constructor)
// Desc : CObject Class Constructor
//-----------------------------------------------------------------------------
CMyObject::CMyObject(void)
{
	// Reset / Clear all required values
	m_pMesh = NULL;
	D3DXMatrixIdentity( &m_mtxWorld );
	D3DXMatrixIdentity( &m_mtxRot );
	D3DXMatrixIdentity( &m_mtxScale );
	m_rotAngles.x = 0.0;
	m_rotAngles.y = 0.0;
	m_rotAngles.z = 0.0;

	m_useCustomAttribMap = false;
	m_hideObject		 = false;
	m_pCustomAtribbMap	 = NULL;
	m_customAtrribCount  = 0;
}

//-----------------------------------------------------------------------------
// Name : CObject () (Alternate Constructor)
// Desc : CObject Class Constructor, sets the internal mesh object
//-----------------------------------------------------------------------------
CMyObject::CMyObject( CMyMesh * pMesh ,OBJECT_PREFS* objectPref)
{
	//TODO: add support for rotations not only around the x axis

	if (!objectPref)//check if we got object preferences if not use default ones
	{
		// Reset / Clear all required values
		D3DXMatrixIdentity( &m_mtxWorld );
		D3DXMatrixIdentity( &m_mtxRot );
		D3DXMatrixIdentity( &m_mtxScale );
		m_rotAngles.x = 0;
		m_rotAngles.y = 0;
		m_rotAngles.z = 0;
	}
	else
	{
		//retrieving the vectors from the objectPref struct for making the code clearer
		D3DXVECTOR3 pos		  = objectPref->pos;
		m_rotAngles = objectPref->rotAngels;
		D3DXVECTOR3 scale	  = objectPref->scale;

		D3DXMATRIX mtxRotX;
		D3DXMATRIX mtxRotY;
		D3DXMATRIX mtxRotZ;

		D3DXMatrixTranslation(&m_mtxWorld,pos.x,pos.y,pos.z);

		D3DXMatrixRotationX(&mtxRotX,m_rotAngles.x);
		D3DXMatrixRotationY(&mtxRotY,m_rotAngles.y);
		D3DXMatrixRotationZ(&mtxRotZ,m_rotAngles.z);
		m_mtxRot = mtxRotX * mtxRotY * mtxRotZ;

		D3DXMatrixScaling(&m_mtxScale,scale.x,scale.y,scale.z);
	}

	m_hideObject		 = false;
	m_useCustomAttribMap = false;
	m_pCustomAtribbMap	 = NULL;
	m_customAtrribCount  = 0;

	// Set Mesh
	m_pMesh = pMesh;
}

CMyObject::~CMyObject(void)
{
	if (m_pMesh) 
		m_pMesh = NULL;

	if (m_pCustomAtribbMap)
		delete []m_pCustomAtribbMap;

	D3DXMatrixIdentity( &m_mtxWorld );
	D3DXMatrixIdentity( &m_mtxRot );
	D3DXMatrixIdentity( &m_mtxScale );
	//m_rotAngle = 0.0;
}

D3DXMATRIX CMyObject::getWorldMatrix()
{
	D3DXMATRIX rot;
	//D3DXMatrixRotationX(&rot,m_rotAngle);
	return m_mtxScale * m_mtxRot * m_mtxWorld;
}

void CMyObject::attachMesh(CMyMesh * pMesh)
{
	m_pMesh = pMesh;
	m_useCustomAttribMap = false;
}
	
void CMyObject::drawSubset(IDirect3DDevice9* pd3dDevice, ULONG AttributeID, ID3DXEffect * effect, UINT numPass, D3DXMATRIX ViewProj) 
{
	ULONG * pAtrributeMap;
	ULONG nAttributeCount;
	//int wtf;

	if (m_useCustomAttribMap)//check if to use custom attributes for this object or the default ones used in mesh creation 
	{
		pAtrributeMap   = m_pCustomAtribbMap;
		nAttributeCount = m_customAtrribCount;
	}
	else
	{
		pAtrributeMap   = m_pMesh->getAtrributeMap();
		nAttributeCount = m_pMesh->getAttributeCount();
	}

	for (ULONG i = 0 ; i < nAttributeCount ; i++)
	{
		if (pAtrributeMap[i] == AttributeID)//check for the corresponding attribute in the attribute map
		{
			if (pd3dDevice)// if there was given a d3d device function should also set world transformation
			{
// 				pd3dDevice->SetTransform( D3DTS_WORLD, &getWorldMatrix() );
// 				pd3dDevice->SetFVF( m_pMesh->GetFVF() );
				//wtf = pd3dDevice->Release();
			}

			effect->SetMatrix(m_matWorldViewProjH, &(getWorldMatrix() * ViewProj) );
			effect->SetMatrix(m_matWorldH, &getWorldMatrix());

			D3DXMATRIX worldInverseTranspose;
			D3DXMatrixInverse(&worldInverseTranspose, 0, &getWorldMatrix());
			D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);
			effect->SetMatrix(m_matWorldTH, &worldInverseTranspose);
			//effect->SetMatrix("matWorldViewProj", &(getWorldMatrix() * ViewProj) );
			//effect->SetMatrix("matWorld", &getWorldMatrix());

			effect->BeginPass(numPass);

			m_pMesh->DrawSubset(i);//found the correct attribute draw it 

			effect->EndPass();
			break;//no need to keep scaning as we have found the attribute
		}
	}
}

bool CMyObject::customiseAtrributes(ULONG* newAttribMap,ULONG atrributeCount)
{
	ULONG meshAtrributeCount = m_pMesh->getAttributeCount();

	if (meshAtrributeCount == atrributeCount)//check if the supplied attribute count  info match the mesh subsets number 
	{
		m_pCustomAtribbMap = new ULONG[atrributeCount];//allocating space for the custom attribute map
		if (!m_pCustomAtribbMap)
			return false;

		memcpy(m_pCustomAtribbMap,newAttribMap,sizeof(ULONG) * atrributeCount);//copying the info to the new map

		m_customAtrribCount = atrributeCount;
		m_useCustomAttribMap = true;

		return true;
	}
	else 
		return false;
}

CMyMesh* CMyObject::getMesh( ) const
{
	return m_pMesh;
}

bool CMyObject::isObjectHidden()
{
	return m_hideObject;
}

void CMyObject::setObjectHidden(bool newStatus)
{
	m_hideObject = newStatus;
}
void CMyObject::setPos(D3DXVECTOR3 newPos)
{
	//D3DXMatrixTranslation(&m_mtxWorld,newPos.x,newPos.y,newPos.z);
	m_mtxWorld._41 = newPos.x;
	m_mtxWorld._42 = newPos.y;
	m_mtxWorld._43 = newPos.z;
}

void CMyObject::setRotAngels(D3DXVECTOR3 rotAngels)
{
	D3DXMATRIX mtxRotX;
	D3DXMATRIX mtxRotY;
	D3DXMATRIX mtxRotZ;

	m_rotAngles = rotAngels;
	D3DXMatrixRotationX(&mtxRotX,m_rotAngles.x);
	D3DXMatrixRotationY(&mtxRotY,m_rotAngles.y);
	D3DXMatrixRotationZ(&mtxRotZ,m_rotAngles.z);
	m_mtxRot = mtxRotX * mtxRotY * mtxRotZ;
}

void CMyObject::Rotate(float x, float y, float z)
{
	D3DXMATRIX mtxRotX;
	D3DXMATRIX mtxRotY;
	D3DXMATRIX mtxRotZ;

	m_rotAngles.x += x;
	m_rotAngles.y += y;
	m_rotAngles.z += z;

	D3DXMatrixRotationX(&mtxRotX,m_rotAngles.x);
	D3DXMatrixRotationY(&mtxRotY,m_rotAngles.y);
	D3DXMatrixRotationZ(&mtxRotZ,m_rotAngles.z);
	m_mtxRot = mtxRotX * mtxRotY * mtxRotZ;
}

void CMyObject::setRotaionMatrix(D3DXMATRIX rotMatrix)
{
	m_mtxRot = rotMatrix;
}

void CMyObject::setShadersHandles(D3DXHANDLE matWorldViewProjH, D3DXHANDLE matWorldH, D3DXHANDLE matWorldTH)
{
	CMyObject::m_matWorldViewProjH = matWorldViewProjH;
	CMyObject::m_matWorldH = matWorldH;
	CMyObject::m_matWorldTH = matWorldTH;
}