#ifndef __CLIGHTH__
#define __CLIGHTH__

#include <d3dx9.h>

class LIGHT_PREFS  
{
public:
	LIGHT_PREFS();
	LIGHT_PREFS(D3DXVECTOR3& newLightPos, D3DXVECTOR4& newLightDir, D3DXVECTOR3 newLightAtten, D3DXVECTOR4& newLightAmbient, D3DXVECTOR4& newLightDiffuse, D3DXVECTOR4& newLightSpecular
				, float newLightSpecularPower, float newLightSpotPower);

	D3DXVECTOR3 lightPos;
	D3DXVECTOR4 lightDir;

	D3DXVECTOR4 lightAmbient;
	D3DXVECTOR4 lightDiffuse;
	D3DXVECTOR4 lightSpecular;

	D3DXVECTOR3 lightAtten;

	float		lightSpotPower;
};

// class CLight
// {
// public:
// 	CLight(void);
// 	virtual ~CLight(void);
// 
// private:
// 	D3DXVECTOR3	m_pos;
// 
// 	D3DXVECTOR4 m_ambientColor;
// 	D3DXVECTOR4 m_diffuseColor;
// 	D3DXVECTOR4 m_specularColor;
// 	float		m_specularPower;
// 
// 	D3DXVECTOR4 m_direction;
// 
// };

#endif // __CLIGHTH__