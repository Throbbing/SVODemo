#include"Sponza.h"

extern s32  gMouseX;
extern s32  gMouseY;
extern s32  gLastMouseX;
extern s32  gLastMouseY;
extern bool gMouseDown;
extern bool gMouseMove;
extern HWND  gHwnd;
using namespace jmxRCore;

void SponzaDemo::init()
{
	DirectXDemo::init();

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	tinyobj::LoadObj(shapes, materials, err, "Mesh/sponza.obj", "Mesh/mtl/",true);
	if (!err.empty())
		cout << err << std::endl;
	mSponza = new D3DMesh(OBJMesh(shapes, materials), BBox(), XMFLOAT4X4(),mgr,md3dDevice);
	mSponza->genTangent(md3dDevice);

	D3D11_INPUT_ELEMENT_DESC input[]=
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TANGENT" ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,32,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	mVS = (ID3D11VertexShader*)createShaderAndLayout(L"SponzaVS.hlsl",
		nullptr, nullptr,"main" ,
		"vs_5_0", EVs, md3dDevice,
		input, 4, &mLayout);
	mPS = (ID3D11PixelShader*)createShaderAndLayout(L"SponzaPS.hlsl",
		nullptr, nullptr, "main",
		"ps_5_0", EPs, md3dDevice);
	mPSN = (ID3D11PixelShader*)createShaderAndLayout(L"SponzaPSN.hlsl",
		nullptr, nullptr, "main",
		"ps_5_0", EPs, md3dDevice);

	mCBWVP = createConstantBuffer(sizeof(XMFLOAT4X4), md3dDevice);
	mCBCamera = createConstantBuffer(PAD16(sizeof(XMFLOAT3)), md3dDevice);
	mCamera.setLens(PI / 4, f32(mWidth) / f32(mHeight), 1.f, 5000.f);


	D3D11_SAMPLER_DESC sd;
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.MaxAnisotropy = 16;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.MipLODBias = 0;
	sd.MinLOD = -FLT_MAX;
	sd.MaxLOD = FLT_MAX;
	HR(md3dDevice->CreateSamplerState(&sd, &mSampler));
}

void SponzaDemo::update(f32 dt)
{
	DirectXDemo::update(dt);

	static float speed = 200.0f;
	if (GetAsyncKeyState('W') & 0x8000)
		mCamera.walk(speed*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCamera.walk(-speed*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCamera.strafe(-speed*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCamera.strafe(speed*dt);

	if (gMouseMove)
	{
		float dx = XMConvertToRadians(0.25f*static_cast<float>(gMouseX - gLastMouseX));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(gMouseY - gLastMouseY));

		mCamera.pitch(dy);
		mCamera.rotateY(dx);


		gLastMouseX = gMouseX;
		gLastMouseY = gMouseY;

		gMouseMove = false;

	}

	mCamera.updateMatrix();
}

void SponzaDemo::run()
{
	f32 silver[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, silver);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	md3dImmediateContext->IASetInputLayout(mLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	u32 stride = sizeof(D3DMesh::D3DVertex);
	u32 offset = 0;

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mSponza->vb, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mSponza->ib, DXGI_FORMAT_R32_UINT, 0);


	XMMATRIX world = XMMatrixIdentity();
	XMMATRIX vp = mCamera.getViewProj();
	XMMATRIX wvp = world*vp;

	D3D11_MAPPED_SUBRESOURCE ms;
	md3dImmediateContext->Map(mCBWVP, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	XMFLOAT4X4* p = (XMFLOAT4X4*)ms.pData;
	XMStoreFloat4x4(p, XMMatrixTranspose(wvp));
	md3dImmediateContext->Unmap(mCBWVP, 0);
	md3dImmediateContext->Map(mCBCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	XMFLOAT3* po = (XMFLOAT3*)ms.pData;
	*po = mCamera.getPos();
	md3dImmediateContext->Unmap(mCBCamera, 0);

	md3dImmediateContext->VSSetShader(mVS, nullptr, 0);

	md3dImmediateContext->PSSetSamplers(0, 1, &mSampler);
	md3dImmediateContext->VSSetConstantBuffers(0, 1, &mCBWVP);
	md3dImmediateContext->PSSetConstantBuffers(0, 1, &mCBCamera);
	for (u32 m = 0; m < mSponza->subMeshs.size(); ++m)
	{	

		ID3D11ShaderResourceView* srv[] =
		{
			mSponza->materials[mSponza->subMeshs[m].matID].diffuseMap,
			mSponza->materials[mSponza->subMeshs[m].matID].bumpMap
		};
		md3dImmediateContext->PSSetShaderResources(0, 2,
			srv);
		
//		if (mSponza->subMeshs[m].matID != 6)
//			continue;

		if (mSponza->materials[mSponza->subMeshs[m].matID].bumpMap)
		{
			md3dImmediateContext->PSSetShader(mPSN, nullptr, 0);
		}
		else
		{
			md3dImmediateContext->PSSetShader(mPS, nullptr, 0);
		}

			md3dImmediateContext->DrawIndexed(mSponza->subMeshs[m].idxCount,
				mSponza->subMeshs[m].idxOffset,
				mSponza->subMeshs[m].vdxOffset);


	
	}

	mSwapChain->Present(0, 0);

}