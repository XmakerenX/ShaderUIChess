#include "CMyMesh.h"
#include "..\CAssetManager.h"

//-----------------------------------------------------------------------------
// Name : CMyMesh (constructor)
//-----------------------------------------------------------------------------
CMyMesh::CMyMesh(void)
{
	m_pMesh = NULL;
	m_pAdjacency = NULL;
	m_pAttribData = NULL;
	m_pAttribute  = NULL;
	m_pIndex      = NULL;
	m_pVertex     = NULL;
	pAttribRemap  = NULL;
	m_pAttribMap  = NULL;

    m_nVertexFVF = 0;
	m_nVertexStride = 0;                // Stride of the vertices
	m_nIndexStride = 0;                 // Stride of the indices
	m_nVertexCount = 0;                 // Number of vertices to use during BuildMesh
	m_nFaceCount = 0;                   // Number of faces to use during BuildMesh
	m_nVertexCapacity = 0;              // We are currently capable of holding this many before a grow
	m_nFaceCapacity = 0;                // We are currently capable of holding this many before a grow
	m_nAttribCount  = 0;

	ZeroMemory( m_strMeshName, MAX_PATH * sizeof(char));
}

//-----------------------------------------------------------------------------
// Name : CMyMesh (destructor)
//-----------------------------------------------------------------------------
CMyMesh::~CMyMesh(void)
{
	Release();
}

//-----------------------------------------------------------------------------
// Name : LoadMeshFromX ()
// Desc : loads a mesh from an xfile
//-----------------------------------------------------------------------------
HRESULT CMyMesh::LoadMeshFromX(LPCSTR pFileName, DWORD Options, LPDIRECT3DDEVICE9 pD3D, CAssetManager* assetManger)
{
	HRESULT      hRet;
	LPD3DXBUFFER pMatBuffer;            // Stores D3DXMATERIAL data (including texture filename)
	LPD3DXBUFFER pEffectBuffer;         // Stores effect file instances.
	ULONG        numMaterials,AttribID;
	bool		 ManageAttribs = false , RemapAttribs = false;


	// Validate parameters
	if ( !pFileName || !pD3D ) return D3DERR_INVALIDCALL;

	Release();

	hRet = D3DXLoadMeshFromX( pFileName, Options, pD3D, &m_pAdjacency, &pMatBuffer, &pEffectBuffer, &numMaterials, &m_pMesh );
	if(FAILED(hRet))	return hRet;

	ManageAttribs = m_attributesCallBack.pFunction == NULL;
	//TODO: add option to store the attribute data in the scene itself for faster rendering
	//m_nAttribCount = numMaterials;//saving how long is the attribute array

	if (ManageAttribs && numMaterials > 0)
	{
		m_nAttribCount = numMaterials;//saving how long is the attribute array
		m_pAttribData = new MESH_ATTRIB_DATA[ numMaterials ];
		if ( !m_pAttribData ) return E_OUTOFMEMORY;
		ZeroMemory( m_pAttribData, m_nAttribCount * sizeof(MESH_ATTRIB_DATA));
	}
	else
	{
		// Allocate attribute remap array
		pAttribRemap = new ULONG[ numMaterials ];
		if ( !pAttribRemap ) return E_OUTOFMEMORY;

		// Default remap to their initial values.
		for ( UINT i = 0; i < numMaterials; ++i ) pAttribRemap[ i ] = i;

		m_pAttribMap = new ULONG[numMaterials];
		if (!m_pAttribMap)
		{
			delete []pAttribRemap;
			return E_OUTOFMEMORY;
		}
		m_nAttribCount = numMaterials;
		//ZeroMemory(pAttribMap,sizeof(ULONG) * numMaterials);
	}
	//if (pMatBuffer != NULL && numMaterials > 0)
	//{
		// Retrieve data pointers
	D3DXMATERIAL* pMaterials = (D3DXMATERIAL*)pMatBuffer->GetBufferPointer();
	D3DXEFFECTINSTANCE * pEffects   = (D3DXEFFECTINSTANCE*)pEffectBuffer->GetBufferPointer();

	for (UINT i=0; i<numMaterials; i++)
	{
		if (ManageAttribs)
		{
			m_pAttribData[i].Material = pMaterials[i].MatD3D;
			// Note : The X File specification contains no ambient material property.
			//        We should ideally set this to full intensity to allow us to 
			//        control ambient brightness via the D3DRS_AMBIENT renderstate.
			m_pAttribData[i].Material.Ambient = D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );

			if (pMaterials[i].pTextureFilename)
			{
				m_pAttribData[i].Texture = assetManger->getTexture(pMaterials[i].pTextureFilename);
				if (m_pAttribData[i].Effect)
					m_pAttribData[i].Effect->AddRef();
// 				//m_pAttribData[i].Texture =gameScene.getTexture(pMaterials[i]->pTextureFilename);
// 				GETTEXTURE GetTexture = (GETTEXTURE)m_TextureCallBack.pFunction;
// 				m_pAttribData[i].Texture = GetTexture( m_TextureCallBack.pContext, pMaterials[i].pTextureFilename );
// 				if ( m_pAttribData[i].Effect ) 
// 					m_pAttribData[i].Effect->AddRef();
			}
		}
		else
		{
			// Request attribute ID via callback
			if ( m_attributesCallBack.pFunction )
			{
				OBJMATERIAL curMatrial;
				curMatrial = pMaterials[i].MatD3D;
				//pMaterials[i].MatD3D = d3d::WHITE_MTRL;
				//pMaterials[i].MatD3D.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
				curMatrial.Ambient = D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );

				AttribID = assetManger->getAttributeID(pMaterials->pTextureFilename, &curMatrial, &pEffects[i]);

// 				GETATTRIBUTEID CollectAttributeID = (GETATTRIBUTEID)m_attributesCallBack.pFunction;
// 				AttribID = CollectAttributeID( m_attributesCallBack.pContext, pMaterials[i].pTextureFilename,
// 					&curMatrial, &pEffects[i] );

				// Store this in our attribute remap table
				pAttribRemap[i] = AttribID;
				m_pAttribMap[i] = AttribID;

				// Determine if any changes are required so far
				//if ( AttribID != i ) RemapAttribs = true;

			} // End if callback available
		}
	}
	//}

		// Clean up buffers
	if ( pMatBuffer    ) pMatBuffer->Release();
	if ( pEffectBuffer ) pEffectBuffer->Release();

	// Remap attributes if required
	if ( pAttribRemap != NULL && RemapAttribs == true )
	{
		ULONG * pAttributes = NULL;

		// Lock the attribute buffer
		hRet = m_pMesh->LockAttributeBuffer( 0, &pAttributes );
		if ( FAILED(hRet) ) 
		{ 
			delete []pAttribRemap;
			return hRet; 
		}

		// Loop through all faces
		for (UINT i = 0; i < m_pMesh->GetNumFaces(); ++i )
		{
			// Retrieve the current attribute ID for this face
			AttribID = pAttributes[i];

			// Replace it with the remap value
			pAttributes[i] = pAttribRemap[AttribID];

		} // Next Face

		// Finish up
		m_pMesh->UnlockAttributeBuffer( );

	} // End if remap attributes

	// Release remap data
	if ( pAttribRemap ) delete []pAttribRemap;

	// Copy the filename over
	//strcpy( m_strMeshName, pFileName );
	strcpy_s(m_strMeshName,MAX_PATH,pFileName);

	// Success!!
	return S_OK;
}

