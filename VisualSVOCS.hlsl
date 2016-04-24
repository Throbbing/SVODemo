
cbuffer cbInfo
{
	uint curLevel;
	uint totalLevel;
	uint sumVoxels;
	uint curNode;
};

cbuffer cbAttri
{
	float4 extent;
	float4 origin;
};

cbuffer cbBrickInfo
{
	uint3 brickExtent;
};







struct VisualPackage
{
	float4x4 world;
	float4  normal;
	float4  color;
};

struct Node
{
	uint brickPtr;
	uint childPtr;
};


StructuredBuffer<Node>		nodesPool;

RWBuffer<int>							visualIndex;
RWStructuredBuffer<VisualPackage>		visualPool;
RWTexture3D<uint>			bricksPool_color;
RWTexture3D<uint>			bricksPool_normal;
//RWTexture3D<uint>			bricksPool_irradiance;



float3 u32tofloat3(uint value)
{
	uint R = ((value & 0x00FF0000) >> 16);
	uint G = ((value & 0x0000FF00) >> 8);
	uint B = (value & 0x000000FF);

	return float3(float(R) / 255.f, float(G) / 255.f, float(B) / 255.f);
}

uint rgba2u32(float r, float g, float b, float a)
{
	uint R = uint(r*255.f);
	uint G = uint(g*255.f);
	uint B = uint(b*255.f);
	uint A = uint(a*255.f);
	return  (0 << 24) + (R << 16) + (G << 8) + B;
}


uint extractPos(uint childPtr)
{
	return (childPtr & 0x3FFFFFFF);
}

uint3 brickPtr2Index(uint brickIndex)
{
	uint bz = brickIndex / (brickExtent.x*brickExtent.y);
	brickIndex %= (brickExtent.x*brickExtent.y);
	uint by = brickIndex / brickExtent.x;
	uint bx = brickIndex%brickExtent.x;
	return uint3(bx, by, bz);
}



[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{

	static uint3 loopindex[] = { uint3(0, 0, 0), uint3(1, 0, 0),
		uint3(0, 1, 0), uint3(0, 0, 1),
		uint3(1, 1, 0), uint3(1, 0, 1),
		uint3(0, 1, 1), uint3(1, 1, 1) };
	float cubeSize = extent.x / pow(2.f, curLevel);
	float offset = cubeSize / 2.f;


	

	float x = (DTid.x*cubeSize + offset) / extent.x;
	float y = (DTid.y*cubeSize + offset) / extent.x;
	float z = (DTid.z*cubeSize + offset) / extent.x;

	uint index = 0;
	for (uint level = 0; level < curLevel; ++level)
	{
		uint tt = uint(pow(2, level));
		uint xx = uint(frac(x*tt)*2.f);
		uint yy = uint(frac(y*tt)*2.f);
		uint zz = uint(frac(z*tt)*2.f);
		
		index = extractPos(nodesPool[index].childPtr) + xx + yy * 2 + zz * 4;
		uint ptr = nodesPool[index].childPtr;

		//既没有子节点，也不是叶子节点，则直接跳过
		if ((!(ptr & 0x80000000))&& (!(ptr & 0x40000000)))
			return;
	}

	//读取数据
	uint brickIndex = nodesPool[index].brickPtr;

	uint3 bi = brickPtr2Index(brickIndex);
	bi *= 2;

	//读取8个Voxel
	float3 normal = float3(0, 0, 0);
		float3 color = float3(0, 0, 0);
		float weight = 0;

	for (uint i = 0; i < 8; ++i)
	{
		uint c = bricksPool_color[bi + loopindex[i]];
		uint n = bricksPool_normal[bi + loopindex[i]];

		if (!c&&!n)
			continue;

		weight += 1;
		float3 fc = u32tofloat3(c);
			float3 fn = u32tofloat3(n);

			fn = fn*2.f - 1.f;
		fn = normalize(fn);

		color += fc;
		normal += fn;
	}
	if (weight < 0.1f)
		weight = 1.f;
	color /= weight;
	normal /= weight;

	normal = normalize(normal);

	

	//计算World Mat
	float4x4 world = float4x4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		origin.x + DTid.x*cubeSize + offset,
		origin.y - DTid.y*cubeSize + offset,
		origin.z + DTid.z*cubeSize + offset, 1.f);


	VisualPackage v;
	v.world = world;
	v.normal = float4(1, 1, 1, 1);
	v.color = float4(color, 0.5f);

	int space;
	InterlockedAdd(visualIndex[0], 1, space);

	visualPool[space] = v;
	
}