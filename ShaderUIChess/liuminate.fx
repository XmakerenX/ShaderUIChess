// -------------------------------------------------------------
// variables that are provided by the application
// -------------------------------------------------------------
//textures
texture UITexture;              // Color texture for mesh

//boolean vars
bool   bHighLight;
bool   bTexture; // was there a texture given?

//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler MeshTextureSampler = 
sampler_state
{
    Texture = <UITexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

// -------------------------------------------------------------
// Pixel Shader (input channels):output channel
// -------------------------------------------------------------
float4 PS(float4 color :COLOR, float2 texUV : TEXCOORD0) : COLOR
{  	
	float4 hLightColor = {1.0f, 0.0f, 0.0f, 1.0f};
	
//	if (bHighLight)
//	{
//		hLightColor = hLightColor * 0.5 + color * 0.5; 
//		return hLightColor * 0.5 + tex2D(MeshTextureSampler, texUV) * 0.54;
//	}
//	else
		//return color * 0.5 + tex2D(MeshTextureSampler, texUV) * 0.5;
		if (bTexture)
			return color * tex2D(MeshTextureSampler, texUV);
		else
			return color;
		//return tex2D(MeshTextureSampler, texUV);
	
}


// -------------------------------------------------------------
// technique for the shaders
// -------------------------------------------------------------
technique illuminateShader
{
    pass P0
    {
        // compile shaders
        PixelShader  = compile ps_3_0 PS();
    }
}
