#include"JmxRMesh.h"
#include<fstream>
#include"Math.h"
using namespace jmxRCore;

bool jmxRCore::ObjLoader::LoadObjMesh(
	std::vector<ObjMesh::ObjVertex>& vertices,
	std::vector<u32>& indices,
	std::vector<ObjSubMesh>& subMeshs,
	std::vector<ObjMaterial>& materials,
	const std::string& fileName,
	const std::string& mtlPath,
	const std::string& texturePath
	)
{
	std::ifstream fin;
	fin.open(fileName);
	if (!fin.good())
	{
		std::cout<<(L"δ�ҵ�OBJ�ļ�-LoadObjMesh")<<std::endl;
		system("pause");
		exit(0);
		return false;
	}
	std::vector<Vec3> posInfo;
	std::vector<Vec3> normalInfo;
	std::vector<Vec2> texCoordInfo;
	posInfo.reserve(JmxR_OBJ_DEFAULT_VERTEX);
	normalInfo.reserve(JmxR_OBJ_DEFAULT_VERTEX);
	texCoordInfo.reserve(JmxR_OBJ_DEFAULT_VERTEX);
	char str[50];
	//usemtl�ؼ��� ����
	int subMeshIndex = -1;
	//������
	int faceIndex = -1;
	bool faceStartFlag = false;
	const  u32 ignoreSize = 200;
	char ignore[ignoreSize];

#pragma region Parsering
	//Parsering
	while (fin >> str)
	{
		switch (str[0])
		{
			//��ȡ���ʿ�
		case 'm':
			if (strcmp(str, "mtllib") == 0)
			{
				char libName[40];
				fin >> libName;
				ReadMtlFile(mtlPath + std::string(libName), texturePath, materials);
			}
			else
			{
				//���Ƕ�ȡ���ʿ�Ĺؼ���
				//����
				fin.getline(ignore, ignoreSize);
			}
			break;
			//v��ͷ����Ҫ�ж���v,vt,����vn
		case 'v':
		{
			switch (str[1])
			{
			case 't':ReadVertexTexCoord(fin, texCoordInfo); break;
			case 'n':ReadVertexNormal(fin, normalInfo); break;
			default:ReadVertexPos(fin, posInfo); break;

			}
		}
		break;


		//����Ϣ
		//û�б�Ҫ���ա��顯������SubMesh
		//SubMesh��Ҫ�����ǲ��ʵĲ�ͬ
		case 'u':
			char subName[100];
			if (strcmp(str, "usemtl") == 0)
			{
				++subMeshIndex;
				faceStartFlag = true;
				fin >> subName;
				ObjSubMesh subMesh;
				subMesh.materialName = std::string(subName);
				subMeshs.push_back(subMesh);

			}
			else
			{
				//�������usemtl�ؼ��֣���������һ��
				fin.getline(ignore, ignoreSize);
				std::cout << "���棺�зǷ�u��ͷ�ؼ���-LoadOBJMesh"<<std::endl;
			}
			break;
		case 'f':
		{	++faceIndex;
		if (subMeshIndex < 0)
		{
			//����һ���յ�SubMesh
			++subMeshIndex;
			faceStartFlag = true;
			ObjSubMesh subMesh;
			subMesh.materialName = std::string(JmxR_OBJ_DEFAULT_SUBMESH_NAME);
			subMeshs.push_back(subMesh);
		}
		if (faceStartFlag)
		{
			subMeshs[subMeshIndex].faceStart = faceIndex;
			faceStartFlag = false;
		}
		char vertex[200];
		fin.getline(vertex, 200);
		u32 fnum = getFaceByString(vertex,
			posInfo,
			texCoordInfo,
			normalInfo,
			vertices,
			indices,
			vertices.size()
			);
		faceIndex += (fnum - 1);
		subMeshs[subMeshIndex].faceCount += fnum;

		break;
		}
		//������������ָ��
		default:fin.getline(ignore, ignoreSize);
			break;

		}

	}
#pragma endregion

	fin.close();

	//MtlLib�в���˳���SubMesh�в���˳����ܲ�һ��
	//����SubMesh��ʹ�ò��ʵ�˳����е���
	std::vector<ObjMaterial> objMaterial;
	for (int i = 0; i != subMeshs.size(); ++i)
	{
		//MessageBoxA(0, subMeshs[i].materialName.c_str(), 0, 0);
		if (subMeshs[i].materialName == JmxR_OBJ_DEFAULT_SUBMESH_NAME)
		{
			objMaterial.push_back(ObjMaterial());
		}
		for (auto& p : materials)
		{
			if (subMeshs[i].materialName == p.materialName)
			{
				objMaterial.push_back(p);
				break;
			}
		}
	}
	materials = objMaterial;
	for (auto& p : materials)
	{
		if (p.diffuseMap .empty())
		{
			std::cout<<"Diffuse Lost"<<std::endl;
		}
	}

	return true;

}


