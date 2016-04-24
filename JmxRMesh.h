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
#include"tiny_obj_loader.h"
#include<string>

#define JmxR_OBJ_DEFAULT_VERTEX  1024
#define JmxR_OBJ_DEFAULT_SUBMESH_NAME  "DEFAULT_SUBMESH"
namespace jmxRCore
{

	struct ObjSubMesh
	{
		std::string		materialName;
		s32			faceStart = 0;
		s32			faceCount = 0;
	};

	struct ObjMaterial
	{
		std::string  materialName;
		Vec4	 ambient = Vec4(1.0F, 1.0F, 1.0F, 1.0F);
		Vec4	 diffuse = Vec4(1.0F, 1.0F, 1.0F, 1.0F);
		Vec4	 specular = Vec4(1.0F, 1.0F, 1.0F, 1.0F);
		std::string	ambientMap;
		std::string	diffuseMap;
		std::string specularMap;
		std::string normalMap;

	};


	class ObjMesh
	{
	public:
		struct ObjVertex
		{
			ObjVertex(){}
			ObjVertex(const Vec3& p,const Vec3& n,const Vec2& t):
				pos(p), normal(n), tex(t){}

			Vec3 pos;
			Vec3 normal;
			Vec2 tex;
			// 32字节
		};
		
		std::vector<ObjSubMesh> subMeshs;
		std::vector<ObjMaterial>materials;
	
		std::vector<ObjVertex>  vertices;
		std::vector<u32>		indices;
		
		
	};


	class ObjLoader
	{
	public:
		ObjLoader() {}
		bool LoadObjMesh(
			std::vector<ObjMesh::ObjVertex>& vertices,
			std::vector<u32>& indices,
			std::vector<ObjSubMesh>& subMeshs,
			std::vector<ObjMaterial>& materials,
			const std::string& fileName,
			const std::string& mtlPath,
			const std::string& texturePath);

	private:
		void ReadMtlFile(const std::string& mtlFile, const std::string& texPath, std::vector<ObjMaterial>& material);
		void ReadVertexPos(std::ifstream& fin, std::vector<Vec3>& verPos);
		void ReadVertexNormal(std::ifstream& fin, std::vector<Vec3>& verNormal);
		void ReadVertexTexCoord(std::ifstream& fin, std::vector<Vec2>& verTexCoord);

		//面索引
		s32 faceStart = 0;
		s32 faceCount = 0;
		//组信息
		s32 groupSize = 0;
		bool hasGroup = false;//判断是否需要生成默认组
	};


	/*
		Obj Utility
	*/
	ObjMesh::ObjVertex getOBJVertexByString(const char* str,
		const std::vector<Vec3>& posInfo,
		const std::vector<Vec2>& texInfo,
		const std::vector<Vec3>& normalInfo
		);

	u32 getFaceByString(const std::string& str,
		const std::vector<Vec3>& posInfo,
		const std::vector<Vec2>& texInfo,
		const std::vector<Vec3>& normalInfo,
		std::vector<ObjMesh::ObjVertex>& vertices,
		std::vector<u32>&		indices,
		u32 VertexIndex
		);

	inline int corObjIndex(int index, int size)
	{
		if (index >= 1)
			return index - 1;
		else
			return size + index;
	}



	void genTangent(const Vec3& p0, const Vec3& p1, const Vec3& p2,
		const Vec2& tex0, const Vec2& tex1, const Vec2& tex2,
		_Out Vec3* T, _Out Vec3* B, _Out Vec3* N);
}