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

	private:
		Camera camera;
		Model robot;

		ShaderProgram program;
		UBO matVPUbo;

		int action = 0; // idle
		GLenum mode = GL_FILL; 

		float angles[PARTSNUM];
		float position = 0.0;
		float angle = 0.0;
		float eyeAngley = 0.0;
		float eyedistance = 40.0;
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

