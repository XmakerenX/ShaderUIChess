#include "CMySprite.h"

D3DXHANDLE CMySprite::s_hTexture = nullptr;
D3DXHANDLE CMySprite::s_hHighlight = nullptr;
D3DXHANDLE CMySprite::s_hTextureBool = nullptr;

//-----------------------------------------------------------------------------
// Name : CMySprite (constructor)
//-----------------------------------------------------------------------------
CMySprite::CMySprite(void)
{
	m_pDevice = nullptr;

	m_vBuffer = nullptr;
	m_iBuffer = nullptr;

// 	m_vb = NULL;
// 	m_ib = NULL;

	UINT m_bufferVertCount = 0;
	UINT m_bufferIndicesCount = 0;

	m_fScaleX = 0;
	m_fscaleY = 0;

	m_bRefresh = true;
}

//-----------------------------------------------------------------------------
// Name : CMySprite (destructor)
//-----------------------------------------------------------------------------
CMySprite::~CMySprite(void)
{
}

//-----------------------------------------------------------------------------
// Name : render ()
// Desc : render to the screen all the Quads that were queued for rendering
// Note : all the Quads that share the same texture are being batch together
//		  for rendering
//-----------------------------------------------------------------------------
HRESULT CMySprite::render(ID3DXEffect * effect)
{
	HRESULT hRet;
	
	LPVOID  pBufVertices   = NULL;
	LPVOID  pBufIndices    = NULL;

	//DWORD vState;

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	m_pDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)8);
	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 

	m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);

	// check if we are using the fixed pipeline or using a shader
	if (effect == NULL)
	{
		// sets the texture stage states D3DTOP_SELECTARG2
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

		m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX ,0);
		m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		m_pDevice->SetTextureStageState(1, 	D3DTSS_COLOROP, D3DTOP_DISABLE);
	}

	UINT numPasses;

	effect->SetBool(s_hHighlight, FALSE);

	if (effect != NULL)
		effect->Begin(&numPasses, 0);

	for (UINT numPass = 0; numPass < numPasses; numPass++)
	{
		//renderQuads(effect, numPass);
		HRESULT hRet;

		LPVOID  pBufVertices   = NULL;
		LPVOID  pBufIndices    = NULL;

		if (true)
		{
			numVerticesToRender = 0;
			numIndicesToRender = 0;

			getQuadsToRenderfromStream(numVerticesToRender, numIndicesToRender, m_vertexStreams[BACKGROUND]);
			getQuadsToRenderfromStream(numVerticesToRender, numIndicesToRender, m_vertexStreams[REGLUAR]);
			getQuadsToRenderfromStream(numVerticesToRender, numIndicesToRender, m_vertexStreams[HiGHLIGHT]);
			getQuadsToRenderfromStream(numVerticesToRender, numIndicesToRender, m_vertexStreams[TOP]);
			
			hRet = m_vBuffer->Lock(0, numVerticesToRender, &pBufVertices, D3DLOCK_DISCARD);
			if ( FAILED(hRet) ) return hRet;

			hRet = m_iBuffer->Lock(0, numIndicesToRender, &pBufIndices, D3DLOCK_DISCARD);
			if ( FAILED(hRet) ) return hRet;

			//enter the vertex streams to the buffer in the following order
			// 	******* ****** ********** ***** **  (!!There are no spaces between textures it is only for clarity)
			// 	tex1    tex2   tex3       high  top
			m_bufferVertCount = 0;
			m_bufferIndicesCount = 0;
			m_indicesOffset = 0;
			m_topQuadsStartIndex = 0;
			m_TexBuffersBounds.clear();

			addStreamToBuffer(pBufVertices, pBufIndices, m_vertexStreams[BACKGROUND], m_bufferVertCount, m_bufferIndicesCount, m_indicesOffset);
			addStreamToBuffer(pBufVertices, pBufIndices, m_vertexStreams[REGLUAR], m_bufferVertCount, m_bufferIndicesCount, m_indicesOffset);
			addStreamToBuffer(pBufVertices, pBufIndices, m_vertexStreams[HiGHLIGHT], m_bufferVertCount, m_bufferIndicesCount, m_indicesOffset);

			m_topQuadsStartIndex = m_TexBuffersBounds.size();
			addStreamToBuffer(pBufVertices, pBufIndices, m_vertexStreams[TOP], m_bufferVertCount, m_bufferIndicesCount, m_indicesOffset);
			m_bRefresh = false;
		}

		m_iBuffer->Unlock();
		m_vBuffer->Unlock();
		// 
		// input the vertex and indices buffers to the device
		m_pDevice->SetStreamSource(0, m_vBuffer, 0, sizeof(CSpriteVertex));
		m_pDevice->SetFVF(SVertex_FVF);
		m_pDevice->SetIndices(m_iBuffer);

		for(UINT i = 0; i < m_topQuadsStartIndex; i++)
		{
			if (m_TexBuffersBounds[i].pTexture)
			{
				// reEnable the texture rendering if it was disabled
				effect->SetBool(s_hTextureBool, TRUE);
				// selects the current texture in the shader
				effect->SetTexture(s_hTexture, m_TexBuffersBounds[i].pTexture);
			}
			else
			{
				// no texture was given disable texture rendering
				m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
				hRet = effect->SetBool(s_hTextureBool, FALSE);
			}

			BUFFER_BOUNDS curTexBufBound = m_TexBuffersBounds[i];
			UINT vertCount = (curTexBufBound.bufferEndVert - curTexBufBound.bufferStartVert) / sizeof(CSpriteVertex);
			UINT indicesCount = (curTexBufBound.bufferEndIndices - curTexBufBound.bufferStartIndices) / sizeof(USHORT);

			if (effect == NULL)
			{
				m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertCount, curTexBufBound.bufferStartIndices, indicesCount / 3);
			}
			else
			{
				hRet = effect->BeginPass(numPass);

				int vertStart = curTexBufBound.bufferStartVert / sizeof(CSpriteVertex);
				int indicesStart = curTexBufBound.bufferStartIndices / sizeof(USHORT);


				hRet = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, vertStart, vertCount, indicesStart, indicesCount / 3);

				effect->EndPass();
			}

			if (!m_TexBuffersBounds[i].pTexture)
			{
				m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			}
		}		
	}

	if (effect != NULL)
		effect->End();

	// clear the vertex Stream after all the quads were rendered to the screen

	clearVertexStream(m_vertexStreams[BACKGROUND]);
	clearVertexStream(m_vertexStreams[REGLUAR]);
	clearVertexStream(m_vertexStreams[HiGHLIGHT]);
	clearVertexStream(m_vertexStreams[TOP]);

	if (effect == NULL)
	{
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		//m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		//m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : renderQuads ()
// Desc : render all the quads that are in the current vertex stream
//-----------------------------------------------------------------------------
// HRESULT CMySprite::renderQuads(ID3DXEffect * effect, UINT numPass)
// {
// 	HRESULT hRet;
// 
// 	LPVOID  pBufVertices   = NULL;
// 	LPVOID  pBufIndices    = NULL;
// 
// 	if (true)
// 	{
// 		numVerticesToRender = 0;
// 		numIndicesToRender = 0;
// 	
// 		for (UINT i = 0; i < m_vertexStream.size(); i++)
// 		{
// 			numVerticesToRender += m_vertexStream[i].numVertices;
// 			numIndicesToRender += m_vertexStream[i].numIndices;
// 		}
// 	
// 		for (UINT i = 0; i < m_highLightVertStream.size(); i++)
// 		{
// 			numVerticesToRender += m_highLightVertStream[i].numVertices;
// 			numIndicesToRender += m_highLightVertStream[i].numIndices;
// 		}
// 	
// 		for (UINT i = 0; i < m_TopVertStream.size(); i++)
// 		{
// 			numVerticesToRender += m_TopVertStream[i].numVertices;
// 			numIndicesToRender += m_TopVertStream[i].numIndices;
// 		}
// 		
// 		hRet = m_vBuffers[REGLUAR]->Lock(0, numVerticesToRender, &pBufVertices, D3DLOCK_DISCARD);
// 		if ( FAILED(hRet) ) return hRet;
// 	
// 		hRet = m_iBuffers[REGLUAR]->Lock(0, numIndicesToRender, &pBufIndices, D3DLOCK_DISCARD);
// 		if ( FAILED(hRet) ) return hRet;
// 	
// 		//enter the vertex streams to the buffer in the following order
// 	    // 	******* ****** ********** ***** **  (!!There are no spaces between textures it is only for clarity)
// 	    // 	tex1    tex2   tex3       high  top
// 		m_bufferVertCount = 0;
// 		m_bufferIndicesCount = 0;
// 		m_indicesOffset = 0;
// 		m_topQuadsStartIndex = 0;
// 		m_TexBuffersBounds.clear();
// 	
// 		addStreamToBuffer(pBufVertices, pBufIndices, m_vertexStream, m_bufferVertCount, m_bufferIndicesCount, m_indicesOffset);
// 		addStreamToBuffer(pBufVertices, pBufIndices, m_highLightVertStream, m_bufferVertCount, m_bufferIndicesCount, m_indicesOffset);
// 	
// 		m_topQuadsStartIndex = m_TexBuffersBounds.size();
// 		addStreamToBuffer(pBufVertices, pBufIndices, m_TopVertStream, m_bufferVertCount, m_bufferIndicesCount, m_indicesOffset);
// 		m_bRefresh = false;
// 	}
// 
// 	m_iBuffers[REGLUAR]->Unlock();
// 	m_vBuffers[REGLUAR]->Unlock();
// // 
// 	// input the vertex and indices buffers to the device
// 	m_pDevice->SetStreamSource(0, m_vBuffers[REGLUAR], 0, sizeof(CSpriteVertex));
// 	m_pDevice->SetFVF(SVertex_FVF);
// 	m_pDevice->SetIndices(m_iBuffers[REGLUAR]);
// // 
// // 	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
// // 	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
// // 	hRet = effect->SetBool(s_hTextureBool, FALSE);
// // 
// // 	effect->BeginPass(numPass);
// // 	hRet = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 6 / 3);
// // 	hRet = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 4, 6, 6 / 3);
// // 
// // 	effect->EndPass();
// 
// 	for(UINT i = 0; i < m_topQuadsStartIndex; i++)
// 	{
// 		if (m_TexBuffersBounds[i].pTexture)
// 		{
// 			// reEnable the texture rendering if it was disabled
// 			effect->SetBool(s_hTextureBool, TRUE);
// 			// selects the current texture in the shader
// 			effect->SetTexture(s_hTexture, m_TexBuffersBounds[i].pTexture);
// 		}
// 		else
// 		{
// 			// no texture was given disable texture rendering
// 			m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
// 			m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
// 			hRet = effect->SetBool(s_hTextureBool, FALSE);
// 		}
// 
// 		BUFFER_BOUNDS curTexBufBound = m_TexBuffersBounds[i];
// 		UINT vertCount = (curTexBufBound.bufferEndVert - curTexBufBound.bufferStartVert) / sizeof(CSpriteVertex);
// 		UINT indicesCount = (curTexBufBound.bufferEndIndices - curTexBufBound.bufferStartIndices) / sizeof(USHORT);
// 
// 		if (effect == NULL)
// 		{
// 			m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertCount, curTexBufBound.bufferStartIndices, indicesCount / 3);
// 		}
// 		else
// 		{
// //  			UINT numPasses;
// //  			effect->Begin(&numPasses, 0);
// // 
// // 			for (UINT numPass = 0; numPass < numPasses; numPass++)
// 			{
// 				hRet = effect->BeginPass(numPass);
// 
// 				int vertStart = curTexBufBound.bufferStartVert / sizeof(CSpriteVertex);
// 				int indicesStart = curTexBufBound.bufferStartIndices / sizeof(USHORT);
// 
// 
// 				hRet = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, vertStart, vertCount, indicesStart, indicesCount / 3);
// 				//hRet = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, indicesCount / 3);
// 				if ( FAILED(hRet) )
// 				{
// 					UINT x = 5;
// 					x += 5;
// 				}
// 				//hRet = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertCount, 0, indicesCount / 3);
// 
// 				effect->EndPass();
// 			}
// 
// 			//effect->End();
// 		}
// 
// 		if (!m_TexBuffersBounds[i].pTexture)
// 		{
// 			m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
// 			m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
// 		}
// 	}
// 
// // 	if (effect == NULL)
// // 	{
// // 		m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_bufferVertCount, 0, m_bufferIndicesCount / 3);
// // 	}
// // 	else
// // 	{
// // 		//UINT numPasses;
// // 		//effect->Begin(&numPasses, 0);
// // 
// // 		//for (UINT numPass = 0; numPass < numPasses; numPass++)
// // 		{
// // 			effect->BeginPass(numPass);
// // 			hRet = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_bufferVertCount, 0, m_bufferIndicesCount / 3);
// // 
// // 			effect->EndPass();
// // 		}
// 
// 		//effect->End();
// //	}
// 
// // 	if (!vertexStream.pTexture)
// // 	{
// // 		m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
// // 		m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
// // 	}
// 
// 	return S_OK;
// }

//-----------------------------------------------------------------------------
// Name : createQuad ()
// Desc : TODO: write description for this func and name it better
//-----------------------------------------------------------------------------
HRESULT CMySprite::createQuad(LPDIRECT3DTEXTURE9 pTexture, RECT& srcRect, POINT quadPos, D3DCOLOR tintColor, STREAMTYPE vertexType)
{
	switch (vertexType)
	{

	case REGLUAR:
		{
			return addQuadToVertStream(pTexture, srcRect, quadPos, tintColor, m_vertexStreams[REGLUAR]);
		}break;

	case BACKGROUND:
		{
			return addQuadToVertStream(pTexture, srcRect, quadPos, tintColor, m_vertexStreams[BACKGROUND]);
		}break;

	case HiGHLIGHT:
		{
			return addQuadToVertStream(pTexture, srcRect, quadPos, tintColor, m_vertexStreams[HiGHLIGHT]);
		}break;

	case TOP:
		{
			return addQuadToVertStream(pTexture, srcRect, quadPos, tintColor, m_vertexStreams[TOP]);
		}break;

	}
	return S_FALSE;
}

//-----------------------------------------------------------------------------
// Name : addQuadToVertStream ()
// Note : the Quads are being batched by texture
//-----------------------------------------------------------------------------
HRESULT CMySprite::addQuadToVertStream(LPDIRECT3DTEXTURE9 pTexture, RECT& srcRect, POINT quadPos, D3DCOLOR tintColor, std::vector<VERTEX_STREAM>& vertStream)
{
	for (UINT i = 0; i < vertStream.size(); i++)
	{
		if (pTexture == vertStream[i].pTexture)
		{
			addQuad(pTexture, srcRect, quadPos, vertStream[i], tintColor);
			return S_OK;
		}
	}

	VERTEX_STREAM vertexStream;

	vertexStream.pTexture = NULL;
	vertexStream.numVertices = 0;
	vertexStream.numIndices = 0;

	addQuad(pTexture, srcRect, quadPos, vertexStream, tintColor);

	vertStream.push_back(vertexStream);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : addQuad ()
// Desc : TODO: write description for this func and name it better
//-----------------------------------------------------------------------------
void CMySprite::addQuad(LPDIRECT3DTEXTURE9 pTexture, RECT& srcRect, POINT quadPos, VERTEX_STREAM& vertexStream, D3DCOLOR tintColor)
{
	// if no Texture was given the quad will use the tintColor as its background Color
	//if (pTexture == NULL)
	//	return;

	UINT vertexCount = vertexStream.numVertices;

	//float fScaleX;
	//float fScaleY;

	CSpriteVertex * pVertices;

	// check if there are vertices in the vertex Stream
	if (vertexCount > 0)
	{
		// Creates a new buffer that will include all previous vertices and the 4 new ones
		pVertices = new CSpriteVertex[4 + vertexCount];

		// copies the the content of the old buffer the new one
		memcpy(pVertices, vertexStream.pVertices, sizeof(CSpriteVertex) * vertexCount);

		// clears the old buffer
		delete []vertexStream.pVertices;
	}
	else
	{
		// Creates a new buffer of size 4 for the 4 vertices of the Quad
		pVertices = new CSpriteVertex[4];
	}


	// gets the texture info to find its width and height
	D3DSURFACE_DESC textureInfo;
	//pTexture->GetLevelDesc(0, &textureInfo);

	float textureWidth, textureHeight;
	float fScaleU, fScaleV;
	float startU, startV;
	// calculate how much to scale  the UV coordinates of the texture
	if (pTexture)
	{
		pTexture->GetLevelDesc(0, &textureInfo);

		//conversion might lose data ...
		textureWidth  = static_cast<float>( srcRect.right - srcRect.left );
		textureHeight = static_cast<float>( srcRect.bottom - srcRect.top );

		fScaleU = static_cast<float>(srcRect.right  - srcRect.left)  / textureInfo.Width;
		fScaleV = static_cast<float>(srcRect.bottom - srcRect.top) / textureInfo.Height;

		startU = static_cast<float>(srcRect.left) / static_cast<float>(textureInfo.Width);
		startV = static_cast<float>(srcRect.top) / static_cast<float>(textureInfo.Height);
	}
	else
	{
		// there is no texture so zero everything that is related to the texture
		// conversion might lose data
		textureWidth =  static_cast<float>( srcRect.right - srcRect.left );
		textureHeight = static_cast<float>( srcRect.bottom - srcRect.top );
		fScaleU = 0;
		fScaleV = 0;
		startU = 0;
		startV = 0;
	}

	// creates the four vertices of our quad 
	pVertices[vertexCount].x		= static_cast<float>( quadPos.x );
	pVertices[vertexCount].y		= static_cast<float>( quadPos.y );
	pVertices[vertexCount].z		= 0;
	pVertices[vertexCount].rhw		= 1;
	pVertices[vertexCount].diffuse  = tintColor;
	pVertices[vertexCount].tu		= startU;
	pVertices[vertexCount++].tv		= startV;


	pVertices[vertexCount].x		= static_cast<float> ( quadPos.x + textureWidth * m_fScaleX );
	pVertices[vertexCount].y		= static_cast<float> ( quadPos.y );
	pVertices[vertexCount].z		= 0;
	pVertices[vertexCount].rhw		= 1;
	pVertices[vertexCount].diffuse	= tintColor;
	pVertices[vertexCount].tu		= startU + 1 * fScaleU;
	pVertices[vertexCount++].tv		= startV;


	pVertices[vertexCount].x		= static_cast<float> ( quadPos.x );
	pVertices[vertexCount].y		= static_cast<float> ( quadPos.y + textureHeight * m_fscaleY );
	pVertices[vertexCount].z		= 0;
	pVertices[vertexCount].rhw		= 1;
	pVertices[vertexCount].diffuse	= tintColor;
	pVertices[vertexCount].tu		= startU;
	pVertices[vertexCount++].tv		= startV + 1 * fScaleV;

	pVertices[vertexCount].x		= static_cast<float> ( quadPos.x + textureWidth * m_fScaleX );
	pVertices[vertexCount].y		= static_cast<float> ( quadPos.y + textureHeight * m_fscaleY );
	pVertices[vertexCount].z		= 0;
	pVertices[vertexCount].rhw		= 1;
	pVertices[vertexCount].diffuse	= tintColor;
	pVertices[vertexCount].tu		= startU + 1 * fScaleU;
	pVertices[vertexCount++].tv		= startV + 1 * fScaleV;

	// update the struct with the new vertex buffer and the new vertex count
	vertexStream.pVertices = pVertices;
	vertexStream.numVertices = vertexCount;

	UINT indicesCount = vertexStream.numIndices;


	// Creates a new buffer that will include all previous indices and the 9 new ones
	USHORT* pIndices = new USHORT[6 + indicesCount];

	// check if there were already indices in the vertex Stream
	if (indicesCount > 0)
	{
		// copies the content of the old buffer to the new one
		memcpy(pIndices,vertexStream.pIndices, sizeof(USHORT) * indicesCount);

		// clear the old buffer memory
		delete []vertexStream.pIndices;
	}

	// get the our current vertex index and our index count
	USHORT vIndex = vertexCount - 4;
	UINT iCount = vertexStream.numIndices;

	//triangle 201
	pIndices[iCount++] = vIndex + 2; // #2 vertex	 0---1 4---5
	pIndices[iCount++] = vIndex;	 // #0 vertex    -   - -
	pIndices[iCount++] = vIndex + 1; // #1 vertex    2---3 6---7

	//triangle 213
	pIndices[iCount++] = vIndex + 2;     // #2 vertex
	pIndices[iCount++] = vIndex + 1;     // #1 vertex
	pIndices[iCount++] = vIndex + 2 + 1; // #3 vertex

	vertexStream.pIndices = pIndices;
	vertexStream.numIndices = iCount;

	// if we just created this vertex stream store in it it's texture handle
	if (vertexStream.pTexture == NULL)
		vertexStream.pTexture = pTexture;

}

//-----------------------------------------------------------------------------
// Name : init ()
// Desc : initialize the sprite class and create the vertex and indices buffers
//        that are going to be used to render the Quads
//-----------------------------------------------------------------------------
HRESULT CMySprite::init(LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT hRet;

	// check that we got a valid device handle
	if (pDevice == NULL)
		return S_FALSE;

	m_pDevice = pDevice;

	// TODO: check if I need to add more usage flags
	// Creates a vertex buffer to store the sprite vertices 
	hRet = m_pDevice->CreateVertexBuffer(sizeof(CSpriteVertex) * 4 * MAX_QUADS, D3DUSAGE_DYNAMIC, SVertex_FVF, D3DPOOL_SYSTEMMEM, &m_vBuffer, NULL);
	if ( FAILED(hRet) ) return hRet;
	hRet = m_pDevice->CreateIndexBuffer(sizeof(USHORT) * 6 * MAX_QUADS, D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_iBuffer, NULL);
	if ( FAILED(hRet) ) return hRet;

	//m_highLightVertStream.numIndices = 0;
	//m_highLightVertStream.numVertices = 0;
	//m_highLightVertStream.pIndices = NULL;
	//m_highLightVertStream.pVertices = NULL;
	//m_highLightVertStream.pTexture = NULL;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : setScale ()
//-----------------------------------------------------------------------------
void CMySprite::setScale(float fscaleX, float fscaleY)
{
	m_fScaleX = fscaleX;
	m_fscaleY = fscaleY;
}

//-----------------------------------------------------------------------------
// Name : setShadersHandles ()
//-----------------------------------------------------------------------------
void CMySprite::setShadersHandles(D3DXHANDLE hTexture, D3DXHANDLE hHightLight, D3DXHANDLE hTextureBool)
{
	s_hTexture = hTexture;
	s_hHighlight = hHightLight;
	s_hTextureBool = hTextureBool;
}

//-----------------------------------------------------------------------------
// Name : onLostDevice ()
// Desc : release the recourses we allocated on the d3d device
//-----------------------------------------------------------------------------
void CMySprite::onLostDevice()
{
	// gets how many refrences to the buffers are left 
	// should return 0..
	ULONG refrenceCount;

	refrenceCount = m_vBuffer->Release();
	refrenceCount = m_iBuffer->Release();

}

//-----------------------------------------------------------------------------
// Name : onResetDevice ()
// Desc : reallocate our vertex and index buffers after the device was lost
//-----------------------------------------------------------------------------
HRESULT CMySprite::onResetDevice()
{
	HRESULT hRet = S_FALSE;

	// Creates a vertex buffer to store the sprite vertices 
	hRet = m_pDevice->CreateVertexBuffer(sizeof(CSpriteVertex) * 4 * MAX_QUADS,D3DUSAGE_WRITEONLY, SVertex_FVF, D3DPOOL_MANAGED, &m_vBuffer, NULL);
	if ( FAILED(hRet) ) return hRet;

	// Creates a indices buffer to store the sprite indices
	hRet = m_pDevice->CreateIndexBuffer(sizeof(USHORT) * 6 * MAX_QUADS, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_iBuffer, NULL);
	if ( FAILED(hRet) ) return hRet;

	return hRet;
	
}
//-----------------------------------------------------------------------------
// Name : addStramToBuffer ()
// Desc : adds the given vertex stream to the buffer we use to draw the sprites
//		  to the screen
//-----------------------------------------------------------------------------
void CMySprite::addStreamToBuffer(LPVOID  pBufVertices, LPVOID pBufIndices, std::vector<VERTEX_STREAM>& vertStream, UINT& bufferVertCount, UINT& bufferIndiceCount, UINT& indicesOffset)
{
	// sanity check
	if (!pBufVertices)
		return;

	if (!pBufIndices)
		return;

	//enter the vertex streams to the buffer in the following order
	// 	******* ****** ********** ***** **  (!!There are no spaces between textures it is only for clarity)
	// 	tex1    tex2   tex3       high  top
	//UINT bufferVertCount = 0;
	//m_TexBuffersBounds.clear();

	for (UINT i = 0; i < vertStream.size(); i++)
	{
		BUFFER_BOUNDS texBufferBounds;

		// convert the buffer void pointer to char pointer in order to move forward in the buffer more easily
		char* pBufVertIndex = static_cast<char*>(pBufVertices);
		pBufVertIndex += bufferVertCount /** sizeof(CSpriteVertex)*/;

		texBufferBounds.bufferStartVert = bufferVertCount;

// 		for (UINT j = 0; j < vertStream[i].numVertices; j++)
// 		{
// 			CSpriteVertex temp = vertStream[i].pVertices[j];
// 			UINT x = 0;
// 			x += 4;
// 		}
		
		memcpy(pBufVertIndex, vertStream[i].pVertices,
			vertStream[i].numVertices * sizeof(CSpriteVertex) );
		bufferVertCount += vertStream[i].numVertices * sizeof(CSpriteVertex);

		texBufferBounds.bufferEndVert = bufferVertCount;

		// convert the buffer void pointer to char pointer in order to move forward in the buffer more easily
		char* pBufIndicesIndex = static_cast<char*>(pBufIndices);
		pBufIndicesIndex += bufferIndiceCount;

		texBufferBounds.bufferStartIndices = bufferIndiceCount;

		if ( indicesOffset > 0)
		{
			for (UINT j = 0; j < vertStream[i].numIndices; j++)
			{
				vertStream[i].pIndices[j] += indicesOffset;
			}
		}

		indicesOffset = vertStream[i].pIndices[vertStream[i].numIndices - 1] + 1;

		memcpy( pBufIndicesIndex , vertStream[i].pIndices,
			vertStream[i].numIndices * sizeof(USHORT) );
		bufferIndiceCount += vertStream[i].numIndices * sizeof(USHORT);

		texBufferBounds.bufferEndIndices = bufferIndiceCount;

		texBufferBounds.pTexture = vertStream[i].pTexture;

		m_TexBuffersBounds.push_back(texBufferBounds);
	}
}

//-----------------------------------------------------------------------------
// Name : renderTopQuads ()
// Desc : render only the quads that should be on top all the other quads
//-----------------------------------------------------------------------------
HRESULT CMySprite::renderTopQuads(ID3DXEffect * effect)
{
	HRESULT hRet = S_OK;

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	m_pDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)8);
	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 

	m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);

	// check if we are using the fixed pipeline or using a shader
	if (effect == NULL)
	{
		// sets the texture stage states D3DTOP_SELECTARG2
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

		m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX ,0);
		m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		m_pDevice->SetTextureStageState(1, 	D3DTSS_COLOROP, D3DTOP_DISABLE);
	}

	m_pDevice->SetStreamSource(0, m_vBuffer, 0, sizeof(CSpriteVertex));
	m_pDevice->SetFVF(SVertex_FVF);
	m_pDevice->SetIndices(m_iBuffer);

	UINT numPasses;

	if (effect != NULL)
		effect->Begin(&numPasses, 0);

	for (UINT numPass = 0; numPass < numPasses; numPass++)
	{
		for(UINT i = m_topQuadsStartIndex; i < m_TexBuffersBounds.size(); i++)
		{
			if (m_TexBuffersBounds[i].pTexture)
			{
				// reEnable the texture rendering if it was disabled
				effect->SetBool(s_hTextureBool, TRUE);
				// selects the current texture in the shader
				effect->SetTexture(s_hTexture, m_TexBuffersBounds[i].pTexture);
			}
			else
			{
				// no texture was given disable texture rendering
				m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
				hRet = effect->SetBool(s_hTextureBool, FALSE);
			}

			BUFFER_BOUNDS curTexBufBound = m_TexBuffersBounds[i];
			UINT vertCount = (curTexBufBound.bufferEndVert - curTexBufBound.bufferStartVert) / sizeof(CSpriteVertex);
			UINT indicesCount = (curTexBufBound.bufferEndIndices - curTexBufBound.bufferStartIndices) / sizeof(USHORT);

			if (effect == NULL)
			{
				m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertCount, curTexBufBound.bufferStartIndices, indicesCount / 3);
			}
			else
			{
				hRet = effect->BeginPass(numPass);

				int vertStart = curTexBufBound.bufferStartVert / sizeof(CSpriteVertex);
				int indicesStart = curTexBufBound.bufferStartIndices / sizeof(USHORT);


				hRet = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, vertStart, vertCount, indicesStart, indicesCount / 3);
				//hRet = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, indicesCount / 3);
				if ( FAILED(hRet) )
				{
					UINT x = 5;
					x += 5;
				}

				effect->EndPass();;
			}

			if (!m_TexBuffersBounds[i].pTexture)
			{
				m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			}
		}
	}

	if (effect != NULL)
		effect->End();

	return hRet;
}

