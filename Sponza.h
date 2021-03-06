/**
*　　　　　　　　�芥掘　　？芥�+ +
*　　　　　　　�芥食潯ォォォ食潯� + +
*　　　　　　　�А　　　　　　？� 　
*　　　　　　　�А　　？ァ　　？� ++ + + +
*　　　　　　 ���������エ������� ��+
*　　　　　　　�А　　　　　　？� +
*　　　　　　　�А　　？漾　　？�
*　　　　　　　�А　　　　　　？� + +
*　　　　　　　�皐ォ掘　　？芥ォ�
*　　　　　　　　　�А　　？А　　　　　　　　　　�
*　　　　　　　　　�А　　？� + + + +
*　　　　　　　　　�А　　？А　　　�Code is far away from bug with the animal protecting　　　　　　　
*　　　　　　　　　�А　　？� + 　　　　舞舗隠嗷,旗鷹涙bug　　
*　　　　　　　　　�А　　？�
*　　　　　　　　　�А　　？А　�+　　　　　　　　　
*　　　　　　　　　�А� 　　�皐ォォォ� + +
*　　　　　　　　　�� 　　　　　　　�禰�
*　　　　　　　　　�� 　　　　　　　�芥�
*　　　　　　　　　�皐粂粂芥ォ廩粂芥� + + + +
*　　　　　　　　　　�З路蓮？З路�
*　　　　　　　　　　�皐潯拭�
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