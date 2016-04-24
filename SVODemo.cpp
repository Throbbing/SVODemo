#include"SVODemo.h"
#include"Scene.h"
#include"SceneParser.h"
#include<algorithm>
#include<numeric>
extern s32  gMouseX;
extern s32  gMouseY;
extern s32  gLastMouseX;
extern s32  gLastMouseY;
extern bool gMouseDown;
extern bool gMouseMove;
extern HWND  gHwnd;

void jmxRCore::SVODemo::init()
{
	DirectXDemo::init();
	
	/*
		读取场景
	*/
	mScene = SceneParser::loadSceneRtRough("Scene/MyScene.xml");
	mCamera = (RoughCamera*)mScene->getCamera(0);
	for (u32 i = 0; i < mScene->getSum(EMesh); ++i)
	{
		RoughMesh* m = (RoughMesh*)mScene->getMesh(i);
		mMeshs.push_back(new D3DMesh(*m,m->localBBox,JmxR2XM(m->worldMat),mMgr,md3dDevice));
	}
	for (u32 i = 0; i < mScene->getSum(ELight); ++i)
	{
		mLights.push_back((RoughLight*)mScene->getLight(i));
	}



	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = false;
	rd.DepthClipEnable = false;
	HR(md3dDevice->CreateRasterizerState(&rd, &mNoCullRs));

	BBox bbox = mScene->getBBox();
	u32 maxSide = 0;
	for (u32 i = 0; i < 3; ++i)
	{
		if (maxSide < bbox.extent(i))
			maxSide = bbox.extent(i);
	}
	mOrthoVP.Width = MaxLevelRes;
	mOrthoVP.Height = MaxLevelRes;
	mOrthoVP.TopLeftX = 0.f;
	mOrthoVP.TopLeftY = 0.f;
	mOrthoVP.MinDepth = 0.f;
	mOrthoVP.MaxDepth = 1.f;

	createShader();
	createBuf();


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

void jmxRCore::SVODemo::update(f32 dt)
{
	DirectXDemo::update(dt);
	static float speed = 200.0f;
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

	

	mCamera->updateMatrix();

}

void jmxRCore::SVODemo::run()
{

	float silver[] = { .5f, .5f, .5f, .5f };
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, silver);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.f, 0);
	SVO();
	

	visualSVO();


	mSwapChain->Present(0, 0);

}

void jmxRCore::SVODemo::createShader()
{
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,32,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};
	/*
		体素化部分shader
	*/
	mVoxelVS = (ID3D11VertexShader*)createShaderAndLayout(L"VoxelizationVS.hlsl", nullptr, nullptr,
		"main", "vs_5_0", EVs, md3dDevice, layout, 4, &mObjLayout);
	mVoxelGS = (ID3D11GeometryShader*)createShaderAndLayout(L"VoxelizationGS.hlsl", nullptr, nullptr,
		"main", "gs_5_0",EGs, md3dDevice);
	mVoxelPS = (ID3D11PixelShader*)createShaderAndLayout(L"VoxelizationPS.hlsl", nullptr, nullptr,
		"main", "ps_5_0", EPs, md3dDevice);

	/*
		Build Octree
	*/
	mFlagCS = (ID3D11ComputeShader*)createShaderAndLayout(L"FlagOctree.hlsl", nullptr, nullptr,
		"main", "cs_5_0", ECs, md3dDevice);
	mAllocCS = (ID3D11ComputeShader*)createShaderAndLayout(L"AllocOctree.hlsl", nullptr, nullptr,
		"main", "cs_5_0", ECs, md3dDevice);
	mWriteLeafCS = (ID3D11ComputeShader*)createShaderAndLayout(L"WriteLeafOctree.hlsl", nullptr, nullptr,
		"main", "cs_5_0", ECs, md3dDevice);
	mMipmapCS = (ID3D11ComputeShader*)createShaderAndLayout(L"MipmapOctree.hlsl", nullptr, nullptr,
		"main", "cs_5_0", ECs, md3dDevice);


	/*
		Visual Pool
	*/
	const D3D11_INPUT_ELEMENT_DESC instanceLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INDEX", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	mVisualVS = (ID3D11VertexShader*)createShaderAndLayout(L"VisualSVOVS.hlsl", nullptr, nullptr,
		"main", "vs_5_0", EVs, md3dDevice,
		instanceLayout, 2, &mInstanceLayout);
	mVisualPS = (ID3D11PixelShader*)createShaderAndLayout(L"VisualSVOPS.hlsl", nullptr, nullptr,
		"main", "ps_5_0", EPs, md3dDevice);
	mVisualCS = (ID3D11ComputeShader*)createShaderAndLayout(L"VisualSVOCS.hlsl", nullptr, nullptr,
		"main", "cs_5_0", ECs, md3dDevice);


}