//-----------------------------------------------------------------------------
// Name : setIfToRefresh ()
// Desc : currently not used 
// TODO : If no use will be found to m_bRefresh DELETE this functions
//-----------------------------------------------------------------------------
void CMySprite::setIfToRefresh(bool bToRefresh)
{
	m_bRefresh = bToRefresh;
}

//-----------------------------------------------------------------------------
// Name : getQuadsToRenderfromStream ()
//-----------------------------------------------------------------------------
void CMySprite::getQuadsToRenderfromStream(UINT& numVerticesToRender, UINT& numIndicesToRender, std::vector<VERTEX_STREAM>& vertexStream)
{
	for (UINT i = 0; i < vertexStream.size(); i++)
	{
		numVerticesToRender += vertexStream[i].numVertices;
		numIndicesToRender += vertexStream[i].numIndices;
	}
}

//-----------------------------------------------------------------------------
// Name : clearVertexStream ()
//-----------------------------------------------------------------------------
void CMySprite::clearVertexStream(std::vector<VERTEX_STREAM>& vertexStream)
{
	for (UINT i = 0; i < vertexStream.size(); i++)
	{
		// free the memory allocated (TO NOT LEAK GOD DAMN MEMORY!!!)
		delete [vertexStream[i].numVertices] vertexStream[i].pVertices;
		delete [vertexStream[i].numIndices]  vertexStream[i].pIndices;
	}
	vertexStream.clear();
}