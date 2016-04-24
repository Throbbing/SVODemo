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
#include"Geometry.h"
#include"JmxRMesh.h"
#include"OBJMesh.h"
#include"Matrix.h"
#include"Camera.h"

namespace jmxRCore
{

	class SceneParser;
	enum ENodeType
	{
		ECamera = 0x0001,
		EMesh = 0x0010,
		ELight = 0x0020
	};

	class SceneNode
	{
	public:
		SceneNode(ENodeType type,const std::string& s) :nodeType(type),str(s){}
		~SceneNode(){}
		bool match(ENodeType type)
		{
			return (nodeType&type);
		}
		void note()
		{
			std::cout << "Note: " << str << std::endl;
		}

	private:
		const ENodeType    nodeType;
		const std::string			str;
	};

	class RoughCamera :public SceneNode, public Camera
	{
	public:
		RoughCamera(const std::string& note) :SceneNode(ECamera, note){}
	};

	class RoughMesh :public SceneNode,public OBJMesh
	{
	public:
		RoughMesh(const std::string& note,
			std::vector<tinyobj::shape_t>& shapes,
			std::vector<tinyobj::material_t>& materials) :SceneNode(EMesh, note),
			OBJMesh(shapes,materials){}
		

	public:
		Mat4x4				worldMat;
		BBox				localBBox;
		
	};

	class RoughLight :public SceneNode
	{
#define RoughPoint 0
#define RoughDir   1
	public:
		RoughLight(const std::string& note) :SceneNode(ELight, note) {}

		Vec4  ambient;
		Vec4  diffuse;
		Vec4  specular;

		Vec4  para;

	};


	

	class Scene
	{
		friend SceneParser;
	public:
		Scene(){}
		~Scene()
		{
			for (auto& p : mCamerasPool)
				delete p;
			for (auto& p : mMeshsPool)
				delete p;
			for (auto& p : mLightsPool)
				delete p;
		}


		SceneNode*		getCamera(u32 i)
		{
			if (i >= mCamerasPool.size())
				return nullptr;
			else
				return mCamerasPool[i];
		}
		SceneNode*		getMesh(u32 i)
		{
			if (i >= mMeshsPool.size())
				return nullptr;
			else
				return mMeshsPool[i];
		}
		SceneNode*		getLight(u32 i)
		{
			if (i >= mLightsPool.size())
				return nullptr;
			else
				return mLightsPool[i];
		}

		u32		getSum(ENodeType t)
		{
			if (t&ECamera)
				return mCamerasPool.size();

			if (t&EMesh)
				return mMeshsPool.size();

			if (t&ELight)
				return mLightsPool.size();
			return 0;
		}

		BBox   getBBox()
		{
			return mSceneBBox;
		}


	private:
		BBox		mSceneBBox;
		std::string					mName;
		std::vector<SceneNode*>		mCamerasPool;
		std::vector<SceneNode*>		mMeshsPool;
		std::vector<SceneNode*>		mLightsPool;
	};


	
}