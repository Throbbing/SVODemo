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
#include"D3DUti.h"
#include"OBJMesh.h"
#include"jmxRMesh.h"
#include"Geometry.h"
namespace jmxRCore
{
	struct D3DSubMesh
	{
		std::string name;
		u32 matID;
		u32 idxCount;
		u32 idxOffset;
		u32	vdxOffset;
	};

	struct D3DMaterial
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;

		ID3D11ShaderResourceView* ambientMap = nullptr;
		ID3D11ShaderResourceView* diffuseMap = nullptr;
		ID3D11ShaderResourceView* bumpMap = nullptr;
		ID3D11ShaderResourceView* alphaMap = nullptr;
	};


	class D3DMesh
	{
	
	public:
		struct D3DVertex
		{
			Vec3 pos;
			Vec3 normal;
			Vec2 tex;
			Vec3 tangent;
		};
		D3DMesh(const OBJMesh& mesh,
			const BBox& localBox,
			const XMFLOAT4X4& w,
			SRVMgr& mgr,
			ID3D11Device* device )
		{
			localBBox = localBox;
			world = w;


			for (auto& m : mesh.subMeshs)
			{
				D3DSubMesh sm;
				sm.name = m.name;
				sm.matID = m.matID;
				sm.idxCount = m.indices.size();
				sm.idxOffset = indices.size();
				sm.vdxOffset = vertices.size();
				subMeshs.push_back(sm);
				D3DVertex v;
				for (u32 i = 0; i < m.positions.size(); ++i)
				{
					v.pos = m.positions[i];
					v.normal = m.normals[i];
					v.tex = m.texs[i];
					v.tangent = m.tangents[i];
					vertices.push_back(v);
				}
				indices.insert(indices.end(), m.indices.begin(), m.indices.end());

			}

			for (auto& mat : mesh.materials)
			{
				D3DMaterial m;
				m.ambient = XMFLOAT4(mat.ambient[0], mat.ambient[1], mat.ambient[2],0.f);
				m.diffuse = XMFLOAT4(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], mat.dissolve);
				m.specular = XMFLOAT4(mat.specular[0], mat.specular[1], mat.specular[2], mat.shininess);

				if (!mat.ambient_texname.empty())
					m.ambientMap = mgr.load(mat.ambient_texname,device);
				if (!mat.diffuse_texname.empty())
					m.diffuseMap = mgr.load(mat.diffuse_texname, device);
				if (!mat.bump_texname.empty())
					m.bumpMap = mgr.load(mat.bump_texname, device);
				if (!mat.alpha_texname.empty())
					m.alphaMap = mgr.load(mat.alpha_texname, device);

				materials.push_back(m);

			}

			vb = createBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER,
				sizeof(D3DVertex)*vertices.size(),
				0, 0, 0,
				&vertices[0], device);
			ib = createBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER,
				sizeof(u32)*indices.size(),
				0, 0, 0,
				&indices[0], device);

			hasTangent = false;

		}
		void genTangent(ID3D11Device* device)
		{
			if (hasTangent)
				return;

			for (auto& m : subMeshs)
			{
				for (u32 i = 0; i < m.idxCount / 3; ++i)
				{
					D3DVertex& v0 = vertices[m.vdxOffset + indices[m.idxOffset + i * 3 + 0]];
					D3DVertex& v1 = vertices[m.vdxOffset + indices[m.idxOffset + i * 3 + 1]];
					D3DVertex& v2 = vertices[m.vdxOffset + indices[m.idxOffset + i * 3 + 2]];

					Vec3 T, B, N;
					jmxRCore::genTangent(v0.pos, v1.pos, v2.pos,
						v0.tex, v1.tex, v2.tex,
						&T, &B, &N);

					v0.tangent = T;
					v1.tangent = T;
					v2.tangent = T;
				}
			}

			ReleaseCom(vb);
			ReleaseCom(ib);
			vb = createBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER,
				sizeof(D3DVertex)*vertices.size(),
				0, 0, 0,
				&vertices[0], device);
			ib = createBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER,
				sizeof(u32)*indices.size(),
				0, 0, 0,
				&indices[0], device);
			hasTangent = true;
		}
		~D3DMesh()
		{
			ReleaseCom(vb);
			ReleaseCom(ib);
		}
		ID3D11Buffer*	vb;
		ID3D11Buffer*	ib;
		BBox		localBBox;
		XMFLOAT4X4  world;

		std::vector<D3DMaterial> materials;
		std::vector<D3DSubMesh>	 subMeshs;
	private:
		std::vector<D3DVertex>	vertices;
		std::vector<u32>		indices;
		bool					hasTangent;
		
	};


	
}