void jmxRCore::SVODemo::createBuf()
{
	/*
		创建Constant Buffer
	*/
	mCBTrans = createConstantBuffer(PAD16(sizeof(CBTrans)), md3dDevice);
	mCBProj = createConstantBuffer(PAD16(sizeof(CBProj)), md3dDevice);
	mCBInfo = createConstantBuffer(PAD16(sizeof(CBInfo)), md3dDevice);
	mCBAttri = createConstantBuffer(PAD16(sizeof(CBAttri)), md3dDevice);
	mCBBrickInfo = createConstantBuffer(PAD16(sizeof(CBBrickInfo)), md3dDevice);
	mCBGroupInfo = createConstantBuffer(PAD16(sizeof(CBGroupInfo)), md3dDevice);

	ID3D11Buffer* buf;

	/*
		Fragment
	*/
	u32 fragmentSize = sizeof(Voxel)*
		MaxLevelRes*
		MaxLevelRes*
		MaxLevelRes*
		FramgmentMultiples;
	buf = createBuffer(D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
		fragmentSize,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, 0, sizeof(Voxel), nullptr,
		md3dDevice);
	mFragmentList.srv = createSRV(buf,
		DXGI_FORMAT_UNKNOWN, D3D11_SRV_DIMENSION_BUFFEREX,
		fragmentSize / sizeof(Voxel), 0, 0,
		md3dDevice);
	mFragmentList.uav = createUAV(buf,
		DXGI_FORMAT_UNKNOWN, D3D11_UAV_DIMENSION_BUFFER,
		fragmentSize / sizeof(Voxel), D3D11_BUFFER_UAV_FLAG_APPEND, 0,
		md3dDevice);

	ReleaseCom(buf);

	/*
		Node Pool
	*/
	mTotalLevel = std::log2f(MaxLevelRes) + 1;
	mTotalNode = 0;
	u32 res = MaxLevelRes;
	while (res)
	{
		mTotalNode += (res*res*res);
		res /= 2;
	}
//	mTotalNode /= 4;
	buf = createBuffer(D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
		mTotalNode*sizeof(Node),
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, 0,
		sizeof(Node), nullptr,
		md3dDevice);
	mNodesPool.srv = createSRV(buf, DXGI_FORMAT_UNKNOWN,
		D3D11_SRV_DIMENSION_BUFFEREX,
		mTotalNode, 0, 0,
		md3dDevice);
	mNodesPool.uav = createUAV(buf, DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER,
		mTotalNode, 0, 0,
		md3dDevice);
	ReleaseCom(buf);





	
	

}

void jmxRCore::SVODemo::createBricksPool()
{
	u32 sum = mTotalBrick;

	u32 r = (u32)std::powf(sum, 0.333334f);
	while ((r*r*r) < sum)
		++r;


	u32 dd[3] = { r, r, r };
	updateCBBrickInfo(mCBBrickInfo, dd, md3dImmediateContext);

	for (u32 i = 0; i < 3; ++i)
	{
		D3D11_TEXTURE3D_DESC td;
		ZeroMemory(&td, sizeof(td));
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		td.Format = DXGI_FORMAT_R32_UINT;
		td.Width = r*BrickRes;
		td.Height = r*BrickRes;
		td.Depth = r*BrickRes;
		td.MipLevels = 1;
		td.MiscFlags = 0;
		td.CPUAccessFlags = 0;
		ID3D11Texture3D* tex;
		HR(md3dDevice->CreateTexture3D(&td, 0, &tex)); 

		mBricksPool.uav[i] = createUAV(tex, DXGI_FORMAT_R32_UINT,
			D3D11_UAV_DIMENSION_TEXTURE3D,
			r*BrickRes, 0, 0,
			md3dDevice);

		ReleaseCom(tex);
	}
}

