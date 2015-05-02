#include "d3d.h"

OBJMATERIAL d3d::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{	
	OBJMATERIAL mtrl;
	mtrl.Ambient  = copyColorToVector(a);
	mtrl.Diffuse  = copyColorToVector(d);
	mtrl.Specular = copyColorToVector(s);
	mtrl.Emissive = copyColorToVector(e);
	mtrl.Power    = p;
	return mtrl;

}

LIGHT_PREFS d3d::InitDirectionalLight(D3DXVECTOR4& direction, D3DXCOLOR color)
{
	LIGHT_PREFS light;
	::ZeroMemory(&light, sizeof(light) );

	//light.lightDir = D3DXVECTOR4(1.0f, -0.0f, 0.25f,0.0f);
	light.lightDir = direction;
	 
	light.lightAmbient = copyColorToVector(color) * 0.4f;
	light.lightDiffuse = copyColorToVector(color);
	light.lightSpecular = copyColorToVector(color);

	return light;
}

LIGHT_PREFS d3d::InitPointLight(D3DXVECTOR3& position, D3DXCOLOR color)
{
	LIGHT_PREFS light;
	::ZeroMemory(&light, sizeof(light) );

	light.lightPos = position;
	light.lightAtten = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	//light.lightAmbient = copyColorToVector(color) * 0.6f;
	light.lightAmbient = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	light.lightDiffuse = copyColorToVector(color);
	light.lightSpecular = copyColorToVector(color);

	return light;
}

LIGHT_PREFS d3d::InitSpotLight(D3DXVECTOR3& position, D3DXVECTOR4& direction, D3DXCOLOR color)
{
	LIGHT_PREFS light;
	::ZeroMemory(&light, sizeof(light) );

	light.lightPos = position;
	light.lightDir = direction;
	light.lightAtten = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	light.lightAmbient = copyColorToVector(color) * 0.6f;
	light.lightDiffuse = copyColorToVector(color);
	light.lightSpecular = copyColorToVector(color);
	light.lightSpotPower = 12.0f;

	return light;
}
