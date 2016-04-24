#include"SceneParser.h"
#include"Transform.h"
#include<algorithm>
#include<sstream>

using namespace tinyxml2;
jmxRCore::Scene* jmxRCore::SceneParser::loadSceneRtRough(const std::string& file)
{
	
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(file.c_str())!=XML_NO_ERROR)
	{
		std::cout << "XML格式错误" << std::endl;
		return nullptr;
	}
	Scene* sc = new Scene;
	
	XMLElement* root = doc.RootElement();

	if (std::strcmp(root->Name(), "scene"))
	{
		std::cout << "错误: 根节点必须为 <scene/>" << std::endl;
		delete sc;
		return nullptr;
	}
	
	ElementBuf rootBuf = readElement(root);
	sc->mName = rootBuf.elemName;
	
	
	tinyxml2::XMLElement* elem = root->FirstChildElement();
	while (elem)
	{
		if (std::strcmp(elem->FirstAttribute()->Name(), "type") != 0)
		{
			std::cout << "错误: 节点type属性错误" << std::endl;
			delete sc;
			return nullptr;
		}
		if (!std::strcmp(elem->FirstAttribute()->Value(), "Camera") != 0)
		{
			readCamera(elem, sc);
			elem = elem->NextSiblingElement();
			continue;
		}
		if (!std::strcmp(elem->FirstAttribute()->Value(), "Mesh") != 0)
		{
			readMesh(elem, sc);
			elem = elem->NextSiblingElement();
			continue;
		}
		if (!std::strcmp(elem->FirstAttribute()->Value(), "Light") != 0)
		{
			readLight(elem, sc);
			elem = elem->NextSiblingElement();
			continue;
		}

	}
	BBox box;
	if (!sc->mMeshsPool.empty())
	{
		box = ((RoughMesh*)(sc->mMeshsPool[0]))->localBBox;
		box=box.transform(((RoughMesh*)(sc->mMeshsPool[0]))->worldMat);
		for (u32 i = 1; i < sc->mMeshsPool.size(); ++i)
		{
			BBox local = ((RoughMesh*)(sc->mMeshsPool[i]))->localBBox;
			box=local.transform(((RoughMesh*)(sc->mMeshsPool[i]))->worldMat);
			box.unionBox(local);
		}
		sc->mSceneBBox = box;
	}
	
	return sc;

}

void jmxRCore::SceneParser::readCamera(tinyxml2::XMLElement* elem, Scene* sc)
{
	
	if (!sc || !elem)
		return;

	ElementBuf buf = readElement(elem);

	std::string note = elem->Value();
	RoughCamera* camera = new RoughCamera("Camera节点 Note: " + note);

	auto ptr = buf.attri.find("pos");
	Vec3 pos;
	if (ptr != buf.attri.end())
	{
		std::istringstream iss(ptr->second);
		iss >> pos.x >> pos.y >> pos.z;

		iss.clear();
	}

	

	Vec3 dir[3] = { Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1) };
	std::string dirs[] = { "right", "up", "look" };
	for (u32 i = 0; i < 3; ++i)
	{
		ptr = buf.attri.find(dirs[i]);
		if (ptr != buf.attri.end())
		{
			std::istringstream iss(ptr->second);
			iss >> dir[i].x >> dir[i].y >> dir[i].z;
		}
	}
	
	f32 frustum[] = { -1, -1, -1,-1 };
	std::string frus[] = { "fov", "near", "far","ratio" };
	for (u32 i = 0; i < 4; ++i)
	{
		ptr = buf.attri.find(frus[i]);
		if (ptr != buf.attri.end())
		{
			std::istringstream iss(ptr->second);
			iss >> frustum[i];
		}
	}

	f32 fov, n, f, r;
	fov = frustum[0] < 0.f ? PI / 2 : angle2Radian(frustum[0]);
	n = frustum[1] < 0.f ? 1.f : frustum[1];
	f = frustum[2] < 0.f ? 1000.f : frustum[2];
	r = frustum[3] < 0.f ? 1.f : frustum[3];

	Vec3 target = pos + 1.f*dir[2];

	camera->setTarget(XMFLOAT3(pos.x, pos.y, pos.z),
		XMFLOAT3(target.x, target.y, target.z),
		XMFLOAT3(dir[1].x, dir[1].y, dir[1].z));

	camera->setLens(fov, r, n, f);

	sc->mCamerasPool.push_back(camera);
	
	

}

