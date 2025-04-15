#include "Animator.h"

Animator::Animator(): currentFrame(0.0), model(nullptr)
{}

Animator::~Animator()
{}

void Animator::target(Model* _model)
{
	clips.clear();
	model = _model;
}

void Animator::addClip(std::string clipName, std::vector<std::vector<KeyFrame>>& allKeyFrames)
{
	clips[clipName] = AnimationClip(clipName);
	std::vector<Track> tracks;

	for (int i = 0; i < model->getPartsAmount(); i++)
	{
		tracks.push_back(Track{
			&(model->getPart(i)),
			allKeyFrames[i]
		});
	}

	clips[clipName].updateTracks(tracks);
}

void Animator::animate(double dt)
{
	currentFrame += dt;
	unsigned totalFrames = currentClip->getAmountOfFrame();
	while (currentFrame > totalFrames)
	{
		currentFrame -= totalFrames;
	}

	for (int i = 0; i < model->getPartsAmount(); i++)
	{
		Track& currentTrack = currentClip->getTrack(i);
		double floorFrame = floor(currentFrame);
		double ceilFrame = ceil(currentFrame);
		if (ceilFrame >= totalFrames) {
			floorFrame = 0;
			ceilFrame = floor(currentFrame);
		}
		KeyFrame& lastKeyFrame = currentTrack.keyFrames[floorFrame];
		KeyFrame& nextKeyFrame = currentTrack.keyFrames[ceilFrame];

		glm::vec3 translate = glm::mix(lastKeyFrame.transOffset, nextKeyFrame.transOffset, dt - int(dt));
		glm::quat rotate = glm::slerp(glm::normalize(glm::quat(glm::radians(lastKeyFrame.rotatOffset))),
			glm::normalize(glm::quat(glm::radians(nextKeyFrame.rotatOffset))), float(dt - int(dt)));

		model->getPart(i).setTranslate(translate);
		model->getPart(i).setRotate(rotate);
	}
}

void Animator::setCurrentClip(std::string clipName)
{
	currentClip = &clips[clipName];
}