void jmxRCore::SVODemo::SVO()
{
	static bool once = false;

	if (once)
		return;

	once = true;

	/*
		体素化
	*/
	
	
	float silver[] = { .5f, .5f, .5f, .5f };
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, silver);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.f, 0);
	md3dImmediateContext->RSSetViewports(1, &mOrthoVP);
	md3dImmediateContext->RSSetState(mNoCullRs);
	
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->IASetInputLayout(mObjLayout);
	
	//Step 1: 设置相机和三个透视矩阵
	BBox sceneBBox = mScene->getBBox();
	f32 maxSide = 0;
	for (u32 i = 0; i < 3; ++i)
	{
		if (maxSide < sceneBBox.extent(i))
			maxSide = sceneBBox.extent(i);
	}
	//z方向
	XMFLOAT3 cameraPos;
	cameraPos = XMFLOAT3((sceneBBox.minP.x + sceneBBox.maxP.x)*0.5f,
		(sceneBBox.minP.y + sceneBBox.maxP.y)*0.5f,
		sceneBBox.minP.z);
	mOrthoCamera.setTarget(cameraPos, XMFLOAT3(cameraPos.x, cameraPos.y, cameraPos.z + 1),
		XMFLOAT3(0, 1, 0));
	mOrthoCamera.updateMatrix();
	XMMATRIX view0 = mOrthoCamera.getView();

	//x方向
	cameraPos = XMFLOAT3(sceneBBox.maxP.x,
		(sceneBBox.minP.y + sceneBBox.maxP.y)*0.5f,
		(sceneBBox.minP.z + sceneBBox.maxP.z)*0.5f);
	mOrthoCamera.setTarget(cameraPos, XMFLOAT3(cameraPos.x - 1, cameraPos.y, cameraPos.z),
		XMFLOAT3(0, 1, 0));
	mOrthoCamera.updateMatrix();
	XMMATRIX view1 = mOrthoCamera.getView();

	//y方向
	cameraPos = XMFLOAT3((sceneBBox.minP.x + sceneBBox.maxP.x)*0.5f,
		sceneBBox.maxP.y,
		(sceneBBox.minP.z + sceneBBox.maxP.z)*0.5f);
	mOrthoCamera.setTarget(cameraPos, XMFLOAT3(cameraPos.x, cameraPos.y - 1, cameraPos.z),
		XMFLOAT3(0, 0, 1));
	mOrthoCamera.updateMatrix();
	XMMATRIX view2 = mOrthoCamera.getView();

	XMMATRIX proj = XMMatrixOrthographicLH(maxSide, maxSide, 0.f, maxSide);
	updateCBProj(mCBProj, view0*proj, view1*proj, view2*proj,
		md3dImmediateContext);

	//Step 2: 设置cbAttri ，绑定shader和Buffer到GPU
	updateCBAttri(mCBAttri, Vec3(maxSide+2, maxSide+2, maxSide+2),
		Vec3(sceneBBox.minP.x, sceneBBox.maxP.y, sceneBBox.minP.z),
		md3dImmediateContext);

	md3dImmediateContext->VSSetShader(mVoxelVS, nullptr, 0);
	md3dImmediateContext->GSSetShader(mVoxelGS, nullptr, 0);
	md3dImmediateContext->PSSetShader(mVoxelPS, nullptr, 0);


	md3dImmediateContext->GSSetConstantBuffers(0, 1, &mCBProj);


	md3dImmediateContext->PSSetConstantBuffers(0, 1, &mCBAttri);
	md3dImmediateContext->PSSetSamplers(0, 1, &mSampler);
	UINT pad[] = { 0, 0 };
	AtomicIndex voxelIndex(md3dDevice,md3dImmediateContext);
	ID3D11UnorderedAccessView* voxelUAVBuf[] = { mFragmentList.uav, voxelIndex() };
	md3dImmediateContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr,
		0, 2, voxelUAVBuf, pad);

	//Step 3:渲染场景，进行体素化
	md3dImmediateContext->RSSetState(mNoCullRs);
	for (u32 i = 0; i < mMeshs.size(); ++i)
	{
		XMMATRIX world = XMLoadFloat4x4(&mMeshs[i]->world);

		updateCBTrans(mCBTrans, world, md3dImmediateContext);
		md3dImmediateContext->VSSetConstantBuffers(0, 1, &mCBTrans);

		u32 stride = sizeof(D3DMesh::D3DVertex);
		u32 offset = 0;
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mMeshs[i]->vb, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mMeshs[i]->ib, DXGI_FORMAT_R32_UINT, 0);

		for (auto &sm : mMeshs[i]->subMeshs)
		{
			md3dImmediateContext->PSSetShaderResources(0, 1, &mMeshs[i]->materials[sm.matID].diffuseMap);
			md3dImmediateContext->DrawIndexed(sm.idxCount, 
				sm.idxOffset, 
				sm.vdxOffset);
		}
	}

	mTotalVoxel = voxelIndex.getValue();
	
	/*
		构建八叉树结构
	*/

	
	//Step 1:清空绑定
	md3dImmediateContext->VSSetShader(nullptr, nullptr, 0);
	md3dImmediateContext->GSSetShader(nullptr, nullptr, 0);
	md3dImmediateContext->PSSetShader(nullptr, nullptr, 0);

	ID3D11Buffer* sb[] = { nullptr };
	md3dImmediateContext->VSSetConstantBuffers(0, 1, sb);
	md3dImmediateContext->PSSetConstantBuffers(0, 1, sb);
	md3dImmediateContext->GSSetConstantBuffers(0, 1, sb);

	md3dImmediateContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr,
		0, 0, nullptr, nullptr);


	//Step 2：自顶向下构建树
	s32 brickInit = 0;
	s32 nodeInit = 1;
	AtomicIndex brickIndex(md3dDevice, md3dImmediateContext, &brickInit);
	AtomicIndex nodeIndex(md3dDevice, md3dImmediateContext, &nodeInit);
	std::vector<s32> initNum; initNum.push_back(1);
	for (u32 i = 1; i < mTotalLevel; ++i)
		initNum.push_back(0);
 	AtomicIndexArray numNode(md3dDevice, md3dImmediateContext, mTotalLevel, &initNum[0]);


	
	for (u32 level = 0; level < mTotalLevel ; ++level)
	{

		updateCBInfo(mCBInfo, level, mTotalLevel, mTotalVoxel,0,md3dImmediateContext);
		//先Flag
		md3dImmediateContext->CSSetShader(mFlagCS, nullptr, 0);
		md3dImmediateContext->CSSetShaderResources(0, 1, &mFragmentList.srv);
		md3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &mNodesPool.uav,nullptr);
		md3dImmediateContext->CSSetConstantBuffers(0, 1, &mCBInfo);
		u32 sqrVoxel = (u32)std::sqrtf((f32)mTotalVoxel);
		u32 dd[] = { PAD16(sqrVoxel) / VoxelDispathUnit, PAD16(sqrVoxel) / VoxelDispathUnit, 1 };
		updateCBGroupInfo(mCBGroupInfo, dd, md3dImmediateContext);
		md3dImmediateContext->CSSetConstantBuffers(1, 1, &mCBGroupInfo);
		md3dImmediateContext->Dispatch(dd[0],dd[1],1);

		//Alloc
		ID3D11ShaderResourceView* spacesrv[] = { nullptr };
		md3dImmediateContext->CSSetShaderResources(0, 1, spacesrv);
		md3dImmediateContext->CSSetShader(mAllocCS, nullptr, 0);
		ID3D11UnorderedAccessView* uavs[] = { mNodesPool.uav, brickIndex(), numNode(), nodeIndex() };
		md3dImmediateContext->CSSetUnorderedAccessViews(0, 4, uavs, nullptr);

		mNumNodePerLevel.push_back(numNode.getValue()[level]);
		u32 sqrNode = (u32)std::sqrtf((f32)mNumNodePerLevel[level]);
		u32 ddd[] = { sqrNode + 1, sqrNode + 1, 1 };
		updateCBInfo(mCBInfo, level, mTotalLevel, mTotalVoxel, mNumNodePerLevel[level], 
			md3dImmediateContext);
		updateCBGroupInfo(mCBGroupInfo, ddd, md3dImmediateContext);
		
		md3dImmediateContext->CSSetConstantBuffers(0, 1, &mCBInfo);
		md3dImmediateContext->CSSetConstantBuffers(1, 1, &mCBGroupInfo);

		md3dImmediateContext->Dispatch(ddd[0], ddd[1], 1);	
		u32 bbb = brickIndex.getValue();

		//清空绑定
		uavs[0] = uavs[1] = uavs[2] = uavs[3] = nullptr;
		md3dImmediateContext->CSSetUnorderedAccessViews(0, 4, uavs, nullptr);
		ID3D11Buffer* spacebuf[] = { nullptr };
		md3dImmediateContext->CSSetConstantBuffers(0, 1, spacebuf);
		md3dImmediateContext->CSSetConstantBuffers(1, 1, spacebuf);
		
	}
	mTotalBrick = brickIndex.getValue();


	


	/*
		填充数据，并进行Mipmap
	*/
	
	//Step 1:根据所有Brick，创建BrickPool（节省空间开销）
	createBricksPool();

	
	
	//Step 2:将数据写入叶子节点

	md3dImmediateContext->CSSetShader(mWriteLeafCS, nullptr, 0);
	u32 sqrVoxel = (u32)std::sqrtf((f32)mTotalVoxel);
	u32 dd[] = { PAD16(sqrVoxel) / VoxelDispathUnit, PAD16(sqrVoxel) / VoxelDispathUnit, 1 };
	updateCBGroupInfo(mCBGroupInfo, dd, md3dImmediateContext);
	ID3D11Buffer* cb[] = { mCBInfo, mCBBrickInfo,mCBGroupInfo };
	md3dImmediateContext->CSSetConstantBuffers(0, 3, cb);
	ID3D11ShaderResourceView* srvs[] = { mFragmentList.srv, mNodesPool.srv };
	md3dImmediateContext->CSSetShaderResources(0, 2, srvs);
	ID3D11UnorderedAccessView* uavlog[] = { mBricksPool.uav[0],
		mBricksPool.uav[1],
		mBricksPool.uav[2]
	};
	md3dImmediateContext->CSSetUnorderedAccessViews(0, 3, uavlog,nullptr);
	


	md3dImmediateContext->Dispatch(dd[0], dd[1], 1);
	


