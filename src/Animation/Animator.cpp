#include "Animator.h"
#include "../Effect/EffectManager.h"

#include <sstream>

Animator::Animator(): 
	currentFrame(0.0), 
	model(nullptr), 
	currentClipName(""), 
	currentClip(nullptr), 
	currentFrameClipName(""),
	currentFrameClip(nullptr)
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
	std::string line, key, value;

	bool setSpeedFlag = false;
	float speed = 1.0f;

	while (in)
	{
		// opening brace of a frame
		//Fix
		std::getline(in, line);
		if (line == "{") continue;
		if (line == "}") continue;


		// Parse the motion array
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> rotations;
		
		int endOfMotionSection = -1;
		while (std::getline(in, line)) {
			if (line.find("],") != std::string::npos) {
				endOfMotionSection += 1;
				if (endOfMotionSection == 1)
				{
					break;
				}
			}
			
			// Parse position vectors [x, y, z]
			if (line.find("[") != std::string::npos) {
				endOfMotionSection -= 1;
				size_t start = line.find("[") + 1;
				size_t end = line.find("]");
				if (start != std::string::npos && end != std::string::npos) {
					std::string vecStr = line.substr(start, end - start);
					std::istringstream iss(vecStr);
					float x, y, z;
					char comma;
					iss >> x >> comma >> y >> comma >> z;
					positions.push_back(glm::vec3(x, y, z));
				}
			}

			if (line.find("],") != std::string::npos) {
				endOfMotionSection += 1;
				if (endOfMotionSection == 1)
				{
					break;
				}
			}

			std::getline(in, line);
			// Parse position vectors [x, y, z]
			if (line.find("[") != std::string::npos) {
				endOfMotionSection -= 1;
				size_t start = line.find("[") + 1;
				size_t end = line.find("]");
				if (start != std::string::npos && end != std::string::npos) {
					std::string vecStr = line.substr(start, end - start);
					std::istringstream iss(vecStr);
					float x, y, z;
					char comma;
					iss >> x >> comma >> y >> comma >> z;
					rotations.push_back(glm::vec3(x, y, z));
				}
			}
		}

		// Parse the effect section
		Effect readinEffect;
		readinEffect.name = "None";
		readinEffect.isStart = false;
		readinEffect.isFinished = false;
		readinEffect.param.dir = glm::vec3(0, 0, 1);
		readinEffect.param.pos = glm::vec3(0, 0, 0);
		

		int endOfEffectSection = -1;
		while (std::getline(in, line)) {
			if (line.find("\"effect\"") != std::string::npos) {
				// Found the effect section
				while (std::getline(in, line)) {
					if (line.find("\"name\"") != std::string::npos) {
						size_t start = line.find(":") + 1;
						size_t nameStart = line.find("\"", start) + 1;
						size_t nameEnd = line.find("\"", nameStart);
						readinEffect.name = line.substr(nameStart, nameEnd - nameStart);
					}
					else if (line.find("\"isStart\"") != std::string::npos) {
						readinEffect.isStart = line.find("\"true\"") != std::string::npos;
					}
					else if (line.find("\"isFinished\"") != std::string::npos) {
						readinEffect.isFinished = line.find("\"true\"") != std::string::npos;
					}
					else if (line.find("\"pos\"") != std::string::npos) {
						// Parse position array [x, y, z]
						size_t start = line.find("[") + 1;
						size_t end = line.find("]");
						if (start != std::string::npos && end != std::string::npos) {
							std::string vecStr = line.substr(start, end - start);
							std::istringstream iss(vecStr);
							float x, y, z;
							char comma;
							iss >> x >> comma >> y >> comma >> z;
							readinEffect.param.pos = glm::vec3(x, y, z);
						}
					}
					else if (line.find("\"dir\"") != std::string::npos) {
						// Parse direction array [x, y, z]
						size_t start = line.find("[") + 1;
						size_t end = line.find("]");
						if (start != std::string::npos && end != std::string::npos) {
							std::string vecStr = line.substr(start, end - start);
							std::istringstream iss(vecStr);
							float x, y, z;
							char comma;
							iss >> x >> comma >> y >> comma >> z;
							readinEffect.param.dir = glm::vec3(x, y, z);
						}
					}
					
					if (line == "    }") {
						// End of effect object
						break;
					}
				}
			}
			
			if (line.find("}") != std::string::npos) {
				endOfEffectSection += 1;
				if (endOfEffectSection == 1)
				{
					break;
				}
			}
		}

		// Convert positions (every other one is a rotation)
		for (size_t i = 0; i < positions.size(); i += 1) {			
			glm::vec3 trans = positions[i];
			glm::vec3 rotDegrees = rotations[i];
			
			glm::quat rotate = glm::normalize(glm::quat(glm::radians(rotDegrees)));
			
			tracks[i].keyFrames.emplace_back(KeyFrame{ trans, rotate, readinEffect });
		}

		/*
		// Add effect data to animation events if needed
		if (effectName != "none") {
			// Create an animation event for this effect
			AnimationEvent event;
			event.eventType = isStart ? EventType::EffectStart : 
							 (isFalse ? EventType::EffectEnd : EventType::EffectUpdate);
			event.effectName = effectName;
			event.effectPosition = effectPos;
			event.effectDirection = effectDir;
			event.frameIndex = tracks[0].keyFrames.size() - 1; // Current frame

			// Add to existing animation events
			clips[clipName].addAnimationEvent(event);
		}
		*/
	}

	clips[clipName].updateTracks(tracks);

	if (!setSpeedFlag) {
		clips[clipName].setSpeed(1.0f);
	} else {
		clips[clipName].setSpeed(speed);
	}
}

