#include "GUI.h"

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/string_cast.hpp"
#include "../App/App.h"
#include "../Effect/EffectManager.h"
#include "../Utilty/Error.h"
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <fstream>
#include <sstream>

#define HEAD 4

namespace CG {

    GUI::GUI(GLFWwindow* window, MainScene* _scene, Animator* _animator) :
        partSelected(0),
        haveEffect(false),
        previousHaveEffect(false),
        effectName(""),
        effectPos(0, 0, 0),
        effectDir(0, 0, 1),
        gs(),
        vao(),
        vbo(),
        ebo()
    {
        if (window == nullptr)
        {
            return;
        }

        init(window, _scene, _animator);
    }

    GUI::~GUI()
    {}

    void GUI::init(GLFWwindow* window, MainScene* _scene, Animator* _animator)
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

        this->animator = _animator;

        bindScene(_scene);
        robot = scene->getModel();
        screenRenderer = scene->getScreenRenderer();
        selectedNode = &(robot->getPart(HEAD));
        currentMode = 0;
        previousMode = 0;

        animationSelected = 0;
        frameSelected = 0;

        for (int i = 0; i < animator->getFrameAmount(); i++)
        {
            frameNamesInStr.emplace_back(std::to_string(i));
        }

        editFrame = false;
        haveSelectFrame = false;

        ShaderInfo shaders[] = {
            { GL_VERTEX_SHADER, "../res/shaders/guiIcon.vp" },
            { GL_FRAGMENT_SHADER, "../res/shaders/guiIcon.fp" },
            { GL_NONE, NULL } };
        gs.load(shaders);

        std::vector<glm::vec3> block = {
        glm::vec3(-1, 1, -1),
        glm::vec3(1, 1, -1),
        glm::vec3(-1, 1, 1),
        glm::vec3(1, 1, 1),
        glm::vec3(-1, -1, -1),
        glm::vec3(1, -1, -1),
        glm::vec3(-1, -1, 1),
        glm::vec3(1, -1, 1),
        };

        std::vector<unsigned int> blockElement = {
            1, 2, 3, 1, 0, 2, 3, 6, 7, 3, 2, 6, 1, 3, 7, 1, 7, 5, 4, 0, 1, 5, 4, 1, 0, 6, 2, 0, 4, 6, 6, 4, 5, 6, 5, 7
        };

        vao.bind();
        vbo.initialize(block);
        GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
        glEnableVertexAttribArray(0);
        ebo.initialize(blockElement);
        vao.unbind();
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
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x * 0.2, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

