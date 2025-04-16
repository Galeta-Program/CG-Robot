#include "GUI.h"

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/string_cast.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <fstream>

#define TOP_BODY 0
#define LEFT_UPPER_ARM 1
#define LEFT_LOWER_ARM 2
#define LEFT_HAND 3
#define HEAD 4
#define RIGHT_UPPER_ARM 5
#define RIGHT_LOWER_ARM 6
#define RIGHT_HAND 7
#define BOTTOM_BODY 8
#define LEFT_THIGH 9
#define LEFT_CALF 10
#define LEFT_FOOT 11
#define RIGHT_THIGH 12
#define RIGHT_CALF 13
#define RIGHT_FOOT 14

namespace CG {

    GUI::GUI(GLFWwindow* window, MainScene* _scene) : partSelected(0)
    {
        if (window == nullptr)
        {
            return;
        }
        init(window, _scene);
    }

    GUI::~GUI()
    {}

    void GUI::init(GLFWwindow* window, MainScene* _scene)
    {
        const char* glsl_version = "#version 460";

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, false); // Disable this because we have manually handle the glfwSetMouseButtonCallback
        ImGui_ImplOpenGL3_Init(glsl_version);

        bindScene(_scene);
        robot = scene->getModel();
        selectedNode = &(robot->getPart(HEAD));
        editmodeFlag = false;

