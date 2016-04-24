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
#include"Scene.h"
#include"tinyxml2.h"
#include"tiny_obj_loader.h"
#include<map>
#include<cstring>

namespace jmxRCore
{
	struct ElementBuf
	{
		std::string				 elemName;
		std::map<std::string, std::string> attri;
	};

	class SceneParser
	{
	public:
		static Scene* loadSceneRtRough(const std::string& file);

	private:
		static ElementBuf readElement(tinyxml2::XMLElement* elem);
		static void			readMesh(tinyxml2::XMLElement* elem,Scene* sc);
		static void			readLight(tinyxml2::XMLElement* elem,Scene* sc);
		static void			readCamera(tinyxml2::XMLElement* elem,Scene* sc);
		static Mat4x4		readTransform(tinyxml2::XMLElement* elem);


	};
}



