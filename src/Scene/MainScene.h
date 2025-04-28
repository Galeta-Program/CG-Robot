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
		MainScene(Camera& _camera, Light& _light, Animator& _animator, ShaderProgram& _program);
		~MainScene();

		bool Initialize();
		void Render();

		inline Model* getModel() { return &robot; }
		inline Animator* getAnimator() { return animator; }
		inline Light* getLight() { return light; }

	private:
		bool loadScene();
		void loadModel();
		void loadAnimation();

		Model robot;
		Camera* camera;
		Light* light;
		Animator* animator;
		ShaderProgram* program;

		UBO matVPUbo;		
		//GLenum mode = GL_FILL;
	};
}

