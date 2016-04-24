

struct VertexIn
{
	float3 posL:POSITION;
	float2 tex: TEXCOORD;
};

struct VertexOut
{
	float4 posH:SV_POSITION;
	float2 tex:TEXCOORD;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;
	vout.posH = float4(vin.posL, 1.f);
	vout.tex = vin.tex;
	return vout;
}
