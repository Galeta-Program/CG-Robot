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
	std::string currentClipName;
	AnimationClip* currentClip;
	std::string currentFrameClipName;
	AnimationClip* currentFrameClip; // for editing frame

public:
	Animator();
	~Animator();

	void target(Model* _model);
	void addClip(std::string clipName, std::vector<std::vector<KeyFrame>>& allKeyFrames, double _speed = 1, std::vector<AnimationEvent> animationEvents = {});
	void addClip(std::string clipName, const char* fileName, std::vector<AnimationEvent> animationEvents = {});

	void animate(double dt);

	void setCurrentClip(std::string clipName);
	void setCurrentClipSpeed(float _speed);

	void makeFrameToClip(unsigned int frameIndex, AnimationClip* _currentClip = nullptr);
	void deleteCurrentFrameClip();

	void resetTime() { currentFrame = 0; }

	inline std::vector<std::string>& getClipNames() { return clipNames; }
	inline std::string getCurrentClipName() const { return currentClipName; }
	inline float getCurrentClipSpeed() const { return currentClip->getSpeed(); };
	inline const std::vector<Track>& getCurrentClipTrack() const { return currentClip->getTracks(); };
	inline unsigned int getFrameAmount() const { return currentClip->getAmountOfFrame(); }

};

