


Texture2D diffuseMap;
Texture2D bumpMap;
SamplerState sam;

cbuffer cbCamera
{
	float3 eyePosW;
};

struct VertexOut
{
	float4 posH:SV_POSITION;
	float3 posW:POSITION;
	float3 normal:NORMAL;
	float2 tex:TEXCOORD;
	float3 tangent:TANGENT;
};

float4 main(VertexOut pin) :SV_TARGET
{
	pin.tangent = normalize(pin.tangent);
	float3 N = normalize(pin.normal);
	float3 T = normalize(pin.tangent - dot(pin.tangent, N)*N);
	float3 B = normalize(cross(N, T));


	float3x3 TBN = float3x3(T, B, N);

	float3 samNormal = bumpMap.Sample(sam, pin.tex).xyz;
//	samNormal /= 255.f;
		samNormal = samNormal*2.f - 1.f;

	float3 normal = mul(samNormal, TBN);
		normal = normalize(normal);

	float3 dir = normalize(float3(1, 1, 1));
		float factor = dot(dir, normal);
	if (factor < 0.00001f)
		factor = 0.f;

	float3 toEye = normalize(eyePosW - pin.posW);
		float3 h = normalize(dir + toEye);
		float s = pow(max(dot(h, normal), 0.0f), 4);

	return  s*float4(0.3f, 0.3f, 0.3f, 0) + diffuseMap.Sample(sam, pin.tex)*
		(float4(0.2f, 0.2f, 0.2f, 0.f) + factor*float4(1.f, 1.f, 1.f, 0.f));
//	return float4(T, 0);
}