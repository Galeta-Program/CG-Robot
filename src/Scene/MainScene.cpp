#include <Utilty/LoadShaders.h>
#include <Utilty/OBJLoader.hpp>
#include <iostream>

#include "MainScene.h"
#include "../Utilty/Error.h"

static glm::mat4 translate(float x, float y, float z)
{
	glm::vec4 t = glm::vec4(x, y, z, 1); //w = 1 ,則x,y,z=0時也能translate
	glm::vec4 c1 = glm::vec4(1, 0, 0, 0);
	glm::vec4 c2 = glm::vec4(0, 1, 0, 0);
	glm::vec4 c3 = glm::vec4(0, 0, 1, 0);
	glm::mat4 M = glm::mat4(c1, c2, c3, t);
	return M;
}
static glm::mat4 scale(float x, float y, float z)
{
	glm::vec4 c1 = glm::vec4(x, 0, 0, 0);
	glm::vec4 c2 = glm::vec4(0, y, 0, 0);
	glm::vec4 c3 = glm::vec4(0, 0, z, 0);
	glm::vec4 c4 = glm::vec4(0, 0, 0, 1);
	glm::mat4 M = glm::mat4(c1, c2, c3, c4);
	return M;
}

static glm::mat4 rotate(float angle, float x, float y, float z)
{
	float r = glm::radians(angle);
	glm::mat4 M = glm::mat4(1);

	glm::vec4 c1 = glm::vec4(cos(r) + (1 - cos(r)) * x * x, (1 - cos(r)) * y * x + sin(r) * z, (1 - cos(r)) * z * x - sin(r) * y, 0);
	glm::vec4 c2 = glm::vec4((1 - cos(r)) * y * x - sin(r) * z, cos(r) + (1 - cos(r)) * y * y, (1 - cos(r)) * z * y + sin(r) * x, 0);
	glm::vec4 c3 = glm::vec4((1 - cos(r)) * z * x + sin(r) * y, (1 - cos(r)) * z * y - sin(r) * x, cos(r) + (1 - cos(r)) * z * z, 0);
	glm::vec4 c4 = glm::vec4(0, 0, 0, 1);
	M = glm::mat4(c1, c2, c3, c4);
	return M;
}

namespace CG
{
	MainScene::MainScene()
	{
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
		UpdateAction(dt);
	}

	void MainScene::Render()
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, mode));

		program.use(); //uniform參數數值前必須先use shader

		//update data to UBO for MVP
		matVPUbo.bind();
		matVPUbo.fillInData(0, sizeof(glm::mat4), camera.GetViewMatrix());
		matVPUbo.fillInData(sizeof(glm::mat4), sizeof(glm::mat4), camera.GetProjectionMatrix());
		matVPUbo.unbind();

		robot.render(program.getId());

		GLCall(glFlush());
	}

	void MainScene::OnResize(int width, int height)
	{
		std::cout << "MainScene Resize: " << width << " " << height << std::endl;
	}

	void MainScene::OnKeyboard(int key, int action)
	{
		std::cout << "MainScene OnKeyboard: " << key << " " << action << std::endl;

		if (action == GLFW_RELEASE || action == GLFW_REPEAT)
		{
			switch (key)
			{
			case GLFW_KEY_1:
				angle += 5;
				if (angle >= 360) angle = 0;
				printf("beta:%f\n", angle);
				break;
			case GLFW_KEY_2:
				angle -= 5;
				if (angle <= 0) angle = 360;
				printf("beta:%f\n", angle);
				break;
			case GLFW_KEY_W:
				eyedistance -= 0.5;
				break;
			case GLFW_KEY_S:
				eyedistance += 0.5;
				break;
			case GLFW_KEY_A:
				eyeAngley -= 10;
				break;
			case GLFW_KEY_D:
				eyeAngley += 10;
				break;
			case GLFW_KEY_R:
				angles[1] -= 5;
				if (angles[1] == -360) angles[1] = 0;
				movey = 0;
				movex = 0;
				break;
			case GLFW_KEY_T:
				angles[2] -= 5;
				if (angles[2] == -360) angles[2] = 0;
				movey = 0;
				movex = 0;
				break;
			}
		}
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
		// set camara
		float eyeRadiany = glm::radians(eyeAngley);
		camera.LookAt(
			glm::vec3(eyedistance * sin(eyeRadiany), 2, eyedistance * cos(eyeRadiany)), // Camera is at (0,0,20), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

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

		// Camera matrix
		camera.LookAt(glm::vec3(0, 10, 25), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		LoadModel();

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
			"../res/models2/top_body.obj",

			"../res/models2/left_upper_arm.obj",
			"../res/models2/left_lower_arm.obj",
			"../res/models2/left_hand.obj",

			"../res/models2/head.obj",

			"../res/models2/right_upper_arm.obj",
			"../res/models2/right_lower_arm.obj",
			"../res/models2/right_hand.obj",

			"../res/models2/bottom_body.obj",

			"../res/models2/left_thigh.obj",
			"../res/models2/left_calf.obj", 
			"../res/models2/left_foot.obj", 

			"../res/models2/right_thigh.obj",
			"../res/models2/right_calf.obj", 
			"../res/models2/right_foot.obj", 
			});

		robot.initialize(mtlPaths, objPaths);
		robot.gatherPartsData();
	}


	void MainScene::UpdateAction(double dt)
	{
		static double _frame = 0;

		if (action == 0) // stand
		{
			_frame = 0;
			for (int i = 0; i < PARTSNUM; i++)
			{
				angles[i] = 0.0f;
			}
			position = 0;
		}
		else if (action == 1) // walk
		{
			_frame += dt;

			if (_frame >= 13)
			{
				_frame = 1;
			}

			int frame = _frame;

			switch (frame)
			{
			case 1:
			case 2:
			case 3:
				angles[1] += 10 * dt;
				angles[9] -= 15 * dt;
				angles[10] += 5 * dt;
				position += 0.1 * dt;
				break;
			case 4:
			case 5:
			case 6:
				angles[1] -= 10 * dt;
				angles[9] += 15 * dt;
				angles[10] -= 5 * dt;
				position -= 0.1 * dt;
				break;
			case 7:
			case 8:
			case 9:
				angles[1] -= 10 * dt;
				angles[9] += 15 * dt;
				angles[10] += 5 * dt;
				position += 0.1 * dt;
				break;
			case 10:
			case 11:
			case 12:
				angles[1] += 10 * dt;
				angles[9] -= 15 * dt;
				angles[10] -= 5 * dt;
				position -= 0.1 * dt;
				break;
			}
		}
	}

	
}