//TODO: rewrite the function to suit the new way of doing stuff or delete it 
//-----------------------------------------------------------------------------
// Name : Draw ()
// Desc : draw the mesh independent of the attribute batching 
// Note : seems to be obsolete but kept till I will revise or delete it
//-----------------------------------------------------------------------------
void CMyMesh::Draw( )
{
	UINT              PassCount  = 0;
	LPDIRECT3DDEVICE9 pD3DDevice = NULL;
	LPD3DXEFFECT      pEffect    = NULL;

	// This function is invalid if there is no managed data
	if ( !m_pAttribData ) return;

	// Retrieve the Direct3D device
	m_pMesh->GetDevice( &pD3DDevice );

	// Render the subsets
	for ( ULONG i = 0; i < m_nAttribCount; ++i )
	{
		// Set the attribute data
		pD3DDevice->SetFVF( GetFVF() );
		//pD3DDevice->SetMaterial( &m_pAttribData[i].Material );
		pD3DDevice->SetTexture( 0, m_pAttribData[i].Texture );
		pEffect = m_pAttribData[i].Effect;

		// Determine method of rendering
		if ( pEffect != NULL )
		{
			// Note : The valid technique we wish to use must already 
			//        as active, usually by the CollectEffect callback.

			// Begin. Records device and shader states, and retrieves the passcount
			if ( FAILED( pEffect->Begin( &PassCount, 0 ) ) ) continue;

			// Loop through each effect file pass
			for ( ULONG j = 0; j < PassCount; ++j )
			{
				// Set the states
				if ( FAILED( pEffect->BeginPass( j ) ) ) continue;

				// Draw this subset.
				m_pMesh->DrawSubset( i );

				// End the effect pass
				pEffect->EndPass();

			} // Next Pass

			// End. Restores device and shader states.
			pEffect->End( );

		} // End if effect provided
		else
		{
			// Otherwise simply render the subset(s)
			m_pMesh->DrawSubset( i );

		} // End if no effect provided

	} // Next attribute

	// Release the device
	pD3DDevice->Release();
}

