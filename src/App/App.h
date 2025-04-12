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

        void rotateAround(float angle, glm::vec3 axis);
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
        float angles[PARTSNUM];
        float position = 0.0;
        float angle = 0.0;
        float eyeAngley = 0.0;
        float eyedistance = 40.0;

        double timeNow = 0;
        double timeLast = 0;
        double timeDelta = 0;

    public:
        MainScene* GetMainScene() const
        {
            return mainScene;
        }
	};
}

