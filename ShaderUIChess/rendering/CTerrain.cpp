#include "CTerrain.h"

//-----------------------------------------------------------------------------
// Name : CTerrain (constructor)
//-----------------------------------------------------------------------------
CTerrain::CTerrain(LPDIRECT3DDEVICE9 pDevice, CAssetManager& assetManger,CMyMesh *pTerrainMesh,TERRAIN_PREFS& TerrianPref)
:CMyObject(pTerrainMesh,&TerrianPref.objPrefs)
{
	m_selectSquare = -1; //max the value which means nothing is selected
	m_fraemSqaure = -1;

	m_threatSquare = -1;
	
// 	std::vector<ULONG>& attackSquaresVec = m_attribSquares[ATTACK];
// 	std::vector<ULONG>& moveSquaresVec = m_attribSquares[MOVE];
// 
// 	attackSquaresVec.push_back(20);
// 	attackSquaresVec.push_back(22);
// 
// 	moveSquaresVec.push_back(16);
// 	moveSquaresVec.push_back(17);

	m_squareMesh = new CMyMesh();
	m_squareMesh->SetDataFormat(VERTEX_FVF, sizeof(USHORT) );

	m_boardFrameMesh = new CMyMesh();
	m_boardFrameMesh->SetDataFormat(VERTEX_FVF, sizeof(USHORT) );

	m_boardFrameAttrib = -1;
	for (UINT i = 0; i < 18; i++)
		m_boardFrameAtribs[i] = -1;

	createTerrain(pDevice, assetManger,TerrianPref.minBounds,TerrianPref.maxBounds,TerrianPref.numCellsWide,TerrianPref.numCellsHigh,TerrianPref.objPrefs.scale);
}

//-----------------------------------------------------------------------------
// Name : CTerrain (destructor)
//-----------------------------------------------------------------------------
CTerrain::~CTerrain(void)
{
	if (m_squareMesh)
	{
		m_squareMesh->Release();
		delete m_squareMesh;
		m_squareMesh = nullptr;
	}

	if (m_boardFrameMesh)
	{
		m_boardFrameMesh->Release();
		delete m_boardFrameMesh;
		m_boardFrameMesh = nullptr;
	}
	//Release();
}