//-----------------------------------------------------------------------------
// Name : DrawSubset ()
// Desc : draws the the selected subset of the mesh 
//-----------------------------------------------------------------------------
void CMyMesh::DrawSubset( ULONG AttributeID )
{
	UINT              PassCount  = 0;
	LPDIRECT3DDEVICE9 pD3DDevice = NULL;
	LPD3DXEFFECT      pEffect    = NULL;

	// Set the attribute data
	if ( m_pAttribData && AttributeID < m_nAttribCount )
	{
		// Retrieve the Direct3D device
		m_pMesh->GetDevice( &pD3DDevice );

		//pD3DDevice->SetMaterial( &m_pAttribData[AttributeID].Material );
		pD3DDevice->SetTexture( 0, m_pAttribData[AttributeID].Texture );
		pEffect = m_pAttribData[AttributeID].Effect;

		// Release the device
		pD3DDevice->Release();

	} // End if attribute data is managed

	// Determine method of rendering
	if ( pEffect != NULL )
	{
		// Note : The valid technique we wish to use must already 
		//        as active, usually by the CollectEffect callback.

		// Begin. Records device and shader states, and retrieves the passcount
		if ( FAILED( pEffect->Begin( &PassCount, 0 ) ) ) return;

		// Loop through each effect file pass
		for ( ULONG j = 0; j < PassCount; ++j )
		{
			// Set the states
			if ( FAILED( pEffect->BeginPass( j ) ) ) continue;

			// Draw this subset.
			m_pMesh->DrawSubset( AttributeID );

			// End the pass
			pEffect->EndPass( );

		} // Next Pass

		// End. Restores device and shader states.
		pEffect->End( );

	} // End if effect provided
	else
	{
		HRESULT hRet;
		// Otherwise simply render the subset(s)
		hRet = m_pMesh->DrawSubset( AttributeID );
		int x;
		x =0;
	} // End if no effect provided
}

//-----------------------------------------------------------------------------
// Name : GetMesh ()
// Desc : returns the pointer to the directx mesh class
//-----------------------------------------------------------------------------
LPD3DXMESH CMyMesh::GetMesh( ) const
{

	if ( !m_pMesh) return NULL;

	// Add a reference to the mesh so don't forget to release it!!!
	m_pMesh->AddRef();
	return m_pMesh;
}

//-----------------------------------------------------------------------------
// Name : GetMeshName ()
// Desc : Retrieve the filename used to load / reference the mesh.
//-----------------------------------------------------------------------------
LPCTSTR CMyMesh::GetMeshName( ) const
{
	return m_strMeshName;
}

//-----------------------------------------------------------------------------
// Name : GetFVF()
// Desc : Retrieves the FVF code used by the mesh, or optionally the
//        FVF of the vertices stored ready for building.
//-----------------------------------------------------------------------------
ULONG CMyMesh::GetFVF( ) const
{
	// Validation!!
	if ( !m_pMesh )
		return m_nVertexFVF; 
	else 
		return m_pMesh->GetFVF();
}

//-----------------------------------------------------------------------------
// Name : setMesh()
// Desc : Receives an already created mesh and wrap in this class 
//-----------------------------------------------------------------------------
void CMyMesh::setMesh(LPD3DXMESH pNewMesh,char  newStrMeshName[MAX_PATH])
{
	if (m_pMesh != NULL)
		return;
	m_pMesh = pNewMesh;
	m_pMesh->AddRef();
	//strcpy(m_strMeshName,newStrMeshName);
	strcpy_s(m_strMeshName,MAX_PATH,newStrMeshName);
	m_nVertexFVF = m_pMesh->GetFVF();
}

