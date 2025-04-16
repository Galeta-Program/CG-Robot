#pragma once
#include "../Scene/SceneGraph/Node.h"

#include <vector>

struct KeyFrame
{
	glm::vec3 transOffset;
	glm::vec3 rotatOffset;
};

struct Track
{
	Node* node;
	std::vector<KeyFrame> keyFrames;
};

class AnimationClip
{
private:
	std::string clipName;
	std::vector<Track> tracks; // all nodes

	// I suppose that every track have the same amount of keyframe
	unsigned int frameAmount;
	float speed;

public:
	AnimationClip() : clipName(""), frameAmount(0), speed(1) {}
	AnimationClip(std::string name): clipName(name), frameAmount(0), speed(1) {}
	AnimationClip(std::string name, float _speed) : clipName(name), frameAmount(0), speed(_speed) {}

	~AnimationClip() {}

	void updateTracks(std::vector<Track>& _tracks);
	void setSpeed(float _speed);

	inline float getSpeed() const { return speed; }
	inline unsigned int getAmountOfFrame() { return frameAmount; }
	inline Track& getTrack(unsigned int index) { return tracks[index]; }
	inline const std::vector<Track>& getTracks() { return tracks; }
};