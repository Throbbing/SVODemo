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
#include<vector>
namespace jmxRCore
{
	struct OBJSubMesh
	{
		OBJSubMesh(tinyobj::shape_t& shape)
		{
			matID = shape.mesh.material_ids[0];

			positions.reserve(shape.mesh.positions.size() / 3);
			for (u32 p = 0; p < shape.mesh.positions.size() / 3; ++p)
			{
				positions.push_back(Vec3(shape.mesh.positions[p * 3 + 0],
					shape.mesh.positions[p * 3 + 1],
					shape.mesh.positions[p * 3 + 2]));
			}

			normals.reserve(shape.mesh.normals.size() / 3);
			for (u32 n = 0; n < shape.mesh.normals.size() / 3; ++n)
			{
				normals.push_back(Vec3(shape.mesh.normals[n * 3 + 0],
					shape.mesh.normals[n * 3 + 1],
					shape.mesh.normals[n * 3 + 2]));
			}

			texs.reserve(shape.mesh.texcoords.size() / 2);
			for (u32 t = 0; t < shape.mesh.texcoords.size() / 2; ++t)
			{
				texs.push_back(Vec2(shape.mesh.texcoords[t * 2 + 0],
					1.f-shape.mesh.texcoords[t * 2 + 1]));
			}

			tangents.resize(normals.size());
			indices = shape.mesh.indices;
			name = shape.name;
		}

		std::string  name;
		u32  matID;
		std::vector<Vec3> positions;
		std::vector<Vec3> normals;
		std::vector<Vec2> texs;
		std::vector<Vec3> tangents;
		std::vector<u32>  indices;
	};

	class OBJMesh
	{
	public:
		OBJMesh(std::vector<tinyobj::shape_t>& shapes,
			std::vector<tinyobj::material_t>& mats)
		{
			subMeshs.reserve(shapes.size());
			for (auto& s : shapes)
				subMeshs.push_back(OBJSubMesh(s));

			materials = mats;

		}

		std::vector<OBJSubMesh>		subMeshs;
		std::vector<tinyobj::material_t> materials;
		
	};
}