//-----------------------------------------------------------------------------
// Name : Release ()
// Desc : Release any and all objects, data etc.
//-----------------------------------------------------------------------------
int CMyMesh::Release( )
{
	int num = -20;
	// Release objects
	if ( m_pAdjacency ) m_pAdjacency->Release();
	if ( m_pMesh      ) num = m_pMesh->Release();

	// Release attribute data.
	if ( m_pAttribData )
	{
		for ( ULONG i = 0; i < m_nAttribCount; i++ )
		{
			// First release the texture object (addref was called earlier)
			if ( m_pAttribData[i].Texture ) m_pAttribData[i].Texture->Release();

			// And also the effect object
			if ( m_pAttribData[i].Effect  ) m_pAttribData[i].Effect->Release();

		} // Next Subset

		delete []m_pAttribData;

	} // End if subset data exists

	if (m_pAttribute)
		delete []m_pAttribute;
	
	if (m_pIndex)
		delete []m_pIndex;

	if (m_pVertex)
		delete []m_pVertex;

	if (m_pAttribMap)
		delete []m_pAttribMap; 

	// Clear out variables
	m_pAdjacency      = NULL;
	m_pMesh           = NULL;
	m_pAttribData     = NULL;
	m_pAttribute      = NULL;
	m_pIndex		  = NULL;
	m_pVertex		  = NULL;

	m_nVertexFVF = 0;
	m_nVertexStride = 0;                // Stride of the vertices
	m_nIndexStride = 0;                 // Stride of the indices
	m_nVertexCount = 0;                 // Number of vertices to use during BuildMesh
	m_nFaceCount = 0;                   // Number of faces to use during BuildMesh
	m_nVertexCapacity = 0;              // We are currently capable of holding this many before a grow
	m_nFaceCapacity = 0;                // We are currently capable of holding this many before a grow
	m_nAttribCount  = 0;


	ZeroMemory( m_strMeshName, MAX_PATH * sizeof(TCHAR));

	return num;

}

//-----------------------------------------------------------------------------
// Name : RegisterTextureCallback () 
// Desc : Registers a callback function for one of the callback types.
//-----------------------------------------------------------------------------
bool CMyMesh::RegisterTextureCallback(LPVOID pFunction, LPVOID pContext )
{
	// You cannot set the functions to anything other than NULL
	// if mesh data already exists (i.e. it's too late to change your mind :)
	if ( pFunction != NULL && m_pAttribData ) return false;

	// Store function pointer and context
	m_TextureCallBack.pFunction = pFunction;
	m_TextureCallBack.pContext  = pContext;

	// Success!!
	return true;
}

//-----------------------------------------------------------------------------
// Name : RegisterAttributesCallback () 
// Desc : Registers a callback function for one of the callback types.
//-----------------------------------------------------------------------------
bool CMyMesh::RegisterAttributesCallback(LPVOID pFunction, LPVOID pContext )
{
	// You cannot set the functions to anything other than NULL
	// if mesh data already exists (i.e. it's too late to change your mind :)
	if ( pFunction != NULL && m_pAttribData ) return false;

	// Store function pointer and context
	m_attributesCallBack.pFunction = pFunction;
	m_attributesCallBack.pContext  = pContext;

	// Success!!
	return true;
}


//-----------------------------------------------------------------------------
// Name : SetDataFormat ()
// Desc : Inform the mesh class about the size of the vertices, and indices
//        about to be fed in.
//-----------------------------------------------------------------------------
void CMyMesh::SetDataFormat( ULONG VertexFVF, ULONG IndexStride )
{
	// Store the values
	m_nVertexFVF    = VertexFVF;
	m_nVertexStride = D3DXGetFVFVertexSize( VertexFVF );
	m_nIndexStride  = IndexStride;
}

//-----------------------------------------------------------------------------
// Name : AddVertex()
// Desc : Adds a vertex, or multiple vertices, to this mesh.
// Note : Returns the index for the first vertex added, or -1 on failure.
//-----------------------------------------------------------------------------
long CMyMesh::AddVertex( ULONG Count /* = 1 */, LPVOID pVertices /* = NULL */  )
{
	UCHAR * pVertexBuffer = NULL;

	if ( m_nVertexCount + Count > m_nVertexCapacity )
	{
		// Adjust our vertex capacity (resize 100 at a time)
		for ( ; m_nVertexCapacity < (m_nVertexCount + Count) ; ) m_nVertexCapacity += 100;

		// Allocate new resized array
		if (!( pVertexBuffer = new UCHAR[ (m_nVertexCapacity) * m_nVertexStride ] )) return -1;

		// Existing Data?
		if ( m_pVertex )
		{
			// Copy old data into new buffer
			memcpy( pVertexBuffer, m_pVertex, m_nVertexCount * m_nVertexStride );

			// Release old buffer
			delete []m_pVertex;

		} // End if

		// Store pointer for new buffer
		m_pVertex = pVertexBuffer;

	} // End if a resize is required

	// Copy over vertex data if provided
	if ( pVertices ) memcpy( &m_pVertex[m_nVertexCount * m_nVertexStride], pVertices, Count * m_nVertexStride );

	// Increase Vertex Count
	m_nVertexCount += Count;

	// Return first vertex
	return m_nVertexCount - Count;
}

