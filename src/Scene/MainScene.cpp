#include <Utilty/LoadShaders.h>
#include <Utilty/OBJLoader.hpp>
#include <iostream>

#include "MainScene.h"
#include "../Utilty/Error.h"

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
	MainScene::MainScene(Camera& _camera)
	{
		camera = &_camera;
	}

	MainScene::~MainScene()
	{
	}

	auto MainScene::Initialize() -> bool
	{
		return LoadScene();
	}

	void MainScene::Update(double dt)
	{
		animator.animate(dt);
	}

	void MainScene::Render()
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, mode));

		program.use(); //uniform參數數值前必須先use shader

		//update data to UBO for MVP
		matVPUbo.bind();
		matVPUbo.fillInData(0, sizeof(glm::mat4), camera->GetViewMatrix());
		matVPUbo.fillInData(sizeof(glm::mat4), sizeof(glm::mat4), camera->GetProjectionMatrix());
		matVPUbo.unbind();

		robot.render(program.getId());

		GLCall(glFlush());
	}
	
	void MainScene::ResetAction()
	{
		this->action = 0; // idle
	}

	void MainScene::SetAction(int action)
	{
		this->action = action;
	}

	void MainScene::SetMode(int mode)
	{
		switch (mode)
		{
		case 0:
			this->mode = GL_FILL;
			break;
		case 1:
			this->mode = GL_LINE;
			break;
		default:
			this->mode = GL_FILL;
			break;
		}
	}

	auto MainScene::LoadScene() -> bool
	{
		ShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "../res/shaders/DSPhong_Material.vp" }, //vertex shader
			{ GL_FRAGMENT_SHADER, "../res/shaders/DSPhong_Material.fp" }, //fragment shader
			{ GL_NONE, NULL } };
		program.load(shaders); //讀取shader

		program.use(); //uniform參數數值前必須先use shader

		/*
		ModelID = glGetUniformLocation(program.getId(), "Model");
		M_KaID = glGetUniformLocation(program.getId(), "Material.Ka");
		M_KdID = glGetUniformLocation(program.getId(), "Material.Kd");
		M_KsID = glGetUniformLocation(program.getId(), "Material.Ks");
		TextureID = glGetUniformLocation(program.getId(), "Texture");

		// use debug
		if (TextureID == -1) std::cout << "Error! Couldn't find this uniform in shader" << std::endl;
		else std::cout << "This uniform is successfully found in shader" << std::endl;
		*/

		LoadModel();
		loadAnimation();

		//UBO
		matVPUbo.initialize(sizeof(glm::mat4) * 2);
		matVPUbo.associateWithShaderBlock(program.getId(), "MatVP", 0);

		return true;
	}

	void MainScene::LoadModel()
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

		animator.target(&robot);
	}

	void MainScene::loadAnimation()
	{
		std::vector<std::vector<KeyFrame>> keyFrameBuffer;

		for (int i = 0; i < 15; i++)
		{
			keyFrameBuffer.emplace_back();
		}

		keyFrameBuffer[TOP_BODY].emplace_back(KeyFrame{ glm::vec3(0.000000, 0.000000, 0.000000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[LEFT_UPPER_ARM].emplace_back(KeyFrame{ glm::vec3(7.650000, 0.000000, 0.000000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[LEFT_LOWER_ARM].emplace_back(KeyFrame{ glm::vec3(2.900000, -6.400000, 0.000000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[LEFT_HAND].emplace_back(KeyFrame{ glm::vec3(3.650000, -8.500000, 3.900000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[HEAD].emplace_back(KeyFrame{ glm::vec3(0.000000, 3.100000, 0.000000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[RIGHT_UPPER_ARM].emplace_back(KeyFrame{ glm::vec3(-7.650000, 0.050000, 0.000000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[RIGHT_LOWER_ARM].emplace_back(KeyFrame{ glm::vec3(-2.950000, -6.400000, 0.000000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[RIGHT_HAND].emplace_back(KeyFrame{ glm::vec3(-3.650000, -8.500000, 3.900000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[BOTTOM_BODY].emplace_back(KeyFrame{ glm::vec3(0.000000, -5.050000, -1.450000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[LEFT_THIGH].emplace_back(KeyFrame{ glm::vec3(3.250000, -6.650000, 2.200000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[LEFT_CALF].emplace_back(KeyFrame{ glm::vec3(3.000000, -10.000000, 1.150000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[LEFT_FOOT].emplace_back(KeyFrame{ glm::vec3(1.350000, -16.500000, -1.550000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[RIGHT_THIGH].emplace_back(KeyFrame{ glm::vec3(-3.250000, -6.650000, 2.200000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[RIGHT_CALF].emplace_back(KeyFrame{ glm::vec3(-3.350000, -10.000000, 1.150000) , glm::vec3(0.000000, 0.000000, 0.000000) });
		keyFrameBuffer[RIGHT_FOOT].emplace_back(KeyFrame{ glm::vec3(-1.350000, -16.500000, -1.550000) , glm::vec3(0.000000, 0.000000, 0.000000) });

		animator.addClip("Stand", keyFrameBuffer);
		animator.setCurrentClip("Stand");
	}
}