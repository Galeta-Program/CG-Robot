#pragma once
#include "../Scene/MainScene.h"
#include "../Model/Model.h"
#include "../Effect/EffectManager.h"
#include "../Graphic/ShaderProgram/GraphicShader.h"
#include <GLFW/glfw3.h>
#include <cstdlib>


namespace CG {

	class GUI
	{
	private:
		VAO vao;
		VBO<glm::vec3> vbo;
		EBO ebo;

		Animator* animator;
		MainScene* scene; // Point to the scene that GUI is currently binding with
		Model* robot;
		Node* selectedNode;
		ScreenRenderer* screenRenderer;
		SkyBox* skyBox;

		GraphicShader gs;

		int currentMode; // 0: showcase, 1: animation, 2: edit
		int previousMode;

		char outFileName[128];
		char inFileName[128];
		char animationName[128];

		bool screenEffectStates[5];
		static const int OBJECT_NUM = 10;
		bool objectsVisibilityStates[OBJECT_NUM];

		int partSelected;
		int animationSelected;
		int frameSelected; // which frame is selected
		int skyBoxSelected;

		std::vector<std::string> skyBoxNamesInStr;
		std::vector<std::string> frameNamesInStr;

		bool editFrame;
		bool haveSelectFrame; // whether a frame is selected

		bool haveEffect;
		bool previousHaveEffect;
		std::string effectName;

		// particle system
		glm::vec3 effectPos;
		glm::vec3 effectDir;

		// lightning
		glm::vec3 effectCenter;
		std::vector<glm::vec3> effectEndpoints;

		int effectSelected;


		void startFrame();
		void mainPanel();
		void showcasePanel();
		void animationPanel();
		void animationClipPanel();
		void chooseFramePannel();
		void speedPanel();
		void instancePanel();
		void screenEffectPanel();
		void objectsVisibilityPanel();
		void skyBoxPanel();
		void lightPanel();
		void editPanel();
		void transformPanel(Node* node);

		void effectPannel();
		void effectParamPanel();
		void fireParamPanel(ParticleSystem& ps);
		void lightningParamPanel(Lightning& ln);
		void fireworkParamPannel(ParticleSystem& ps);

		void exportPanel();
		void importPanel();
		void _render();

		void renderIcon(glm::vec3 pos);

	public:
		GUI(GLFWwindow* window, MainScene* _scene, Animator* _animator);
		~GUI();

		void init(GLFWwindow* window, MainScene* _scene, Animator* _animator);
		void bindScene(MainScene* _scene);
		void render();
		void terminate();

		void exportFrame();
		void exportFromEditor();
		void exportFromAnimator();

		void renderEffectIcon(
			const glm::mat4& viewMatrix,
			const glm::mat4& projectionMatrix,
			unsigned int emitter = -1);
	};
}