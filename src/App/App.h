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
#include "../Graphic/ShaderProgram/shaderProgram.h"
#include "../Animation/Animator.h"


namespace CG
{
	class App
	{
    public:
        App();
        ~App();

        bool initialize();
        void loop();
        void terminate();

        static void setMode(bool isEditMode);
        inline Camera& getCamera() { return camera; }

    private:
        void update(double dt);
        void render();

        void GLInit();

    private:
        GLFWwindow* mainWindow;

        GUI gui;
        Camera camera;
        Light light;
        Animator animator;
        ShaderProgram program;
        MainScene* mainScene;

        double timeNow = 0;
        double timeLast = 0;
        double timeDelta = 0;

        static bool editMode;
	};
}

