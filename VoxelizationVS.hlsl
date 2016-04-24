

cbuffer cbTrans
{
	float4x4 world;
}

struct VertexIn
{
	float3 posL:POSITION;
	float3 normalL:NORMAL;
	float2 tex:TEXCOORD;
	float3 tangent:TANGENT;
};

struct VertexOut
{
	float3 posW:POSITION;
	float3 normalW:NORMAL;
	float2 tex:TEXCOORD;
};
VertexOut main(VertexIn vin )
{
	VertexOut vout;
	vout.posW = mul(float4(vin.posL, 1.f), world).xyz;
	vout.normalW = mul(float4(vin.normalL, 0.f), world).xyz;
	vout.tex = vin.tex;
	return vout;
}