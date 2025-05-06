#include "AnimationClip.h"

void AnimationClip::updateTracks(std::vector<Track>& _tracks)
{
	for (int i = 0; i < _tracks.size(); i++)
	{
		tracks.emplace_back(_tracks[i]);
		frameAmount = _tracks[i].keyFrames.size();
	}
}

void AnimationClip::updateAnimationEvents(std::vector<AnimationEvent>& _animationEvents)
{
	for (int i = 0; i < _animationEvents.size(); i++)
	{
		animationEvents.emplace_back(_animationEvents[i]);
	}
}

void AnimationClip::setSpeed(float _speed)
{
	if (_speed <= 0)
	{
		return;
	}

	speed = _speed;
}
