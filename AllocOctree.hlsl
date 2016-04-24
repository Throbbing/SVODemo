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



/*
	Nodes Pool
	Read-Write
*/
RWStructuredBuffer<Node>  nodesPool;

RWBuffer<int> brickIndex;

RWBuffer<int> numNode;

RWBuffer<int> nodeIndex;


[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x + DTid.y*groupSize.x;
	if (index >= curNode)
		return;


	uint offset = 0;
	for (uint i = 0; i < curLevel; ++i)
		offset += numNode[i];
	
	uint pos = offset + index;

	if (nodesPool[pos].childPtr & 0x40000000)
	{
		/*
			如果是叶子节点，不需要划分，但是需要分配brick
		*/
		uint indexBrickSpace;
		InterlockedAdd(brickIndex[0], 1, indexBrickSpace);
		nodesPool[pos].brickPtr = indexBrickSpace;
		return;
	}
	if (nodesPool[pos].childPtr & 0x80000000)
	{
		
		InterlockedAdd(numNode[curLevel + 1], 8);
		

		uint indexSpace;
		InterlockedAdd(nodeIndex[0], 8, indexSpace);
		nodesPool[pos].childPtr |= indexSpace;

		
		uint indexBrickSpace;
		InterlockedAdd(brickIndex[0], 1, indexBrickSpace);
		nodesPool[pos].brickPtr = indexBrickSpace;
	}
	


}