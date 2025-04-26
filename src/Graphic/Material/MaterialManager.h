#pragma once
#include "Material.h"

#include <map>
#include <vector>
#include <glm/glm.hpp>

class MaterialManager
{
private:
	static MaterialManager instance;

	std::map<std::string, std::string> mtlNameToStrKey;
	std::map<std::string, Material> strKeyToMtl;

	MaterialManager() {}
	~MaterialManager() {}

	std::string parseKey(const MaterialKey& key);

public:
	static MaterialManager& getInstance();

	void registerMaterial(const char* path, const char* prefix);
	void use(GLuint program, const std::string& name, const std::vector<int>& units = std::vector<int>(0));
};

