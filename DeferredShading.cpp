#include"DeferredShading.h"


#define FORWARD

using namespace jmxRCore;


extern s32  gMouseX ;
extern s32  gMouseY ;
extern s32  gLastMouseX ;
extern s32  gLastMouseY ;
extern bool gMouseDown ;
extern bool gMouseMove ;
extern HWND  gHwnd;


struct cbTrans
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 wvp;
	
};
#define MaxLight 100
struct cbLights
{
	cbLights(){}
	cbLights(const cbLights& rhs)
	{
		for (u32 i = 0; i < MaxLight; ++i)
		{
			ambients[i] = rhs.ambients[i];
			diffuses[i] = rhs.diffuses[i];
			speculars[i] = rhs.speculars[i];
			values[i] = rhs.values[i];
		}
		lightNum = rhs.lightNum;
	}
	cbLights& operator= (const cbLights& rhs)
	{
		for (u32 i = 0; i < MaxLight; ++i)
		{
			ambients[i] = rhs.ambients[i];
			diffuses[i] = rhs.diffuses[i];
			speculars[i] = rhs.speculars[i];
			values[i] = rhs.values[i];
		}
		lightNum = rhs.lightNum;
		return *this;
	}
	Vec4 ambients[MaxLight];
	Vec4 diffuses[MaxLight];
	Vec4 speculars[MaxLight];
	Vec4 values[MaxLight];
	u32  lightNum ;
};

struct cbMaterial
{
	cbMaterial()
	{
		ambient = Vec4(.2f, .2f, .2f, .2f);
		diffuse = Vec4(.4f, .4f, .4f, .4f);
		specular = Vec4(0.5f, 0.5f, 0.5f, 16);
	}
	Vec4 ambient ;
	Vec4 diffuse ;
	Vec4 specular ;
};

struct QuadV
{
	Vec3 pos;
	Vec2 tex;
};
void DeferredShadingDemo::init()
{
	DirectXDemo::init();

	mScene = SceneParser::loadSceneRtRough("Scene/MyScene.xml");

	mCamera = (RoughCamera*)mScene->getCamera(0);
	for (u32 i = 0; i < mScene->getSum(EMesh); ++i)
	{
		mMeshs.push_back((RoughMesh*)mScene->getMesh(i));
	}
	for (u32 i = 0; i < mScene->getSum(ELight); ++i)
	{
		mLights.push_back((RoughLight*)mScene->getLight(i));
	}

	createShader();
	createBuffer();
}

