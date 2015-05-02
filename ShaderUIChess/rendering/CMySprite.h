#ifndef  _CMYSPRITE_H
#define  _CMYSPRITE_H

#include <vector>
#include <d3dx9.h>
#include "../rendering/d3d.h"

class CSpriteVertex;

#define SVertex_FVF			D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1

const UINT MAX_QUADS = 150;

// when adding types to STREAMTYPE  REMEMBER TO UPDATE STREAMTYPE_MAX!!!
enum STREAMTYPE {BACKGROUND,REGLUAR, HiGHLIGHT, TOP, STREAMTYPE_MAX = 4};

// groups together all the quads vertices and indices that have the same texture 
struct VERTEX_STREAM
{
	LPDIRECT3DTEXTURE9 pTexture;

	CSpriteVertex *	   pVertices;
	USHORT        *    pIndices;

	UINT			   numVertices;
	UINT			   numIndices;	

};

struct BUFFER_BOUNDS
{
	LPDIRECT3DTEXTURE9 pTexture;
	UINT bufferStartVert;
	UINT bufferStartIndices;
	UINT bufferEndVert;
	UINT bufferEndIndices;
};

//-----------------------------------------------------------------------------
// Name : CVertex (Class)
// Desc : Vertex class used to construct & store vertex components.
// Note : A simple vertex type, used to store only tex coords and pos.
//-----------------------------------------------------------------------------
class CSpriteVertex
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CSpriteVertex( float fX, float fY, float fZ, const D3DCOLOR & diffuseColor, float ftu = 0.0f, float ftv = 0.0f ) 
	{
		x = fX;
		y = fY;
		z = fZ;
		rhw = 1;
		diffuse = diffuseColor;
		tu = ftu;
		tv = ftv; 
	}

	CSpriteVertex() 
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		rhw = 1;
		diffuse = d3d::WHITE;
		tu = 0.0f;
		tv = 0.0f; 
	}

	//-------------------------------------------------------------------------
	// Public Variables for This Class
	//-------------------------------------------------------------------------
	float       x;          // Vertex Position X Component
	float       y;          // Vertex Position Y Component
	float       z;          // Vertex Position Z Component
	float		rhw;
	D3DCOLOR	diffuse;    // Vertex diffuse color
	float       tu;         // Texture u coordinate
	float       tv;         // Texture v coordinate
};

class CMySprite
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CMySprite(void);
	virtual ~CMySprite(void);

	//-------------------------------------------------------------------------
	// public functions for This Class.
	//-------------------------------------------------------------------------
	HRESULT createQuad	(LPDIRECT3DTEXTURE9 pTexture, RECT& srcRect, POINT quadPos, D3DCOLOR tintColor, STREAMTYPE vertexType);
	void    addQuad		(LPDIRECT3DTEXTURE9 pTexture, RECT& srcRect, POINT quadPos, VERTEX_STREAM& vertexStream, D3DCOLOR tintColor);

	HRESULT addQuadToVertStream			(LPDIRECT3DTEXTURE9 pTexture, RECT& srcRect, POINT quadPos, D3DCOLOR tintColor, std::vector<VERTEX_STREAM>& vertStream);
	void	getQuadsToRenderfromStream	(UINT& numVerticesToRender, UINT& numIndicesToRender, std::vector<VERTEX_STREAM>& vertexStream);
	void	clearVertexStream			(std::vector<VERTEX_STREAM>& vertexStream);

	HRESULT init		(LPDIRECT3DDEVICE9 pDevice);

	HRESULT render		(ID3DXEffect * effect);
	//HRESULT renderQuads (ID3DXEffect * effect, UINT numPass);
	HRESULT renderTopQuads (ID3DXEffect * effect);

	void	setScale	(float fscaleX, float fscaleY);
	void	onLostDevice();
	HRESULT	onResetDevice();

	void	addStreamToBuffer(LPVOID  pBufVertices, LPVOID pBufIndices, std::vector<VERTEX_STREAM>& vertStream, UINT& bufferVertCount, UINT& bufferIndiceCount, UINT& indicesOffset);

	void	setIfToRefresh(bool bToRefresh);
	
	//-------------------------------------------------------------------------
	// static functions for This Class.
	//-------------------------------------------------------------------------
	static void setShadersHandles(D3DXHANDLE hTexture, D3DXHANDLE hHightLight, D3DXHANDLE hTextureBool);

private:
	ID3DXMesh* m_pMesh;
	LPDIRECT3DDEVICE9 m_pDevice;

	LPDIRECT3DVERTEXBUFFER9 m_vBuffer;
	PDIRECT3DINDEXBUFFER9   m_iBuffer;

	std::vector<BUFFER_BOUNDS> m_TexBuffersBounds;
	UINT m_bufferVertCount;
	UINT m_bufferIndicesCount;
	UINT m_indicesOffset;
	UINT m_topQuadsStartIndex;

	UINT numVerticesToRender;
	UINT numIndicesToRender;

// 	LPDIRECT3DVERTEXBUFFER9 m_vb;
// 	PDIRECT3DINDEXBUFFER9	m_ib;
// 
// 	LPDIRECT3DVERTEXBUFFER9 m_highLightVB;
// 	PDIRECT3DINDEXBUFFER9   m_highLightIB;
// 
// 	LPDIRECT3DVERTEXBUFFER9 m_topVB;
// 	PDIRECT3DINDEXBUFFER9   m_topIB;


	//STREAM TYPES are BACKGROUND REGULAR HIGHTLIGHT TOP
	std::vector<VERTEX_STREAM> m_vertexStreams[STREAMTYPE_MAX];

	//std::vector<VERTEX_STREAM> m_vertexStream; // vertex stream for all the regular sprites

	float m_fScaleX;
	float m_fscaleY;

	// handle to the texture var in the shader 
	// used to select the current texture in the shader
	static D3DXHANDLE s_hTexture;
	static D3DXHANDLE s_hHighlight;
	static D3DXHANDLE s_hTextureBool;

	bool m_bRefresh;
};

#endif  //_CMYSPRITE_H