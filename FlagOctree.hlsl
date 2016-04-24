
cbuffer cbInfo
{
	uint curLevel;
	uint totalLevel;
	uint sumVoxels;
	uint curNode;
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
	Fragments List
	Read
*/
StructuredBuffer<Voxel> FragmentsList;



/*
	Nodes Pool
	Read-Write
*/
RWStructuredBuffer<Node>  nodesPool;



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


	Voxel voxel = FragmentsList[voxelIndex];
	uint curIndex = 0;
	for (uint level = 0; level < curLevel; ++level)
	{

		int tt = pow(2.f, level);
		uint xx = uint(frac(voxel.pos.x*tt)*2.f);
		uint yy = uint(frac(voxel.pos.y*tt)*2.f);
		uint zz = uint(frac(voxel.pos.z*tt)*2.f);
		curIndex = extractPos(nodesPool[curIndex].childPtr) + xx + yy * 2 + zz * 4;
	}
	


	if (curLevel == totalLevel - 1)
	{
		
		/*
			赋予叶节点标记，叶节点不需要划分，但是需要赋予brick
		*/
		

		InterlockedOr(nodesPool[curIndex].childPtr, 0x40000000);
//		log[curIndex] = 1;
//		log0[curIndex] = nodesPool[curIndex].childPtr;
			
		
		return;
	}

	InterlockedOr(nodesPool[curIndex].childPtr , 0x80000000);
	
}