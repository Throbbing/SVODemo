cbuffer cbInfo
{
	uint curLevel;
	uint totalLevel;
	uint sumVoxels;
	uint curNode;
};

cbuffer cbBrickInfo
{
	uint3 brickExtent;
};

cbuffer cbGroupInfo
{
	uint3 groupSize;
};

struct Node
{
	uint brickPtr;
	uint childPtr;
};
/*
	Nodes Pool
*/

StructuredBuffer<Node>  nodesPool;

RWBuffer<int> numNode;

/*
	Brick Pool
*/


RWTexture3D<uint> bricksPool_color;
RWTexture3D<uint> bricksPool_normal;
RWTexture3D<uint> bricksPool_irradiance;


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

	uint index = DTid.x + DTid.y*groupSize.x;
	if (index >= curNode)
		return;

	uint offset = 0;

	for (uint level = 0; level < curLevel; ++level)
	{
		offset += numNode[level];
	}
	index += offset ;
	if (!(nodesPool[index].childPtr & 0x80000000))
		return;

	uint childPos = (nodesPool[index].childPtr & 0x3FFFFFFF);
	uint3 fb = brickPtr2Index(nodesPool[index].brickPtr);
	fb *= 2;

		uint3 loopindex[] = { uint3(0, 0, 0), uint3(1, 0, 0),
		uint3(0, 1, 0), uint3(0, 0, 1),
		uint3(1, 1, 0), uint3(1, 0, 1),
		uint3(0, 1, 1), uint3(1, 1, 1) };
		
		for (uint i = 0; i < 8; ++i)
		{
			uint3 bi = brickPtr2Index(nodesPool[childPos++].brickPtr);
				bi *= 2;
			/*
			float3 c = u32tofloat3(bricksPool_color[bi]);
				c += u32tofloat3(bricksPool_color[bi + uint3(1, 0, 0)]);
			c += u32tofloat3(bricksPool_color[bi + uint3(0, 1, 0)]);
			c += u32tofloat3(bricksPool_color[bi + uint3(0, 0, 1)]);
			c += u32tofloat3(bricksPool_color[bi + uint3(1, 1, 0)]);
			c += u32tofloat3(bricksPool_color[bi + uint3(1, 0, 1)]);
			c += u32tofloat3(bricksPool_color[bi + uint3(0, 1, 1)]);
			c += u32tofloat3(bricksPool_color[bi + uint3(1, 1, 1)]);
			c /= 8.f;

			float3 n = u32tofloat3(bricksPool_normal[bi]);

				n += u32tofloat3(bricksPool_normal[bi + uint3(1, 0, 0)]);
			n += u32tofloat3(bricksPool_normal[bi + uint3(0, 1, 0)]);
			n += u32tofloat3(bricksPool_normal[bi + uint3(0, 0, 1)]);
			n += u32tofloat3(bricksPool_normal[bi + uint3(1, 1, 0)]);
			n += u32tofloat3(bricksPool_normal[bi + uint3(1, 0, 1)]);
			n += u32tofloat3(bricksPool_normal[bi + uint3(0, 1, 1)]);
			n += u32tofloat3(bricksPool_normal[bi + uint3(1, 1, 1)]);
			n /= 8.f;

			
			uint ir = bricksPool_irradiance[bi] / 8;
			ir += bricksPool_irradiance[bi + uint3(1, 0, 0)] / 8;
			ir += bricksPool_irradiance[bi + uint3(0, 1, 0)] / 8;
			ir += bricksPool_irradiance[bi + uint3(0, 0, 1)] / 8;
			ir += bricksPool_irradiance[bi + uint3(1, 1, 0)] / 8;
			ir += bricksPool_irradiance[bi + uint3(1, 0, 1)] / 8;
			ir += bricksPool_irradiance[bi + uint3(0, 1, 1)] / 8;
			ir += bricksPool_irradiance[bi + uint3(1, 1, 1)] / 8;
			*/
			float weight = 0.f;
			float3 c = float3(0.f, 0.f, 0.f);
				float3 n = float3(0.f, 0.f, 0.f);
			for (uint j = 0; j < 8; ++j)
			{
				uint ic = bricksPool_color[bi + loopindex[j]];
				uint inn = bricksPool_normal[bi + loopindex[j]];

				if (!ic)
					continue;

				weight += 1.f;

				c += u32tofloat3(ic);
				n += u32tofloat3(inn);
			}
			if (weight < 0.01f)
				weight = 1.f;

			c /= weight;
			n /= weight;
			n = normalize(n);

			bricksPool_color[fb + loopindex[i]] = rgba2u32(c.x,c.y,c.z,0.f);
			bricksPool_normal[fb + loopindex[i]] = rgba2u32(n.x,n.y,n.z,0.f);
			bricksPool_irradiance[fb + loopindex[i]] = 0;


		}
					
					
					
		
}