/**
*　　　　　　　　┏┓　　　┏┓+ +
*　　　　　　　┏┛┻━━━┛┻┓ + +
*　　　　　　　┃　　　　　　　┃ 　
*　　　　　　　┃　　　━　　　┃ ++ + + +
*　　　　　　 ████━████ ┃+
*　　　　　　　┃　　　　　　　┃ +
*　　　　　　　┃　　　┻　　　┃
*　　　　　　　┃　　　　　　　┃ + +
*　　　　　　　┗━┓　　　┏━┛
*　　　　　　　　　┃　　　┃　　　　　　　　　　　
*　　　　　　　　　┃　　　┃ + + + +
*　　　　　　　　　┃　　　┃　　　　Code is far away from bug with the animal protecting　　　　　　　
*　　　　　　　　　┃　　　┃ + 　　　　神兽保佑,代码无bug　　
*　　　　　　　　　┃　　　┃
*　　　　　　　　　┃　　　┃　　+　　　　　　　　　
*　　　　　　　　　┃　 　　┗━━━┓ + +
*　　　　　　　　　┃ 　　　　　　　┣┓
*　　　　　　　　　┃ 　　　　　　　┏┛
*　　　　　　　　　┗┓┓┏━┳┓┏┛ + + + +
*　　　　　　　　　　┃┫┫　┃┫┫
*　　　　　　　　　　┗┻┛　
*/
#pragma once

#include"JmxRConfig.h"
#include"Scene.h"
#include"tinyxml2.h"
#include"D3DUti.h"

/*
	Node		0			1				2~31
			  划分Flag	   叶Flag		   子节点指针

			  0~31
			 对应的Brick指针
*/



namespace jmxRCore
{

	struct Voxel
	{
		Vec3 pos;
		u32  normal;
		u32  color;
	};

	struct Node
	{
		u32 brickPtr;
		u32 childPtr;
	};

	struct VisualPackage
	{
		Mat4x4 world;
		Vec4 normal;
		Vec4 color;
	};

	struct CBInfo
	{
		u32 curLevel;
		u32 totalLevel;
		u32 sumVoxels;
		u32 curNode;
	};
	void updateCBInfo(ID3D11Buffer* buf, u32 cur, u32 total,u32 sumv,u32 cn,
		ID3D11DeviceContext* dc);


	struct CBTrans
	{
		XMFLOAT4X4 world;
	};
	void updateCBTrans(ID3D11Buffer* buf, CXMMATRIX w,
		ID3D11DeviceContext* dc);

	struct CBProj
	{
		XMFLOAT4X4 vproj[3];
	};
	void updateCBProj(ID3D11Buffer* buf, CXMMATRIX vp0,CXMMATRIX vp1,CXMMATRIX vp2,
		ID3D11DeviceContext* dc);

	struct CBAttri
	{
		Vec4 extent;
		Vec4 origin;
	};
	void updateCBAttri(ID3D11Buffer* buf, Vec3 e, Vec3 o,
		ID3D11DeviceContext* dc);
	struct CBBrickInfo
	{
		u32 extent[3];
	}; 
	void updateCBBrickInfo(ID3D11Buffer* buf, u32 d[3],
		ID3D11DeviceContext* dc);

	struct CBGroupInfo
	{
		u32 groupSize[3];
	};
	void updateCBGroupInfo(ID3D11Buffer* buf, u32 d[3],
		ID3D11DeviceContext* dc);
	
	struct FragmentList
	{
		FragmentList() { uav = nullptr; srv = nullptr; }
		~FragmentList(){ ReleaseCom(uav); ReleaseCom(srv); }
		ID3D11UnorderedAccessView* uav;		//AppendStructuredBuffer<Voxel>   VoxelPS
		ID3D11ShaderResourceView*  srv;		//StructuredBuffer<Voxel>		Flag|WriteLeaf
	};

	struct NodesPool
	{
		NodesPool() { uav = nullptr; srv = nullptr; }
		~NodesPool() { ReleaseCom(uav); ReleaseCom(srv); }
		ID3D11UnorderedAccessView* uav;		//RWStructuredBuffer<Node>    Flag|Alloc
		ID3D11ShaderResourceView*  srv;		//StructuredBuffer<Node>	  WriteLeaf|Mipmap
	};

	struct BricksPool
	{
		BricksPool() { uav[0] = nullptr; uav[1] = nullptr; uav[2] = nullptr; }
		~BricksPool()
		{
			for (u32 i = 0; i < 3; ++i)
				ReleaseCom(uav[i]);
		}
		ID3D11UnorderedAccessView* uav[3];  //RWTexture3D<uint>  WriteLeaf|Mipmap
	};

	struct VisualPool
	{
		VisualPool(){ srv = nullptr; uav = nullptr; }
		~VisualPool(){ ReleaseCom(srv); ReleaseCom(uav); }

		ID3D11ShaderResourceView* srv;
		ID3D11UnorderedAccessView* uav;
	};




	class AtomicIndex
	{
	public:
		AtomicIndex(ID3D11Device* device,ID3D11DeviceContext* context,
			s32* data=nullptr);
		~AtomicIndex()
		{
			ReleaseCom(buf);
			ReleaseCom(staging);
			ReleaseCom(uav);
		}


		ID3D11UnorderedAccessView* operator()()
		{
			return uav;
		}


		s32 getValue();

	


	private:
		ID3D11Buffer*  buf = nullptr;
		ID3D11Buffer*  staging = nullptr;
		ID3D11UnorderedAccessView*  uav = nullptr;
		ID3D11Device*	device = nullptr;
		ID3D11DeviceContext* context = nullptr;

	};

	class AtomicIndexArray
	{
	public:
		AtomicIndexArray(ID3D11Device* device, ID3D11DeviceContext* context,
			u32 count,s32* data=nullptr);
		~AtomicIndexArray()
		{
			ReleaseCom(buf);
			ReleaseCom(staging);
			ReleaseCom(uav);
		}


		ID3D11UnorderedAccessView* operator()()
		{
			return uav;
		}


		std::vector<s32> getValue();




	private:
		ID3D11Buffer*  buf = nullptr;
		ID3D11Buffer*  staging = nullptr;
		ID3D11UnorderedAccessView*  uav = nullptr;
		ID3D11Device*	device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		u32				count;
	};

	
}

