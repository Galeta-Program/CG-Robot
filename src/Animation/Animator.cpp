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

void Animator::addClip(std::string clipName, std::vector<std::vector<KeyFrame>>& allKeyFrames, double _speed /* = 1 */, std::vector<AnimationEvent> animationEvents)
{
	clips[clipName] = AnimationClip(clipName, _speed);
	clipNames.emplace_back(clipName);
	clips[clipName].updateAnimationEvents(animationEvents);

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

void Animator::addClip(std::string clipName, const char* fileName, std::vector<AnimationEvent> animationEvents)
{
	clips[clipName] = AnimationClip(clipName);
	clipNames.emplace_back(clipName);
	clips[clipName].updateAnimationEvents(animationEvents);

	std::vector<Track> tracks;
	for (int i = 0; i < model->getPartsAmount(); i++)
	{
		tracks.emplace_back(Track{
			&(model->getPart(i)),
			{}
			});
	}

	std::ifstream in(fileName);
	std::string buffer[3];

	bool setSpeedFlag = false;
	while (1)
	{
		if (in.peek() == EOF)
		{
			break;
		}

		// CHeck if speed specified.
		std::streampos pos = in.tellg();
		if (in >> buffer[0])
		{
			if (buffer[0] == "speed")
			{
				in >> buffer[0];
				clips[clipName].setSpeed(stof(buffer[0]));
				setSpeedFlag = true;
			}
			else
			{
				in.seekg(pos);
			}
		}

		for (int i = 0; i < model->getPartsAmount(); i++)
		{
			if (!(in >> buffer[0] >> buffer[1] >> buffer[2]))
			{
				break;
			}
			glm::vec3 trans(stof(buffer[0]), stof(buffer[1]), stof(buffer[2]));

			if (!(in >> buffer[0] >> buffer[1] >> buffer[2]))
			{
				break;
			}
			glm::quat rotate(
				glm::normalize(
					glm::quat(
						glm::radians(
							glm::vec3(stof(buffer[0]), stof(buffer[1]), stof(buffer[2]))
						)
					)
				)
			);

			tracks[i].keyFrames.emplace_back(KeyFrame{ trans, rotate });
		}
	}

	clips[clipName].updateTracks(tracks);

	if (!setSpeedFlag)
	{
		clips[clipName].setSpeed(1.0f);
	}
}

void Animator::animate(double dt)
{
	currentFrame += dt * currentClip->getSpeed();
	unsigned totalFrames = currentClip->getAmountOfFrame();

	if (totalFrames == 0)
	{
		return;
	}

	currentFrame = fmod(currentFrame, totalFrames);

	unsigned int frame0 = (unsigned int)(floor(currentFrame));
	unsigned int frame1 = (frame0 + 1) % totalFrames;

	double interpolation = currentFrame - (double)(frame0);

	for (int i = 0; i < model->getPartsAmount(); i++)
	{
		Track& currentTrack = currentClip->getTrack(i);

		KeyFrame& lastKeyFrame = currentTrack.keyFrames[frame0];
		KeyFrame& nextKeyFrame = currentTrack.keyFrames[frame1];

		glm::vec3 translate = glm::mix(lastKeyFrame.transOffset, nextKeyFrame.transOffset, interpolation);
		glm::quat rotate = glm::slerp(glm::normalize(lastKeyFrame.rotatOffset),
			glm::normalize(nextKeyFrame.rotatOffset), float(interpolation));

		model->getPart(i).setTranslate(translate);
		model->getPart(i).setRotate(rotate);
	}

	for (const auto& animationEvent : currentClip->getAnimationEvents())
	{
		if (frame0 == animationEvent.frameNum)
			animationEvent.onTrigger();
	}
}


void Animator::setCurrentClip(std::string clipName)
{
	currentClip = &clips[clipName];
}

void Animator::setCurrentClipSpeed(float _speed)
{
	if (currentClip == nullptr)
	{
		return;
	}

	currentClip->setSpeed(_speed);
}
