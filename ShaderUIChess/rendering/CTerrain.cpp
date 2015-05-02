#include "CTerrain.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pDevice, CAssetManager& assetManger,CMyMesh *pTerrainMesh,TERRAIN_PREFS& TerrianPref)
:CMyObject(pTerrainMesh,&TerrianPref.objPrefs)
{
	m_selectSquare = -1; //max the value which means nothing is selected
	m_fraemSqaure = -1;

	m_selectSquare = 1;
	
	std::vector<ULONG>& attackSquaresVec = m_attribSquares[ATTACK];
	std::vector<ULONG>& moveSquaresVec = m_attribSquares[MOVE];

	attackSquaresVec.push_back(20);
	attackSquaresVec.push_back(22);

	moveSquaresVec.push_back(16);
	moveSquaresVec.push_back(17);

	m_squareMesh = new CMyMesh();
	m_squareMesh->SetDataFormat(VERTEX_FVF, sizeof(USHORT) );

	createTerrain(pDevice, assetManger,TerrianPref.minBounds,TerrianPref.maxBounds,TerrianPref.numCellsWide,TerrianPref.numCellsHigh,TerrianPref.objPrefs.scale);
}

CTerrain::~CTerrain(void)
{
	if (m_squareMesh)
	{
		m_squareMesh->Release();
		delete m_squareMesh;
		m_squareMesh = NULL;
	}
	//Release();
}

