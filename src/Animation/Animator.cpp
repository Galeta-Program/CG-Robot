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
	clipNames.emplace_back(clipName);

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

void Animator::addClip(std::string clipName, const char* fileName)
{
	clips[clipName] = AnimationClip(clipName);
	clipNames.emplace_back(clipName);

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
	while (1)
	{
		if (in.peek() == EOF)
		{
			break;
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
			glm::vec3 rotate(stof(buffer[0]), stof(buffer[1]), stof(buffer[2]));

			tracks[i].keyFrames.emplace_back(KeyFrame{ trans, rotate });
		}
	}

	clips[clipName].updateTracks(tracks);
}

void Animator::animate(double dt)
{
	currentFrame += dt * 5;
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
		glm::quat rotate = glm::slerp(glm::normalize(glm::quat(glm::radians(lastKeyFrame.rotatOffset))),
			glm::normalize(glm::quat(glm::radians(nextKeyFrame.rotatOffset))), float(interpolation));

		model->getPart(i).setTranslate(translate);
		model->getPart(i).setRotate(rotate);
	}
}

void Animator::setCurrentClip(std::string clipName)
{
	currentClip = &clips[clipName];
}
