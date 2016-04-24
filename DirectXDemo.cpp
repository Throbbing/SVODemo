#include"DirectXDemo.h"
#include"Vector.h"

#include<windowsx.h>
using namespace jmxRCore;


s32  gMouseX = 0;
s32  gMouseY = 0;
s32  gLastMouseX = 0;
s32  gLastMouseY = 0;
bool gMouseDown = false;
bool gMouseMove = false;
HWND  gHwnd;

LRESULT CALLBACK jmxRCore::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		return 0;
	case WM_SIZE:
		return 0;
	case WM_ENTERSIZEMOVE:

		return 0;
	case WM_EXITSIZEMOVE:

		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		gLastMouseX = GET_X_LPARAM(lParam);
		gLastMouseY = GET_Y_LPARAM(lParam);
		SetCapture(gHwnd);
		return 0;
	case WM_MOUSEMOVE:
		if ((wParam & MK_LBUTTON) != 0)
			gMouseMove = true;
		gMouseX = GET_X_LPARAM(lParam);
		gMouseY = GET_Y_LPARAM(lParam);

		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		ReleaseCapture();
		return 0;

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
void jmxRCore::DirectXDemo::init()
{
	initWindow();
	initD3D();
	gHwnd = mHwnd;
}

void jmxRCore::DirectXDemo::update(f32 dt)
{
	calFPS(dt);
}

void jmxRCore::DirectXDemo::calFPS(f32 dt)
{
	static s32 frameCnt = 0;
	static f32 timeElapsed = 0.0f;

	frameCnt++;

	static f32 totalTime = 0.f;
	totalTime += dt;

	// Compute averages over one second period.
	if ((totalTime - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mHwnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void jmxRCore::DirectXDemo::run()
{
	f32 bgc[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView,
		bgc);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.f, 0);



	mSwapChain->Present(0, 0);
}

void jmxRCore::DirectXDemo::initWindow()
{
	mHinstance = GetModuleHandle(NULL);

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = jmxRCore::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mHinstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"JmxRDemo";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);

	}


	RECT R = { 0, 0, mWidth, mHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mHwnd = CreateWindow(L"JmxRDemo", L"D3DHardDisplay",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mHinstance, 0);
	if (!mHwnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);

	}

	ShowWindow(mHwnd, SW_SHOW);
	UpdateWindow(mHwnd);
}

void jmxRCore::DirectXDemo::initD3D()
{
	UINT createDeviceFlag = 0;
#if defined(DEBUG)|defined(_DEBUG)
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;

#endif // defined(DEBUG)|

	D3D_FEATURE_LEVEL feature;
	HR(D3D11CreateDevice(
		NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, createDeviceFlag,
		0, 0,
		D3D11_SDK_VERSION,
		&md3dDevice,
		&feature,
		&md3dImmediateContext));
	if (feature != D3D_FEATURE_LEVEL_11_0)
	{
		ErrorBox(L"错误，Dx11不被支持!");
		return;
	}


	DXGI_SWAP_CHAIN_DESC sd;

	sd.BufferDesc.Width = mWidth;
	sd.BufferDesc.Height = mHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;


	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	sd.BufferCount = 1;
	sd.OutputWindow = mHwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	//Create DXGISWAPCHAIN
	//first,we need do it by IDXGIFactory
	//
	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));

	ReleaseCom(dxgiDevice);
	ReleaseCom(dxgiAdapter);
	ReleaseCom(dxgiFactory);

	assert(md3dDevice);
	assert(md3dImmediateContext);
	assert(mSwapChain);

	ReleaseCom(mRenderTargetView);
	ReleaseCom(mDepthStencilView);
	ReleaseCom(mDepthStencilBuffer);
	//ReleaseCom(mBackBuffer);


	HR(mSwapChain->ResizeBuffers(
		1,
		mWidth,
		mHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		NULL
		));
	//	ID3D11Texture2D* mBackBuffer;
	HR(mSwapChain->GetBuffer(0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&mBackBuffer)));

	HR(md3dDevice->CreateRenderTargetView(mBackBuffer,
		NULL,
		&mRenderTargetView));


	//	ReleaseCom(mBackBuffer);

	D3D11_TEXTURE2D_DESC td;
	td.Width = mWidth;
	td.Height = mHeight;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = NULL;
	td.MiscFlags = NULL;

	HR(md3dDevice->CreateTexture2D(
		&td,
		NULL,
		&mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(
		mDepthStencilBuffer,
		NULL,
		&mDepthStencilView));

	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	mViewPort.TopLeftX = 0;
	mViewPort.TopLeftY = 0;
	mViewPort.Width = static_cast<float>(mWidth);
	mViewPort.Height = static_cast<float>(mHeight);
	mViewPort.MinDepth = 0.0f;
	mViewPort.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mViewPort);
}