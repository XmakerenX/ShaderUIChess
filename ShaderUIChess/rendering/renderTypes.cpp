#include "renderTypes.h"

D3DXVECTOR4 copyColorToVector(D3DXCOLOR color)
{
	D3DXVECTOR4 v;
	v.x = color.r;
	v.y = color.g;
	v.z = color.b;
	v.w = color.a;

	return v;
}

D3DXVECTOR4 copyColorToVector(D3DCOLORVALUE color)
{
	D3DXVECTOR4 v;
	v.x = color.r;
	v.y = color.g;
	v.z = color.b;
	v.w = color.a;

	return v;
}