\
	
	
	
	//Step 3:自底向上进行Mipmap
	cb[0] = cb[1] = cb[2] = nullptr;
	srvs[0] = srvs[1] = nullptr;
	ID3D11UnorderedAccessView* uavs[] = { nullptr, nullptr, nullptr };
	md3dImmediateContext->CSSetConstantBuffers(0, 3, cb);
	md3dImmediateContext->CSSetShaderResources(0, 2, srvs);
	md3dImmediateContext->CSSetUnorderedAccessViews(0, 3, uavs, nullptr);

\

	//倒数第二层开始(最后一层为叶子节点)
	for (s32 level = mTotalLevel - 2; level >=0; --level)
	{
		md3dImmediateContext->CSSetShader(mMipmapCS, nullptr, 0);

		u32 sqrNode = (u32)std::sqrtf((f32)mNumNodePerLevel[level]);
		u32 ddd[] = { sqrNode + 1, sqrNode + 1, 1 };	
		updateCBInfo(mCBInfo, level, mTotalLevel, mTotalVoxel, mNumNodePerLevel[level],
			md3dImmediateContext);
		updateCBGroupInfo(mCBGroupInfo, ddd, md3dImmediateContext);
		cb[0] = mCBInfo; cb[1] = mCBBrickInfo; cb[2] = mCBGroupInfo;
		md3dImmediateContext->CSSetConstantBuffers(0, 3, cb);
		md3dImmediateContext->CSSetShaderResources(0, 1, &mNodesPool.srv);

		ID3D11UnorderedAccessView* srv4[] = { numNode(), 
			mBricksPool.uav[0], mBricksPool.uav[1], mBricksPool.uav[2] };
		md3dImmediateContext->CSSetUnorderedAccessViews(0, 4, srv4,nullptr);

		md3dImmediateContext->Dispatch(ddd[0], ddd[1], 1);
		
		
	}
	

	ID3D11UnorderedAccessView* srv4[] = { nullptr, nullptr, nullptr, nullptr };
	md3dImmediateContext->CSSetUnorderedAccessViews(0, 4, srv4, nullptr);
	md3dImmediateContext->RSSetViewports(1, &mViewPort);

	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	f32 f[] = { 0, 0, 0, 0 };
	md3dImmediateContext->OMSetBlendState(nullptr, f, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(nullptr, 1);

	createVisual(CURLEVEL);
}

