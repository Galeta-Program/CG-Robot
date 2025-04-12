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
		MainScene(Camera& _camera);
		~MainScene();

		auto Initialize() -> bool;
		void Update(double dt);
		void Render();

		void ResetAction();
		void SetAction(int action);

		void SetMode(int mode);

		inline Model* getModel() { return &robot; }

	private:
		auto LoadScene() -> bool;

		void LoadModel();

		// void UpdateAction(double dt);

	private:
		Model robot;

		ShaderProgram program;
		UBO matVPUbo;

		Camera* camera;

		int action = 0; // idle
		GLenum mode = GL_FILL; 

		GLfloat movex, movey;

		/* unused
		int materialCount[PARTSNUM];
		GLuint ModelID;
		GLuint M_KaID;
		GLuint M_KdID;
		GLuint M_KsID;
		GLuint TextureID;
		*/
	};
}

