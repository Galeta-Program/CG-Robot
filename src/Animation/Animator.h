#pragma once
#include "../Model/Model.h"
#include "AnimationClip.h"

#include <string>
#include <map>

class Animator
{
private:
	Model* model;
	std::map<std::string, AnimationClip> clips;

	double currentFrame;
	AnimationClip* currentClip;

public:
	Animator();
	~Animator();

	void target(Model* _model);
	void addClip(std::string clipName, std::vector<std::vector<KeyFrame>>& allKeyFrames);
	void animate(double dt);

	void setCurrentClip(std::string clipName);
	void resetTime() { currentFrame = 0; }
};