//-----------------------------------------------------------------------------
// Name : createTerrain ()
//-----------------------------------------------------------------------------
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

	//-------------------------------------
	// Create board Mesh Vertices
	//-------------------------------------
	m_numCellsHigh = numCellsHigh;
	m_numCellsWide = numCellsWide;
	m_scaleVec = vecScale;

	USHORT numVertsX = numCellsWide+1;
	USHORT numVertsZ = numCellsHigh+1;

	m_stepX = 1.0f;
	m_stepZ = 1.0f;
	m_meshScale = vecScale;

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
			pos.x += m_stepX;
			VertexCount++;
		}

		Tv++;
		// Increment Z
		pos.z += m_stepZ;
	}
	//end of  preparing the vertex buffer

	//preparing the index buffer 
	int count = 0;
	USHORT vIndex = 0;

	//-------------------------------------
	// Create board Mesh attribute data
	//-------------------------------------
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

	//-------------------------------------
	// Create board Mesh index buffer
	//-------------------------------------
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

	//---------------------------------------------------------
	// finish adding all the vertices and build board mesh
	//---------------------------------------------------------
	if ( m_pMesh->AddVertex(VertexCount,pVertices) < 0)
		return E_OUTOFMEMORY;
	

	hRet = m_pMesh->BuildMesh(D3DXMESH_MANAGED,pDevice);

	delete []pVertices;
	delete []pIndices;

	//---------------------------------------------------------
	// Create board Frame vertices
	//---------------------------------------------------------
	VertexCount = 0;
	Tu = 0.0f;
	Tv = 0.0f;
	
	pos = D3DXVECTOR3(minBounds.x, 0, minBounds.y);

	D3DXVECTOR3 boardFramePos;
	//boardFramePos.x = pos.x - m_stepX - ((float)2 / vecScale.x);
	boardFramePos.x = pos.x - m_stepX + ((float)2 / vecScale.x);
	boardFramePos.y = pos.y;
	//boardFramePos.z = pos.z - m_stepZ - ((float)2 / vecScale.z);
	boardFramePos.z = pos.z - m_stepZ + ((float)2 / vecScale.z);

	D3DXVECTOR3 framePos;
	framePos = boardFramePos;

	int boardFrameCells = (numVertsX * numVertsZ) + 4;
	pVertices  = new CMyVertex[boardFrameCells];

	//---------------------------------------------------------
	// lower part of the board frame vertices
	//---------------------------------------------------------
	Tv = 1.0f;
	for (int z = 0; z < 2; z++)
	{
		Tu = 0.0f;
		
		framePos.x = boardFramePos.x;

		for (int x = 0; x < numVertsX + 2; x++)
		{
			// Create the verts
			pVertices[VertexCount].x = framePos.x /** vecScale.x*/;
			pVertices[VertexCount].y = framePos.y /** vecScale.y*/;
			pVertices[VertexCount].z = framePos.z /** vecScale.z*/;
			pVertices[VertexCount].Normal = D3DXVECTOR3(0.0f , 1.0f ,0.0f);
			pVertices[VertexCount].tu = Tu;
			pVertices[VertexCount].tv = Tv;

			Tu++;

			// Increment x across
			if (x == 0 || x == numVertsX)
				//framePos.x += ((float)2 /  vecScale.x);
				framePos.x -= ((float)2 / vecScale.x);

			framePos.x += m_stepX;
			VertexCount++;
		}

		//framePos.z += m_stepZ + ((float)2 / vecScale.z);
		framePos.z += m_stepZ - ((float)2 / vecScale.z);

		Tv--;
	}

	Tu = 0.0f;
	Tv = 0.0f;

	boardFramePos.x = pos.x + m_stepX * m_numCellsWide;
	boardFramePos.y = pos.y;
	boardFramePos.z = pos.z + m_stepZ;

	framePos = boardFramePos;

	//---------------------------------------------------------
	// right part of the board frame vertices
	//---------------------------------------------------------
	Tv = 1.0f;
	for (int z = 2; z < numVertsZ + 2; z++)
	{
		Tu = 9.0f;

		framePos.x = boardFramePos.x;

		if ( z == (numVertsZ + 1) )
			//Tv = -6.0f;
			Tv += 2;

		for (int x = 0; x < 2; x++)
		{

			// Create the verts
			pVertices[VertexCount].x = framePos.x /** vecScale.x*/;
			pVertices[VertexCount].y = framePos.y/* * vecScale.y*/;
			pVertices[VertexCount].z = framePos.z /** vecScale.z*/;
			pVertices[VertexCount].Normal = D3DXVECTOR3(0.0f , 1.0f ,0.0f);
			pVertices[VertexCount].tu = Tu;
			pVertices[VertexCount].tv = Tv;


			Tu++;

			// Increment x across
			//framePos.x += m_stepX + ((float)2 / vecScale.x);
			framePos.x += m_stepX - ((float)2 / vecScale.x);
			VertexCount++;
		}

		if ( z == 9)
			//framePos.z += ((float)2 / vecScale.z);
			framePos.z -= ((float)2 / vecScale.z);

		framePos.z += m_stepZ;

		Tv--;
	}

	//boardFramePos.x = pos.x - m_stepX - ((float)2 / vecScale.x);
	boardFramePos.x = pos.x - m_stepX + ((float)2 / vecScale.x);
	boardFramePos.y = pos.y;
	boardFramePos.z = pos.z + m_stepZ * numCellsHigh;

	framePos = boardFramePos;

	Tu = 0.0f;
	Tv = -6.0f;

	//---------------------------------------------------------
	// upper part of the board frame vertices
	//---------------------------------------------------------
	for (int z = 0; z < 2; z++)
	{
		Tu = 16.0f;
		framePos.x = boardFramePos.x;

		for (int x = 0; x < numVertsX; x++)
		{

			// Create the verts
			pVertices[VertexCount].x = framePos.x /** vecScale.x*/;
			pVertices[VertexCount].y = framePos.y /** vecScale.y*/;
			pVertices[VertexCount].z = framePos.z /** vecScale.z*/;
			pVertices[VertexCount].Normal = D3DXVECTOR3(0.0f , 1.0f ,0.0f);
			pVertices[VertexCount].tu = Tu;
			pVertices[VertexCount].tv = Tv;

			if (x == 0)
				Tu++;
			else
				Tu--;

			// Increment x across
			if (x == 0)
				//framePos.x += ((float)2 / vecScale.x);
				framePos.x -= ((float)2 / vecScale.x );

			framePos.x += m_stepX;
			VertexCount++;
		}

		//framePos.z += m_stepZ + ((float)2 / vecScale.z);
		framePos.z += m_stepZ - ((float)2 / vecScale.z);

		Tv++;
	}

	//boardFramePos.x = pos.x - m_stepX - ((float)2 / vecScale.x);
	boardFramePos.x = pos.x - m_stepX + ((float)2 / vecScale.x);
	boardFramePos.y = pos.y;
	boardFramePos.z = pos.z /*+ m_stepZ*/;

	framePos = boardFramePos;

	Tu = 0.0f;
	Tv = 1.0f;

	//---------------------------------------------------------
	// left part of the board frame vertices
	//---------------------------------------------------------
	for (int z = 0; z < numVertsZ + 1; z++)
	{
		Tu = 0.0f;

		framePos.x = boardFramePos.x;

		for (int x = 0; x < 2; x++)
		{
			// Create the verts
			pVertices[VertexCount].x = framePos.x /** vecScale.x*/;
			pVertices[VertexCount].y = framePos.y /** vecScale.y*/;
			pVertices[VertexCount].z = framePos.z /** vecScale.z*/;
			pVertices[VertexCount].Normal = D3DXVECTOR3(0.0f , 1.0f ,0.0f);
			pVertices[VertexCount].tu = Tu;
			pVertices[VertexCount].tv = Tv;

			Tu--;

			// Increment x across
			//framePos.x += m_stepX + ((float)2 / vecScale.x);
			framePos.x += m_stepX - ((float)2 / vecScale.x);
			VertexCount++;
		}

		framePos.z += m_stepZ;

		Tv++;
	}

	vIndex = 0;

	//---------------------------------------------------------
	// lower part of the board frame indexes
	//---------------------------------------------------------
	pIndices = new USHORT[6];

	ULONG attribID = 0;

	for (UINT x = 0; x < numCellsWide + 2; x++)
	{
		// first triangle
		pIndices[count++] = vIndex;
		pIndices[count++] = vIndex+numVertsX + 2;
		pIndices[count++] = vIndex+numVertsX + 2 + 1;

		// second triangle
		pIndices[count++] = vIndex;
		pIndices[count++] = vIndex+numVertsX + 2 + 1;
		pIndices[count++] = vIndex+1;

		//attribID = 0;

		if ( m_boardFrameMesh->AddFace(2,pIndices,attribID++) < 0)
			return E_OUTOFMEMORY;

		delete []pIndices;
		count = 0;
		pIndices = NULL;
		pIndices = new USHORT[6];

		vIndex++;
	}
	
	vIndex += numVertsX + 1;
	//---------------------------------------------------------
	// right part of the board frame indexes
	//---------------------------------------------------------
	for (UINT x = 0; x < numCellsWide + 1; x++)
	{
		// first triangle
		pIndices[count++] = vIndex;
		pIndices[count++] = vIndex + 2;
		pIndices[count++] = vIndex +2 + 1;

		// second triangle
		pIndices[count++] = vIndex;
		pIndices[count++] = vIndex+ 2 + 1;
		pIndices[count++] = vIndex+1;

		if (x == numCellsWide)
			attribID = 9;

		if ( m_boardFrameMesh->AddFace(2,pIndices,attribID++) < 0)
			return E_OUTOFMEMORY;

		delete []pIndices;
		count = 0;
		pIndices = NULL;
		pIndices = new USHORT[6];

		vIndex +=2;
	}

	SHORT rightEndingIndex = vIndex - 2;

	vIndex += 2;

	SHORT upperStartingIndex = vIndex;
	attribID = 0;

	//---------------------------------------------------------
	// upper part of the board frame indexes
	//---------------------------------------------------------
	pIndices = new USHORT[6];
	for (UINT x = 0; x < numCellsWide; x++)
	{
		// first triangle
		pIndices[count++] = vIndex;
		pIndices[count++] = vIndex+numVertsX;
		pIndices[count++] = vIndex+numVertsX + 1;

		// second triangle
		pIndices[count++] = vIndex;
		pIndices[count++] = vIndex+numVertsX + 1;
		pIndices[count++] = vIndex+1;

		//attribID = 0;

		if ( m_boardFrameMesh->AddFace(2,pIndices,attribID++) < 0)
			return E_OUTOFMEMORY;

		delete []pIndices;
		count = 0;
		pIndices = NULL;
		pIndices = new USHORT[6];

		vIndex++;
	}

	//---------------------------------------------------------
	// adding the connecting square between upper and right
	//---------------------------------------------------------
	// first triangle
	pIndices[count++] = vIndex ; //rightEndingIndex
	pIndices[count++] = vIndex+numVertsX;
	pIndices[count++] = rightEndingIndex + 2;

	// second triangle
	pIndices[count++] = vIndex;
	pIndices[count++] = rightEndingIndex + 2;
	pIndices[count++] = rightEndingIndex;

	if ( m_boardFrameMesh->AddFace(2,pIndices,attribID++) < 0)
		return E_OUTOFMEMORY;

	delete []pIndices;
	count = 0;
	pIndices = NULL;
	pIndices = new USHORT[6];

	vIndex += numVertsX + 1;

	attribID = 11;
	//---------------------------------------------------------
	// adding the connecting square between lower and left
	//---------------------------------------------------------
	// first triangle
	 	pIndices[count++] = vIndex;
	 	pIndices[count++] = vIndex + 2;
	 	pIndices[count++] = vIndex + 2 + 1;
	 
	 // second triangle
		pIndices[count++] = vIndex;
		pIndices[count++] = vIndex+ 2 + 1;
		pIndices[count++] = vIndex+1;
	 
	 	if ( m_boardFrameMesh->AddFace(2,pIndices,10) < 0)
	 		return E_OUTOFMEMORY;
	 
	 	delete []pIndices;
	 	count = 0;
	 	pIndices = NULL;
	 	pIndices = new USHORT[6];

		vIndex +=2;
