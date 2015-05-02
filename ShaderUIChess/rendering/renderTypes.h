#ifndef __RENDERTYPESH__
#define __RENDERTYPESH__

#include <d3dx9.h>

D3DXVECTOR4 copyColorToVector(D3DXCOLOR color);
D3DXVECTOR4 copyColorToVector(D3DCOLORVALUE color);

//-----------------------------------------------------------------------------
// Structures & Enumerators
//-----------------------------------------------------------------------------
//material struct that uses d3dxvector4 to define color to more easily sent it to the shader
struct OBJMATERIAL
{
	D3DXVECTOR4   Diffuse;        /* Diffuse color RGBA */
	D3DXVECTOR4   Ambient;        /* Ambient color RGB */
	D3DXVECTOR4   Specular;       /* Specular 'shininess' */
	D3DXVECTOR4   Emissive;       /* Emissive color RGB */
	float           Power;        /* Sharpness if specular highlight */

	OBJMATERIAL& operator = ( CONST D3DMATERIAL9& m)
	{
		this->Ambient = copyColorToVector(m.Ambient);
		this->Diffuse = copyColorToVector(m.Diffuse);
		this->Emissive = copyColorToVector(m.Emissive);
		this->Specular = copyColorToVector(m.Specular);
		this->Power = m.Power;

		return *this;
	}
};

struct MESH_ATTRIB_DATA    // Stores managed data for an individual mesh attribute
{
	OBJMATERIAL        Material;   // Material to use for this attribute
	LPDIRECT3DTEXTURE9  Texture;    // Texture to use for this attribute
	LPD3DXEFFECT        Effect;     // Effect to use for this attribute

};

#endif // __RENDERTYPESH__