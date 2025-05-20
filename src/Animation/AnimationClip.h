#pragma once
#include "../Scene/SceneGraph/Node.h"
#include "../Effect/ParticleSystem.h"
#include "../Effect/EffectManager.h"

#include <vector>
#include <functional>

struct KeyFrame
{
	glm::vec3 transOffset;
	glm::quat rotatOffset;
};

struct Track
{
	Node* node;
	std::vector<KeyFrame> keyFrames;
};

struct AnimationEvent
{
	unsigned int frameNum;
	std::function<void()> onTrigger;
};

class AnimationClip
{
private:
	std::string clipName;
	std::vector<Track> tracks; // all nodes

	// Suppose that every track have the same amount of keyframe
	unsigned int frameAmount;
	float speed;

	std::vector<AnimationEvent> animationEvents;

public:
	AnimationClip() : clipName(""), frameAmount(0), speed(1) {}
	AnimationClip(std::string name): clipName(name), frameAmount(0), speed(1) {}
	AnimationClip(std::string name, float _speed) : clipName(name), frameAmount(0), speed(_speed) {}

	~AnimationClip() {}

	void updateTracks(std::vector<Track>& _tracks);
	void updateAnimationEvents(std::vector<AnimationEvent>& _animationEvents);
	void setSpeed(float _speed);

	inline float getSpeed() const { return speed; }
	inline unsigned int getAmountOfFrame() { return frameAmount; }
	inline Track& getTrack(unsigned int index) { return tracks[index]; }
	inline KeyFrame& getFrame(unsigned int trackIndex, unsigned int frameIndex) { return tracks[trackIndex].keyFrames[frameIndex]; }
	inline const std::vector<Track>& getTracks() { return tracks; }
	inline const std::vector<AnimationEvent>& getAnimationEvents() { return animationEvents; }
};