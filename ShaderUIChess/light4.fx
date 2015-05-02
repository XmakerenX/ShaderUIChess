// -------------------------------------------------------------
// consts and structs
// -------------------------------------------------------------
#define MAX_ACTIVE_LIGHTS 4
int numActiveLights;

struct light
{
	float3 pos;
	float4 direction;
	float3 Attenuation012;

	float4 ambient;
	float4 diffuse;
	float4 specular;
	float  spotPower;
};

// -------------------------------------------------------------
// variables that are provided by the application
// -------------------------------------------------------------
float4x4 matWorldViewProj;	
float4x4 matWorld;
float4x4 matWorldInverseT;

//camera position
float4 vecEye;

//vars that defeine light and matrial color
//matrial
float4 vecAmbientMtrl;
float4 vecDiffuseMtrl;
float4 vecSpecularMtrl;
float  SpecularPower;

light lights[MAX_ACTIVE_LIGHTS];

//boolean vars
bool   bHighLight;

//textures
texture MeshTexture;              // Color texture for mesh

//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler MeshTextureSampler = 
sampler_state
{
    Texture = <MeshTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

// -------------------------------------------------------------
// Output channels
// -------------------------------------------------------------
struct VS_OUTPUT
{
      float4 posH    : POSITION0;
	  float2 texUV   : TEXCOORD0;
      float3 posW    : TEXCOORD1;
	  float4 color   : TEXCOORD2;

    //float4 Pos  : POSITION;
	//float2 texUV : TEXCOORD0;
	//float4 lightColor : TEXCOORD1;
	//float4 texCoordProj: TEXCOORD2;
};

// -------------------------------------------------------------
// vertex shader function (input channels)
// -------------------------------------------------------------
VS_OUTPUT VS(float4 posL : POSITION, float3 normalL : NORMAL, float2 texCord0 : TEXCOORD0, uniform bool bTexture)
{
	// Zero out our output
	VS_OUTPUT Out = (VS_OUTPUT)0;  	

    // Transform normal to world space.
    float3 normalW = mul(float4(normalL, 0.0f), matWorldInverseT).xyz;
    normalW = normalize(normalW);

    // Transform vertex position to world space.
    Out.posW  = mul(posL, matWorld).xyz;

	float4 outputColor = 0;

	// Compute the vector from the vertex to the eye position.
    float3 toEye = normalize(vecEye - Out.posW);

	// Ambient Light Computation.
	int activeLights = min(numActiveLights, MAX_ACTIVE_LIGHTS);
	for(int i = 0; i < activeLights; i++)
	{
		float3 ambient = (vecAmbientMtrl * lights[i].ambient).rgb;
		
		// Diffuse Light Computation.
		float3 s;
		float3 r;
		float  A;
		float  spot;
		
		if (lights[i].pos.x != 0 || lights[i].pos.y != 0 || lights[i].pos.z != 0) //check if a light pos was given
		{
			// Unit vector from vertex to light source.
			float3 lightVecW = normalize(lights[i].pos - Out.posW);	 
			
			r = reflect(-lightVecW, normalW);
								
			s = max( dot(normalW, lightVecW), 0.0f);
			
				// Attentuation.
			float d = distance(lights[i].pos, Out.posW);
			
			A = lights[i].Attenuation012.x + lights[i].Attenuation012.y * d + lights[i].Attenuation012.z * d * d;
			
			// Spotlight factor.
			if (lights[i].spotPower != 0)
				spot = pow(max(dot(-lightVecW, lights[i].direction), 0.0f), lights[i].spotPower);
			else
				spot = 1;
		}
		else
		{
			r = reflect(-lights[i].direction, normalW);
			
			s = max(dot(lights[i].direction, normalW), 0.0f); // no pos was given treat it like a directional light
			A = 1;
			spot = 1;
		}
		
		float3 diffuse = s * (vecDiffuseMtrl * lights[i].diffuse).rgb;

		float t  = pow(max(dot(r, toEye), 0.0f), SpecularPower);
	
		float3 spec = t*(vecSpecularMtrl * lights[i].specular).rgb;
	
		float3 color = spot * ( ambient + ((diffuse + spec) / A) );
		float4 lightColor = float4(color,vecDiffuseMtrl.a);
		
		if (outputColor.r == 0 && outputColor.b == 0 && outputColor.g == 0)
		{
			outputColor = lightColor;
		}
		else
		{
			//outputColor = lightColor * 0.5 + outputColor *0.5; 
			outputColor = lightColor + outputColor;
		}
		
	}

	float4 highlight = {1.0f, 1.0f, 1.0f, 1.0f};
	if (bHighLight)
	{
		if (outputColor.r != 0)
			outputColor.r = outputColor.r * 0.5 + highlight.r * 0.5;
			
		if (outputColor.g != 0)
			outputColor.g  = outputColor.g * 0.5 + highlight.g * 0.5;
			
		if (outputColor.b != 0)
			outputColor.b = outputColor.b * 0.5 + highlight.b * 0.5;
	}
		
	Out.color = outputColor;
	
    // Transform to homogeneous clip space.
    // Out.posH = mul( float4(posL, 1.0f), matWorldViewProj);
	Out.posH = mul(posL, matWorldViewProj);
	 	 
	Out.texUV = texCord0;

    // Done--return the output.
    return Out;

}

// -------------------------------------------------------------
// Pixel Shader (input channels):output channel
// -------------------------------------------------------------
float4 PS(float2 texUV : TEXCOORD0, float3 posW : TEXCOORD1, float4 color   : TEXCOORD2, uniform bool bTexture) : COLOR
{  

	if (bTexture)
		//return outputColor;
		return color * 0.5 + tex2D(MeshTextureSampler, texUV) * 0.5;
		//return tex2D(MeshTextureSampler, texUV);
	else
		return color;
}


// -------------------------------------------------------------
// technique for the shaders
// -------------------------------------------------------------
technique lightShader
{
    pass P0
    {
        // compile shaders
        VertexShader = compile vs_3_0 VS(false);
        PixelShader  = compile ps_3_0 PS(false);
    }
}

technique lightTexShader
{
	pass P0
	{
		 // compile shaders
        VertexShader = compile vs_3_0 VS(true);
        PixelShader  = compile ps_3_0 PS(true);
	}
}
