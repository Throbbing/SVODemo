cbuffer cbWVP
{
	float4x4 wvp;
};

struct VertexIn
{
	float3 posL:POSITION;
	float3 normal:NORMAL;
	float2 tex:TEXCOORD;
	float3 tangent:TANGENT;
};

struct VertexOut
{
	float4 posH:SV_POSITION;
	float3 posW:POSITION;
	float3 normal:NORMAL;
	float2 tex:TEXCOORD;
	float3 tangent:TANGENT;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;
	vout.posH = mul(float4(vin.posL, 1.f), wvp);
	vout.posW = vin.posL;
	vout.normal = vin.normal;
	vout.tex = vin.tex;
	vout.tangent = vin.tangent;
	return vout;
}