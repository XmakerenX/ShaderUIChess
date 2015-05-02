#pragma once

#include "../main.h"
#include "renderTypes.h"
#include "CMyVertex.h"
#include <d3dx9mesh.h>
/*#include "CGameWin.h"*/

class CAssetManager;
class CGameWin;

class CMyMesh
{
public:

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CMyMesh(void);
	virtual ~CMyMesh(void);

	//-------------------------------------------------------------------------
	// Public Functions for This Class
	//-------------------------------------------------------------------------
	HRESULT            LoadMeshFromX      ( LPCSTR pFileName, DWORD Options, LPDIRECT3DDEVICE9 pD3D, CAssetManager* assetManger );

	void			   setMesh	         ( LPD3DXMESH pNewMesh,char  newStrMeshName[MAX_PATH] );

	// Object access functions
	LPD3DXMESH         GetMesh            ( ) const;
	ULONG              GetFVF             ( ) const;

	// Variable gathering functions
	LPCTSTR      	   GetMeshName		 ( ) const;

	// Rendering functions
	void               Draw               ( );
	void               DrawSubset         ( ULONG AttributeID );

	int			       Release			  ( );
 
	bool               RegisterTextureCallback   ( LPVOID pFunction, LPVOID pContext );
	bool			   RegisterAttributesCallback( LPVOID pFunction, LPVOID pContext );

	HRESULT			   initAttributeRemap( );		//allocate space for the attribRemap array
	HRESULT			   remapAttributes( );		//remapping the attribites acroding to the remap array
	HRESULT			   GenerateAdjacency( float Epsilon  = 1e-3f  );

	//Mesh creation functions
	void               SetDataFormat      ( ULONG VertexFVF, ULONG IndexStride );
	long               AddVertex          ( ULONG VertexCount = 1, LPVOID pVertices = NULL );
	long               AddFace            ( ULONG FaceCount = 1, LPVOID pIndices = NULL, ULONG AttribID = 0 );
	long               AddAttributeData   ( ULONG AttributeCount );

	MESH_ATTRIB_DATA  *GetAttributeData   ( ) const;
	ULONG		 	  *getAttributeRemap  ( ) const; 
	ULONG			  *getAtrributeMap	 ( ) const;
	ULONG			   getAttributeCount  ( ) const;
	ULONG			   GetNumFaces( )		  const;
	LPD3DXBUFFER	   GetAdjacencyBuffer( ) const;

	bool			   isMenageAtrributes ( );

	void			   setAttribMap		 ( ULONG * pAttribmap, ULONG attribCount);

	HRESULT            BuildMesh         ( ULONG Options, LPDIRECT3DDEVICE9 pDevice, bool ReleaseOriginals = true );
	HRESULT            Optimize			 ( ULONG Flags, CMyMesh * pMeshOut, LPD3DXBUFFER * ppFaceRemap  = NULL , LPD3DXBUFFER * ppVertexRemap  = NULL , LPDIRECT3DDEVICE9 pD3DDevice  = NULL  );
	HRESULT			   OptimizeInPlace	 ( DWORD Flags, LPD3DXBUFFER * ppFaceRemap  = NULL , LPD3DXBUFFER * ppVertexRemap  = NULL  );
	HRESULT			   Attach			 ( LPD3DXBASEMESH pMesh, LPD3DXBUFFER pAdjacency  = NULL  );
	ULONG			   GetOptions		 ( ) const;


private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class
	//-------------------------------------------------------------------------
	LPD3DXMESH          m_pMesh;						// Pointer to the DirectX mesh interface
	char                m_strMeshName[MAX_PATH];        // The filename used to load the mesh or later on for referencing

	LPD3DXBUFFER        m_pAdjacency;                   // Stores adjacency information

	// Managed Attribute Data
	MESH_ATTRIB_DATA   *m_pAttribData;                  // Individual mesh attribute data.

	//unManaged Attribute Data
	ULONG			   *pAttribRemap;

	// Mesh creation data.
	ULONG               m_nVertexFVF;
	ULONG               m_nVertexStride;                // Stride of the vertices
	ULONG               m_nIndexStride;                 // Stride of the indices
	ULONG               m_nVertexCount;                 // Number of vertices to use during BuildMesh
	ULONG               m_nFaceCount;                   // Number of faces to use during BuildMesh
	ULONG               m_nVertexCapacity;              // We are currently capable of holding this many before a grow
	ULONG               m_nFaceCapacity;                // We are currently capable of holding this many before a grow
	ULONG              *m_pAttribute;                   // Attribute ID's for all faces
	UCHAR              *m_pIndex;                       // The face index data
	UCHAR              *m_pVertex;                      // The physical vertices.

protected:
	ULONG               m_nAttribCount;                 // Number of items in the attribute data array.
	ULONG			   *m_pAttribMap;

	CALLBACK_FUNC       m_TextureCallBack;
	CALLBACK_FUNC       m_attributesCallBack;

};
