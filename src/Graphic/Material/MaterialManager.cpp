#include "MaterialManager.h"
#include "../src/Utilty/OBJLoader.hpp"


MaterialManager MaterialManager::instance;

std::string MaterialManager::parseKey(const MaterialKey& key)
{
	std::string strKey = std::to_string(key.Ka[0]) + "|" +
		std::to_string(key.Ka[1]) + "|" +
		std::to_string(key.Ka[2]) + "|" +
		std::to_string(key.Kd[0]) + "|" +
		std::to_string(key.Kd[1]) + "|" +
		std::to_string(key.Kd[2]) + "|" +
		std::to_string(key.Ks[0]) + "|" +
		std::to_string(key.Ks[1]) + "|" +
		std::to_string(key.Ks[2]) + "|" +
		key.map_Kd + "|" +
		key.map_Ns + "|" +
		key.map_refl + "|" +
		key.map_Ke + "|" +
		key.map_d + "|" +
		key.map_Bump;

	return strKey;
}

void MaterialManager::registerMaterial(const char* path, const char* prefix)
{
	std::vector<glm::vec3> Kds;
	std::vector<glm::vec3> Kas;
	std::vector<glm::vec3> Kss;
	std::vector<std::string> Materials; //mtl-name
	std::vector<std::string> map_Kd;
	std::vector<std::string> map_Ns;
	std::vector<std::string> map_refl;
	std::vector<std::string> map_Ke;
	std::vector<std::string> map_d;
	std::vector<std::string> map_Bump;

	LoadMTL(path, Kds, Kas, Kss, Materials, map_Kd, map_Ns, map_refl, map_Ke, map_d, map_Bump);

	for (int i = 0; i < Kds.size(); i++)
	{
		MaterialKey key;
		key.Ka = Kas[i];
		key.Kd = glm::vec3(1, 1, 1); // not in mtl
		key.Ks = Kss[i];
		key.map_Kd = prefix + map_Kd[i];
		key.map_Ns = prefix + map_Ns[i];
		key.map_refl = prefix + map_refl[i];
		key.map_Ke = prefix + map_Ke[i];
		key.map_d = prefix + map_d[i];
		key.map_Bump = prefix + map_Bump[i];

		std::string strKey = parseKey(key);
		if (strKeyToMtl.find(strKey) == strKeyToMtl.end())
		{
			strKeyToMtl[strKey] = Material(key, strKey);
			mtlNameToStrKey[Materials[i]] = strKey;
		}
		else if (mtlNameToStrKey.find(Materials[i]) == mtlNameToStrKey.end())
		{
			mtlNameToStrKey[Materials[i]] = strKey;

		}
	}
}

void MaterialManager::use(GLuint program, const std::string& name, const std::vector<int>& units /*= std::vector<int>(0)*/)
{
	if (mtlNameToStrKey.find(name) != mtlNameToStrKey.end())
	{
		if (strKeyToMtl.find(mtlNameToStrKey[name]) != strKeyToMtl.end())
		{
			strKeyToMtl[mtlNameToStrKey[name]].use(program);
		}
	}
}

MaterialManager& MaterialManager::getInstance()
{
	return instance;
}