HRESULT CTerrain::createTerrain( LPDIRECT3DDEVICE9 pDevice, CAssetManager& assetManger, D3DXVECTOR3 minBounds, D3DXVECTOR3 maxBounds ,UINT numCellsWide, UINT numCellsHigh, D3DXVECTOR3 vecScale )
{
	HRESULT hRet;
	bool ManageAttribs = m_pMesh->isMenageAtrributes();

// 	D3DXVECTOR3 minBounds;
// 	D3DXVECTOR3 maxBounds;
// 	int numCellsWide;
// 	int numCellsHigh;
//calculate the step size from one vertex to the other along both the x and the z axis 
// 	float stepX=(maxBounds.x-minBounds.x)/numCellsWide;
// 	float stepZ=(maxBounds.z-minBounds.z)/numCellsHigh;

	m_numCellsHigh = numCellsHigh;
	m_numCellsWide = numCellsWide;
	m_scaleVec = vecScale;

	USHORT numVertsX = numCellsWide+1;
	USHORT numVertsZ = numCellsHigh+1;

	float stepX = 1.0f;
	float stepZ = 1.0f;

	//preparing the vertex buffer for the terrain mesh
	CMyVertex* pVertices = new CMyVertex[numVertsX * numVertsZ];
	if (!pVertices) return E_OUTOFMEMORY;

	ZeroMemory( pVertices,sizeof(CMyVertex) * (numVertsX * numVertsZ) );

	// Set the start position
	D3DXVECTOR3 pos(minBounds.x, 0, minBounds.z);

	int VertexCount=0;
	float Tu = 0.0f;
	float Tv = 0.0f;

	// Loop across and up
	for (int z=0 ; z<numVertsZ ; z++)
	{
		Tu = 0.0f;
		pos.x = minBounds.x;

		for (int x=0 ; x<numVertsX ; x++)
		{
			// Create the verts
			pVertices[VertexCount].x = pos.x ;//* vecScale.x;
			pVertices[VertexCount].y = pos.y ;//* vecScale.y;
			pVertices[VertexCount].z = pos.z ;//* vecScale.z;
			pVertices[VertexCount].Normal = D3DXVECTOR3(0.0f , 1.0f ,0.0f);
			pVertices[VertexCount].tu = Tu;
			pVertices[VertexCount].tv = Tv;

			Tu++;

			// Increment x across
			pos.x += stepX;
			VertexCount++;
		}

		Tv++;
		// Increment Z
		pos.z+=stepZ;
	}
	//end of  preparing the vertex buffer

	//preparing the index buffer 
	int count = 0;
	USHORT vIndex = 0;


	//creating the attribute data 
	MESH_ATTRIB_DATA* pAttribData;
	//ULONG AttribID[2]; not used anymore??
	//creating a pure white material
	OBJMATERIAL Material;
	ZeroMemory( &Material, sizeof(OBJMATERIAL));
	Material.Diffuse = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0f );
	Material.Ambient = D3DXVECTOR4( 1.0, 1.0, 1.0, 1.0f );


	ULONG attribMap[2];

	if (ManageAttribs)
	{	
		if ( m_pMesh->AddAttributeData(2) < 0)
			return E_OUTOFMEMORY;

		pAttribData = m_pMesh->GetAttributeData();

		for (int i = 0; i < 2; i++)
		{
			pAttribData[i].Texture = assetManger.getTexture(Textures[i]);
		 	pAttribData[i].Material = Material;
		 	pAttribData[i].Effect = NULL;
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			attribMap[i] = assetManger.getAttributeID(Textures[i], &Material, NULL);
			//attribMap[i] = m_pMesh->getAttributes(Textures[i],&Material,NULL);
		}

		m_pMesh->setAttribMap(attribMap, 2);
	}

	//the max number of indices in the terrain is numCellsHigh * numCellsWide * how many indices per cell which in our case for now is 6 ...
	USHORT* pIndices = new USHORT[6];
	for (UINT z = 0; z < numCellsHigh; z++)
	{
// 		delete []pIndices;
// 		pIndices = NULL;
// 		pIndices = new USHORT[6];

		for (UINT x = 0; x < numCellsWide; x++)
		{
			ULONG attribID;
			// first triangle
			pIndices[count++] = vIndex;
			pIndices[count++] = vIndex+numVertsX;
			pIndices[count++] = vIndex+numVertsX+1;

			// second triangle
			pIndices[count++] = vIndex;
			pIndices[count++] = vIndex+numVertsX+1;
			pIndices[count++] = vIndex+1;

			if (vIndex % 2 == 0)
			{
				if (ManageAttribs)
					attribID = 0;
				else
					attribID = 0;
			}
			else
			{
				if (ManageAttribs)
					attribID = 1;
				else
					attribID = 1;
			}

			if ( m_pMesh->AddFace(2,pIndices,attribID) < 0)
				return E_OUTOFMEMORY;

			delete []pIndices;
			count = 0;
			pIndices = NULL;
			pIndices = new USHORT[6];

			vIndex++;
		}
		vIndex++;
	}
	//end of preparing the index buffer 

	if ( m_pMesh->AddVertex(VertexCount,pVertices) < 0)
		return E_OUTOFMEMORY;
	

	hRet = m_pMesh->BuildMesh(D3DXMESH_MANAGED,pDevice);

	delete []pVertices;
	delete []pIndices;

	//creating a squre mesh
	pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	VertexCount = 0;
	Tu = 0.0f;
	Tv = 0.0f;

	pVertices = new CMyVertex[4];

	if (!pVertices)
		return E_OUTOFMEMORY;

	for (UINT z = 0; z < 2; z++)
	{
		Tu = 0.0f;
		pos.x = 0;

		for (UINT x = 0; x < 2; x++)
		{
			// Create the verts
			pVertices[VertexCount].x = pos.x * vecScale.x;
			pVertices[VertexCount].y = pos.y * vecScale.y;
			pVertices[VertexCount].z = pos.z * vecScale.z;
			pVertices[VertexCount].Normal = D3DXVECTOR3(0.0f , 1.0f ,0.0f);
			pVertices[VertexCount].tu = Tu;
			pVertices[VertexCount].tv = Tv;

			Tu++;

			// Increment x across
			pos.x += stepX;
			VertexCount++;
		}

		Tv++;
		// Increment Z
		pos.z += stepZ;
	}

	pIndices = new USHORT[6];
	vIndex = 0;
	count = 0;
	for (int z=0 ; z < 1 ; z++)
	{
		for (int x=0 ; x < 1 ; x++)
		{
			ULONG attribID;
			// first triangle
			pIndices[count++] = vIndex;
			pIndices[count++] = vIndex+2;
			pIndices[count++] = vIndex+3;

			// second triangle
			pIndices[count++] = vIndex;
			pIndices[count++] = vIndex+3;
			pIndices[count++] = vIndex+1;

			attribID = 0;

			if ( m_squareMesh->AddFace(2,pIndices, 0) < 0)
				return E_OUTOFMEMORY;

			delete []pIndices;
			count = 0;
			pIndices = NULL;
			pIndices = new USHORT[6];

			vIndex++;
		}
		vIndex++;
	}

	if ( m_squareMesh->AddVertex(4,pVertices) < 0)
		return E_OUTOFMEMORY;

	m_squareMesh->BuildMesh(D3DXMESH_MANAGED,pDevice);

	OBJMATERIAL matrial;
	matrial = d3d::BLUE_MTRL;

	m_attribIDs[MOVE] = assetManger.getAttributeID(NULL,&matrial,NULL);

	matrial = d3d::YELLOW_MTRL;
	m_attribIDs[SELECT] = assetManger.getAttributeID(NULL, &matrial, NULL);

	matrial = d3d::RED_MTRL;
	m_attribIDs[ATTACK] = assetManger.getAttributeID(NULL,&matrial,NULL);

	matrial = d3d::WHITE_MTRL;
	//matrial.Diffuse.w = 1.0;
	matrial.Diffuse.x = 0;
	matrial.Diffuse.y = 0;
	matrial.Diffuse.z = 0;
	matrial.Diffuse.w = 0.0f;

	matrial.Specular.x = 0;
	matrial.Specular.y = 0;
	matrial.Specular.z = 0;
	matrial.Specular.w = 0;

	//m_attribIDs[FRAME] = assetManger.getAttributeID("frame.png",&matrial,NULL);
	m_attribIDs[FRAME] = assetManger.getAttributeID("bla.png",&matrial,NULL);

	if (pVertices)
		delete []pVertices;

	if (pIndices)
		delete []pIndices;

	return hRet;	
	//AddFace(numCellsHigh * numCellsWide * 2,indices,?);

	
}

