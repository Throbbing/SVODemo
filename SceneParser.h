/**
*　　　　　　　　�芥掘　　？芥�+ +
*　　　　　　　�芥食潯ォォォ食潯� + +
*　　　　　　　�А　　　　　　？� 　
*　　　　　　　�А　　？ァ　　？� ++ + + +
*　　　　　　 ���������エ������� ��+
*　　　　　　　�А　　　　　　？� +
*　　　　　　　�А　　？漾　　？�
*　　　　　　　�А　　　　　　？� + +
*　　　　　　　�皐ォ掘　　？芥ォ�
*　　　　　　　　　�А　　？А　　　　　　　　　　�
*　　　　　　　　　�А　　？� + + + +
*　　　　　　　　　�А　　？А　　　�Code is far away from bug with the animal protecting　　　　　　　
*　　　　　　　　　�А　　？� + 　　　　舞舗隠嗷,旗鷹涙bug　　
*　　　　　　　　　�А　　？�
*　　　　　　　　　�А　　？А　�+　　　　　　　　　
*　　　　　　　　　�А� 　　�皐ォォォ� + +
*　　　　　　　　　�� 　　　　　　　�禰�
*　　　　　　　　　�� 　　　　　　　�芥�
*　　　　　　　　　�皐粂粂芥ォ廩粂芥� + + + +
*　　　　　　　　　　�З路蓮？З路�
*　　　　　　　　　　�皐潯拭�
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



