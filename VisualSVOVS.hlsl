cbuffer cbTrans
{
	float4x4 vp;
};

struct VisualPackage
{
	float4x4 world;
	float4 normal;
	float4 color;
};

struct VertexIn
{
	float3 posL:POSITION;
	uint index:INDEX;
};

struct VertexOut
{
	float4 posH:SV_POSITION;
	float3 normalW:NORMAL;
	float3 color:COLOR;
};

StructuredBuffer<VisualPackage> visualPool;

VertexOut main(VertexIn vin)
{
	VertexOut vout;
	VisualPackage visual = visualPool[vin.index];
	vout.posH = mul(float4(vin.posL,1.f),mul(visual.world,vp));
//	vout.posH = mul(float4(vin.posL, 1.f), vp);
//	vout.normalW = mul(float4(visual.normal,0.f),visual.world).xyz;
	vout.color = visual.color.xyz;
	return vout;
}

