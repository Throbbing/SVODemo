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
#include"DirectXDemo.h"
#include"Scene.h"
#include"SceneParser.h"

namespace jmxRCore
{
	class DeferredShadingDemo :public DirectXDemo
	{
	public:

		DeferredShadingDemo(u32 width,u32 height) :DirectXDemo(width,height){}
		~DeferredShadingDemo()
		{
			if (mScene)
				delete mScene;
			ReleaseCom(mDeferredVS);
			ReleaseCom(mForwardVS);
			ReleaseCom(mDeferredPS0);
			ReleaseCom(mDeferredPS1);
			ReleaseCom(mForwardPS);
			ReleaseCom(mObjLayout);
			ReleaseCom(mQuadLayout);
			ReleaseCom(mRTVPosW);
			ReleaseCom(mRTVNormalW);
			ReleaseCom(mSRVPosW);
			ReleaseCom(mSRVNormalW);

			ReleaseCom(mCBLights);
			ReleaseCom(mCBMaterial);
			ReleaseCom(mCBTrans);
			ReleaseCom(mCBCamera);

			ReleaseCom(mQuadVB);
			ReleaseCom(mQuadIB);
			for (auto& p : mVBs)
				ReleaseCom(p);
			for (auto& p : mIBs)
				ReleaseCom(p);
		}
		
		virtual void init();
		virtual void update(f32 dt);
		virtual void run();
	private:
		void createBuffer();
		void createShader();
		Scene*						mScene = nullptr;
		ID3D11VertexShader*			mDeferredVS = nullptr;
		ID3D11VertexShader*			mForwardVS = nullptr;
		ID3D11PixelShader*			mDeferredPS0 = nullptr;
		ID3D11PixelShader*			mDeferredPS1 = nullptr;
		ID3D11PixelShader*			mForwardPS = nullptr;
		ID3D11InputLayout*			mObjLayout = nullptr;
		ID3D11InputLayout*			mQuadLayout = nullptr;

		ID3D11RenderTargetView*		mRTVPosW = nullptr;
		ID3D11RenderTargetView*		mRTVNormalW = nullptr;
		ID3D11ShaderResourceView*   mSRVPosW = nullptr;
		ID3D11ShaderResourceView*	mSRVNormalW = nullptr;

		RoughCamera*				mCamera;
		std::vector<RoughMesh*>		mMeshs;
		std::vector<RoughLight*>	mLights;

		std::vector<ID3D11Buffer*>	mVBs;
		std::vector<ID3D11Buffer*>	mIBs;

		ID3D11Buffer*			mQuadVB;
		ID3D11Buffer*			mQuadIB;

		ID3D11Buffer*				mCBLights;
		ID3D11Buffer*				mCBMaterial;
		ID3D11Buffer*				mCBTrans;
		ID3D11Buffer*				mCBCamera;



		bool						mDefer = false;
	};
}

