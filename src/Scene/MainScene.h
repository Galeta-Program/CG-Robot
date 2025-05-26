#pragma once

#include <array>
#include <string>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "../Graphic/ShaderProgram/ComputeShader.h"
#include "../Graphic/ShaderProgram/GraphicShader.h"
#include "../Model/Model.h"
#include "../Graphic/UBO.h"
#include "../Animation/Animator.h"
#include "../Scene/Light.h"
#include "../Model/LoadedObject.h"
#include "../Model/ManualObject.h"
#include "../Model/SkyBox.h"
#include "../Effect/ParticleSystem.h"
#include "../Effect/ShadowSystem.h"
#include "../Scene/ScreenRenderer.h"

#include "../Graphic/VAO.h"
#include "../Graphic/VBO.h"
#include "../Graphic/Material/Texture.h"

constexpr auto PARTSNUM = 15;

namespace CG
{
	class MainScene
	{
	public:
		MainScene(Camera& _camera, Light& _light, Animator& _animator, GraphicShader& _program, ParticleSystem& ps);
		~MainScene();

		bool Initialize(int display_w, int display_h);
		void Render(double timeNow, double timeDelta, int display_w, int display_h);
		void Resize(int display_w, int display_h);

		inline Model* getModel() { return &robot; }
		inline Animator* getAnimator() { return animator; }
		inline Light* getLight() { return light; }
		inline ScreenRenderer* getScreenRenderer() { return &screenRenderer; }

	private:
		bool loadScene(int display_w, int display_h);
		void loadModel();
		void loadAnimation();

		Model robot;
		Camera* camera;
		Light* light;
		Animator* animator;
		GraphicShader* program;
		ParticleSystem* firePS;
		ManualObject ground;
		SkyBox skyBox;

		ManualObject sphare;
		ShadowSystem shadowSystem;
		ScreenRenderer screenRenderer;

		/*
		* debug use
		VAO vao;
		VBO<float> vbo;
		Texture texture;
		GraphicShader testQuadShader;
		*/

		UBO matVPUbo;	
		//GLenum mode = GL_FILL;
	};
}