        ImGui::Begin("Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        if (ImGui::BeginTabBar("Mode"))
        {
            if (ImGui::BeginTabItem("Showcase"))
            {
                haveSelectFrame = false;
                currentMode = 0;

                if (previousMode != currentMode)
                {
                    previousMode = currentMode;
                    App::setMode(currentMode);
                }

                showcasePanel();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Animation"))
            {
                currentMode = 1;

                if (previousMode != currentMode)
                {
                    previousMode = currentMode;
                    App::setMode(currentMode);
                }

                animationPanel();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Edit"))
            {
                currentMode = 2;

                if (previousMode != currentMode)
                {
                    previousMode = currentMode;
                    App::setMode(currentMode);
                    for (int i = 0; i < robot->getPartsAmount(); i++)
                    {
                        robot->getPart(i).updateEuler();
                    }
                }

                editPanel();
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
        ImGui::End();

        if (currentMode == 2)
        {
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.8, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x * 0.2, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

            ImGui::Begin("Transform", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            transformPanel(selectedNode);
            effectPannel();
            ImGui::End();
        }
        else
        {
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.8, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x * 0.2, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

            ImGui::Begin("Configurations", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

            if (ImGui::CollapsingHeader("Speed", ImGuiTreeNodeFlags_DefaultOpen))
            {
                speedPanel();
            }
            if (ImGui::CollapsingHeader("Instance", ImGuiTreeNodeFlags_DefaultOpen))
            {
                instancePanel();
            }
            if (ImGui::CollapsingHeader("Screen Effect", ImGuiTreeNodeFlags_DefaultOpen))
            {
                screenEffectPanel();
            }
            if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
            {
                lightPanel();
            }

            if (currentMode == 1)
            {
                if (ImGui::CollapsingHeader("Select Frame", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    chooseFramePannel();
                }
            }
            ImGui::End();
        }
    }

    void GUI::showcasePanel()
    {
        animationClipPanel();
        importPanel();
    }

    void GUI::animationClipPanel()
    {
        ImGui::SeparatorText("Usage");
        ImGui::Text("Select an animation to play.");
        ImGui::SeparatorText("Animations");

        ImGui::PushItemWidth(-1);  // Make ListBox fill available width

        std::vector<const char*> animationNames;
        for (const auto& str : animator->getClipNames()) {
            animationNames.push_back(str.c_str());
        }

        if (ImGui::ListBox("##AnimationList",
            &animationSelected,
            animationNames.data(),
            (int)(animationNames.size()),
            (int)(animationNames.size())))
        {
            animator->deleteCurrentFrameClip();

            if (animationNames.size() > animationSelected)
            {
                animator->setCurrentClip(std::string(animationNames[animationSelected]));
            }

            frameNamesInStr.clear();
            for (int i = 0; i < animator->getFrameAmount(); i++)
            {
                frameNamesInStr.emplace_back(std::to_string(i));
            }

            frameSelected = 0;
            haveSelectFrame = false;
        }
        ImGui::PopItemWidth();
    }

    void GUI::chooseFramePannel()
    {
        ImGui::SeparatorText("Usage");
        ImGui::Text("Select an frame to play.");
        ImGui::SeparatorText("Frames");

        ImGui::PushItemWidth(-1);  // Make ListBox fill available width

        std::vector<const char*> frameNames;
        for (int i = 0; i < frameNamesInStr.size(); i++)
        {
            frameNames.emplace_back(frameNamesInStr[i].c_str());
        }

        if (ImGui::ListBox("##FrameList",
            &frameSelected,
            frameNames.data(),
            (int)(frameNames.size()),
            (int)(frameNames.size())))
        {
            haveSelectFrame = true;
            if (frameSelected >= (int)frameNames.size())
            {
                frameSelected = 0;
            }

            animator->makeFrameToClip((int)(std::stoi(std::string(frameNames[frameSelected]))));
        }
        ImGui::PopItemWidth();
    }

    void GUI::animationPanel()
    {
        animationClipPanel();
        importPanel();
        exportPanel();
    }

    void GUI::instancePanel()
    {
        ImGui::SeparatorText("Usage");
        ImGui::Text("Set the number of instances.\n");

        int instancingCount = robot->getInstancingCount();
        if (ImGui::DragInt("    ", &instancingCount, 0.5f, 0, 10000, "%d"))
        {
            robot->modifyInstance(instancingCount);
        }
    }

    void GUI::screenEffectPanel()
    {
        ImGui::SeparatorText("Usage");
        ImGui::Text("Set the effect of screen.\n");

        const char* screenEffects[] = { "Pixelation", "Toonshader", "Motion Blur" };
        
        ImGui::Checkbox(screenEffects[0], &screenEffectStates[0]);
        float pixalSize = screenRenderer->getPixelSize();
        if (ImGui::DragFloat("      ", &pixalSize, 0.05f, 0.01f, 100.0f, "%.3f"))
        {
            screenRenderer->setPixelSize(pixalSize);
        }
        ImGui::Checkbox(screenEffects[1], &screenEffectStates[1]);
        ImGui::Checkbox(screenEffects[2], &screenEffectStates[2]);

        screenRenderer->setPixelate(screenEffectStates[0]);
        screenRenderer->setToonshader(screenEffectStates[1]);
        screenRenderer->setMotionBlur(screenEffectStates[2]);
     
    }

    void GUI::speedPanel()
    {
        ImGui::SeparatorText("Usage");
        ImGui::Text("Configure the speed of the animation.\n");

        float speed = animator->getCurrentClipSpeed();
        if (ImGui::DragFloat("     ", &speed, 0.005f, -0.1f, 20.0f, "%.3f"))
        {
            animator->setCurrentClipSpeed(speed);
        }
    }

    void GUI::lightPanel()
    {
        ImGui::SeparatorText("Usage");
        ImGui::Text("Configure the position,\n\
shininess and color of \n\
the light source.\n");

        ImGui::SeparatorText("Translate");
        glm::vec3 pos = scene->getLight()->getPos();
        if (ImGui::DragFloat("light x", &pos[0], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            scene->getLight()->setPosition(pos);
        }
        if (ImGui::DragFloat("light y", &pos[1], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            scene->getLight()->setPosition(pos);
        }
        if (ImGui::DragFloat("light z", &pos[2], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            scene->getLight()->setPosition(pos);
        }

        ImGui::SeparatorText("Shininess");
        float shininess = scene->getLight()->getShininess();
        if (ImGui::DragFloat("shininess", &shininess, 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            scene->getLight()->setShininess(shininess);
        }

        ImGui::SeparatorText("Color");
        glm::vec3 color = scene->getLight()->getColor();
        if (ImGui::DragFloat("light r", &color[0], 0.0005f, 0, 1, "%.3f"))
        {
            scene->getLight()->setColor(color);
        }
        if (ImGui::DragFloat("light g", &color[1], 0.0005f, 0, 1, "%.3f"))
        {
            scene->getLight()->setColor(color);
        }
        if (ImGui::DragFloat("light b", &color[2], 0.0005f, 0, 1, "%.3f"))
        {
            scene->getLight()->setColor(color);
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
            "Right Paw",
            "Left Upper Arm",
            "Left Lower Arm",
            "Left Paw",
            "Right Thigh",
            "Right Calf",
            "Right Foot",
            "Left Thigh",
            "Left Calf",
            "Left Foot",
            
            "Left Thumb 1",
            "Left Thumb 2",
            "Left Thumb 3",
            "Left Index 1",
            "Left Index 2",
            "Left Index 3",
            "Left Middle 1",
            "Left Middle 2",
            "Left Middle 3",
            "Left Ring 1",
            "Left Ring 2",
            "Left Ring 3",
            "Left Little 1",
            "Left Little 2",
            "Left Little 3",

            "Right Thumb 1",
            "Right Thumb 2",
            "Right Thumb 3",
            "Right Index 1",
            "Right Index 2",
            "Right Index 3",
            "Right Middle 1",
            "Right Middle 2",
            "Right Middle 3",
            "Right Ring 1",
            "Right Ring 2",
            "Right Ring 3",
            "Right Little 1",
            "Right Little 2",
            "Right Little 3",

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
            case 15: index = 15; break;
            case 16: index = 16; break;
            case 17: index = 17; break;
            case 18: index = 18; break;
            case 19: index = 19; break;
            case 20: index = 20; break;
            case 21: index = 21; break;
            case 22: index = 22; break;
            case 23: index = 23; break;
            case 24: index = 24; break;
            case 25: index = 25; break;
            case 26: index = 26; break;
            case 27: index = 27; break;
            case 28: index = 28; break;
            case 29: index = 29; break;
            case 30: index = 30; break;
            case 31: index = 31; break;
            case 32: index = 32; break;
            case 33: index = 33; break;
            case 34: index = 34; break;
            case 35: index = 35; break;
            case 36: index = 36; break;
            case 37: index = 37; break;
            case 38: index = 38; break;
            case 39: index = 39; break;
            case 40: index = 40; break;
            case 41: index = 41; break;
            case 42: index = 42; break;
            case 43: index = 43; break;
            case 44: index = 44; break;

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

        if (currentMode == 2)
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
        if (haveSelectFrame && currentMode == 2)
        {
            ImGui::Checkbox("Edit frame only", &editFrame);
        }

        if (ImGui::Button("Export"))
        {
            if (currentMode == 2)
            {
                if (haveSelectFrame && editFrame)
                {
                    exportFrame();
                }
                else
                {
                    exportFromEditor();
                }
            }
            else if (currentMode == 1)
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
            animator->addClip(std::string(animationName), ("../res/animation/" + std::string(inFileName)).c_str());
        }

    }

    void GUI::transformPanel(Node* node)
    {
        glm::vec3 trans = node->getTranslateOffset();
        glm::vec3 angle = node->getEulerRotateAngle();

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

    void GUI::effectPannel()
    {
        ImGui::SeparatorText("Usage");
        ImGui::Text("Select an effect to apply.");
        ImGui::SeparatorText("Effects");

        ImGui::PushItemWidth(-1);  // Make ListBox fill available width

        EffectManager& efm = EffectManager::getInstance();

        std::vector<const char*> effectNames;
        effectNames.push_back("None");
        for (const auto& str : efm.getNameList()) {
            effectNames.push_back(str.c_str());
        }

        if (ImGui::ListBox("##EffectList",
            &effectSelected,
            effectNames.data(),
            (int)(effectNames.size()),
            (int)(effectNames.size())))
        {
            if (effectSelected != 0)
            {
                effectName = effectNames[effectSelected];
                haveEffect = true;
            }
            else
            {
                haveEffect = false;
            }
        }

        if (haveEffect)
        {
            effectParamPanel();
        }

        ImGui::PopItemWidth();
    }

    void GUI::effectParamPanel()
    {
        ImGui::SeparatorText("Usage");
        ImGui::Text("Modify the direction and position of the effect.");

        EffectManager& efm = EffectManager::getInstance();

        bool isParticle = efm.getEffect(effectName).isParticle;
        if (effectName == "Fire")
        {
            fireParamPanel(*efm.getEffect(effectName).ps);
        }
        else if(effectName == "Lightning")
        {
            lightningParamPanel(*efm.getEffect(effectName).ln);
        }
        else if (effectName == "Firework")
        {
            fireworkParamPannel(*efm.getEffect(effectName).ps);
        }
    }

    void GUI::fireParamPanel(ParticleSystem& ps)
    {
        glm::vec3 trans = ps.getEmitterPos(0);

        ImGui::SeparatorText("Translation");
        if (ImGui::DragFloat("x (Translate) ", &trans[0], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            ps.setEmitterPos(0, trans);
        }
        if (ImGui::DragFloat("y (Translate) ", &trans[1], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            ps.setEmitterPos(0, trans);
        }
        if (ImGui::DragFloat("z (Translate) ", &trans[2], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            ps.setEmitterPos(0, trans);
        }
    }

    void GUI::lightningParamPanel(Lightning& ln)
    {
        if (ImGui::Button("Add Endpoints"))
        {
            ln.addEndPoints(glm::vec3(0, 0, 10));
        }


        ImGui::SeparatorText("Translation of Center");
        glm::vec3 center = ln.getCenter();
        if (ImGui::DragFloat("Center x (Translate)", &center[0], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            ln.setCenter(center);
        }
        if (ImGui::DragFloat("Center y (Translate)", &center[1], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            ln.setCenter(center);
        }
        if (ImGui::DragFloat("Center z (Translate)", &center[2], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            ln.setCenter(center);
        }

        std::vector<glm::vec3> endpoints = ln.getEndPoints();

        for (int i = 0; i < endpoints.size(); i++)
        {
            ImGui::SeparatorText(("Translation of Endpoints " + std::to_string(i)).data());

            std::string name = "Endpoint " + std::to_string(i);
            if (ImGui::DragFloat((name + " x (Translate)").data(), &endpoints[i][0], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                ln.updateEndPoints(i, endpoints[i]);
            }
            if (ImGui::DragFloat((name + " y (Translate)").data(), &endpoints[i][1], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                ln.updateEndPoints(i, endpoints[i]);
            }
            if (ImGui::DragFloat((name + " z (Translate)").data(), &endpoints[i][2], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                ln.updateEndPoints(i, endpoints[i]);
            }
        }
    }

    void GUI::fireworkParamPannel(ParticleSystem& ps)
    {
        if (ImGui::Button("Add Emitters"))
        {
            unsigned int count = ps.getEmitterCount();
            ps.addEmitter(1000);
            ps.setEmitterVelocity(count - 1, 5.0f);
            ps.setEmitterAcceleration(count - 1, 5.0f);
            ps.setEmitterParticleSize(count - 1, 4.0f);
        }

        unsigned int emittersCount = ps.getEmitterCount();
        std::vector<glm::vec3> emittersPos;
        bool* emittersEnableState;
        
        emittersEnableState = new bool[emittersCount];

        for (int i = 0; i < emittersCount; i++) 
        {
            emittersPos.emplace_back(ps.getEmitterPos(i));
            emittersEnableState[i] = ps.getEmitterEnableState(i);
        }

        for (int i = 0; i < emittersCount; i++)
        {
            ImGui::SeparatorText(("Translation of Emitters " + std::to_string(i)).data());

            std::string name = "Emitter " + std::to_string(i);
            if (ImGui::DragFloat((name + " x (Translate)").data(), &emittersPos[i][0], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                ps.setEmitterPos(i, emittersPos[i]);
            }
            if (ImGui::DragFloat((name + " y (Translate)").data(), &emittersPos[i][1], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                ps.setEmitterPos(i, emittersPos[i]);
            }
            if (ImGui::DragFloat((name + " z (Translate)").data(), &emittersPos[i][2], 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                ps.setEmitterPos(i, emittersPos[i]);
            }

            if (ImGui::Checkbox(("Enable " + name).data(), &emittersEnableState[i]))
            {
                if (emittersEnableState[i])
                {
                    ps.enableEmitter(i);
                }
                else
                {
                    ps.disableEmitter(i);
                }
            }
                    
        }
    }

    void GUI::_render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void GUI::renderIcon(glm::vec3 pos)
    {
        gs.use();

        vao.bind();
        vbo.bind();
        ebo.bind();

        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), pos);

        GLCall(GLuint ModelID = glGetUniformLocation(gs.getId(), "u_Model"));
        if (ModelID != -1)
        {
            GLCall(glUniformMatrix4fv(ModelID, 1, GL_FALSE, glm::value_ptr(modelMatrix)));
        }

        glEnable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void GUI::exportFrame()
    {
        std::string filePath = std::string("../res/animation/") + std::string(outFileName);
        
        // First read the file
        std::ifstream inFile(filePath);
        if (!inFile.is_open()) {
            return; // Handle error appropriately
        }

        std::vector<std::string> fileLines;
        std::string line;
        while (std::getline(inFile, line)) 
        {
            fileLines.emplace_back(line);
        }
        inFile.close();

        unsigned int startLine = 30 * frameSelected;
        unsigned int endLine = startLine + 30; // 15 * 2

        if (startLine >= fileLines.size()) 
        {
            return; 
        }

        fileLines.erase(fileLines.begin() + startLine, fileLines.begin() + endLine);

        std::vector<std::string> modifiedFrame;
        for (int i = 0; i < 15; i++)
        {
            Node* node = &(robot->getPart(i));
            glm::vec3 trans = node->getTranslateOffset();
            glm::vec3 rotate = node->getEulerRotateAngle();

            modifiedFrame.emplace_back(
                std::to_string(trans[0]) + " " +
                std::to_string(trans[1]) + " " +
                std::to_string(trans[2])
            );
            modifiedFrame.emplace_back(
                std::to_string(rotate[0]) + " " +
                std::to_string(rotate[1]) + " " +
                std::to_string(rotate[2]) 
            );
        }

        fileLines.insert(fileLines.begin() + startLine, 
                        modifiedFrame.begin(), 
                        modifiedFrame.end());

        std::ofstream outFile(filePath);
        if (!outFile.is_open()) 
        {
            return; 
        }

        for (const auto& _line : fileLines) 
        {
            outFile << _line << '\n';
        }
        outFile.close();
    }

    void GUI::exportFromEditor()
    {
        std::ofstream outFile((std::string("../res/animation/") + std::string(outFileName)).c_str(), std::ios_base::app); // append
        
        outFile << "{\n" << "    \"motion\" : [\n";
        
        for (int i = 0; i < 45; i++) // 45 nodes
        {
            Node* node = &(robot->getPart(i));
            glm::vec3 trans = node->getTranslateOffset();
            glm::vec3 rotate = node->getEulerRotateAngle();
            outFile << "        [" << trans[0] << ", " << trans[1] << ", " << trans[2] << "],\n" <<
                "        [" << rotate[0] << ", " << rotate[1] << ", " << rotate[2] << "],\n";
        }
        
        outFile << "    " << "],\n";
        outFile << "    \"effect\" : {\n";
        if (haveEffect == false && previousHaveEffect == false)
        {
            outFile << "        \"name\" : \"None\"\n";
            outFile << "    }\n";
        }
        else
        {
            
            outFile << "        \"name\" : \"" << effectName << "\",\n";
            if (haveEffect != previousHaveEffect)
            {
                if (haveEffect)
                {
                    outFile << "        \"isStart\" : \"true\",\n";
                    outFile << "        \"isFinished\" : \"false\",\n";
                }
                else
                {
                    outFile << "        \"isStart\" : \"false\",\n";
                    outFile << "        \"isFinished\" : \"true\",\n";
                }
                previousHaveEffect = haveEffect;
            }
            else if (haveEffect)
            {
                outFile << "        \"isStart\" : \"false\",\n";
                outFile << "        \"isFalse\" : \"false\",\n";
            }

            EffectManager& efm = EffectManager::getInstance();
            if (efm.getEffect(effectName).isParticle)
            {
                outFile << "        \"param\" : {\n";
                for (unsigned int i = 0; i < efm.getEffect(effectName).ps->getEmitterCount(); i++)
                {
                    effectPos = efm.getEffect(effectName).ps->getEmitterPos(i);
                    effectDir = efm.getEffect(effectName).ps->getEmitterDir(i);
                    outFile << "            \"pos\" : [" << effectPos[0] << ", " << effectPos[1] << ", " << effectPos[2] << "],\n";
                    outFile << "            \"dir\" : [" << effectDir[0] << ", " << effectDir[1] << ", " << effectDir[2] << "],\n";
                    outFile << "            \"enable\" : " << efm.getEffect(effectName).ps->getEmitterEnableState(i) << ",\n";
                }
                outFile << "        }\n";
                outFile << "    }\n";
            }
            else
            {
                effectCenter = efm.getEffect(effectName).ln->getCenter();
                effectEndpoints = efm.getEffect(effectName).ln->getEndPoints();

                outFile << "        \"param\" : {\n";
                outFile << "            \"center\" : [" << effectCenter[0] << ", " << effectCenter[1] << ", " << effectCenter[2] << "],\n";
                outFile << "            \"endpoints\" : [\n";
                for (int i = 0; i < effectEndpoints.size(); i++)
                {
                    outFile << "                                [" << effectEndpoints[i][0] << ", " << effectEndpoints[i][1] << ", " << effectEndpoints[i][2] << "]";
                    if (i != effectEndpoints.size() - 1)
                    {
                        outFile << ",\n";
                    }
                    else
                    {
                        outFile << "\n";
                    }
                }
                outFile << "            ]\n";
                outFile << "        }\n";
                outFile << "    }\n";
            }

        }
        outFile << "}\n";
    }

    void GUI::exportFromAnimator()
    {
        std::ofstream outFile((std::string("../res/animation/") + std::string(outFileName)).c_str()); // overwrite
        const std::vector<Track>& tracks = animator->getCurrentClipTrack();

        outFile << "{\n" << "    \"speed\" : " << animator->getCurrentClipSpeed() << "\n}\n";

        for (int frame = 0; frame < tracks[0].keyFrames.size(); frame++)
        {
            outFile << "{\n" << "    \"motion\" : [\n";
            for (int i = 0; i < 45; i++)
            {
                Node* node = &(robot->getPart(i));
                glm::vec3 trans = tracks[i].keyFrames[frame].transOffset;
                glm::vec3 rotate = glm::degrees(glm::eulerAngles(tracks[i].keyFrames[frame].rotatOffset));
                outFile << "        [" << trans[0] << ", " << trans[1] << ", " << trans[2] << "],\n" <<
                    "        [" << rotate[0] << ", " << rotate[1] << ", " << rotate[2] << "],\n";
            }
            outFile << "    " << "],\n";
            outFile << "    \"effect\" : {\n";

            if (tracks[0].keyFrames[frame].effect.name == "None")
            {
                outFile << "        \"name\" : \"None\"\n";
                outFile << "    }\n";
            }
            else
            {
                outFile << "        \"name\" : \"" << tracks[0].keyFrames[frame].effect.name << "\",\n";
                if (tracks[0].keyFrames[frame].effect.isStart)
                {
                    outFile << "        \"isStart\" : \"true\",\n";
                }
                else
                {
                    outFile << "        \"isStart\" : \"false\",\n";
                }

                if (tracks[0].keyFrames[frame].effect.isFinished)
                {
                    outFile << "        \"isFinished\" : \"true\",\n";
                }
                else
                {
                    outFile << "        \"isFinished\" : \"false\",\n";
                }

                if (tracks[0].keyFrames[frame].effect.isParticleEffect)
                {
                    std::vector<glm::vec3> pos = tracks[0].keyFrames[frame].effect.param.pos;
                    std::vector<glm::vec3> dir = tracks[0].keyFrames[frame].effect.param.dir;
                    std::vector<bool> enable = tracks[0].keyFrames[frame].effect.param.enable;

                    outFile << "        \"param\" : {\n";

                    for (unsigned int i = 0; i < tracks[0].keyFrames[frame].effect.param.pos.size(); i++)
                    {
                        outFile << "            \"pos\" : [" << pos[i][0] << ", " << pos[i][1] << ", " << pos[i][2] << "],\n";
                        outFile << "            \"dir\" : [" << dir[i][0] << ", " << dir[i][1] << ", " << dir[i][2] << "],\n";
                        if (enable.size() != 0)
                        {
                            outFile << "            \"enable\" : " << enable[i] << ",\n";
                        }
                        else
                        {
                            outFile << "            \"enable\" : true,\n";
                        }
                    }
                    outFile << "        }\n";
                    outFile << "    }\n";
                }
                else
                {
                    glm::vec3 center = tracks[0].keyFrames[frame].effect.param.center;
                    std::vector<glm::vec3> enpPoints = tracks[0].keyFrames[frame].effect.param.endpoints;

                    outFile << "        \"param\" : {\n";
                    outFile << "            \"center\" : [" << center[0] << ", " << center[1] << ", " << center[2] << "],\n";
                    outFile << "            \"endpoints\" : [\n";
                    for (int i = 0; i < enpPoints.size(); i++)
                    {
                        outFile << "                                [" << enpPoints[i][0] << ", " << enpPoints[i][1] << ", " << enpPoints[i][2] << "]";
                        if (i != enpPoints.size() - 1)
                        {
                            outFile << ",\n";
                        }
                        else
                        {
                            outFile << "\n";
                        }
                    }
                    outFile << "            ]\n";
                    outFile << "        }\n";
                    outFile << "    }\n";
                }
            }
        }

        outFile << "}\n";
    }

    void GUI::renderEffectIcon(
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        unsigned int emitter /* = -1 */)
    {
        if (haveEffect)
        {
            gs.use();
            GLCall(GLuint ViewID = glGetUniformLocation(gs.getId(), "u_View"));
            if (ViewID != -1)
            {
                GLCall(glUniformMatrix4fv(ViewID, 1, GL_FALSE, glm::value_ptr(viewMatrix)));
            }
            GLCall(GLuint ProjID = glGetUniformLocation(gs.getId(), "u_Projection"));
            if (ProjID != -1)
            {
                GLCall(glUniformMatrix4fv(ProjID, 1, GL_FALSE, glm::value_ptr(projectionMatrix)));
            }

            EffectManager& efm = EffectManager::getInstance();
            if (efm.getEffect(effectName).isParticle)
            {
                renderIcon(efm.getEffect(effectName).ps->getEmitterPos(emitter));
                for (int i = 0; i < efm.getEffect(effectName).ps->getEmitterCount(); i++)
                {
                    renderIcon(efm.getEffect(effectName).ps->getEmitterPos(i));
                }
            }
            else
            {
                glm::vec3 emitterPos = efm.getEffect(effectName).ln->getCenter();
                renderIcon(emitterPos);
                std::vector<glm::vec3> endPoints = efm.getEffect(effectName).ln->getEndPoints();
                for (int i = 0; i < endPoints.size(); i++)
                {
                    renderIcon(endPoints[i]);
                }
            }
            
            gs.unUse();
        }
    }
}
