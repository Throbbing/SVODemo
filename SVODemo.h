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

#pragma  once

#include"SVOUti.h"
#include"DirectXDemo.h"
#include"D3DMesh.h"
#define MaxLevelRes 256
#define FramgmentMultiples 1
#define BrickRes	2
#define BricksPoolRes 128
#define VoxelDispathUnit 16
#define NodeDispatchUnit 4
#define CURLEVEL 7

#define PAD32(x) ((x+31)&(~31))
#define PAD4(x)  ((x+3)&(~3))

namespace jmxRCore
{
	class SVODemo :public DirectXDemo
	{
	public:
		SVODemo(u32 width, u32 height) :DirectXDemo(width, height){}
		~SVODemo()
		{
			ReleaseCom(mVoxelVS);
			ReleaseCom(mVoxelGS);
			ReleaseCom(mVoxelPS);
			ReleaseCom(mVisualVS);
			ReleaseCom(mVisualPS);

			ReleaseCom(mFlagCS);
			ReleaseCom(mAllocCS);
			ReleaseCom(mWriteLeafCS);
			ReleaseCom(mMipmapCS);
			ReleaseCom(mVisualCS);

			ReleaseCom(mCBTrans);
			ReleaseCom(mCBProj);
			ReleaseCom(mCBAttri);
			ReleaseCom(mCBInfo);
			ReleaseCom(mCBBrickInfo);
			ReleaseCom(mCBGroupInfo);

			ReleaseCom(mObjLayout);
			ReleaseCom(mInstanceLayout);
			ReleaseCom(mNoCullRs);
			ReleaseCom(mSampler);

			ReleaseCom(mVisualVB);
			ReleaseCom(mVisualIB);
			ReleaseCom(mVisualInstanceBuf);

			for (auto& p : mMeshs)
				delete p;
			if (mScene)
				delete mScene;
		}
		virtual void init();
		virtual void update(f32 dt);
		virtual void run();

	private:
		void createShader();
		void createBuf();
		void createBricksPool();
		void createVisual(u32 level);
		void SVO();
		void visualSVO();

		ID3D11VertexShader*    mVoxelVS;
		ID3D11GeometryShader*  mVoxelGS;
		ID3D11PixelShader*	   mVoxelPS;
		ID3D11VertexShader*	   mVisualVS;
		ID3D11PixelShader*	   mVisualPS;

		ID3D11ComputeShader*   mFlagCS;
		ID3D11ComputeShader*   mAllocCS;
		ID3D11ComputeShader*   mWriteLeafCS;
		ID3D11ComputeShader*   mMipmapCS;
		ID3D11ComputeShader*   mVisualCS = nullptr;

		ID3D11Buffer*		   mCBTrans;
		ID3D11Buffer*		   mCBProj;
		ID3D11Buffer*		   mCBAttri;
		ID3D11Buffer*		   mCBInfo;
		ID3D11Buffer*		   mCBBrickInfo;
		ID3D11Buffer*		   mCBGroupInfo;

		ID3D11RasterizerState* mNoCullRs;
		ID3D11InputLayout*	   mObjLayout;
		ID3D11InputLayout*     mInstanceLayout;

		ID3D11SamplerState*	   mSampler;

		D3D11_VIEWPORT		   mOrthoVP;

		FragmentList		   mFragmentList;
		NodesPool			   mNodesPool;
		BricksPool			   mBricksPool;
		VisualPool			   mVisualPool;

		std::vector<s32>	   mNumNodePerLevel;
		u32					   mTotalLevel;
		u32					   mTotalNode;
		u32					   mTotalVoxel;
		u32					   mTotalBrick;

		Camera				   mOrthoCamera;

		RoughCamera*				mCamera;
		std::vector<D3DMesh*>		mMeshs;
		std::vector<RoughLight*>	mLights;

		ID3D11Buffer*				mVisualVB;
		ID3D11Buffer*				mVisualIB;
		ID3D11Buffer*				mVisualInstanceBuf;

		Scene*				   mScene;

		SRVMgr						mMgr;
		
	};
}