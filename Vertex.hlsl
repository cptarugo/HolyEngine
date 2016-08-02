struct Material
{
	float4 Ambiente;
	float4 Difuso;
	float4 Especular; // w = Exponencial de especulacao
	float4 Refletir;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	Material gMaterial;
};

/*
struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
};
*/
struct VertexOut
{
	float4 PosH : SV_POSITION;
	/*float4 Color : COLOR;*/
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
};


//
//	Main Shaders
//

VertexOut VS(float3 PosL : POSITION, float3 NormalL : NORMAL)
{
	VertexOut vout;


	//Transform to world space
	vout.PosW = mul(float4(PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(NormalL, (float3x3)gWorldInvTranspose);

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(PosL, 1.0f), gWorldViewProj);

	// Just pass vertex color into the pixel shader.
	//vout.Color = Color;

	return vout;
}