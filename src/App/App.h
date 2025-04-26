#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Scene/MainScene.h"
#include "../Window/GUI.h"
#include "../Scene/Camera.h"
#include "../Scene/Light.h"

namespace CG
{
	class App
	{
    public:
        App();
        ~App();

        auto initialize() -> bool;
        void loop();
        void terminate();

        inline Camera& getCamera() { return camera; }

    private:
        void update(double dt);
        void render();

        void GLInit();

    private:
        GLFWwindow* mainWindow;

        GUI gui;
        MainScene* mainScene;
        Camera camera;
        Light light;

        double timeNow = 0;
        double timeLast = 0;
        double timeDelta = 0;
	};
}

