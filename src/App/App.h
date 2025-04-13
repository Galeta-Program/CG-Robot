#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Window/ControlWindow.h"
#include "Scene/MainScene.h"
#include "../Scene/GUI.h"
#include "../Scene/Camera.h"

namespace CG
{
	class App
	{
    public:
        App();
        ~App();

        auto Initialize() -> bool;
        void Loop();
        void Terminate();

        inline Camera& getCamera() { return camera; }

    private:
        void Update(double dt);
        void Render();

        void GLInit();

    private:
        GLFWwindow* mainWindow;

        //ControlWindow* controlWindow;
        //bool showControlWindow;
        GUI gui;
        MainScene* mainScene;
        Camera camera;

        double timeNow = 0;
        double timeLast = 0;
        double timeDelta = 0;
	};
}