//-----------------------------------------------------------------------------
// Name : AddFace()
// Desc : Adds a face, or multiple faces, to this mesh.
// Note : Returns the index for the first face added, or -1 on failure.
//-----------------------------------------------------------------------------
long CMyMesh::AddFace( ULONG Count /* = 1 */, LPVOID pIndices /* = NULL */, ULONG AttribID /* = 0 */ )
{
	UCHAR * pIndexBuffer = NULL;
	ULONG * pAttributeBuffer = NULL;

	if ( m_nFaceCount + Count > m_nFaceCapacity )
	{
		// Adjust our face capacity (resize 100 at a time)
		for ( ; m_nFaceCapacity < (m_nFaceCount + Count) ; ) m_nFaceCapacity += 100;

		// Allocate new resized array
		if (!( pIndexBuffer = new UCHAR[ (m_nFaceCapacity * 3) * m_nIndexStride ] )) return -1;

		// Existing Data?
		if ( m_pIndex )
		{
			// Copy old data into new buffer
			memcpy( pIndexBuffer, m_pIndex, (m_nFaceCount * 3) * m_nIndexStride );

			// Release old buffer
			delete []m_pIndex;

		} // End if

		// Store pointer for new buffer
		m_pIndex = pIndexBuffer;

		// Allocate new resized attribute array
		if (!( pAttributeBuffer = new ULONG[ m_nFaceCapacity ] )) return -1;

		// Existing Data?
		if ( m_pAttribute )
		{
			// Copy old data into new buffer
			memcpy( pAttributeBuffer, m_pAttribute, m_nFaceCount * sizeof(ULONG) );

			// Release old buffer
			delete []m_pAttribute;

		} // End if

		// Store pointer for new buffer
		m_pAttribute = pAttributeBuffer;

	} // End if a resize is required

	// Copy over index and attribute data if provided
	if ( pIndices ) memcpy( &m_pIndex[(m_nFaceCount * 3) * m_nIndexStride], pIndices, (Count * 3) * m_nIndexStride );
	for ( ULONG i = m_nFaceCount; i < m_nFaceCount + Count; ++i) m_pAttribute[i] = AttribID;

	// Increase Face Count
	m_nFaceCount += Count;

	// Return first face
	return m_nFaceCount - Count;
}

//-----------------------------------------------------------------------------
// Name : AddAttributeData()
// Desc : Adds an attribute, or multiple attributes, to this mesh.
// Note : Returns the index for the attribute added, or -1 on failure.
//-----------------------------------------------------------------------------
long CMyMesh::AddAttributeData( ULONG Count /* = 1 */ )
{
	MESH_ATTRIB_DATA * pAttribBuffer = NULL;

	// Allocate new resized array
	if (!( pAttribBuffer = new MESH_ATTRIB_DATA[ m_nAttribCount + Count ] )) return -1;

	// Existing Data?
	if ( m_pAttribData )
	{
		// Copy old data into new buffer
		memcpy( pAttribBuffer, m_pAttribData, m_nAttribCount * sizeof(MESH_ATTRIB_DATA) );

		// Release old buffer
		delete []m_pAttribData;

	} // End if

	// Store pointer for new buffer
	m_pAttribData = pAttribBuffer;

	// Clear the new items
	ZeroMemory( &m_pAttribData[m_nAttribCount], Count * sizeof(MESH_ATTRIB_DATA) );

	// Increase Attrib Count
	m_nAttribCount += Count;

	// Return first Attrib
	return m_nAttribCount - Count;
}

