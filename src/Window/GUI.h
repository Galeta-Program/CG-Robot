#pragma once
#include "../Scene/MainScene.h"
#include "../Model/Model.h"

#include <GLFW/glfw3.h>
#include <cstdlib>


namespace CG {

	class GUI
	{
	private:
		Animator* animator;
		MainScene* scene; // Point to the scene that GUI is currently binding with
		Model* robot;
		Node* selectedNode;

		bool editmodeFlag;
		bool previousMode;
		char outFileName[128];
		char inFileName[128];
		char animationName[128];

		int partSelected;
		int animationSelected;

		void startFrame();
		void mainPanel();
		void animationPanel();
		void speedPanel();
		void instancePanel();
		void lightPanel();
		void editPanel();
		void transformPanel(Node* node);
		void exportPanel();
		void importPanel();
		void _render();

	public:
		GUI(GLFWwindow* window, MainScene* _scene, Animator* _animator);
		~GUI();

		void init(GLFWwindow* window, MainScene* _scene, Animator* _animator);
		void bindScene(MainScene* _scene);
		void render();
		void terminate();

		void exportFromEditor();
		void exportFromAnimator();
	};

}