void jmxRCore::ObjLoader::ReadMtlFile(const std::string& mtlFile,
	const std::string& texPath,
	std::vector<ObjMaterial>& material)
{
	std::fstream fin;
	fin.open(mtlFile);
	if (!fin.good())
	{
		std::cout<<"δ�ҵ�mtl�ļ�-ReadMtlFile"<<std::endl;
		system("pause");
		exit(0);
		
	}

	const int ignoreSize = 200;
	char ignore[ignoreSize];

	char str[100];
	int materialIndex = -1;
#pragma region Parsering
	while (fin >> str)
	{
		switch (str[0])
		{
		case 'n':
			if (strcmp(str, "newmtl") == 0)
			{
				//�����²���
				++materialIndex;
				ObjMaterial objMaterial;
				char name[50];
				fin >> name;
				objMaterial.materialName = std::string(name);
				material.push_back(objMaterial);
			}
			else
			{
				//����
				fin.getline(ignore, ignoreSize);
			}
			break;
		case 'K':
			if (materialIndex < 0)
			{
				std::cout<<(L"mtl�ļ���ʽ����û���½�����ָ��-ReadMtlFile")<<std::endl;
				system("pause");
				exit(0);
				
			}
			switch (str[1])
			{
			case 'a'://����ϵ��
				fin >> material[materialIndex].ambient.x;
				fin >> material[materialIndex].ambient.y;
				fin >> material[materialIndex].ambient.z;
				material[materialIndex].ambient.w = 1.0f;
				break;
			case 'd'://������ϵ��
				fin >> material[materialIndex].diffuse.x;
				fin >> material[materialIndex].diffuse.y;
				fin >> material[materialIndex].diffuse.z;
				material[materialIndex].diffuse.w = 1.0f;
				break;
			case 's':
				fin >> material[materialIndex].specular.x;
				fin >> material[materialIndex].specular.y;
				fin >> material[materialIndex].specular.z;
				//	material[materialIndex].specular.w = 4.0f;
				break;
			default:fin.getline(ignore, ignoreSize);
				break;
			}
			break;
		case 'N'://shinness
			if (materialIndex < 0)
			{
				std::cout<<"mtl�ļ���ʽ����û���½�����ָ��-ReadMtlFile"<<std::endl;
				exit(0);
			}
			if (str[1] == 's')
				fin >> material[materialIndex].specular.w;
			break;
		case 'm':
			if (materialIndex < 0)
			{
				std::cout << "mtl�ļ���ʽ����û���½�����ָ��-ReadMtlFile" << std::endl;
				exit(0);
			}
			switch (str[5])
			{
			case 'a':
			{
				std::string name;
				fin >> name;

				material[materialIndex].ambientMap = texPath+name;
				break;

			}
			case 'd':
			{
				std::string dname;
				fin >> dname;
	
				material[materialIndex].diffuseMap = texPath+dname;

				break;

			}
			case 's':
			{
				std::string name;
				fin >> name;
			
				material[materialIndex].specularMap = texPath+name;
				break;

			}
			case 'u':
			{
				std::string name;
				fin >> name;
				material[materialIndex].normalMap = texPath + name;
			}
			default:
				fin.getline(ignore, ignoreSize);
				break;
			}

			break;

//		case 'b': //bump
	//	{	
	//		std::string nor;
	//		fin >> nor;
	//		material[materialIndex].normalMap = texPath + nor;
		//	break;
	//	}
		
		default:fin.getline(ignore, ignoreSize);
			break;
		}

	}
#pragma endregion


}

void jmxRCore::ObjLoader::ReadVertexPos(std::ifstream& fin, std::vector<Vec3>& posInfo)
{
	Vec3 pos;
	fin >> pos.x;
	fin >> pos.y;
	fin >> pos.z;
	//pos.x *= -1.0f;
	//pos.z *= -1.0f;
	posInfo.push_back(pos);
}

void jmxRCore::ObjLoader::ReadVertexTexCoord(std::ifstream& fin, std::vector<Vec2>& texCoordInfo)
{
	Vec2 texCoord;
	fin >> texCoord.x;
	fin >> texCoord.y;
	texCoord.y = 1.0 - texCoord.y;
	texCoordInfo.push_back(texCoord);
}
void jmxRCore::ObjLoader::ReadVertexNormal(std::ifstream& fin, std::vector<Vec3>& normalInfo)
{
	Vec3 normal;
	fin >> normal.x;
	fin >> normal.y;
	fin >> normal.z;
	//normal.x *= -1.0f;
	//normal.z *= -1.0f;
	normalInfo.push_back(normal);
}

