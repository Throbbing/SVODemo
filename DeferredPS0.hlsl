

struct VertexOut
{
	float4 posH:SV_POSITION;
	float3 posW:POSITION;
	float3 normalW:NORMAL;
};

struct PixelOut
{
	float4 posW:SV_TARGET0;
	float4 normalW:SV_TARGET1;
};

PixelOut main(VertexOut vin)
{
	PixelOut pout;
	pout.posW = float4(vin.posW, 1.f);
	pout.normalW = float4(vin.normalW, 0.f);

	return pout;
	
}