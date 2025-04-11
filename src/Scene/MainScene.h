#pragma once

#include <array>
#include <string>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "../Graphic/ShaderProgram/shaderProgram.h"
#include "../Model/Model.h"
#include "../Graphic/UBO.h"

constexpr auto PARTSNUM = 15;

namespace CG
{
	class MainScene
	{
	public:
		MainScene();
		~MainScene();

		auto Initialize() -> bool;
		void Update(double dt);
		void Render();

		void OnResize(int width, int height);
		void OnKeyboard(int key, int action);

		void ResetAction();
		void SetAction(int action);

		void SetMode(int mode);

	private:
		auto LoadScene() -> bool;

		void LoadModel();

		void UpdateAction(double dt);
		void UpdateModel();

	private:
		Camera camera;

		Model robot;
		UBO matVPUbo;

		ShaderProgram program;

		int action = 0; // idle
		GLenum mode = 0; // fill

		float angles[PARTSNUM];
		float position = 0.0;
		float angle = 0.0;
		float eyeAngley = 0.0;
		float eyedistance = 40.0;
		float size = 1;
		GLfloat movex, movey;
		GLint MatricesIdx;
		GLuint ModelID;

		int materialCount[PARTSNUM];

		GLuint M_KaID;
		GLuint M_KdID;
		GLuint M_KsID;
		GLuint TextureID;

		glm::mat4 Model;
		glm::mat4 Models[PARTSNUM];
	};
}

