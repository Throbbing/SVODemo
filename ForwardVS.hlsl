


cbuffer cbTrans
{
	float4x4 world;
	float4x4 wvp;
};

struct VertexIn
{
	float3 posL:POSITION;
	float3 normalL:NORMAL;
	float2 tex:TEXCOORD;
};

struct VertexOut
{
	float4 posH:SV_POSITION;
	float3 posW:POSITION;
	float3 normalW:NORMAL;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;
	vout.posH = mul(float4(vin.posL, 1.f), wvp);
	vout.posW = mul(float4(vin.posL, 1.f), world).xyz;
	vout.normalW = mul(float4(vin.normalL, 0.f), world).xyz;
	return vout;
}