#include <iostream>
#include <functional>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "App.h"


namespace CG
{
	double lastCursorX;
	double lastCursorY;

	bool mouseMiddlePressed;
	bool mouseRightPressed;


	static void keyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
			App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
			Camera* camera = &(app->getCamera());
			const float rotatSpeed = 2.0f;
			const float transSpeed = 0.5f;

			if (action == GLFW_REPEAT || action == GLFW_PRESS)
			{
				if (key == GLFW_KEY_W) // camera forward
				{
					if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS) {
						camera->zoom(-transSpeed);

					}
					else
					{
						camera->rotateAround(-rotatSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
					}
				}
				if (key == GLFW_KEY_S) // camera backward
				{
					if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS) {
						camera->zoom(transSpeed);						
					}
					else
					{
						camera->rotateAround(rotatSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
					}
				}
				if (key == GLFW_KEY_D) // camera go right
				{
					if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS) {
						camera->flatTranslate(transSpeed, 0);
					}
					else
					{
						camera->rotateAround(rotatSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
					}
				}
				if (key == GLFW_KEY_A) // camera go left
				{
					if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS) {
						camera->flatTranslate(-transSpeed, 0);
					}
					else
					{
						camera->rotateAround(-rotatSpeed, glm::vec3(0.0f, 1.0f, 0.0f));

					}
				}
				if (key == GLFW_KEY_E) // camera go up
				{
					camera->flatTranslate(0, transSpeed);
				}
				if (key == GLFW_KEY_Q) // camera go down
				{
					camera->flatTranslate(0, - transSpeed);
				}
			}
		
	}

	static void windowResize(GLFWwindow* window, int width, int height)
	{}

	static void mouseEvent(GLFWwindow* window, int button, int action, int mods)
	{

		ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureMouse) {
			if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
			{
				glfwGetCursorPos(window, &lastCursorX, &lastCursorY);
				mouseMiddlePressed = true;

			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
			{
				mouseMiddlePressed = false;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			{
				glfwGetCursorPos(window, &lastCursorX, &lastCursorY);
				mouseRightPressed = true;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
			{
				mouseRightPressed = false;
			}
		}
	}

	static void mouseScroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		const float transSpeed = 1.5f;

		App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
		Camera* camera = &(app->getCamera());
		camera->zoom(-transSpeed * yoffset);

	}

	static void cursorEvent(GLFWwindow* window, double xpos, double ypos)
	{
		if (mouseMiddlePressed || mouseRightPressed)
		{
			App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
			Camera* camera = &(app->getCamera());

			double x = xpos - lastCursorX;
			double y = ypos - lastCursorY;

			float transSpeed = 0.125f;
			float rotatSpeed = 0.25f;

			if (mouseMiddlePressed)
			{
				camera->rotateAround(-y * rotatSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
				camera->rotateAround(-x * rotatSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
			}

			if (mouseRightPressed)
			{
				camera->flatTranslate(-x * transSpeed, y * transSpeed);
			}

			lastCursorX = xpos;
			lastCursorY = ypos;
		}
	}

	App::App():
		gui(nullptr, nullptr)
	{
		mainWindow = nullptr;

		/*
		controlWindow = nullptr;
		showControlWindow = true;
		*/

		mainScene = nullptr;
	}

	App::~App()
	{

	}

	auto App::Initialize() -> bool
	{
		// Set error callback
		glfwSetErrorCallback([](int error, const char* description)
			{ fprintf(stderr, "GLFW Error %d: %s\n", error, description); });

		// Initialize GLFW
		if (!glfwInit())
			return false;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

		// Create window with graphics context
		mainWindow = glfwCreateWindow(1280, 720, "Group6", nullptr, nullptr);
		if (mainWindow == nullptr)
			return false;
		glfwMakeContextCurrent(mainWindow);
		glfwSwapInterval(1); // Enable vsync

		// Initialize GLEW
		glewExperimental = GL_TRUE;
		GLenum glew_err = glewInit();
		if (glew_err != GLEW_OK)
		{
			throw std::runtime_error(std::string("Error initializing GLEW, error: ") + (const char*)glewGetErrorString(glew_err));
			return false;
		}

		GLInit();

		glfwSetWindowUserPointer(mainWindow, this);
		glfwSetKeyCallback(mainWindow, keyPress);
		glfwSetFramebufferSizeCallback(mainWindow, windowResize);
		glfwSetMouseButtonCallback(mainWindow, mouseEvent);
		glfwSetScrollCallback(mainWindow, mouseScroll);
		glfwSetCursorPosCallback(mainWindow, cursorEvent);

		// Camera matrix
		camera.LookAt(glm::vec3(0, -20, 40), glm::vec3(0, -20, 0), glm::vec3(0, 1, 0));

		//controlWindow = new ControlWindow();
		//controlWindow->Initialize();

		mainScene = new MainScene(camera);
		mainScene->Initialize();

		//controlWindow->SetTargetScene(mainScene);
		gui.init(mainWindow, mainScene);

		// Initialization done
		return true;
	}

	void App::Loop()
	{
		while (!glfwWindowShouldClose(mainWindow))
		{
			glfwPollEvents();

			timeNow = glfwGetTime();
			timeDelta = timeNow - timeLast;
			timeLast = timeNow;
			Update(timeDelta);

			Render();
			gui.render();

			/*
			if (showControlWindow)
			{
				controlWindow->Display();
			}
			*/
			
			ImGuiIO& io = ImGui::GetIO();
			(void)io;
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			glfwSwapBuffers(mainWindow);
		}
	}

	void App::Terminate()
	{
		gui.terminate();

		glfwDestroyWindow(mainWindow);
		glfwTerminate();
	}

	void App::Update(double dt)
	{
		mainScene->Update(dt);
	}

	void App::Render()
	{
		int display_w, display_h;
		glfwGetFramebufferSize(mainWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		mainScene->Render();
	}
	void App::GLInit()
	{
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glClearColor(0.0, 0.0, 0.0, 1); //black screen
	}
}