        animationSelected = 0;
    }

    void GUI::bindScene(MainScene* _scene)
    {
        if (_scene != nullptr)
        {
            scene = _scene;
        }
    }

    void GUI::render()
    {
        startFrame();
        mainPanel();
        _render();
    }

    void GUI::terminate()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void GUI::startFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GUI::mainPanel()
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(320, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

        ImGui::Begin("Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        if (ImGui::BeginTabBar("Mode"))
        {
            if (ImGui::BeginTabItem("Animation"))
            {
                editmodeFlag = false;
                scene->setMode(editmodeFlag);
                animationPanel();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Edit"))
            {
                editmodeFlag = true;
                scene->setMode(editmodeFlag);
                editPanel();
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
        ImGui::End();

        if (editmodeFlag)
        {
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 320, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(320, 250), ImGuiCond_Always);

            ImGui::Begin("Transform", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            transformPanel(selectedNode);
            ImGui::End();
        }
        else
        {
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 320, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(320, 250), ImGuiCond_Always);

            ImGui::Begin("Speed", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            speedPanel();
            ImGui::End();
        }
    }

    void GUI::animationPanel()
    {
        ImGui::SeparatorText("Usage");
        ImGui::Text("Select an animation to play.");
        ImGui::SeparatorText("Animations");

        ImGui::PushItemWidth(-1);  // Make ListBox fill available width

        std::vector<const char*> animationNames;
        for (const auto& str : scene->getAnimator()->getClipNames()) {
            animationNames.push_back(str.c_str());
        }
        
        if (ImGui::ListBox("##AnimationList", 
            &animationSelected, 
            animationNames.data(), 
            (int)(animationNames.size()),
            (int)(animationNames.size())))
        {
            scene->getAnimator()->setCurrentClip(std::string(animationNames[animationSelected]));
        }
        ImGui::PopItemWidth();

        importPanel();
        exportPanel();

    }

    void GUI::speedPanel()
    {
        ImGui::SeparatorText("Usage");
        ImGui::Text("Configure the speed of the animation.\n");

        float speed = scene->getAnimator()->getCurrentClipSpeed();
        if (ImGui::DragFloat("drag float", &speed, 0.005f, -0.1f, 10.0f, "%.3f"))
        {
            scene->getAnimator()->setCurrentClipSpeed(speed);
        }
    }

    void GUI::editPanel()
    {
        ImGui::SeparatorText("Usage");
        ImGui::Text("1. Press WASDQE to move camara.\n\
2. Right click your mouse and drag it\n\
   to move the camara up, down, left \n\
   and right\n\
3. Scroll your mouse up and down\n\
   to voom the camera in and out.\n\
4. Press ALT + WASD to rotate camara.\n\
5. Press your mouse middle button \n\
   to rotate camera.\n\
6. Choose a part to modify.\n\
7. Press \"Report\" to output transformation\n\
   info of each parts.\n");
        ImGui::SeparatorText("Parts");
        // Part names array
        const char* partNames[] = {
            "Head",
            "Upper Body",
            "Lower Body",
            "Right Upper Arm",
            "Right Lower Arm",
            "Right Hand",
            "Left Upper Arm",
            "Left Lower Arm",
            "Left Hand",
            "Right Thigh",
            "Right Calf",
            "Right Foot",
            "Left Thigh",
            "Left Calf",
            "Left Foot"
        };

        ImGui::PushItemWidth(-1);  // Make ListBox fill available width
        if (ImGui::ListBox("##PartsList", &partSelected, partNames, IM_ARRAYSIZE(partNames), IM_ARRAYSIZE(partNames)))
        {
            int index;
            switch (partSelected)
            {
            case 0: index = 4; break;
            case 1: index = 0; break;
            case 2: index = 8; break;
            case 3: index = 5; break;
            case 4: index = 6; break;
            case 5: index = 7; break;
            case 6: index = 1; break;
            case 7: index = 2; break;
            case 8: index = 3; break;
            case 9: index = 12; break;
            case 10: index = 13; break;
            case 11: index = 14; break;
            case 12: index = 9; break;
            case 13: index = 10; break;
            case 14: index = 11; break;
            }

            Model* robot = scene->getModel();
            selectedNode = &robot->getPart(index);
        }
        ImGui::PopItemWidth();

        exportPanel();

    }

    void GUI::exportPanel()
    {
        ImGui::SeparatorText("Export current motion");

        if (editmodeFlag)
        {
            ImGui::Text("(1) The file will be put under \n\
\"res/animation/\" directory.\n\
(2) Will append current motion to the file\n\
everytime you press the button.");
        }
        else
        {
            ImGui::Text("(1) The file will be put under \n\
\"res/animation/\" directory.\n\
(2) Will overwrite whole animation \n\
and its speed to the file\n\
everytime you press the button.\n");
        }

        ImGui::InputTextWithHint(" ", "ex. report.txt", outFileName, IM_ARRAYSIZE(outFileName));
        ImGui::Text("");
        if (ImGui::Button("Export"))
        {
            if (editmodeFlag)
            {
                exportFromEditor();
            }
            else
            {
                exportFromAnimator();
            }
        }
    }

    void GUI::importPanel()
    {
        ImGui::SeparatorText("Import animation file");
        ImGui::Text("(1) The file will be import from \n\
\"res/animation/\" directory.\n\
(2) Give the animation a name.\n");
        
        ImGui::InputTextWithHint("  ", "The import \"file name\".", inFileName, IM_ARRAYSIZE(inFileName));
        ImGui::InputTextWithHint("   ", "Name the animation.", animationName, IM_ARRAYSIZE(animationName));

        ImGui::Text("\n");

        if (ImGui::Button("Import"))
        {
            scene->getAnimator()->addClip(std::string(animationName), ("../res/animation/" + std::string(inFileName)).c_str());
        }

    }

    void GUI::transformPanel(Node* node)
    {
        glm::vec3 trans = node->getTranslateOffset();
        glm::vec3 angle = node->getRotateAngle();

        ImGui::SeparatorText("Translation");
        if (ImGui::DragFloat("x (Translate)", &trans[0], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            node->setTranslate(trans);
        }
        if (ImGui::DragFloat("y (Translate)", &trans[1], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            node->setTranslate(trans);
        }
        if (ImGui::DragFloat("z (Translate)", &trans[2], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            node->setTranslate(trans);
        }

        ImGui::SeparatorText("Rotation");
        if (ImGui::DragFloat("x (Rotate)", &angle[0], 0.05f, -360.0f, 360.0f, "%.3f"))
        {
            node->setRotate(angle);
        }
        if (ImGui::DragFloat("y (Rotate)", &angle[1], 0.05f, -360.0f, 360.0f, "%.3f"))
        {
            node->setRotate(angle);
        }
        if (ImGui::DragFloat("z (Rotate)", &angle[2], 0.05f, -360.0f, 360.0f, "%.3f"))
        {
            node->setRotate(angle);
        }
    }

    void GUI::_render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void GUI::exportFromEditor()
    {
        // set output file
        std::ofstream outFile((std::string("../res/animation/") + std::string(outFileName)).c_str(), std::ios_base::app); // append

        // write the local coord of each node
        for (int i = 0; i < 15; i++)
        {
            Node* node = &(robot->getPart(i));
            glm::vec3 trans = node->getTranslateOffset();
            glm::vec3 rotate = node->getRotateAngle();
            outFile << trans[0] << " " << trans[1] << " " << trans[2] << std::endl <<
                rotate[0] << " " << rotate[1] << " " << rotate[2] << std::endl;
        }
    }

    void GUI::exportFromAnimator()
    {
        // set output file
        std::ofstream outFile((std::string("../res/animation/") + std::string(outFileName)).c_str()); // overwrite
        const std::vector<Track>& tracks = scene->getAnimator()->getCurrentClipTrack();

        // Output the speed
        outFile << "speed " << scene->getAnimator()->getCurrentClipSpeed() << std::endl; 

        // write the local coord of each node
        for (int frame = 0; frame < tracks[0].keyFrames.size(); frame++)
        {
            for (int i = 0; i < 15; i++) // 15 nodes
            {
                Node* node = &(robot->getPart(i));
                glm::vec3 trans = tracks[i].keyFrames[frame].transOffset;
                glm::vec3 rotate = tracks[i].keyFrames[frame].rotatOffset;
                outFile << trans[0] << " " << trans[1] << " " << trans[2] << std::endl <<
                    rotate[0] << " " << rotate[1] << " " << rotate[2] << std::endl;
            }
        }
    }


}