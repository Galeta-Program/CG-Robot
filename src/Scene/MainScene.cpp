#include "MainScene.h"
#include "../Utilty/Error.h"

#include <iostream>

#define TOP_BODY 0
#define LEFT_UPPER_ARM 1
#define LEFT_LOWER_ARM 2
#define LEFT_HAND 3
#define HEAD 4
#define RIGHT_UPPER_ARM 5
#define RIGHT_LOWER_ARM 6
#define RIGHT_HAND 7
#define BOTTOM_BODY 8
#define LEFT_THIGH 9
#define LEFT_CALF 10
#define LEFT_FOOT 11
#define RIGHT_THIGH 12
#define RIGHT_CALF 13
#define RIGHT_FOOT 14

namespace CG
{
	MainScene::MainScene(Camera& _camera, Light& _light, Animator& _animator, ShaderProgram& _program)
	{
		camera = &_camera;
		light = &_light;
		animator = &_animator;
		program = &_program;
	}

	MainScene::~MainScene()
	{}

	bool MainScene::Initialize()
	{
		return loadScene();
	}

	void MainScene::Render()
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		// GLCall(glPolygonMode(GL_FRONT_AND_BACK, mode));

		matVPUbo.fillInData(0, sizeof(glm::mat4), camera->GetViewMatrix());
		matVPUbo.fillInData(sizeof(glm::mat4), sizeof(glm::mat4), camera->GetProjectionMatrix());

		robot.render(program->getId(), camera);
		GLCall(glFlush());
	}
	
	bool MainScene::loadScene()
	{
		loadModel();
		loadAnimation();

		//UBO
		matVPUbo.initialize(sizeof(glm::mat4) * 2);
		matVPUbo.associateWithShaderBlock(program->getId(), "u_MatVP", 0);

		return true;
	}

	void MainScene::loadModel()
	{
		std::vector<std::string> mtlPaths({
			"../res/models2/top_body.mtl",
			 
			"../res/models2/left_upper_arm.mtl",
			"../res/models2/left_lower_arm.mtl",
			"../res/models2/left_hand.mtl",
			 
			"../res/models2/head.mtl",
			 
			"../res/models2/right_upper_arm.mtl",
			"../res/models2/right_lower_arm.mtl",
			"../res/models2/right_hand.mtl",
			 
			"../res/models2/bottom_body.mtl",
			 
			"../res/models2/left_thigh.mtl", 
			"../res/models2/left_calf.mtl", 
			"../res/models2/left_foot.mtl",
			 
			"../res/models2/right_thigh.mtl",
			"../res/models2/right_calf.mtl", 
			"../res/models2/right_foot.mtl", 
			});

		std::vector<std::string> objPaths({
			"../res/models2/top_body.obj", // 0

			"../res/models2/left_upper_arm.obj", // 1
			"../res/models2/left_lower_arm.obj", // 2
			"../res/models2/left_hand.obj", // 3

			"../res/models2/head.obj", // 4

			"../res/models2/right_upper_arm.obj", // 5
			"../res/models2/right_lower_arm.obj", // 6
			"../res/models2/right_hand.obj", // 7

			"../res/models2/bottom_body.obj", // 8

			"../res/models2/left_thigh.obj", // 9
			"../res/models2/left_calf.obj", // 10
			"../res/models2/left_foot.obj", // 11

			"../res/models2/right_thigh.obj", //12
			"../res/models2/right_calf.obj", // 13
			"../res/models2/right_foot.obj", // 14
			});

		std::vector<std::vector<unsigned int>> relationship({
			{TOP_BODY, LEFT_UPPER_ARM, HEAD, RIGHT_UPPER_ARM, BOTTOM_BODY},
			{LEFT_UPPER_ARM, LEFT_LOWER_ARM},
			{LEFT_LOWER_ARM, LEFT_HAND},
			{RIGHT_UPPER_ARM, RIGHT_LOWER_ARM},
			{RIGHT_LOWER_ARM, RIGHT_HAND},
			{BOTTOM_BODY, LEFT_THIGH, RIGHT_THIGH},
			{LEFT_THIGH, LEFT_CALF},
			{LEFT_CALF, LEFT_FOOT},
			{RIGHT_THIGH, RIGHT_CALF},
			{RIGHT_CALF, RIGHT_FOOT}
			});

		robot.initialize(mtlPaths, objPaths);
		robot.setPartsRelationship(relationship);
		robot.gatherPartsData();

		animator->target(&robot);
	}

	void MainScene::loadAnimation()
	{
		std::vector<std::vector<KeyFrame>> keyFrameBuffer;

		for (int i = 0; i < 15; i++)
		{
			keyFrameBuffer.emplace_back();
		}

		keyFrameBuffer[TOP_BODY].emplace_back(KeyFrame{ glm::vec3(0.000000, 0.000000, 0.000000) ,			glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_UPPER_ARM].emplace_back(KeyFrame{ glm::vec3(7.650000, 0.000000, 0.000000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_LOWER_ARM].emplace_back(KeyFrame{ glm::vec3(2.900000, -6.400000, 0.000000) ,	glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_HAND].emplace_back(KeyFrame{ glm::vec3(3.650000, -8.500000, 3.900000) ,			glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[HEAD].emplace_back(KeyFrame{ glm::vec3(0.000000, 3.100000, 0.000000) ,				glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_UPPER_ARM].emplace_back(KeyFrame{ glm::vec3(-7.650000, 0.050000, 0.000000) ,	glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_LOWER_ARM].emplace_back(KeyFrame{ glm::vec3(-2.950000, -6.400000, 0.000000) ,	glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_HAND].emplace_back(KeyFrame{ glm::vec3(-3.650000, -8.500000, 3.900000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[BOTTOM_BODY].emplace_back(KeyFrame{ glm::vec3(0.000000, -5.050000, -1.450000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_THIGH].emplace_back(KeyFrame{ glm::vec3(3.250000, -6.650000, 2.200000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_CALF].emplace_back(KeyFrame{ glm::vec3(3.000000, -10.000000, 1.150000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_FOOT].emplace_back(KeyFrame{ glm::vec3(1.350000, -16.500000, -1.550000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_THIGH].emplace_back(KeyFrame{ glm::vec3(-3.250000, -6.650000, 2.200000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_CALF].emplace_back(KeyFrame{ glm::vec3(-3.350000, -10.000000, 1.150000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_FOOT].emplace_back(KeyFrame{ glm::vec3(-1.350000, -16.500000, -1.550000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });

		animator->addClip("Stand", keyFrameBuffer);
		animator->setCurrentClip("Stand");

		// Add animation here
		animator->addClip("Walk", "../res/animation/walk.anim");	
		animator->addClip("Sit-Up", "../res/animation/sit-up.anim");
		animator->addClip("Push-Up", "../res/animation/push-up.anim");
		animator->addClip("Hopak", "../res/animation/hopak.anim");
		animator->addClip("APT", "../res/animation/apt.anim");
		animator->addClip("Shadow_Clone_Jutsu", "../res/animation/shadow_clone_jutsu.anim",
			{
				AnimationEvent{1, [=]() { robot.modifyInstance(1); }},
				AnimationEvent{8, [=]() { robot.modifyInstance(3); }}
			});
	}
}