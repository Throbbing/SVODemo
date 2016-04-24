cbuffer cbMaterial
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

cbuffer cbLights
{
	float4 ambients[100];
	float4 diffuses[100];
	float4 speculars[100];
	float4 values[100];
	int	   lightNum;
};

cbuffer cbCamera
{
	float4 viewPos;
};

float4 PointLight(float3 normal, float3 hitPoint ,float4 amb, float4 dif, float4 spe, float4 pos)
{
	float3 toLight = normalize(pos.xyz - hitPoint);
		float3 toView = normalize(viewPos.xyz - hitPoint);
		float factor =  dot(normal,toLight);
	if (factor < 0.000001f)
		return float4(0, 0, 0, 0);

	float3 h = normalize(toLight + toView);
		float s = max(0.f, dot(normal, h));

		return amb*ambient + dif*diffuse*factor + spe*float4(specular.xyz, 0.f)*pow(s, specular.w);

	

	
}

struct VertexOut
{
	float4 posH:SV_POSITION;
	float3 posW:POSITION;
	float3 normalW:NORMAL;
};

float4 main(VertexOut pin) : SV_TARGET
{
	
	pin.normalW = normalize(pin.normalW);

	float4 color = float4(0, 0, 0, 0);
//	[unroll]
	for (int i = 0; i < lightNum; ++i)
	{
		float4 c = PointLight(pin.normalW, pin.posW, ambients[i], diffuse[i], speculars[i], values[i]);
			color += c;

	}
	
	return color;
}