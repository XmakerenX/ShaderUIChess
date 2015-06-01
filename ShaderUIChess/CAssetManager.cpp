#include "CAssetManager.h"
#include "rendering/CMyMesh.h"
#include "rendering/CMyObject.h"

//-----------------------------------------------------------------------------
// Name : CAssetManager (constructor)
// Desc : initialize all the data pools to zero and set the device
//        for this assetMnager if was one given 
//-----------------------------------------------------------------------------
CAssetManager::CAssetManager(IDirect3DDevice9* pD3DDevice/* = NULL*/)
{
	//setting data pools for attributes, objects and meshes
	m_nTextureCount = 0;
	m_nMaterialCount = 0;
	m_nAttribCount   = 0;

	m_pMesh			 = NULL;
	m_nMeshCount	 = 0;
	m_nAllocMesh     = 0;

	//set to texture format to unknown till one will be given to us
	m_fmtTexture = D3DFMT_UNKNOWN;

	m_pD3DDevice = pD3DDevice;
}

//-----------------------------------------------------------------------------
// Name : CAssetManager (destructor) 
// Desc : free all the memory that was allocated by the assetManger
//-----------------------------------------------------------------------------
CAssetManager::~CAssetManager(void)
{
	Release();
}

//-----------------------------------------------------------------------------
// Name : AllocMesh()
// Desc : allocate space for a new mesh , or multiple meshes , for this assetManger
// Note : Returns the index for the mesh added, or -1 on failure.
//-----------------------------------------------------------------------------
long CAssetManager::AllocMesh( ULONG Count /* = 1 */ )
{
	CMyMesh ** pMeshBuffer = NULL;

	// Allocate new resized array
	if (!( pMeshBuffer = new CMyMesh*[ m_nAllocMesh + Count ] )) return -1;

	// Existing Data?
	if ( m_pMesh )
	{
		// Copy old data into new buffer
		memcpy( pMeshBuffer, m_pMesh, m_nAllocMesh * sizeof(CMyMesh*) );

		// Release old buffer
		delete []m_pMesh;

	} // End if

	// Store pointer for new buffer
	m_pMesh = pMeshBuffer;

	// Clear the new items
	ZeroMemory( &m_pMesh[m_nAllocMesh], Count * sizeof(CMyMesh*) );

	// Increase Mesh Count
	m_nAllocMesh += Count;

	// Return first Mesh
	return m_nAllocMesh - Count;
}

