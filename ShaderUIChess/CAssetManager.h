#ifndef __CASSETMANAGERH__
#define __CASSETMANAGERH__

#include <d3dx9.h>
#include "rendering/renderTypes.h"
#include <vector>
#include "rendering/CMySprite.h"
#include "CTimer.h"

class CMyMesh;

//-----------------------------------------------------------------------------
// Definitions, Macros & Constants
//-----------------------------------------------------------------------------
const ULONG MAX_TEXTURES		= 10000;       // Store a maximum of 10,000 textures
const ULONG MAX_MATERIALS		= 1000;        // Store a maximum of 1000 materials
const ULONG MAX_ATTRIBUTES		= 10000;       // Store a maximum of 10,000 attribute combinations

const ULONG MAX_ATTRIB_IDS      = 10;		   // The max attribute IDs per Mesh	   

//-----------------------------------------------------------------------------
// Typedefs, Structures and Enumerators
//-----------------------------------------------------------------------------
struct TEXTURE_ITEM
{
	LPSTR               FileName;   // File used to create the texture
	LPDIRECT3DTEXTURE9  Texture;    // The texture pointer itself.
};

struct ATTRIBUTE_ITEM
{
	long TextureIndex;           // Index into the texture array
	long MaterialIndex;          // Index into the material array
};

//TODO: think of maybe using some string to represent the Font instead of a couple of vars
//NOTE: having this vars seems to be quite useful for determining the font properties so string idea is bah
struct FONT_ITEM
{
	int		   height;   // Font height
	UINT	   width;    // Font width
	UINT	   avgWidth; // average char width
	UINT	   weight;   // Font weight aka bold style or other more stronger styles
	BOOL	   Italic;   // use Italic style or not
	LPD3DXFONT pFont;    // pointer to the font itself

	FONT_ITEM::FONT_ITEM(int newHeight, UINT newWidth, UINT newAvgWidth, UINT newWeight, BOOL newItalic, LPD3DXFONT pNewFont)
	{
		height = newHeight;
		width  = newWidth;
		avgWidth = newAvgWidth;
		weight = newWeight;
		Italic = newItalic;
		pFont  = pNewFont;
	}

	bool FONT_ITEM::operator==(const FONT_ITEM& other) const 
	{
		if (height == other.height && width == other.width && weight ==other.weight && Italic == other.Italic)
			return true;
		else
			return false;
	}

};

class CAssetManager
{
public:
	CAssetManager(IDirect3DDevice9* pD3DDevice = NULL);
	virtual ~CAssetManager(void);

	//-------------------------------------------------------------------------
	// Public Functions for This Class
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Functions that handle mesh allocation and storage
	//-------------------------------------------------------------------------
	long				  AllocMesh		   ( ULONG Count  = 1);

	HRESULT				  AddMesh		   ( );
	HRESULT			   	  AddMesh		   ( char meshPath[MAX_PATH], DWORD meshOptions = D3DXMESH_MANAGED);
	HRESULT				  AddMesh		   ( MESH_ATTRIB_DATA AttribData[], ULONG attribCount, LPD3DXMESH pD3DMesh, char meshPath[MAX_PATH]);

	CMyMesh *			  getMesh		   ( ULONG meshIndex);
	CMyMesh *			  getLastLoadedMesh( );

	//-------------------------------------------------------------------------
	// Functions that handle attributes,textures and materials allocation and storage 
	//-------------------------------------------------------------------------
	ULONG				  getAttribCount   ( );
	ULONG				  getAttributeID   (LPCTSTR strTextureFile, const OBJMATERIAL * pMaterial, const D3DXEFFECTINSTANCE * pEffectInstance = NULL );
	const ATTRIBUTE_ITEM& getAtribItem	   (ULONG attribID) const;

	LPDIRECT3DTEXTURE9	  getTexture	   (LPCTSTR FileName, UINT* textureIndex = NULL, D3DFORMAT fmtTexture = D3DFMT_UNKNOWN);
	const TEXTURE_ITEM*   getTextureItem   (ULONG textureIndex)	 const;
	LPDIRECT3DTEXTURE9	  getTexturePtr    (ULONG textureIndex);
	const OBJMATERIAL&	  getMaterialItem  (ULONG materialIndex) const;

	//-------------------------------------------------------------------------
	// Functions that attributes,textures,materials and sprites allocation and storage 
	//-------------------------------------------------------------------------
	HRESULT				  addFont		   (int height, UINT width, UINT weight, BOOL Italic);
	LPD3DXFONT			  getFont		   (int height, UINT width, UINT weight, BOOL Italic, UINT& fontIndex);
	FONT_ITEM			  getFontItem	   (UINT fontIndex) const;
	const LPD3DXFONT	  getFontPtr	   (UINT fontIndex) const;

	LPD3DXSPRITE		  getSprite		   ( );
	LPD3DXSPRITE		  getTopSprite	   ( );
	CMySprite*			  getMySprite	   ( );

	void				  SetTextureFormat ( D3DFORMAT fmtTexture);
	void				  setD3DDevice	   ( IDirect3DDevice9* pD3DDevice);
	LPDIRECT3DDEVICE9	  getD3DDevice	   ();

	CTimer*				  getTimer		   ( );

	//-------------------------------------------------------------------------
	// Functions that handle device creation, lost and reset
	//-------------------------------------------------------------------------
	HRESULT				  onCreateDevice   ( LPDIRECT3DDEVICE9 pD3DDevice);
	HRESULT				  onResetDevice    ( );
	void				  onLostDevice     ( );
	void				  onDestoryDevice  ( );

	void				  Release		   ( );

private:
	IDirect3DDevice9*        m_pD3DDevice;       // Direct3D Device Object

	CMyMesh				   **m_pMesh;                        // Array of loaded scene meshes
	ULONG					 m_nMeshCount;                   // Number of meshes currently stored
	ULONG					 m_nAllocMesh;

	ATTRIBUTE_ITEM		     m_pAttribCombo[MAX_ATTRIBUTES]; // Table of attribute combinations
	ULONG                    m_nAttribCount; 

	TEXTURE_ITEM           * m_pTextureList [MAX_TEXTURES];
	ULONG					 m_nTextureCount;

	D3DFORMAT			     m_fmtTexture;					// Texture format to use when building textures.

	OBJMATERIAL              m_pMaterialList[MAX_MATERIALS]; // Array of material structures.
	ULONG                    m_nMaterialCount;               // Number of materials stored

    LPD3DXSPRITE			 m_pSprite;  // The sprite the GUI is bring rendered to
	LPD3DXSPRITE			 m_pTopSprite;
	CMySprite				 m_sprite;   // my own sprite interface which now renders the GUI

	std::vector<FONT_ITEM>   m_Fonts;   // holds all the created fonts

	CTimer					 m_timer;
};

#endif // __CASSETMANAGERH__