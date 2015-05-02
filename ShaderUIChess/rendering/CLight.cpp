#include "CLight.h"

LIGHT_PREFS::LIGHT_PREFS()
{
	lightPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	lightDir = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

	lightAtten = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	lightAmbient = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	lightDiffuse = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	lightSpecular = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	lightSpotPower = 0.0f;
}

LIGHT_PREFS::LIGHT_PREFS(D3DXVECTOR3& newLightPos, D3DXVECTOR4& newLightDir, D3DXVECTOR3 newLightAtten, D3DXVECTOR4& newLightAmbient, D3DXVECTOR4& newLightDiffuse, D3DXVECTOR4& newLightSpecular
						 , float newLightSpecularPower, float newLightSpotPower)
{
	lightPos = newLightPos;
	lightDir = newLightDir;

	lightAtten = newLightAtten; 

	lightAmbient = newLightAmbient;
	lightDiffuse = newLightDiffuse;
	lightSpecular = newLightSpecular;
	lightSpotPower = newLightSpotPower;

}

// CLight::CLight(void)
// {
// }
// 
// CLight::CLight(LIGHT_PREFS& lightPrefs)
// {
// 	m_pos = lightPrefs.lightPos;
// 	m_ambientColor  = lightPrefs.lightAmbient;
// 	m_diffuseColor  = lightPrefs.lightDiffuse;
// 	m_specularColor = lightPrefs.lightSpecular;
// 	m_specularPower = lightPrefs.lightSpecularPower;
// 
// 	m_direction = lightPrefs.lightDir;
// }
// 
// CLight::~CLight(void)
// {
// }