void jmxRCore::SVODemo::createVisual(u32 level)
{
	ReleaseCom(mVisualVB);
	ReleaseCom(mVisualIB);
	ReleaseCom(mVisualInstanceBuf);
	ReleaseCom(mVisualPool.srv);
	ReleaseCom(mVisualPool.uav);

	/*
		创建Cube
	*/
	f32 res = std::powf(2.f, level);
	BBox box = mScene->getBBox();
	f32 maxSide = 0;
	for (u32 i = 0; i < 3; ++i)
	{
		if (maxSide < box.extent(i))
			maxSide = box.extent(i);
	}
	f32 size = (maxSide / res)*0.5f;

	XMFLOAT3  v[8];
	v[0] = XMFLOAT3(-size, -size, -size);
	v[1] = XMFLOAT3(-size, size, -size);
	v[2] = XMFLOAT3(size, size, -size);
	v[3] = XMFLOAT3(size, -size, -size);

	v[4] = XMFLOAT3(-size, -size, size);
	v[5] = XMFLOAT3(-size, size, size);
	v[6] = XMFLOAT3(size, size, size);
	v[7] = XMFLOAT3(size, -size, size);


	//创建Buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * 8;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vbsd;
	vbsd.pSysMem = v;

	HR(md3dDevice->CreateBuffer(&vbd, &vbsd, &mVisualVB));


	u32 indices[36];

	//前面
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 2; indices[4] = 3; indices[5] = 0;

	//左面
	indices[6] = 4; indices[7] = 5; indices[8] = 1;
	indices[9] = 1; indices[10] = 0; indices[11] = 4;

	//上面	
	indices[12] = 1; indices[13] = 5; indices[14] = 6;
	indices[15] = 6; indices[16] = 2; indices[17] = 1;

	//右面
	indices[18] = 2; indices[19] = 6; indices[20] = 7;
	indices[21] = 7; indices[22] = 3; indices[23] = 2;

	//下面
	indices[24] = 4; indices[25] = 0; indices[26] = 3;
	indices[27] = 3; indices[28] = 7; indices[29] = 4;

	//后面
	indices[30] = 7; indices[31] = 6; indices[32] = 5;
	indices[33] = 5; indices[34] = 4; indices[35] = 7;

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = sizeof(u32) * 36;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA isd;
	isd.pSysMem = indices;

	HR(md3dDevice->CreateBuffer(&ibd, &isd, &mVisualIB));


	/*
		根据SVO后，每层的节点，创建Visual Pool
	*/
	ID3D11Buffer* buf;
	buf = createBuffer(D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
		mNumNodePerLevel[level] * sizeof(VisualPackage),
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, 0,
		sizeof(VisualPackage), nullptr,
		md3dDevice);
	mVisualPool.srv = createSRV(buf, DXGI_FORMAT_UNKNOWN,
		D3D11_SRV_DIMENSION_BUFFEREX,
		mNumNodePerLevel[level], 0, 0,
		md3dDevice);
	mVisualPool.uav = createUAV(buf, DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER,
		mNumNodePerLevel[level], 0, 0,
		md3dDevice);

	/*
		更新cbuffer
	*/
	updateCBInfo(mCBInfo, level, mTotalLevel, mTotalVoxel,mNumNodePerLevel[level],
		md3dImmediateContext);
	updateCBAttri(mCBAttri, Vec3(maxSide, maxSide, maxSide),
		Vec3(box.minP.x, box.maxP.y, box.minP.z), md3dImmediateContext);
	

	/*
		绑定Buffer到CS
	*/
	s32 i[] = { 0 };
	AtomicIndex visualIndex(md3dDevice, md3dImmediateContext,i);
	
	md3dImmediateContext->CSSetShader(mVisualCS, nullptr, 0);
	
	ID3D11Buffer* csbuf[] = { mCBInfo, mCBAttri, mCBBrickInfo };
	md3dImmediateContext->CSSetConstantBuffers(0, 3, csbuf);
	md3dImmediateContext->CSSetShaderResources(0, 1, &mNodesPool.srv);
	
	ID3D11UnorderedAccessView* csuavs[] = { visualIndex(),mVisualPool.uav,
		mBricksPool.uav[0],
		mBricksPool.uav[1]
//		mBricksPool.uav[2],
		
		 };
	md3dImmediateContext->CSSetUnorderedAccessViews(0, 4, csuavs,nullptr);
	

	u32 ures = u32(res);
	md3dImmediateContext->Dispatch(ures, ures, ures);

	
	

	
	/*
		解除Buf
	*/
	
	md3dImmediateContext->CSSetShader(nullptr, nullptr, 0);
	csbuf[0] = nullptr; csbuf[1] = nullptr; csbuf[2] = nullptr;
	csuavs[0] = nullptr; csuavs[1] = nullptr; csuavs[2] = nullptr;
	csuavs[3] = nullptr;// csuavs[4] = nullptr;
	md3dImmediateContext->CSSetConstantBuffers(0, 3, csbuf);
	md3dImmediateContext->CSSetUnorderedAccessViews(0, 4, csuavs, nullptr);
	ID3D11ShaderResourceView* srv[] = { nullptr };
	md3dImmediateContext->CSSetShaderResources(0, 1, srv);
	
	
	/*
		创建Instances
	*/
	
	u32 sum = visualIndex.getValue();
	std::vector<u32> instances;
	for (u32 i = 0; i < sum; ++i)
		instances.push_back(i);
	vbd.ByteWidth = sizeof(u32)*sum;

	D3D11_SUBRESOURCE_DATA insd;
	insd.pSysMem = &instances[0];

	HR(md3dDevice->CreateBuffer(&vbd, &insd, &mVisualInstanceBuf));
	
	
	

}

void jmxRCore::SVODemo::visualSVO()
{
	f32 silver[] = { 0.5f, 0.5f, 0.5f, 0.5f };
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, silver);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	md3dImmediateContext->IASetInputLayout(mInstanceLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	u32 stride[] = { sizeof(XMFLOAT3), sizeof(u32) };
	u32 offset[] = { 0, 0 };
	ID3D11Buffer* vbs[] = { mVisualVB, mVisualInstanceBuf };
	md3dImmediateContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
	md3dImmediateContext->IASetIndexBuffer(mVisualIB, DXGI_FORMAT_R32_UINT,0);

	
	XMMATRIX vp = mCamera->getViewProj();
	updateCBTrans(mCBTrans, vp, md3dImmediateContext);

	md3dImmediateContext->VSSetShader(mVisualVS, nullptr, 0);
	md3dImmediateContext->PSSetShader(mVisualPS, nullptr, 0);

	md3dImmediateContext->VSSetConstantBuffers(0, 1, &mCBTrans); 
	md3dImmediateContext->VSSetShaderResources(0, 1, &mVisualPool.srv);

	md3dImmediateContext->DrawIndexedInstanced(36, mNumNodePerLevel[CURLEVEL],
		0, 0, 0);

	



}