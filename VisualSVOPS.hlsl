

struct VertexOut
{
	float4 posH:SV_POSITION;
	float3 normalW:NORMAL;
	float3 color:COLOR;
};

float4 main(VertexOut pin):SV_TARGET
{
	return float4(pin.color, 0.5f);
}