// 	pIndices[count++] = 0 +numVertsX + 2 ;
// 	pIndices[count++] = vIndex;
// 	pIndices[count++] = vIndex + 1;
// 
// 	// second triangle
// 	pIndices[count++] = 0 +numVertsX + 2;
// 	pIndices[count++] = vIndex + 1;
// 	pIndices[count++] = 0 +numVertsX + 2 + 1;
// 
// 	if ( m_boardFrameMesh->AddFace(2,pIndices,10) < 0)
// 		return E_OUTOFMEMORY;
// 
// 	delete []pIndices;
// 	count = 0;
// 	pIndices = NULL;
// 	pIndices = new USHORT[6];

	//---------------------------------------------------------
	// right part of the board frame indexes
	//---------------------------------------------------------
	for (UINT x = 0; x < numCellsWide - 1; x++)
	{
		// first triangle
		pIndices[count++] = vIndex;
		pIndices[count++] = vIndex + 2;
		pIndices[count++] = vIndex +2 + 1;

		// second triangle
		pIndices[count++] = vIndex;
		pIndices[count++] = vIndex+ 2 + 1;
		pIndices[count++] = vIndex+1;

		//attribID = 0;

		if ( m_boardFrameMesh->AddFace(2,pIndices,attribID++) < 0)
			return E_OUTOFMEMORY;

		delete []pIndices;
		count = 0;
		pIndices = NULL;
		pIndices = new USHORT[6];

		vIndex +=2;
	}

	//---------------------------------------------------------
	// adding the connecting square between left and upper
	//---------------------------------------------------------
	// first triangle
