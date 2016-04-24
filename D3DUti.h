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
#include"Vector.h"
#include"Matrix.h"
#include<d3d11.h>
#include<d3dx11.h>
#include<dxerr.h>
#include<xnamath.h>
#include<map>

#if defined(DEBUG) | defined(_DEBUG)

#ifndef HR

#define  HR(x)																			\
{																						\
	HRESULT hr = (x);																	\
if (FAILED(hr))																		\
	{																					\
	DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true);								\
	}																					\
}

#endif // !HR

#else
#ifndef HR
#define HR(x) (x)
#endif // !HR

#endif // defined(DEBUG) 

#define ReleaseCom(p) {if(p)p->Release();p=NULL;}
#define ErrorBox(text) {MessageBox(0, text, 0, 0);}


#define PAD16(x)		((x+15)&(~15))

namespace jmxRCore
{
	enum EShader
	{
		EVs,
		EGs,
		EHs,
		EDs,
		ECs,
		EPs,

	};
	void* createShaderAndLayout(LPCTSTR srcFile,
		const D3D10_SHADER_MACRO * macro,
		LPD3D10INCLUDE include,
		LPCSTR name, LPCSTR sm, EShader es,
		ID3D11Device* device,
		const D3D11_INPUT_ELEMENT_DESC* desc = nullptr,
		u32 eleNum = 0,
		ID3D11InputLayout** layout = nullptr);


	ID3D11Buffer* createConstantBuffer(u32 byteWidth,ID3D11Device* device,
		D3D11_USAGE usage = D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_FLAG cpuFlag = D3D11_CPU_ACCESS_WRITE
		);

	ID3D11ShaderResourceView* createSRV(ID3D11Resource* src, DXGI_FORMAT format,
		D3D11_SRV_DIMENSION dim,
		u32 arg0, u32 arg1, u32 arg2,
		ID3D11Device* device);

	ID3D11UnorderedAccessView* createUAV(ID3D11Resource* src, DXGI_FORMAT format,
		D3D11_UAV_DIMENSION dim,
		u32 arg0, u32 arg1, u32 arg2,
		ID3D11Device* device);

	ID3D11Buffer* createBuffer(D3D11_USAGE usage, u32 bindflag,
		u32 byteWidth,
		u32 misc,
		u32 cpuflag,
		u32 stride,void* data,
		ID3D11Device* device);


	inline Mat4x4 XM2JmxR(XMFLOAT4X4 mat)
	{
		return Mat4x4(mat._11,mat._12,mat._13,mat._14,
			mat._21,mat._22,mat._23,mat._24,
			mat._31,mat._32,mat._33,mat._34,
			mat._41, mat._42, mat._43, mat._44);
	}
	inline Vec4 XM2JmxR(XMFLOAT4 vec)
	{
		return Vec4(vec.x, vec.y, vec.z, vec.w);
	}
	inline XMFLOAT4X4 JmxR2XM(Mat4x4 mat)
	{
		return XMFLOAT4X4(mat(0, 0), mat(0, 1), mat(0, 2), mat(0, 3),
			mat(1, 0), mat(1, 1), mat(1, 2), mat(1, 3),
			mat(2, 0), mat(2, 1), mat(2, 2), mat(2, 3),
			mat(3, 0), mat(3, 1), mat(3, 2), mat(3, 3));
	}
	inline XMFLOAT4 JmxR2XM(Vec4 vec)
	{
		return XMFLOAT4(vec.x, vec.y, vec.z, vec.w);
	}


	
	class SRVMgr
	{
	public:
		ID3D11ShaderResourceView* load(const std::string& name,
			ID3D11Device* device)
		{
			if (srvs.find(name) != srvs.end())
				return srvs[name];

			ID3D11ShaderResourceView* srv;
			HR(D3DX11CreateShaderResourceViewFromFileA(device, name.c_str(),
				nullptr, nullptr, &srv, nullptr));

			srvs[name] = srv;

			return srvs[name];
		}

		~SRVMgr()
		{
			for (auto& p : srvs)
				ReleaseCom(p.second);
		}

	private:
		std::map<std::string, ID3D11ShaderResourceView*>  srvs;
	};

	

}