u32 jmxRCore::getFaceByString(const std::string& str,
	const std::vector<Vec3>& posInfo,
	const std::vector<Vec2>& texInfo,
	const std::vector<Vec3>& normalInfo,
	std::vector<ObjMesh::ObjVertex>& vertices,
	std::vector<u32>&		indices,
	u32 VertexIndex
	)
{
	//UINT faceCount = 1;
	u32 VertexCount = 0;
	std::istringstream iss(str);
	static std::string vertex;
	while (iss >> vertex)
	{
		vertices.push_back(jmxRCore::getOBJVertexByString(vertex.c_str(),
			posInfo,
			texInfo,
			normalInfo));
		++VertexCount;
	}
	if (VertexCount == 3)
	{
		//������Ƭ
		indices.push_back(VertexIndex);
		indices.push_back(VertexIndex + 1);
		indices.push_back(VertexIndex + 2);
		return 1;
	}
	else if (VertexCount == 4)
	{
		//�ı���
		//��������Ƭ
		indices.push_back(VertexIndex);
		indices.push_back(VertexIndex + 1);
		indices.push_back(VertexIndex + 2);

		//��������Ƭ
		indices.push_back(VertexIndex);
		indices.push_back(VertexIndex + 2);
		indices.push_back(VertexIndex + 3);
		return 2;
	}
	else
	{
		std::cout<<"�Ƿ�����Ƭ��ʽ-getFaceByString"<<std::endl;
		exit(0);
		return 0;
	}
}


ObjMesh::ObjVertex jmxRCore::getOBJVertexByString(const char* str, 
	const std::vector<Vec3>& posInfo,
	const std::vector<Vec2>& texInfo,
	const std::vector<Vec3>& normalInfo
	)
{
	int index = 0;
	int numIndex = 0;
	char num[20];
	bool flag = true;
	int  data[4];
	int size = 0;
	ObjMesh::ObjVertex v;

	while (str[index] != '\0')
	{
		if (str[index] != '/')
		{
			num[numIndex++] = str[index++];
			flag = false;
		}
		else
		{
			if (flag)
			{
				++index;
				continue;
			}
			flag = true;
			num[numIndex] = '\0';
			data[size++] = std::atoi(num);

			numIndex = 0;
			++index;

		}

	}
	if (str[index - 1] != '/')
	{
		num[numIndex] = '\0';
		data[size++] = std::atoi(num);

	}

	//"f ��������/uv������/��������"��
	switch (size)
	{
	case 1:v.pos = posInfo[jmxRCore::corObjIndex(data[0], posInfo.size())];

		break;
	case 2:v.pos = posInfo[jmxRCore::corObjIndex(data[0], posInfo.size())];
		v.normal = normalInfo[jmxRCore::corObjIndex(data[1], normalInfo.size())];

		break;
	case 3:v.pos = posInfo[jmxRCore::corObjIndex(data[0], posInfo.size())];
		v.tex = texInfo[jmxRCore::corObjIndex(data[1], texInfo.size())];
		v.normal = normalInfo[jmxRCore::corObjIndex(data[2], normalInfo.size())];

		break;
	default:std::cout<<"δ֪��OBJ��ʽ -getVertexByString"<<std::endl;
		exit(0);
		break;
	}


	return v;

}

void jmxRCore::genTangent(const Vec3& p0, const Vec3& p1, const Vec3& p2,
	const Vec2& tex0, const Vec2& tex1, const Vec2& tex2,
	_Out Vec3* T, _Out Vec3* B, _Out Vec3* N)
{
	
	Vec3 e0 = p1 - p0;
	Vec3 e1 = p2 - p0;

//	Vec2 t0 = frac(tex0);
//	Vec2 t1 = frac(tex1);
//	Vec2 t2 = frac(tex2);
	Vec2 t0 = tex0;
	Vec2 t1 = tex1;
	Vec2 t2 = tex2;
	

	f32 u0 = t1.x - t0.x;
	f32 v0 = t1.y - t0.y;

	f32 u1 = t2.x - t0.x;
	f32 v1 = t2.y - t0.y;

	f32 inv =  (u0*v1 - v0*u1);
	if (std::fabs(inv) < 0.000000001f)
		inv = 1.f;
	inv = 1.f / inv;

	Vec3 Tt;
	Tt.x = v1*e0.x + (-v0*e1.x);
	Tt.y = v1*e0.y + (-v0*e1.y);
	Tt.z = v1*e0.z + (-v0*e1.z);
	Tt *= inv;
	Vec3 Bt;
	Bt.x = (-u1*e0.x) + u0*e1.x;
	Bt.y = (-u1*e0.y) + u0*e1.y;
	Bt.z = (-u1*e0.z) + u0*e1.z;
	Bt *= inv;


	Vec3 v10 = p1 - p0;
	Vec3 v20 = p2 - p0;
	Vec3 Nt = cross(v10, v20);
	Nt.normalize();
	Tt.normalize();
	Bt.normalize();

	Tt = Tt - dot(Nt, Tt)*Nt;
	Bt = Bt - dot(Nt, Bt)*Nt - dot(Tt, Bt)*Tt;

	Tt.normalize();
	Bt.normalize();

	*T = Tt;
	*B = Bt;
	*N = Nt;

}