// 	pIndices[count++] = vIndex;
// 	pIndices[count++] = upperStartingIndex;
// 	pIndices[count++] = upperStartingIndex + 1;
// 
// 	// second triangle
// 	pIndices[count++] = vIndex;
// 	pIndices[count++] = upperStartingIndex + 1;
// 	pIndices[count++] = vIndex + 1;
// 
// 	if ( m_boardFrameMesh->AddFace(2,pIndices,attribID++) < 0)
// 		return E_OUTOFMEMORY;
// 
// 	delete []pIndices;
// 	count = 0;
	pIndices = NULL;

	//---------------------------------------------------------
	// finish adding all the vertices and build board mesh
	//---------------------------------------------------------
	if ( m_boardFrameMesh->AddVertex(VertexCount,pVertices) < 0)
		return E_OUTOFMEMORY;


	hRet = m_boardFrameMesh->BuildMesh(D3DXMESH_MANAGED,pDevice);

	OBJMATERIAL matrial;

	matrial = d3d::WHITE_MTRL;
	//m_boardFrameAttrib = assetManger.getAttributeID("woodFrame.png", &matrial, NULL);
	m_boardFrameAtribs[0] =  assetManger.getAttributeID("data\\textures\\board\\frameLowerLeft.png",&matrial, nullptr);
	m_boardFrameAtribs[1] =  assetManger.getAttributeID("data\\textures\\board\\frameH.png",&matrial, nullptr);
	m_boardFrameAtribs[2] =  assetManger.getAttributeID("data\\textures\\board\\frameG.png",&matrial, nullptr);
	m_boardFrameAtribs[3] =  assetManger.getAttributeID("data\\textures\\board\\frameF.png",&matrial, nullptr);
	m_boardFrameAtribs[4] =  assetManger.getAttributeID("data\\textures\\board\\frameE.png",&matrial, nullptr);
	m_boardFrameAtribs[5] =  assetManger.getAttributeID("data\\textures\\board\\frameD.png",&matrial, nullptr);
	m_boardFrameAtribs[6] =	 assetManger.getAttributeID("data\\textures\\board\\frameC.png",&matrial, nullptr);
	m_boardFrameAtribs[7] =	 assetManger.getAttributeID("data\\textures\\board\\frameB.png",&matrial, nullptr);
	m_boardFrameAtribs[8] =	 assetManger.getAttributeID("data\\textures\\board\\frameA.png",&matrial, nullptr);
	m_boardFrameAtribs[9] =	 assetManger.getAttributeID("data\\textures\\board\\frameLowerRight.png",&matrial, nullptr);
	m_boardFrameAtribs[10] = assetManger.getAttributeID("data\\textures\\board\\frame8.png",&matrial, nullptr);
	m_boardFrameAtribs[11] = assetManger.getAttributeID("data\\textures\\board\\frame7.png",&matrial, nullptr);
	m_boardFrameAtribs[12] = assetManger.getAttributeID("data\\textures\\board\\frame6.png",&matrial, nullptr);
	m_boardFrameAtribs[13] = assetManger.getAttributeID("data\\textures\\board\\frame5.png",&matrial, nullptr);
	m_boardFrameAtribs[14] = assetManger.getAttributeID("data\\textures\\board\\frame4.png",&matrial, nullptr);
	m_boardFrameAtribs[15] = assetManger.getAttributeID("data\\textures\\board\\frame3.png",&matrial, nullptr);
	m_boardFrameAtribs[16] = assetManger.getAttributeID("data\\textures\\board\\frame2.png",&matrial, nullptr);
	m_boardFrameAtribs[17] = assetManger.getAttributeID("data\\textures\\board\\frame1.png",&matrial, nullptr);


	//m_boardFrameAttrib = assetManger.getAttributeID("white.jpg", &matrial, NULL);

	//---------------------------------------------------------
	// Create a Square mesh for highlighting selected squares
	//---------------------------------------------------------
	//creating a squre mesh
	pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	VertexCount = 0;
	Tu = 0.0f;
	Tv = 0.0f;

	pVertices = new CMyVertex[4];

	if (!pVertices)
		return E_OUTOFMEMORY;

	//---------------------------------------------------------
	// Create square Vertices
	//---------------------------------------------------------
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
			pos.x += m_stepX;
			VertexCount++;
		}

		Tv++;
		// Increment Z
		pos.z += m_stepZ;
	}

	//---------------------------------------------------------
	// Create square indexes
	//---------------------------------------------------------
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

	//---------------------------------------------------------
	// build square mesh and attributes
	//---------------------------------------------------------
	if ( m_squareMesh->AddVertex(4,pVertices) < 0)
		return E_OUTOFMEMORY;

	m_squareMesh->BuildMesh(D3DXMESH_MANAGED,pDevice);

	//OBJMATERIAL matrial;

	matrial = d3d::YELLOW_MTRL;
	m_attribIDs[SELECT] = assetManger.getAttributeID(NULL, &matrial, NULL);

	matrial = d3d::BLUE_MTRL;
	m_attribIDs[MOVE] = assetManger.getAttributeID(NULL,&matrial,NULL);

