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

public:
	AnimationClip() : clipName(""), frameAmount(0) {}
	AnimationClip(std::string name): clipName(name), frameAmount(0) {}
	~AnimationClip() {}

	void updateTracks(std::vector<Track>& _tracks);

	inline unsigned int getAmountOfFrame() { return frameAmount; }
	inline Track& getTrack(unsigned int index) { return tracks[index]; }
};