void Animator::animate(double dt)
{
	AnimationClip* clip;
	if (currentFrameClip != nullptr)
	{
		clip = currentFrameClip;
	}
	else
	{
		clip = currentClip;
	}

	if (clip == nullptr)
	{
		return;
	}

	currentFrame += dt * clip->getSpeed();
	unsigned totalFrames = clip->getAmountOfFrame();

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
		Track& currentTrack = clip->getTrack(i);

		KeyFrame& lastKeyFrame = currentTrack.keyFrames[frame0];
		KeyFrame& nextKeyFrame = currentTrack.keyFrames[frame1];

		glm::vec3 translate = glm::mix(lastKeyFrame.transOffset, nextKeyFrame.transOffset, interpolation);
		glm::quat rotate = glm::slerp(glm::normalize(lastKeyFrame.rotatOffset),
			glm::normalize(nextKeyFrame.rotatOffset), float(interpolation));

		if (i == 0)
		{
			EffectManager& efm = EffectManager::getInstance();
			std::string effectName = lastKeyFrame.effect.name;
			if (effectName != "None")
			{
				bool isStart = lastKeyFrame.effect.isStart;
				bool isFinished = lastKeyFrame.effect.isFinished;
				glm::vec3 effectPos = glm::mix(lastKeyFrame.effect.param.pos, nextKeyFrame.effect.param.pos, interpolation);
				glm::vec3 effectDir = lastKeyFrame.effect.param.dir;

				ParticleSystem* currentEffect = &efm.getEffect(effectName);
				if (isStart)
				{
					currentEffect->setEmitterPos(0, effectPos);
					currentEffect->setEmitterDir(0, effectDir);
					currentEffect->emit();
					efm.setCurrentEffect(effectName);
				}
				if (isFinished)
				{
					efm.setCurrentEffect("None");
				}
			}
			
		}

		model->getPart(i).setTranslate(translate);
		model->getPart(i).setRotate(rotate);
	}

	for (const auto& animationEvent : clip->getAnimationEvents())
	{
		if (frame0 == animationEvent.frameNum)
			animationEvent.onTrigger();
	}
}


void Animator::setCurrentClip(std::string clipName)
{
	if (clips.find(clipName) == clips.end())
	{
		return;
	}

	currentClipName = clipName;
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

void Animator::makeFrameToClip(unsigned int frameIndex, AnimationClip* _currentClip /* = nullptr */)
{
	if (_currentClip == nullptr)
	{
		std::string frameClipName = currentClipName + "frame" + std::to_string(frameIndex);
		if (clips.find(frameClipName) != clips.end())
		{
			return;
		}

		std::vector<Track> tmpTrack;

		for (int i = 0; i < model->getPartsAmount(); i++)
		{
			std::vector<KeyFrame> frames;
			frames.emplace_back(currentClip->getFrame(i, frameIndex));
			tmpTrack.emplace_back(Track{&(model->getPart(i)), frames });
		}

		clips[frameClipName] = AnimationClip();
		clips[frameClipName].updateTracks(tmpTrack);

		deleteCurrentFrameClip();

		currentFrameClipName = frameClipName;
		currentFrameClip = &clips[frameClipName];
	}
}

void Animator::deleteCurrentFrameClip()
{
	if (currentFrameClip != nullptr)
	{
		clips.erase(currentFrameClipName);
		currentFrameClip = nullptr;
	}
}
