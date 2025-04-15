#pragma once
#include "../Model/Model.h"
#include "AnimationClip.h"

#include <string>
#include <map>
#include <fstream>

class Animator
{
private:
	Model* model;
	std::map<std::string, AnimationClip> clips;
	std::vector<std::string> clipNames;

	double currentFrame;
	AnimationClip* currentClip;

public:
	Animator();
	~Animator();

	void target(Model* _model);
	void addClip(std::string clipName, std::vector<std::vector<KeyFrame>>& allKeyFrames);
	void addClip(std::string clipName, const char* fileName);

	void animate(double dt);

	void setCurrentClip(std::string clipName);
	void resetTime() { currentFrame = 0; }

	inline std::vector<std::string>& getClipNames() { return clipNames; }
};

