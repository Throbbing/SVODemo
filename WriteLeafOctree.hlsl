
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

struct Voxel
{
	float3 pos;
	uint normal;
	uint color;
};

/*
	Fragment List
*/
StructuredBuffer<Voxel>  fragmentsList;

/*
	Nodes Pool
*/
StructuredBuffer<Node>  nodesPool;

/*
	Brick Pool
*/
RWTexture3D<uint> bricksPool_color;
RWTexture3D<uint> bricksPool_normal;
RWTexture3D<uint> bricksPool_irradiance;



uint extractPos(uint childPtr)
{
	return (childPtr & 0x3FFFFFFF);
}


[numthreads(16, 16, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	
	uint voxelIndex = DTid.x + DTid.y*groupSize.x * 16;
	if (voxelIndex >= sumVoxels)
		return;
	Voxel voxel = fragmentsList[voxelIndex];
	uint index = 0;

	for (uint level = 0; level < totalLevel-1 ; ++level)
	{
		uint tt = uint(pow(2, level));
		uint x = uint(frac(voxel.pos.x*tt)*2.f);
		uint y = uint(frac(voxel.pos.y*tt)*2.f);
		uint z = uint(frac(voxel.pos.z*tt)*2.f);
		index = extractPos(nodesPool[index].childPtr) + x + y * 2 + z * 4;
	}

	/*
		到达叶节点
		对应叶节点对应的Brick
	*/
	uint brickIndex = nodesPool[index].brickPtr;
	uint bz = brickIndex / (brickExtent.x*brickExtent.y);
	brickIndex %= (brickExtent.x*brickExtent.y);
	uint by = brickIndex / brickExtent.x;
	uint bx = brickIndex%brickExtent.x;

	/*
		1个Brick由2*2*2个Voxel 组成
	*/
	uint ttt = uint(pow(2, totalLevel - 2));
	uint vx = uint(frac(voxel.pos.x*ttt) * 2);
	uint vy = uint(frac(voxel.pos.y*ttt) * 2);
	uint vz = uint(frac(voxel.pos.z*ttt) * 2);

	


	bricksPool_color[uint3(bx * 2 + vx, by * 2 + vy, bz * 2 + vz)] = voxel.color;
	bricksPool_normal[uint3(bx * 2 + vx, by * 2 + vy, bz * 2 + vz)] = voxel.normal;
	bricksPool_irradiance[uint3(bx * 2 + vx, by * 2 + vy, bz * 2 + vz)] =11111;
	
//	bricksPool_color[uint3(bx * 2, by * 2, bz * 2)] = voxel.color;
	


}