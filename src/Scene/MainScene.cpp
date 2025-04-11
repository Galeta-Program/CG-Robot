#include <Utilty/LoadShaders.h>
#include <Utilty/OBJLoader.hpp>

#include "MainScene.h"

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
		UpdateModel();
	}

	void MainScene::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, mode);

		program.use(); //uniform參數數值前必須先use shader

		//update data to UBO for MVP
		matVPUbo.bind();
		matVPUbo.fillInData(0, sizeof(glm::mat4), &camera.GetViewMatrix()[0][0]);
		matVPUbo.fillInData(sizeof(glm::mat4), sizeof(glm::mat4), &camera.GetProjectionMatrix()[0][0]);
		matVPUbo.unbind();

		robot.render(program.getId());

		glFlush();
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
		*/

		// use debug
		if (TextureID == -1) std::cout << "Error! Couldn't find this uniform in shader" << std::endl;
		else std::cout << "This uniform is successfully found in shader" << std::endl;

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

	void MainScene::UpdateModel()
	{
		glm::mat4 Rotatation[PARTSNUM];
		glm::mat4 Translation[PARTSNUM];
		for (int i = 0; i < PARTSNUM; i++)
		{
			Models[i] = glm::mat4(1.0f);
			Rotatation[i] = glm::mat4(1.0f);
			Translation[i] = glm::mat4(1.0f);
		}
		float r, pitch, yaw, roll;
		float alpha, beta, gamma;

		//身體=======================================================
		//== 上身
		beta = angle;
		Rotatation[0] = rotate(beta, 0, 1, 0);
		Translation[0] = translate(0, 15.8 + position, 0);
		Models[0] = Translation[0] * Rotatation[0];

		//== 下身
		Translation[8] = translate(0, -5.3, 0);
		Models[8] = Models[0] * Translation[8] * Rotatation[8];
		//=============================================================



		//頭==========================================================
		Translation[4] = translate(0, 3.2, -0.5);
		Models[4] = Models[0] * Translation[4] * Rotatation[4];
		//============================================================



		//左手=======================================================
		//== 左上手臂
		yaw = glm::radians(beta); r = 3.7;
		alpha = angles[1];
		gamma = 10;
		Rotatation[1] = rotate(alpha, 1, 0, 0) * rotate(gamma, 0, 0, 1);//向前旋轉*向右旋轉
		Translation[1] = translate(7.3, 0, 0);

		Models[1] = Models[0] * Translation[1] * Rotatation[1];

		//== 左下手臂
		pitch = glm::radians(alpha); r = 3;
		roll = glm::radians(gamma);
		static int i = 0;
		i += 5;
		alpha = angles[2] - 0;
		//上手臂+下手臂向前旋轉*向右旋轉
		Rotatation[2] = rotate(alpha, 1, 0, 0);
		//延x軸位移以上手臂為半徑的圓周長:translate(0,r*cos,r*sin)
		//延z軸位移以上手臂為半徑角度:translate(r*sin,-rcos,0)
		Translation[2] = translate(3, -6.5, 0);

		Models[2] = Models[1] * Translation[2] * Rotatation[2];

		//== 左手掌
		pitch = glm::radians(alpha);
		//b = glm::radians(angles[2]);
		roll = glm::radians(gamma);
		//手掌角度與下手臂相同
		//Rotatation[3] = Rotatation[2];
		//延x軸位移以上手臂為半徑的圓周長:translate(0,r*cos,r*sin) ,角度為上手臂+下手臂
		Translation[3] = translate(3.7, -8.6, 4);
		Models[3] = Models[2] * Translation[3] * Rotatation[3];
		//============================================================

		//右手=========================================================
		//== 右上手臂
		gamma = -10; alpha = angles[5] = -angles[1];
		Rotatation[5] = rotate(alpha, 1, 0, 0) * rotate(gamma, 0, 0, 1);
		Translation[5] = translate(-7.3, 0, 0);
		Models[5] = Models[0] * Translation[5] * Rotatation[5];

		//== 右下手臂
		angles[6] = angles[2];
		pitch = glm::radians(alpha); r = -3;
		roll = glm::radians(gamma);
		alpha = angles[6] - 0;
		Rotatation[6] = rotate(alpha, 1, 0, 0);
		Translation[6] = translate(-3, -6.5, 0);
		Models[6] = Models[5] * Translation[6] * Rotatation[6];

		//== 右手掌
		pitch = glm::radians(alpha);
		//b = glm::radians(angles[7]);
		roll = glm::radians(gamma);
		Translation[7] = translate(-3.7, -8.6, 4);
		Models[7] = Models[6] * Translation[7] * Rotatation[7];
		//=============================================================





		//左腳=========================================================
		//== 左大腿
		alpha = angles[9]; gamma = 0;
		Rotatation[9] = rotate(alpha, 1, 0, 0) * rotate(gamma, 0, 0, 1);
		Translation[9] = translate(3.3, -7, 2.5);
		Models[9] = Models[8] * Translation[9] * Rotatation[9];

		//== 左小腿
		pitch = glm::radians(alpha);
		roll = glm::radians(gamma);
		alpha = angles[10];
		Translation[10] = translate(3, -9.5, 1.2);
		Rotatation[10] = rotate(alpha, 1, 0, 0);
		Models[10] = Models[9] * Translation[10] * Rotatation[10];



		//== 左腳
		alpha = angles[11];
		Translation[11] = translate(1.5, -17, -1.5);
		Rotatation[11] = rotate(alpha, 1, 0, 0);
		Models[11] = Models[10] * Translation[11] * Rotatation[11];
		//=============================================================

		//右腳=========================================================
		//== 右大腿
		alpha = angles[12] = -angles[9];
		gamma = -0;
		Rotatation[12] = rotate(alpha, 1, 0, 0) * rotate(gamma, 0, 0, 1);
		Translation[12] = translate(-3.3, -7, 2.5);
		Models[12] = Models[8] * Translation[12] * Rotatation[12];

		//== 右小腿
		pitch = glm::radians(alpha);
		roll = glm::radians(gamma);
		alpha = angles[13] = angles[10];
		Translation[13] = translate(-3, -9.5, 1.2);
		Rotatation[13] = rotate(alpha, 1, 0, 0);
		Models[13] = Models[12] * Translation[13] * Rotatation[13];

		//== 右腳
		alpha = angles[14];
		Translation[14] = translate(-1.5, -17, -1.5);
		Rotatation[14] = rotate(alpha, 1, 0, 0);
		Models[14] = Models[13] * Translation[14] * Rotatation[14];
		//=============================================================
	}
}