void DeferredShadingDemo::update(f32 dt)
{
	DirectXDemo::update(dt);

	static float speed = 20.0f;
	if (GetAsyncKeyState('W') & 0x8000)
		mCamera->walk(speed*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCamera->walk(-speed*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCamera->strafe(-speed*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCamera->strafe(speed*dt);

	if (gMouseMove)
	{
		float dx = XMConvertToRadians(0.25f*static_cast<float>(gMouseX - gLastMouseX));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(gMouseY - gLastMouseY));

		mCamera->pitch(dy);
		mCamera->rotateY(dx);


		gLastMouseX = gMouseX;
		gLastMouseY = gMouseY;

		gMouseMove = false;

	}

	if (GetAsyncKeyState('F') & 0x8000)
	{
		mDefer = true;
		std::cout << "Deferred Shading" <<std::endl;
	}
	if (GetAsyncKeyState('R') & 0x8000)
	{
		mDefer = false;
		std::cout << "Forward Shading" << std::endl;
	}

	mCamera->updateMatrix();
}

void DeferredShadingDemo::run()
{
	f32 silver[4] = { .5f, .5f, .5f, .5f };
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, silver);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.f, 0);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->IASetInputLayout(mObjLayout);



	XMMATRIX world;
	XMMATRIX vp = mCamera->getViewProj();

	if (!mDefer)
	{
		md3dImmediateContext->VSSetShader(mForwardVS, nullptr, 0);
		md3dImmediateContext->PSSetShader(mForwardPS, nullptr, 0);

		u32 i = 0;
		for (auto& mesh : mMeshs)
		{
			/*
				Forward Shading
			*/
			world = XMLoadFloat4x4(&JmxR2XM(mesh->worldMat));
			XMMATRIX wvp = world*vp;

			D3D11_MAPPED_SUBRESOURCE ms;
			md3dImmediateContext->Map(mCBTrans, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
			cbTrans* trans = (cbTrans*)ms.pData;
			XMStoreFloat4x4(&trans->world, XMMatrixTranspose(world));
			XMStoreFloat4x4(&trans->wvp, XMMatrixTranspose(wvp));
			md3dImmediateContext->Unmap(mCBTrans, 0);

			ZeroMemory(&ms, sizeof(ms));
			md3dImmediateContext->Map(mCBCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
			XMFLOAT4* cp = (XMFLOAT4*)ms.pData;
			XMFLOAT3 pp = mCamera->getPos();
			*cp = XMFLOAT4(pp.x, pp.y, pp.z, 0.f);
			md3dImmediateContext->Unmap(mCBCamera, 0);

			ID3D11Buffer* bufs[] = { mCBMaterial, mCBLights, mCBCamera };
			md3dImmediateContext->VSSetConstantBuffers(0, 1, &mCBTrans);
			md3dImmediateContext->PSSetConstantBuffers(0, 3, bufs);

			u32 stride = sizeof(ObjMesh::ObjVertex);
			u32 offset = 0;
			md3dImmediateContext->IASetVertexBuffers(0, 1, &mVBs[i], &stride, &offset);
			md3dImmediateContext->IASetIndexBuffer(mIBs[i], DXGI_FORMAT_R32_UINT, 0);
			md3dImmediateContext->DrawIndexed(mesh->indices.size(), 0, 0);

			++i;
		}
	}
	else
	{
		/*
			Deferred Shading
		*/
		md3dImmediateContext->VSSetShader(mForwardVS, nullptr, 0);
		md3dImmediateContext->PSSetShader(mDeferredPS0, nullptr, 0);

		float black[] = { 0.f, 0.f, 0.f, -1.f };
		md3dImmediateContext->ClearRenderTargetView(mRTVPosW, black);
		md3dImmediateContext->ClearRenderTargetView(mRTVNormalW, black);


		ID3D11RenderTargetView* rt[] = { mRTVPosW, mRTVNormalW };
		md3dImmediateContext->OMSetRenderTargets(2, rt, mDepthStencilView);

		u32 i = 0;
		for (auto& mesh : mMeshs)
		{

			world = XMLoadFloat4x4(&JmxR2XM(mesh->worldMat));
			XMMATRIX wvp = world*vp;

			D3D11_MAPPED_SUBRESOURCE ms;
			md3dImmediateContext->Map(mCBTrans, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
			cbTrans* trans = (cbTrans*)ms.pData;
			XMStoreFloat4x4(&trans->world, XMMatrixTranspose(world));
			XMStoreFloat4x4(&trans->wvp, XMMatrixTranspose(wvp));
			md3dImmediateContext->Unmap(mCBTrans, 0);
			md3dImmediateContext->VSSetConstantBuffers(0, 1, &mCBTrans);

			u32 stride = sizeof(ObjMesh::ObjVertex);
			u32 offset = 0;
			md3dImmediateContext->IASetVertexBuffers(0, 1, &mVBs[i], &stride, &offset);
			md3dImmediateContext->IASetIndexBuffer(mIBs[i], DXGI_FORMAT_R32_UINT, 0);
			md3dImmediateContext->DrawIndexed(mesh->indices.size(), 0, 0);

			++i;
		}
		
		md3dImmediateContext->VSSetShader(mDeferredVS, nullptr,0);
		md3dImmediateContext->PSSetShader(mDeferredPS1, nullptr, 0);
		md3dImmediateContext->IASetInputLayout(mQuadLayout);


		md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.f, 0.f); 
		md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);


		D3D11_MAPPED_SUBRESOURCE ms;
		md3dImmediateContext->Map(mCBCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
		XMFLOAT4* cp = (XMFLOAT4*)ms.pData;
		XMFLOAT3 pp = mCamera->getPos();
		*cp = XMFLOAT4(pp.x, pp.y, pp.z, 0.f);
		md3dImmediateContext->Unmap(mCBCamera, 0);

		ID3D11Buffer* bufs[] = { mCBMaterial, mCBLights, mCBCamera };
		md3dImmediateContext->PSSetConstantBuffers(0, 3, bufs);

		ID3D11ShaderResourceView* srvs[] = { mSRVPosW, mSRVNormalW };
		md3dImmediateContext->PSSetShaderResources(0, 2, srvs);


		u32 stride = sizeof(QuadV);
		u32 offset = 0;
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mQuadVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mQuadIB, DXGI_FORMAT_R32_UINT, 0);
		md3dImmediateContext->DrawIndexed(6, 0, 0);

		
		srvs[0] = nullptr;
		srvs[1] = nullptr;
		md3dImmediateContext->PSSetShaderResources(0, 2, srvs);
		


	}



	mSwapChain->Present(0, 0);
}

void DeferredShadingDemo::createShader()
{
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	mForwardVS = (ID3D11VertexShader*)createShaderAndLayout(L"ForwardVS.hlsl",
		nullptr, nullptr, "main", "vs_5_0", EVs, md3dDevice,
		layout, 3, &mObjLayout);

	mForwardPS = (ID3D11PixelShader*)createShaderAndLayout(L"ForwardPS.hlsl",
		nullptr, nullptr, "main", "ps_5_0", EPs, md3dDevice);

	const D3D11_INPUT_ELEMENT_DESC quad[]=
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	mDeferredVS= (ID3D11VertexShader*)createShaderAndLayout(L"DeferredVS.hlsl",
		nullptr, nullptr, "main", "vs_5_0", EVs, md3dDevice,
		quad, 2, &mQuadLayout);
	mDeferredPS0 = (ID3D11PixelShader*)createShaderAndLayout(L"DeferredPS0.hlsl",
		nullptr, nullptr, "main", "ps_5_0", EPs, md3dDevice);
	mDeferredPS1 = (ID3D11PixelShader*)createShaderAndLayout(L"DeferredPS1.hlsl",
		nullptr, nullptr, "main", "ps_5_0", EPs,md3dDevice);

}

void DeferredShadingDemo::createBuffer()
{
	for (auto& mesh : mMeshs)
	{
		D3D11_BUFFER_DESC vbd;
		ZeroMemory(&vbd, sizeof(vbd));
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.ByteWidth = sizeof(ObjMesh::ObjVertex)*mesh->vertices.size();
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		vbd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA vsd;
		vsd.pSysMem = &mesh->vertices[0];

		ID3D11Buffer* vb;
		HR(md3dDevice->CreateBuffer(&vbd, &vsd, &vb));
		mVBs.push_back(vb);


		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.ByteWidth = sizeof(u32)*mesh->indices.size();
		ibd.MiscFlags = 0;
		ibd.CPUAccessFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA isd;
		isd.pSysMem = &mesh->indices[0];
		ID3D11Buffer* ib;
		HR(md3dDevice->CreateBuffer(&ibd, &isd, &ib));
		mIBs.push_back(ib);
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = PAD16(sizeof(cbTrans));
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.StructureByteStride = 0;
	bd.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer(&bd, 0, &mCBTrans));

	bd.ByteWidth = PAD16(sizeof(cbMaterial));
	HR(md3dDevice->CreateBuffer(&bd, 0, &mCBMaterial));

	bd.ByteWidth = PAD16(sizeof(cbLights));
	HR(md3dDevice->CreateBuffer(&bd, 0, &mCBLights));


	bd.ByteWidth = sizeof(XMFLOAT4);
	HR(md3dDevice->CreateBuffer(&bd, 0, &mCBCamera));


	D3D11_MAPPED_SUBRESOURCE ms;
	md3dImmediateContext->Map(mCBMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	cbMaterial* mat = (cbMaterial*)ms.pData;
	*mat = cbMaterial();
	md3dImmediateContext->Unmap(mCBMaterial, 0);

	cbLights lights;
	for (u32 i = 0; i < mLights.size(); ++i)
	{
		lights.ambients[i] = mLights[i]->ambient;
		lights.diffuses[i] = mLights[i]->diffuse;
		lights.speculars[i] = mLights[i]->specular;
		lights.values[i] = mLights[i]->para;
	}
	lights.lightNum = mLights.size();
	ZeroMemory(&ms, sizeof(ms));
	md3dImmediateContext->Map(mCBLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	cbLights* l = (cbLights*)ms.pData;
	*l = lights;
	md3dImmediateContext->Unmap(mCBLights, 0);



	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	td.Width = mWidth;
	td.Height = mHeight;
	td.MipLevels = 1;
	td.MiscFlags = 0;
	td.SampleDesc.Quality = 0;
	td.SampleDesc.Count = 1;
	td.CPUAccessFlags = 0;
	td.ArraySize = 1;

	ID3D11Texture2D* texPos;
	ID3D11Texture2D* texNormal;
	HR(md3dDevice->CreateTexture2D(&td, 0, &texPos));
	HR(md3dDevice->CreateTexture2D(&td, 0, &texNormal));

	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	rtvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	HR(md3dDevice->CreateRenderTargetView(texPos, &rtvd, &mRTVPosW));
	HR(md3dDevice->CreateRenderTargetView(texNormal, &rtvd, &mRTVNormalW));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	HR(md3dDevice->CreateShaderResourceView(texPos, &srvd, &mSRVPosW));
	HR(md3dDevice->CreateShaderResourceView(texNormal, &srvd, &mSRVNormalW));

	ReleaseCom(texPos);
	ReleaseCom(texNormal);



	QuadV qv[4];
	qv[0].pos = Vec3(-1.f, -1.f, 0);
	qv[0].tex = Vec2(0.f, 1.f);

	qv[1].pos = Vec3(-1.f, 1.f, 0);
	qv[1].tex = Vec2(0.f, 0.f);

	qv[2].pos = Vec3(1.f, 1.f,0.f);
	qv[2].tex = Vec2(1.f, 0.f);
	
	qv[3].pos = Vec3(1.f, -1.f, 0.f);
	qv[3].tex = Vec2(1.f, 1.f);

	D3D11_BUFFER_DESC qvbd;
	ZeroMemory(&qvbd, sizeof(qvbd));
	qvbd.Usage = D3D11_USAGE_IMMUTABLE;
	qvbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	qvbd.ByteWidth = sizeof(QuadV) * 4;
	qvbd.CPUAccessFlags = 0;
	qvbd.MiscFlags = 0;
	qvbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA qsd;
	qsd.pSysMem = qv;

	HR(md3dDevice->CreateBuffer(&qvbd, &qsd, &mQuadVB));

	u32 i[6] =
	{	0, 1, 2,
		2, 3, 0
	};

	D3D11_BUFFER_DESC qibd;
	ZeroMemory(&qibd, sizeof(qibd));
	qibd.Usage = D3D11_USAGE_IMMUTABLE;
	qibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	qibd.ByteWidth = sizeof(u32) * 6;
	qibd.CPUAccessFlags = 0;
	qibd.MiscFlags = 0;
	qibd.StructureByteStride = 0;

	qsd.pSysMem = i;

	HR(md3dDevice->CreateBuffer(&qibd, &qsd, &mQuadIB));

	

}