void jmxRCore::SceneParser::readMesh(tinyxml2::XMLElement* elem, Scene* sc)
{
	
	if (!sc || !elem)
		return;

	ElementBuf meshBuf = readElement(elem);

	std::string path, mtl, tex;

	auto ptr = meshBuf.attri.find("name");
	if (ptr == meshBuf.attri.end())
	{
		std::cout << "错误：Mesh节点未找到文件名" << std::endl;
		return;
	}
	
	path = ptr->second;
	ptr = meshBuf.attri.find("path");
	if (ptr != meshBuf.attri.end())
		path = ptr->second+"/"+path;

	ptr = meshBuf.attri.find("mtl");
	if (ptr != meshBuf.attri.end())
		mtl = ptr->second;
	ptr = meshBuf.attri.find("tex");
	if (ptr != meshBuf.attri.end())
		tex = ptr->second;

	mtl.push_back('/');

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	tinyobj::LoadObj(shapes, materials, err, path.c_str(), mtl.c_str(), true);

	RoughMesh* mesh = new RoughMesh("Mesh节点",
		shapes, materials);

	for (auto &s : mesh->subMeshs)
	{
		for (auto& v : s.positions)
		{
			if (v.x < mesh->localBBox.minP.x)
				mesh->localBBox.minP.x = v.x;
			else if (v.x>mesh->localBBox.maxP.x)
				mesh->localBBox.maxP.x = v.x;

			if (v.y < mesh->localBBox.minP.y)
				mesh->localBBox.minP.y = v.y;
			else if (v.y>mesh->localBBox.maxP.y)
				mesh->localBBox.maxP.y = v.y;

			if (v.z < mesh->localBBox.minP.z)
				mesh->localBBox.minP.z = v.z;
			else if (v.z>mesh->localBBox.maxP.z)
				mesh->localBBox.maxP.z = v.z;
		}
	}


	XMLElement* trans = elem->FirstChildElement();
	Mat4x4 mat = Mat4x4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	if (trans)
	{
		if (!std::strcmp(trans->Name(), "transform"))
		{

			while (trans)
			{
				mat = mat* readTransform(trans);

				trans = trans->NextSiblingElement();
			}
		}
	}

	mesh->worldMat = mat;

	sc->mMeshsPool.push_back(mesh);
	
}

void jmxRCore::SceneParser::readLight(tinyxml2::XMLElement* elem, Scene* sc)
{
	
	if (!sc || !elem)
		return;

	ElementBuf buf = readElement(elem);

	auto ptr = buf.attri.find("method");
	if (ptr == buf.attri.end())
	{
		std::cout << "错误：光源节点未找到method属性" << std::endl;
		return;
	}

	RoughLight* light = new RoughLight(elem->Value());
	if (ptr->second == "Point")
		light->para.w = RoughPoint;
	else
		light->para.w = RoughDir;

	ptr = buf.attri.find("value");
	if (ptr == buf.attri.end())
	{
		std::cout << "错误：光源节点未找到value属性" << std::endl;
		return;
	}
	std::istringstream is(ptr->second);
	is >> light->para.x >> light->para.y >> light->para.z;
	std::string spe[] = { "ambient", "diffuse", "specular" };
	Vec4 sp[3];
	for (u32 i = 0; i < 3; ++i)
	{
		ptr = buf.attri.find(spe[i]);
		if (ptr != buf.attri.end())
		{
			std::istringstream iss(ptr->second);
			iss >> sp[i].x >> sp[i].y >> sp[i].z >> sp[i].w;
		}
	}

	light->ambient = sp[0];
	light->diffuse = sp[1];
	light->specular = sp[2];
	
	sc->mLightsPool.push_back(light);
		
}


jmxRCore::ElementBuf jmxRCore::SceneParser::readElement(tinyxml2::XMLElement* elem)
{
	
	const tinyxml2::XMLAttribute* attr = elem->FirstAttribute();

	ElementBuf buf;
	buf.elemName = elem->Name();
	while (attr)
	{
		buf.attri[attr->Name()] = attr->Value();
		attr = attr->Next();
	}

	return buf;
}

jmxRCore::Mat4x4 jmxRCore::SceneParser::readTransform(tinyxml2::XMLElement* elem)
{
	
	ElementBuf buf = readElement(elem);
	std::string type;
	auto ptr = buf.attri.find("type");
	if (ptr == buf.attri.end())
	{
		std::cout << "错误：Transform节点中未找到type属性" << std::endl;
		return Mat4x4(1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1);
	}
	type = ptr->second;
	auto value = buf.attri.find("value");
	if (value == buf.attri.end())
	{
		std::cout << "错误：Transform节点中未找到value属性" << std::endl;
		return Mat4x4(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}
	if (type == "Scale")
	{
		std::istringstream iss(value->second);
		Vec3 scale;
		iss >> scale.x >> scale.y >> scale.z;
		return Transform::Mat4x4Scale(scale);
	}
	if (type == "Rotate")
	{
		std::istringstream iss(value->second);
		f32 rot;
		iss >> rot;
		auto a = buf.attri.find("axis");
		if (a == buf.attri.end())
		{
			std::cout << "错误：Transform节点中未找到axis属性" << std::endl;
			return Mat4x4(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);
		}
		iss.clear();
		std::istringstream isss(a->second);
		Vec3 axis;
		isss >> axis.x >> axis.y >> axis.z;

		return Transform::Mat4x4Rotate(angle2Radian(rot), axis);

	}

	if (type == "Translate")
	{
		std::istringstream iss(value->second);
		Vec3 translate;
		iss >> translate.x >> translate.y >> translate.z;

		return Transform::Mat4x4Translate(translate);
	}
	
	return Mat4x4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

}