//-----------------------------------------------------------------------------
// Name : BuildMesh()
// Desc : Assuming mesh data has been entered, this function will build the
//        internal mesh, and release that original data.
//-----------------------------------------------------------------------------
HRESULT CMyMesh::BuildMesh( ULONG Options, LPDIRECT3DDEVICE9 pDevice, bool ReleaseOriginals /* = true */ )
{
	HRESULT hRet;
	LPVOID  pVertices   = NULL;
	LPVOID  pIndices    = NULL;
	ULONG  *pAttributes = NULL;

	// Validate Requirements
	if ( !m_pVertex || !m_pIndex || !m_pAttribute || !pDevice ) return D3DERR_INVALIDCALL;

	// First release the original mesh if one exists
	if ( m_pMesh ) { m_pMesh->Release(); m_pMesh = NULL; }

	// Force 32 bit mesh if required
	if ( m_nIndexStride == 4 ) Options |= D3DXMESH_32BIT;

	// Create the blank empty mesh
	hRet = D3DXCreateMeshFVF( m_nFaceCount, m_nVertexCount, Options, VERTEX_FVF, pDevice, &m_pMesh );
	if ( FAILED(hRet) ) return hRet;

	// Lock the vertex buffer and copy the data
	hRet = m_pMesh->LockVertexBuffer( 0, &pVertices );
	if ( FAILED(hRet) ) return hRet;
	memcpy( pVertices, m_pVertex, m_nVertexCount * m_nVertexStride );
	m_pMesh->UnlockVertexBuffer();

	// Lock the index buffer and copy the data
	hRet = m_pMesh->LockIndexBuffer( 0, &pIndices );
	if ( FAILED(hRet) ) return hRet;
	memcpy( pIndices, m_pIndex, (m_nFaceCount * 3) * m_nIndexStride );
	m_pMesh->UnlockIndexBuffer();

	// Lock the attribute buffer and copy the data
	hRet = m_pMesh->LockAttributeBuffer( 0, &pAttributes );
	if ( FAILED(hRet) ) return hRet;
	memcpy( pAttributes, m_pAttribute, m_nFaceCount * sizeof(ULONG) );
	m_pMesh->UnlockAttributeBuffer();

	// Release the original data if requested
	if ( ReleaseOriginals )
	{
		if ( m_pVertex    ) delete []m_pVertex; 
		if ( m_pIndex     ) delete []m_pIndex;
		if ( m_pAttribute ) delete []m_pAttribute;

		m_nVertexCount    = 0;
		m_nFaceCount      = 0;
		m_pAttribute      = NULL;
		m_pIndex          = NULL;
		m_pVertex         = NULL;
		m_nVertexCapacity = 0;
		m_nFaceCapacity   = 0;

	} // End if release

	// We're done :)
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : GetAttributeData()
// Desc : Retrieves the attribute data array.
//-----------------------------------------------------------------------------
MESH_ATTRIB_DATA * CMyMesh::GetAttributeData( ) const
{
	return m_pAttribData;
}

HRESULT CMyMesh::initAttributeRemap()
{
	// Allocate attribute remap array
	pAttribRemap = new ULONG[ m_nAttribCount ];
	if ( !pAttribRemap ) return E_OUTOFMEMORY;

	// Default remap to their initial values.
	for ( UINT i = 0; i < m_nAttribCount; ++i ) pAttribRemap[ i ] = i;

	return S_OK;
}

HRESULT CMyMesh::remapAttributes()
{
	// Remap attributes if required
	HRESULT hRet;
	ULONG AttribID;

	if ( pAttribRemap != NULL)
	{
		ULONG * pAttributes = NULL;

		// Lock the attribute buffer
		hRet = m_pMesh->LockAttributeBuffer( 0, &pAttributes );
		if ( FAILED(hRet) ) 
		{ 
			delete []pAttribRemap;
			return hRet; 
		}

		// Loop through all faces
		for (UINT  i = 0; i < m_pMesh->GetNumFaces(); ++i )
		{
			// Retrieve the current attribute ID for this face
			AttribID = pAttributes[i];

			// Replace it with the remap value
			pAttributes[i] = pAttribRemap[AttribID];

		} // Next Face

		// Finish up
		m_pMesh->UnlockAttributeBuffer( );

	} // End if remap attributes
	else
		return S_FALSE;

	// Release remap data
	if ( pAttribRemap ) delete []pAttribRemap;

	return S_OK;
}

ULONG * CMyMesh::getAttributeRemap( ) const
{
	return pAttribRemap;
}

ULONG *	CMyMesh::getAtrributeMap( ) const
{
	return m_pAttribMap;
}

ULONG CMyMesh::getAttributeCount( ) const
{
	return m_nAttribCount;
}

void CMyMesh::setAttribMap( ULONG * pAttribmap, ULONG attribCount)
{
	if (m_pAttribMap)
		delete []m_pAttribMap;

	m_pAttribMap = new ULONG[attribCount];
	memcpy(m_pAttribMap,pAttribmap,sizeof(ULONG) * attribCount);
	m_nAttribCount = attribCount;
}

bool CMyMesh::isMenageAtrributes()
{
	return m_attributesCallBack.pFunction == NULL;
}

//-----------------------------------------------------------------------------
// Name : GenerateAdjacency ()
// Desc : Generate the adjacency information for this mesh.
//-----------------------------------------------------------------------------
HRESULT CMyMesh::GenerateAdjacency( float Epsilon /* = 1e-3f */ )
{
	HRESULT hRet;

	// Validate Requirements
	if ( !m_pMesh ) return D3DERR_INVALIDCALL;

	// Clear out any old adjacency information
	if (m_pAdjacency) m_pAdjacency->Release();

	// Create the new adjacency buffer
	hRet = D3DXCreateBuffer( GetNumFaces() * (3 * sizeof(DWORD)), &m_pAdjacency );
	if ( FAILED(hRet) ) return hRet;

	// Generate the new adjacency information
	hRet = m_pMesh->GenerateAdjacency( Epsilon, (DWORD*)m_pAdjacency->GetBufferPointer() );
	if ( FAILED(hRet) ) return hRet;

	// Success !!
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : GetNumFaces()
// Desc : Retrieves the number of faces in the mesh, or optionally the
//        number of faces vertices stored ready for building.
//-----------------------------------------------------------------------------
ULONG CMyMesh::GetNumFaces( ) const
{
	// Validation!!
	if ( !m_pMesh )
		return m_nFaceCount; 
	else 
		return m_pMesh->GetNumFaces();
}

//-----------------------------------------------------------------------------
// Name : GetAdjacencyBuffer()
// Desc : Retrieves the underlying adjacency buffer object
//-----------------------------------------------------------------------------
LPD3DXBUFFER CMyMesh::GetAdjacencyBuffer( ) const
{
	if ( !m_pAdjacency ) return NULL;

	// Add a reference to the buffer so don't forget to release it!!!
	//m_pAdjacency->AddRef();
	return m_pAdjacency;
}

//-----------------------------------------------------------------------------
// Name : Optimize ()
// Desc : Allows us to optimize the mesh in several ways.
//-----------------------------------------------------------------------------
HRESULT CMyMesh::Optimize( ULONG Flags, CMyMesh * pMeshOut, LPD3DXBUFFER * ppFaceRemap /* = NULL */, LPD3DXBUFFER * ppVertexRemap /* = NULL */, LPDIRECT3DDEVICE9 pD3DDevice /* = NULL */ )
{
	HRESULT             hRet;
	LPD3DXMESH          pOptimizeMesh = NULL;
	LPD3DXBUFFER        pFaceRemapBuffer = NULL;
	LPD3DXBUFFER        pAdjacency = NULL;
	ULONG              *pData      = NULL;

	// Validate requirements
	if ( !m_pMesh || !pMeshOut ) return D3DERR_INVALIDCALL;

	// Generate adjacency if not yet available
	if (!m_pAdjacency)
	{
		hRet = GenerateAdjacency();
		if ( FAILED(hRet) ) return hRet;

	} // End if no adjacency

	// If no new device was passed... 
	if ( !pD3DDevice )
	{
		// we'll use the same device as this mesh
		// This automatically calls 'AddRef' for the device
		m_pMesh->GetDevice( &pD3DDevice );

	} // End if no new device
	else
	{
		// Otherwise we'll add a reference here so that we can
		// release later for both cases without doing damage :)
		pD3DDevice->AddRef();

	} // End if new device

	// Allocate new adjacency output buffer
	hRet = D3DXCreateBuffer( (3 * GetNumFaces()) * sizeof(ULONG), &pAdjacency );
	if ( FAILED(hRet) ) { pD3DDevice->Release(); return hRet; }

	// Allocate the output face remap if requested
	if ( ppFaceRemap )
	{
		// Allocate new face remap output buffer
		hRet = D3DXCreateBuffer( GetNumFaces() * sizeof(ULONG), ppFaceRemap );
		if ( FAILED(hRet) ) { pD3DDevice->Release(); pAdjacency->Release(); return hRet; }
		pData = (ULONG*)(*ppFaceRemap)->GetBufferPointer();

	} // End if allocate face remap data

	// Attempt to optimize the mesh
	hRet = m_pMesh->Optimize( Flags, (ULONG*)m_pAdjacency->GetBufferPointer(), (ULONG*)pAdjacency->GetBufferPointer(),
		pData, ppVertexRemap, &pOptimizeMesh );
	if ( FAILED(hRet) ) { pD3DDevice->Release(); return hRet; }

	// Attach this D3DX mesh to the output mesh
	// This automatically adds a reference to the mesh passed in.
	pMeshOut->Attach( pOptimizeMesh, pAdjacency );

	// We can now release our copy of the optimized mesh and the adjacency buffer
	pOptimizeMesh->Release();
	pAdjacency->Release();

	// Copy over attributes if there is anything here
	if ( m_pAttribData )
	{
		// Add the correct number of attributes
		if ( pMeshOut->AddAttributeData( m_nAttribCount ) < 0 ) return E_OUTOFMEMORY;

		// Copy over attribute data
		MESH_ATTRIB_DATA * pAttributes = pMeshOut->GetAttributeData();
		for ( ULONG i = 0; i < m_nAttribCount; ++i )
		{
			MESH_ATTRIB_DATA * pAttrib = &pAttributes[i];
			// Store details
			pAttrib->Material = m_pAttribData[i].Material;
			pAttrib->Texture  = m_pAttribData[i].Texture;
			pAttrib->Effect   = m_pAttribData[i].Effect;

			// Add references so that objects aren't released when either of these
			// meshes are released, or vice versa.
			if ( pAttrib->Texture ) pAttrib->Texture->AddRef();
			if ( pAttrib->Effect  ) pAttrib->Effect->AddRef();

		} // Next Attribute

	} // End if managed

	// Release our referenced D3D Device
	if (pD3DDevice) pD3DDevice->Release();

	// Success!!
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : OptimizeInPlace ()
// Desc : Allows us to optimize the mesh in several ways.
//-----------------------------------------------------------------------------
HRESULT CMyMesh::OptimizeInPlace( DWORD Flags, LPD3DXBUFFER * ppFaceRemap /* = NULL */, LPD3DXBUFFER * ppVertexRemap /* = NULL */ )
{
	HRESULT      hRet;
	LPD3DXBUFFER pFaceRemapBuffer = NULL;
	ULONG       *pData = NULL;

	// Validate Requirements
	if ( !m_pMesh ) return D3DERR_INVALIDCALL;

	// Generate adjacency if none yet provided
	if (!m_pAdjacency)
	{
		hRet = GenerateAdjacency();
		if ( FAILED(hRet) ) return hRet;

	} // End if no adjacency

	// Allocate the output face remap if requested
	if ( ppFaceRemap )
	{
		// Allocate new face remap output buffer
		hRet = D3DXCreateBuffer( GetNumFaces() * sizeof(ULONG), ppFaceRemap );
		if ( FAILED(hRet) ) { return hRet; }
		pData = (ULONG*)(*ppFaceRemap)->GetBufferPointer();

	} // End if allocate face remap data

	// Optimize the data
	hRet = m_pMesh->OptimizeInplace( Flags, (DWORD*)m_pAdjacency->GetBufferPointer(), (DWORD*)m_pAdjacency->GetBufferPointer(), pData, ppVertexRemap );
	if ( FAILED( hRet ) ) return hRet;

	// Adjacency information altered, regenerate it.
	hRet = GenerateAdjacency();
	if ( FAILED(hRet) ) return hRet;

	// Success!!
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : Attach ()
// Desc : Attach an already created mesh type to this CTriMesh.
// Note : Because no attribute data is transferred, this will only really be
//        of any use for incidental processes, non managed rendering or in
//        cases where attribute data is manually transferred.
//-----------------------------------------------------------------------------
HRESULT CMyMesh::Attach( LPD3DXBASEMESH pMesh, LPD3DXBUFFER pAdjacency /* = NULL */ )
{
	HRESULT hRet;

	// Validate Requirements
	if ( !pMesh ) return D3DERR_INVALIDCALL;

	// Clear our current data
	Release();

	// Store this mesh (ensuring that it really is of the expected type)
	// This will automatically add a reference of the type required
	hRet = pMesh->QueryInterface( IID_ID3DXMesh, (void**)&m_pMesh );
	if ( FAILED(hRet) ) return hRet;

	// Calculate strides etc
	m_nVertexFVF    = m_pMesh->GetFVF();
	m_nVertexStride = m_pMesh->GetNumBytesPerVertex();
	m_nIndexStride  = (GetOptions() & D3DXMESH_32BIT) ? 4 : 2;

	// If adjacency information was passed, reference it
	// if none was passed, it will be generated later, if required.
	if ( pAdjacency )
	{
		m_pAdjacency = pAdjacency;
		m_pAdjacency->AddRef();

	} // End if adjacency was passed

	// Success!!
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : GetOptions()
// Desc : Retrieves the option set used by the mesh.
//-----------------------------------------------------------------------------
ULONG CMyMesh::GetOptions( ) const
{
	// Validation!!
	if ( !m_pMesh )
		return 0; 
	else 
		return m_pMesh->GetOptions();
}