//-----------------------------------------------------------------------------
// Name : AddMesh()
// Desc : create a mesh object in the data pool and give it default initialization 
//-----------------------------------------------------------------------------
HRESULT CAssetManager::AddMesh()
{
	long meshIndex;

	if (m_nAllocMesh == m_nMeshCount)
		meshIndex = AllocMesh();
	else
		meshIndex = m_nMeshCount;

	if (meshIndex == -1)//failed to alloc memory for another mesh
		return S_FALSE;

	m_pMesh[meshIndex] = new CMyMesh();
	m_pMesh[meshIndex]->SetDataFormat(VERTEX_FVF, sizeof(USHORT));

	m_nMeshCount++;
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : AddMesh()
// Desc : crate a mesh object and set it values based on the mesh file given
// Note : Default loading options are D3DXMESH_MANAGED
//-----------------------------------------------------------------------------
HRESULT CAssetManager::AddMesh(const char meshPath[MAX_PATH], DWORD meshOptions /*= D3DXMESH_MANAGED*/)
{
	HRESULT hr;
	long meshIndex;

	if (m_nAllocMesh == m_nMeshCount)
		meshIndex = AllocMesh();
	else
		meshIndex = m_nMeshCount;

	if (meshIndex == -1)//failed to alloc memory for another mesh
		return false;

	m_pMesh[meshIndex] = new CMyMesh();
	hr = m_pMesh[meshIndex]->LoadMeshFromX(meshPath, meshOptions, m_pD3DDevice, this);

	if (FAILED(hr))
		return hr;

	m_nMeshCount++;
	return S_OK;

}

//-----------------------------------------------------------------------------
// Name : AddMesh()
// Desc : crate a mesh object and set it values based on parameters given
//		  (allows to create mesh on runtime rather than load it from file)
// Note : a LPD3DXMESH needs to be created pre call to this function and to have  a valid mesh in it
//-----------------------------------------------------------------------------
HRESULT CAssetManager::AddMesh(MESH_ATTRIB_DATA AttribData[], ULONG attribCount, LPD3DXMESH pD3DMesh, char meshPath[MAX_PATH])
{
	long meshIndex;
	ULONG AttribID[MAX_ATTRIB_IDS];
	MESH_ATTRIB_DATA* pAttribData;

	// check if there is enough allocated space if not allocate more space
	if (m_nAllocMesh == m_nMeshCount)
		meshIndex = AllocMesh();
	else
		meshIndex = m_nMeshCount;

	//check if failed to alloc memory for another mesh
	if (meshIndex == -1)
		return S_FALSE;

	//check that parameters are valid
	if (AttribData == NULL || attribCount <= 0 || pD3DMesh == NULL)
		return S_FALSE;


	//create new mesh using the parameters given
	m_pMesh[meshIndex] = new CMyMesh();
	m_pMesh[meshIndex]->SetDataFormat(VERTEX_FVF, sizeof(USHORT));

	if (m_pMesh[meshIndex]->AddAttributeData(attribCount) == -1)
		return S_FALSE;


	m_pMesh[meshIndex]->setMesh(pD3DMesh, meshPath);


	pAttribData = m_pMesh[meshIndex]->GetAttributeData();

	for (ULONG i = 0; i < attribCount; i++)
	{
		pAttribData[i].Effect = AttribData[i].Effect;
		pAttribData[i].Material = AttribData[i].Material;
		pAttribData[i].Texture = AttribData[i].Texture;
	}

	for (ULONG i = 0; i < attribCount; i ++)
	{
		AttribID[i] = getAttributeID(NULL, &pAttribData[i].Material, NULL);
	}

	m_pMesh[meshIndex]->setAttribMap(AttribID,attribCount);

	m_nMeshCount++;
	return S_OK;

}

//-----------------------------------------------------------------------------
// Name : getMesh()
// Desc : returns a mesh from data pool based index number
//-----------------------------------------------------------------------------
CMyMesh * CAssetManager::getMesh(ULONG meshIndex)
{
	if (meshIndex >= m_nMeshCount)
		return NULL;
	else
		return m_pMesh[meshIndex];
}

//-----------------------------------------------------------------------------
// Name : getMesh()
// Desc : returns a pointer to the last mesh created by the assetManger
// Note : mainly use with the default addMesh() to allow outside classes to finish
//        the creation of that mesh
//-----------------------------------------------------------------------------
CMyMesh * CAssetManager::getLastLoadedMesh()
{
	return m_pMesh[m_nMeshCount - 1];
}

//-----------------------------------------------------------------------------
// Name : getAttribCount()
// Desc : returns how many attributes have been created
//-----------------------------------------------------------------------------
ULONG CAssetManager::getAttribCount()
{
	return m_nAttribCount;
}

//-----------------------------------------------------------------------------
// Name : getAttributeID ()
// Desc : This function is called by the mesh to request a copy of the index
//        to place into it's attribute buffer, based on the values passed.
//-----------------------------------------------------------------------------
ULONG CAssetManager::getAttributeID(LPCTSTR strTextureFile, const OBJMATERIAL * pMaterial, const D3DXEFFECTINSTANCE * pEffectInstance )
{
	ULONG          i;
	long           TextureMatch = -1, MaterialMatch = -1;
	bool           TexMatched = false, MatMatched = false;


	// Loop through the attribute combination table to see if one already exists
	for ( i = 0; i < m_nAttribCount; ++i )
	{
		ATTRIBUTE_ITEM * pAtrItem = &m_pAttribCombo[i];
		long TextureIndex  = pAtrItem->TextureIndex;
		long MaterialIndex = pAtrItem->MaterialIndex;

		TEXTURE_ITEM   * pTexItem = NULL; 
		OBJMATERIAL   * pMatItem = NULL; 

		// Retrieve pointers
		if ( TextureIndex  >= 0 ) pTexItem = m_pTextureList[ TextureIndex ];
		if ( MaterialIndex >= 0 ) pMatItem = &m_pMaterialList[ MaterialIndex ];

		// Neither are matched so far
		TexMatched = false;
		MatMatched = false;

		// If both sets are NULL, this is a match, otherwise perofrm the real match
		if ( pTexItem == NULL && strTextureFile == NULL )
			TexMatched = true;
		else if ( pTexItem != NULL && strTextureFile != NULL )
			if ( strcmp( pTexItem->FileName, strTextureFile ) == 0 ) TexMatched = true;

		if ( pMatItem == NULL && pMaterial == NULL )
			MatMatched = true;
		else if ( pMatItem != NULL && pMaterial != NULL )
			if ( memcmp( pMaterial, pMatItem, sizeof(D3DMATERIAL9) ) == 0 ) MatMatched = true;

		// Store the matched indices in case we can use the later on
		if ( TexMatched ) TextureMatch = TextureIndex;
		if ( MatMatched ) MaterialMatch = MaterialIndex;

		// If they both matched up at the same time, we have a winner
		if ( TexMatched == true && MatMatched == true ) return i;

	} // Next Attribute Combination

	ATTRIBUTE_ITEM AttribItem;

	// First build the material index, or add if necessary
	// The previous material / texture matching was an early out case because of the fact
	// that we were already doing the test. But if we didn't find anything we better do the
	// full search.
	if ( MaterialMatch < 0 && pMaterial != NULL )
	{
		for ( i = 0; i < m_nMaterialCount; ++i )
		{
			// Is there a match ?
			if ( memcmp( pMaterial, &m_pMaterialList[i], sizeof(D3DMATERIAL9) ) == 0 ) MaterialMatch = i;

			// If we found a match, bail
			if ( MaterialMatch >= 0 ) break;

		} // Next Attribute Combination

	} // End if no material match

	if ( TextureMatch < 0 && strTextureFile != NULL )
	{
		for ( i = 0; i < m_nTextureCount; ++i )
		{
			if ( !m_pTextureList[i] || !m_pTextureList[i]->FileName ) continue;

			// Is there a match ?
			if ( strcmp(strTextureFile, m_pTextureList[i]->FileName) == 0 )
				TextureMatch = i;
			//if ( _tcsicmp( strTextureFile, pGameWin->m_pTextureList[i]->FileName ) == 0 ) TextureMatch = i;

			// If we found a match, bail
			if ( TextureMatch >= 0 ) break;

		} // Next Attribute Combination

	} // End if no Texture match

	// Now build the material index, or add if necessary
	if ( MaterialMatch < 0 && pMaterial != NULL )
	{
		AttribItem.MaterialIndex = m_nMaterialCount;
		m_pMaterialList[ m_nMaterialCount++ ] = *pMaterial;

	} // End if no material match
	else
	{
		AttribItem.MaterialIndex = MaterialMatch;

	} // End if material match

	// Now build the texture index, or add if necessary
	if ( TextureMatch < 0 && strTextureFile != NULL )
	{
		// We know it doesn't exist, but we can still use
		// collect texture to do the work for us.
		if (getTexture(strTextureFile ))
			AttribItem.TextureIndex = m_nTextureCount - 1;
		else
			AttribItem.TextureIndex = -1;

	} // End if no texture match
	else
	{
		AttribItem.TextureIndex = TextureMatch;

	} // End if Texture match


	// Store this new attribute combination item
	m_pAttribCombo[ m_nAttribCount++ ] = AttribItem;

	// Return the new attribute index
	return m_nAttribCount - 1;
}

//-----------------------------------------------------------------------------
// Name : getAttribCount()
// Desc : returns how many attributes have been created
//-----------------------------------------------------------------------------
const ATTRIBUTE_ITEM& CAssetManager::getAtribItem(ULONG attribID) const
{
	return m_pAttribCombo[attribID];
}

//-----------------------------------------------------------------------------
// Name : getTexture () 
// Desc : This function return a handle to a texture if the texture is not in 
//		  cache the function will load it from the file
// Note : There is no need to give texture format if one was registered in
//		  the class unless you want a specific format for whatever reason
//-----------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 CAssetManager::getTexture(LPCTSTR FileName, UINT* textureIndex/* = NULL*/, bool powerOf2/* = true*/, D3DFORMAT fmtTexture/*= D3DFMT_UNKNOWN*/)
{
	HRESULT        hRet;
	TEXTURE_ITEM * pNewTexture = NULL;

	if (!FileName) return NULL;

	if (fmtTexture == D3DFMT_UNKNOWN)// if no texture format was given take the format that was registered into the class
		if (m_fmtTexture != D3DFMT_UNKNOWN)//if there is also no format registered return null 
			fmtTexture = m_fmtTexture;
		else
			return NULL;


	//searching if the texture is already in memory 
	for (ULONG i = 0 ; i < m_nTextureCount; i++)
	{
		//m_pTextureList[i]->FileName==FileName;
		if (strcmp(m_pTextureList[i]->FileName, FileName) == 0)
		{
			//return back the texutre Index
			if (textureIndex)
				*textureIndex = i;

			//return the pointer to the Texture which is already in the memory
			return m_pTextureList[i]->Texture;
		}
	}// Next Texture

	//texture not found adding the new texture to the memory
	pNewTexture= new TEXTURE_ITEM;
	if (!pNewTexture) return NULL;
	ZeroMemory( pNewTexture, sizeof(TEXTURE_ITEM) );

	//TODO: order the path thingy so it will work
	// Build filename string
	// 	char Buffer[MAX_PATH];
	// 	_tcscpy( Buffer, pScene->m_strDataPath );
	// 	_tcscat( Buffer, FileName);

	// Create the texture (use 3 mip levels max)
	if (powerOf2)
	{
		hRet = D3DXCreateTextureFromFileEx(m_pD3DDevice, FileName, D3DX_DEFAULT, D3DX_DEFAULT, 3, 0,
			fmtTexture, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
			0, NULL, NULL, &pNewTexture->Texture );
	}
	else
	{
		hRet = D3DXCreateTextureFromFileEx(m_pD3DDevice, FileName, D3DX_DEFAULT, D3DX_DEFAULT, 3, 0,
			fmtTexture, D3DPOOL_MANAGED, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
			0, NULL, NULL, &pNewTexture->Texture );
	}

	if (FAILED(hRet)) 
	{
		delete pNewTexture; 
		return NULL; 
	}

	// Duplicate the filename for future lookups
	pNewTexture->FileName = _strdup( FileName );
	// Store this item
	m_pTextureList[ m_nTextureCount++ ] = pNewTexture;

	// fill send back the texture Index
	if (textureIndex)
		*textureIndex = m_nTextureCount - 1;

	// Return the texture pointer
	return pNewTexture->Texture;


}

//-----------------------------------------------------------------------------
// Name : getTextureItem ()
// Desc : returns texture item from data pool based on index
//-----------------------------------------------------------------------------
const TEXTURE_ITEM* CAssetManager::getTextureItem(ULONG textureIndex) const
{
	if (textureIndex < m_nTextureCount)
		return m_pTextureList[textureIndex];
	else
		return NULL;
}

//-----------------------------------------------------------------------------
// Name : getTexturePtr ()
// Desc : returns texture pointer from data pool based on index
//-----------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 CAssetManager::getTexturePtr(ULONG textureIndex)
{
	if (textureIndex < m_nTextureCount)
		return m_pTextureList[textureIndex]->Texture;
	else
		return NULL;
}

//-----------------------------------------------------------------------------
// Name : getMaterialItem ()
// Desc : returns material item from data pool based on index
//-----------------------------------------------------------------------------
const OBJMATERIAL& CAssetManager::getMaterialItem(ULONG materialIndex) const
{
	return m_pMaterialList[materialIndex];
}

//-----------------------------------------------------------------------------
// Name : addFont()
// Desc : adds a new font to the fonts storage based on the received parameters
//-----------------------------------------------------------------------------
HRESULT CAssetManager::addFont(int height, UINT width, UINT weight, BOOL Italic)
{
	HRESULT hr;
	ID3DXFont* pFont;

	// creates the new Font
	//TODO: add something that will initialize the static values  
	//hr = D3DXCreateFont(m_pD3DDevice, height, width, weight, 1, Italic, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY
	//	,DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);
	hr = D3DXCreateFont(m_pD3DDevice, height, width, weight, 1, Italic, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY
		,DEFAULT_PITCH | FF_DONTCARE, "times new roman", &pFont);

	if (FAILED(hr))
	{
		return hr;
	}

	TEXTMETRICA textMetric;
	pFont->GetTextMetrics(&textMetric);



	//create the new Font item
	FONT_ITEM newFont(height, width, textMetric.tmAveCharWidth, weight, Italic, pFont);
	//FONT_ITEM newFont(height,textMetric.tmAveCharWidth,weight,Italic,pFont);

	//store it in the new Font in the vector
	m_Fonts.push_back(newFont);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : getFont()
// Desc : returns a pointer to the font requested if the font doesn't exist 
//	      in the storage it will create a new font with the requested parameters
//-----------------------------------------------------------------------------
 LPD3DXFONT CAssetManager::getFont(int height, UINT width, UINT weight, BOOL Italic, UINT& fontIndex)
{
	FONT_ITEM newFont(height, width, 0, weight, Italic, NULL);

	//loop thought all existing fonts and check if the font exist
	for (UINT i = 0; i < m_Fonts.size() ; i++)
	{

		//check if the fonts have the same parameters
		 if (m_Fonts[i] == newFont)
		 {
			 //font already exist returns a pointer to it and it's index for faster access next time
			 fontIndex = i;
			 return m_Fonts[i].pFont;
		 }
	}

	//no font was found create a new one
	if ( FAILED(addFont(height,width,weight,Italic)) )
		return NULL;

	// returns the pointer to the font and its index
	fontIndex = m_Fonts.size() - 1;
	return m_Fonts[m_Fonts.size() - 1].pFont;

}

//-----------------------------------------------------------------------------
// Name : getFontPtr()
// Desc : returns a font item based on the given index
//-----------------------------------------------------------------------------
FONT_ITEM CAssetManager::getFontItem(UINT fontIndex) const
 {
	 return m_Fonts[fontIndex];
 }

//-----------------------------------------------------------------------------
// Name : getFontPtr()
// Desc : returns the pointer to the font based on the given Index 
//        it will returns null on invalid index
//-----------------------------------------------------------------------------
const LPD3DXFONT CAssetManager::getFontPtr(UINT fontIndex) const
 {
	 if (fontIndex > m_Fonts.size())
		 return NULL;
	 else
		 return m_Fonts[fontIndex].pFont;
 }
//-----------------------------------------------------------------------------
// Name : getSprite()
// Desc : returns a pointer to the sprite
//-----------------------------------------------------------------------------
LPD3DXSPRITE CAssetManager::getSprite()
{
	return m_pSprite;
}

//-----------------------------------------------------------------------------
// Name : getTopSprite()
// Desc : returns a pointer to the sprite2
//-----------------------------------------------------------------------------
LPD3DXSPRITE CAssetManager::getTopSprite()
{
	return m_pTopSprite;
}

//-----------------------------------------------------------------------------
// Name : getMySprite()
// Desc : returns a pointer to the sprite
//-----------------------------------------------------------------------------
CMySprite* CAssetManager::getMySprite()
{
	return &m_sprite;
}

//-----------------------------------------------------------------------------
// Name : SetTextureFormat()
// Desc : set what texture format should all loaded texture be
//-----------------------------------------------------------------------------
void CAssetManager::SetTextureFormat(D3DFORMAT fmtTexture)
{
	m_fmtTexture = fmtTexture;
}

//-----------------------------------------------------------------------------
// Name : setD3DDevice()
// Desc : set with that device our assetMnager is working with and using to load
//        textures and meshes
//-----------------------------------------------------------------------------
void CAssetManager::setD3DDevice(IDirect3DDevice9* pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
}

//-----------------------------------------------------------------------------
// Name : getD3DDevice()
//-----------------------------------------------------------------------------
LPDIRECT3DDEVICE9 CAssetManager::getD3DDevice()
{
	return m_pD3DDevice;
}

//-----------------------------------------------------------------------------
// Name : getTimer()
//-----------------------------------------------------------------------------
CTimer* CAssetManager::getTimer()
{
	return &m_timer;
}

//-----------------------------------------------------------------------------
// Name : onCreateDevice()
// Desc : register the device the asset Manager should use 
//		  and also initialize sprites
//-----------------------------------------------------------------------------
HRESULT CAssetManager::onCreateDevice(LPDIRECT3DDEVICE9 pD3DDevice)
{
	HRESULT hr;

	m_pD3DDevice = pD3DDevice;
	hr = D3DXCreateSprite(m_pD3DDevice, &m_pSprite);
	hr = D3DXCreateSprite(m_pD3DDevice, &m_pTopSprite);

	m_sprite.init(pD3DDevice);
	m_sprite.setScale(1.0f, 1.0f);

	return hr;
}

//-----------------------------------------------------------------------------
// Name : onResetDevice()
// Desc : reset sprites when the device is being reseted
//-----------------------------------------------------------------------------
HRESULT CAssetManager::onResetDevice()
{
	HRESULT hr = S_OK;

	for (UINT i = 0; i < m_Fonts.size(); i++)
		m_Fonts[i].pFont->OnResetDevice();

	if (m_pSprite)
		hr = m_pSprite->OnResetDevice();

	if (m_pTopSprite)
		hr = m_pTopSprite->OnResetDevice();

	return hr;
}

//-----------------------------------------------------------------------------
// Name : onLostDevice()
// Desc : being called when the device is lost to clear sprite data
//-----------------------------------------------------------------------------
void CAssetManager::onLostDevice()
{
	for (UINT i = 0; i < m_Fonts.size(); i++)
		m_Fonts[i].pFont->OnLostDevice();

	if (m_pSprite)
		m_pSprite->OnLostDevice();

	if (m_pTopSprite)
		m_pTopSprite->OnLostDevice();

}

//-----------------------------------------------------------------------------
// Name : onDestoryDevice()
// Desc : being called when the device is lost to destroy sprite data
//-----------------------------------------------------------------------------
void CAssetManager::onDestoryDevice()
{
	m_pD3DDevice = NULL;

	for (UINT i = 0; i < m_Fonts.size(); i++)
		m_Fonts[i].pFont->Release();

	if (m_pSprite)
		m_pSprite->Release();

	if (m_pTopSprite)
		m_pTopSprite->Release();
}

//-----------------------------------------------------------------------------
// Name : Release()
// Desc : free all the memory allocated by the class and restore class values to 0
//-----------------------------------------------------------------------------
void CAssetManager::Release()
{
	// Release any loaded meshes
	if ( m_pMesh )
	{
		for (ULONG i = 0; i < m_nAllocMesh; ++i )
		{
			if ( m_pMesh[i] ) delete m_pMesh[i];

		} // Next Mesh

		// Delete the outer array
		delete []m_pMesh;
		m_pMesh = NULL;

	} // End if any meshes

	m_nMeshCount       = 0;
	m_nAllocMesh       = 0;

	//Release the current stored textures 
	for (ULONG i = 0; i < m_nTextureCount; ++i )
	{
		if ( m_pTextureList[i] )
		{
			if ( m_pTextureList[i]->Texture ) m_pTextureList[i]->Texture->Release();
			if ( m_pTextureList[i]->FileName ) free( m_pTextureList[i]->FileName );
			delete m_pTextureList[i];

		} // End if available

	} // Next texture

	ZeroMemory( m_pTextureList, MAX_TEXTURES * sizeof(TEXTURE_ITEM*));
	m_nTextureCount    = 0;

	//Release the current stored materials
	ZeroMemory( m_pMaterialList, MAX_MATERIALS * sizeof(D3DMATERIAL9));
	m_nMaterialCount   = 0;

	//Release the current stored attributes
	ZeroMemory( m_pAttribCombo, MAX_ATTRIBUTES * sizeof(ATTRIBUTE_ITEM));
	m_nAttribCount     = 0;

	m_pD3DDevice       = NULL;

}
