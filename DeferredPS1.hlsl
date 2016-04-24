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

float4 PointLight(float3 normal, float3 hitPoint, float4 amb, float4 dif, float4 spe, float4 pos)
{
	float3 toLight = normalize(pos.xyz - hitPoint);
		float3 toView = normalize(viewPos.xyz - hitPoint);
		float factor = dot(normal, toLight);
	if (factor < 0.000001f)
		return float4(0, 0, 0, 0);

	float3 h = normalize(toLight + toView);
		float s = max(0.f, dot(normal, h));

	return amb*ambient + dif*diffuse*factor + spe*float4(specular.xyz, 0.f)*pow(s, specular.w);


}
SamplerState sam
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D srvPos;
Texture2D srvNormal;


struct VertexOut
{
	float4 posH:SV_POSITION;
	float2 tex:TEXCOORD;
};
float4 main(VertexOut pin):SV_Target
{
	float4 hitPos = srvPos.SampleLevel(sam, pin.tex, 0);
	float3 normal = normalize(srvNormal.SampleLevel(sam, pin.tex, 0).xyz);


	clip(hitPos.w);
//	[unroll]
	float4 color = float4(0, 0, 0, 0);
	for (int i = 0; i < lightNum; ++i)
	{
		float4 c = PointLight(normal, hitPos.xyz, ambients[i], diffuse[i], speculars[i], values[i]);
			color += c;

	}

	return color;
}