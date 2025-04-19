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
#include "../Animation/Animator.h"
#include "../Scene/Light.h"

constexpr auto PARTSNUM = 15;

namespace CG
{
	class MainScene
	{
	public:
		MainScene(Camera& _camera, Light& _light);
		~MainScene();

		auto Initialize() -> bool;
		void Update(double dt);
		void Render();

		void setMode(bool isEditMode);

		inline Model* getModel() { return &robot; }
		inline Animator* getAnimator() { return &animator; }
		inline Light* getLight() { return light; }

	private:
		auto LoadScene() -> bool;
		void LoadModel();
		void loadAnimation();
	private:
		Model robot;
		Camera* camera;
		Light* light;
		Animator animator;

		ShaderProgram program;
		UBO matVPUbo;

		bool editMode;

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

