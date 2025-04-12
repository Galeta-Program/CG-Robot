#pragma once
#include "../Scene/MainScene.h"
#include "../Model/Model.h"

#include <GLFW/glfw3.h>
#include <cstdlib>


namespace CG {
	class GUI
	{
	private:
		MainScene* scene; // Point to the scene that GUI is currently binding with
		Model* robot;
		Node* selectedNode;

		bool seperateMode;

		int partSelected;
		void startFrame();
		void mainPanel();
		void transformPanel(Node* node);
		void _render();

	public:
		GUI(GLFWwindow* window, MainScene* _scene);
		~GUI();

		void init(GLFWwindow* window, MainScene* _scene);
		void bindScene(MainScene* _scene);
		void render();
		void terminate();

		void report();
	};

}