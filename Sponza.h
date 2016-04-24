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
#include"Camera.h"
#include"D3DMesh.h"

namespace jmxRCore
{
	class SponzaDemo :public DirectXDemo
	{
	public:
		SponzaDemo(u32 width, u32 height) :DirectXDemo(width, height){}
		~SponzaDemo()
		{
			ReleaseCom(mVS);
			ReleaseCom(mPS);
			ReleaseCom(mLayout);

			ReleaseCom(mCBWVP);
			delete mSponza;
		}
		virtual void init();
		virtual void update(f32 dt);
		virtual void run();
	private:
		D3DMesh*		mSponza;
		Camera		mCamera;

		ID3D11VertexShader* mVS;
		ID3D11PixelShader*	mPS;
		ID3D11PixelShader*	mPSN;
		ID3D11InputLayout*  mLayout;
		ID3D11SamplerState* mSampler;

		ID3D11Buffer*		mCBWVP;
		ID3D11Buffer*		mCBCamera;
		SRVMgr				mgr;

	};
}