// 	matrial = d3d::YELLOW_MTRL;
// 	m_attribIDs[SELECT] = assetManger.getAttributeID(NULL, &matrial, NULL);

	matrial = d3d::RED_MTRL;
	m_attribIDs[ATTACK] = assetManger.getAttributeID(NULL,&matrial,NULL);
	m_attribIDs[THREAT] = assetManger.getAttributeID(nullptr, &matrial, nullptr);

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
	m_attribIDs[FRAME] = assetManger.getAttributeID("data/textures/board/bla.png",&matrial,NULL);

	if (pVertices)
		delete []pVertices;

	if (pIndices)
		delete []pIndices;

	return hRet;	
	//AddFace(numCellsHigh * numCellsWide * 2,indices,?);

	
}

//-----------------------------------------------------------------------------
// Name : drawSubset ()
//-----------------------------------------------------------------------------
void CTerrain::drawSubset(IDirect3DDevice9* pd3dDevice, ULONG AttributeID, ID3DXEffect * effect, UINT numPass, D3DXMATRIX ViewProj)
{
	HRESULT hr;

	CMyObject::drawSubset(pd3dDevice, AttributeID, effect , numPass, ViewProj);

	for (UINT i = 0; i < 18; i++)
	{
		if (AttributeID == m_boardFrameAtribs[i])
		{
			HRESULT hRet;

			hRet = effect->SetMatrix(m_matWorldViewProjH, &(getWorldMatrix() * ViewProj) );
			hRet = effect->SetMatrix(m_matWorldH, &getWorldMatrix());

			hRet = effect->BeginPass(numPass);

			m_boardFrameMesh->DrawSubset(i);
			//m_pMesh->DrawSubset(0);

			effect->EndPass();
		}
	}

// 	if (AttributeID == m_boardFrameAttrib)
// 	{
// 		HRESULT hRet;
// 
// 		hRet = effect->SetMatrix(m_matWorldViewProjH, &(getWorldMatrix() * ViewProj) );
// 		hRet = effect->SetMatrix(m_matWorldH, &getWorldMatrix());
// 
// 		hRet = effect->BeginPass(numPass);
// 
// 		m_boardFrameMesh->DrawSubset(0);
// 		//m_pMesh->DrawSubset(0);
// 
// 		effect->EndPass();
// 
// 	}

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

	if (m_attribIDs[THREAT] == AttributeID && m_threatSquare != -1)
	{
		drawSquare(m_threatSquare, effect, numPass, ViewProj);
	}

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, (DWORD)0.0f);
	//pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, (DWORD)0.0f);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//-----------------------------------------------------------------------------
// Name : drawSquare ()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Name : setFrameSqaure ()
//-----------------------------------------------------------------------------
void CTerrain::setFrameSqaure(DWORD faceCount)
{
	ULONG sqaure = faceCount /2;
	if (sqaure < 64)
		m_fraemSqaure = sqaure;
}