void CTerrain::drawSubset(IDirect3DDevice9* pd3dDevice, ULONG AttributeID, ID3DXEffect * effect, UINT numPass, D3DXMATRIX ViewProj)
{
	HRESULT hr;

	CMyObject::drawSubset(pd3dDevice, AttributeID, effect , numPass, ViewProj);

	hr = pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
	//pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, (DWORD)-0.0005f);
	//pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, (DWORD)1.0f);
	  
	//hr = pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//hr = pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	for (UINT i = 0; i < 2; i++)
	{
		if (m_attribIDs[i] == AttributeID)
		{
			std::vector<ULONG>& curSquaeVec = m_attribSquares[i];

			for (UINT j = 0; j < curSquaeVec.size(); j++)
			{
				drawSquare(curSquaeVec[j], effect, numPass, ViewProj);
// 				D3DXMATRIX  MSquareW;
// 				ULONG x,z;
// 
// 
// 				ULONG curSqaure = curSquaeVec[j];
// 				D3DXVECTOR3 curSquarePos = D3DXVECTOR3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);//get terrain pos
// 
// 				x = ( (curSqaure % (m_numCellsWide * 2) ) / 2 );
// 				z = curSqaure / (m_numCellsWide * 2);
// 
// 				curSquarePos += D3DXVECTOR3(x * m_scaleVec.x, 0.001f, z * m_scaleVec.z);
// 				D3DXMatrixTranslation(&MSquareW, curSquarePos.x, curSquarePos.y, curSquarePos.z);
// 
// 				hr = effect->SetMatrix("matWorldViewProj", &(MSquareW * ViewProj) );
// 				hr = effect->SetMatrix("matWorld", &MSquareW);
// 
// 				hr = effect->BeginPass(numPass);
// 
// 				m_squareMesh->DrawSubset(0);
// 				//m_pMesh->DrawSubset(0);
// 
// 				effect->EndPass();

			}
			
			//m_squareMesh->DrawSubset(0);
		}
	}

	if ( m_attribIDs[SELECT] == AttributeID && m_selectSquare != -1)
	{
		drawSquare(m_selectSquare, effect, numPass, ViewProj);
	}

	if (m_attribIDs[FRAME] == AttributeID && m_fraemSqaure != -1)
	{
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		drawSquare(m_fraemSqaure, effect, numPass, ViewProj);
	}

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, (DWORD)0.0f);
	//pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, (DWORD)0.0f);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTerrain::drawSquare(ULONG squareNum, ID3DXEffect * effect, UINT numPass, D3DXMATRIX ViewProj)
{
	HRESULT hRet;

	D3DXMATRIX  MSquareW;
	ULONG x,z;


	ULONG curSqaure = squareNum;
	D3DXVECTOR3 curSquarePos = D3DXVECTOR3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);//get terrain pos

	x = ( (curSqaure % (m_numCellsWide) )  );
	z = curSqaure / (m_numCellsWide );

	curSquarePos += D3DXVECTOR3(x * m_scaleVec.x, 0, z * m_scaleVec.z);
	D3DXMatrixTranslation(&MSquareW,curSquarePos.x, curSquarePos.y, curSquarePos.z);

	effect->SetMatrix(m_matWorldViewProjH, &(MSquareW * ViewProj) );
	effect->SetMatrix(m_matWorldH, &MSquareW);
// 	effect->SetMatrix("matWorldViewProj", &(MSquareW * ViewProj) );
// 	effect->SetMatrix("matWorld", &MSquareW);

	hRet = effect->BeginPass(numPass);

	m_squareMesh->DrawSubset(0);
	//m_pMesh->DrawSubset(0);

	effect->EndPass();

	return hRet;
}

void CTerrain::setFrameSqaure(DWORD faceCount)
{
	ULONG sqaure = faceCount /2;
	if (sqaure < 64)
		m_fraemSqaure = sqaure;
}