#include "MainScene.h"
#include "../Utilty/Error.h"

#include <iostream>

#define TOP_BODY 0
#define LEFT_UPPER_ARM 1
#define LEFT_LOWER_ARM 2
#define LEFT_PAW 3
#define HEAD 4
#define RIGHT_UPPER_ARM 5
#define RIGHT_LOWER_ARM 6
#define RIGHT_PAW 7
#define BOTTOM_BODY 8
#define LEFT_THIGH 9
#define LEFT_CALF 10
#define LEFT_FOOT 11
#define RIGHT_THIGH 12
#define RIGHT_CALF 13
#define RIGHT_FOOT 14

#define LEFT_THUMB_1 15
#define LEFT_THUMB_2 16
#define LEFT_THUMB_3 17
#define LEFT_INDEX_1 18
#define LEFT_INDEX_2 19
#define LEFT_INDEX_3 20
#define LEFT_MIDDLE_1 21
#define LEFT_MIDDLE_2 22
#define LEFT_MIDDLE_3 23
#define LEFT_RING_1 24
#define LEFT_RING_2 25
#define LEFT_RING_3 26
#define LEFT_LITTLE_1 27
#define LEFT_LITTLE_2 28
#define LEFT_LITTLE_3 29

#define RIGHT_THUMB_1 30
#define RIGHT_THUMB_2 31
#define RIGHT_THUMB_3 32
#define RIGHT_INDEX_1 33
#define RIGHT_INDEX_2 34
#define RIGHT_INDEX_3 35
#define RIGHT_MIDDLE_1 36
#define RIGHT_MIDDLE_2 37
#define RIGHT_MIDDLE_3 38
#define RIGHT_RING_1 39
#define RIGHT_RING_2 40
#define RIGHT_RING_3 41
#define RIGHT_LITTLE_1 42
#define RIGHT_LITTLE_2 43
#define RIGHT_LITTLE_3 44

namespace CG
{
	MainScene::MainScene(Camera& _camera, Light& _light, Animator& _animator, GraphicShader& _program)
	{
		camera = &_camera;
		light = &_light;
		animator = &_animator;
		program = &_program;
	}

	MainScene::~MainScene()
	{}

	bool MainScene::Initialize(int display_w, int display_h)
	{
		/* debug use
		std::vector<float> v = {
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,

			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f
		};
		
		vao.bind(); // Assuming this VAO is dedicated to the test quad
		vbo.initialize(v); // Assuming this VBO is dedicated to the test quad
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);                         // aPos (vec3)
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3)); // aTexCoord (vec2)
		
		// Load the texture you want to test (e.g., fire.png)
		// texture.LoadTexture("../res/models2/Robot_Base_color 2.png"); 
		texture.LoadTexture("../res/pointSprites/fire.png"); // Let's test fire.png

		// Load shaders for the test quad
		ShaderInfo testShaders[] = {
			{ GL_VERTEX_SHADER, "../res/shaders/TestTexture.vp" },
			{ GL_FRAGMENT_SHADER, "../res/shaders/TestTexture.fp" },
			{ GL_NONE, NULL } };
		testQuadShader.load(testShaders);
		
		// Unbind VAO to avoid accidental modification by loadScene() if it uses VAO 0
		glBindVertexArray(0); 
		*/
		return loadScene(display_w, display_h); // Be cautious if loadScene reuses vao, vbo, or texture members
	}

	void MainScene::Render(double timeNow, double timeDelta, int display_w, int display_h)
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		glm::mat4 lightSpaceMatrix = shadowSystem.set(light->getPos());
		sphare.render(camera, &shadowSystem.getShaderProgram());
		//box.render(camera, &shadowShader, GL_QUADS);
		ground.render(camera, &shadowSystem.getShaderProgram(), GL_QUADS);
		robot.render(shadowSystem.getShaderProgram().getId(), camera);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		matVPUbo.fillInData(0, sizeof(glm::mat4), camera->GetViewMatrix());
		matVPUbo.fillInData(sizeof(glm::mat4), sizeof(glm::mat4), camera->GetProjectionMatrix());

		screenRenderer.set();
		skyBox.render(camera, GL_QUADS);

		ground.getShaderProgram().use();
		light->bind(ground.getShaderProgram().getId());
		glUniformMatrix4fv(glGetUniformLocation(ground.getShaderProgram().getId(), "u_LightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowSystem.getShadowMap());
		glUniform1i(glGetUniformLocation(ground.getShaderProgram().getId(), "u_ShadowMap"), 0);
		ground.render(camera, nullptr, GL_QUADS);

		sphare.getShaderProgram().use();
		light->bind(sphare.getShaderProgram().getId());
		glUniformMatrix4fv(glGetUniformLocation(sphare.getShaderProgram().getId(), "u_LightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glUniform3fv(glGetUniformLocation(sphare.getShaderProgram().getId(), "u_CameraPos"), 1, glm::value_ptr(camera->getPos()));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowSystem.getShadowMap());
		glUniform1i(glGetUniformLocation(sphare.getShaderProgram().getId(), "u_ShadowMap"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox.getTexture().getId());
		glUniform1i(glGetUniformLocation(sphare.getShaderProgram().getId(), "u_Skybox"), 1);
		sphare.render(camera);

		program->use();
		glUniformMatrix4fv(glGetUniformLocation(program->getId(), "u_LightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowSystem.getShadowMap());
		glUniform1i(glGetUniformLocation(program->getId(), "u_ShadowMap"), 1);
		robot.render(program->getId(), camera);
				
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		screenRenderer.render();

		GLCall(glFlush());
	}

	void MainScene::Resize(int display_w, int display_h)
	{
		screenRenderer.resize(display_w, display_h);
	}
	
	bool MainScene::loadScene(int display_w, int display_h)
	{
		loadModel();
		loadAnimation();

		//UBO
		matVPUbo.initialize(sizeof(glm::mat4) * 2);
		matVPUbo.associateWithShaderBlock(program->getId(), "u_MatVP", 0);
		matVPUbo.associateWithShaderBlock(ground.getShaderProgram().getId(), "u_MatVP", 0);
		matVPUbo.associateWithShaderBlock(sphare.getShaderProgram().getId(), "u_MatVP", 0);
		matVPUbo.associateWithShaderBlock(skyBox.getObject().getShaderProgram().getId(), "u_MatVP", 0);

		screenRenderer.initialize(display_w, display_h);
		screenRenderer.setShader("../res/shaders/Pixelate_Vertex.vp", "../res/shaders/Pixelate_Fragment.fp");

		return true;
	}

	void MainScene::loadModel()
	{
		std::vector<std::string> mtlPaths({
			"../res/models2/top_body.mtl",

			"../res/models2/left_upper_arm.mtl",
			"../res/models2/left_lower_arm.mtl",
			"../res/models2/left_paw.mtl",

			"../res/models2/head.mtl",

			"../res/models2/right_upper_arm.mtl",
			"../res/models2/right_lower_arm.mtl",
			"../res/models2/right_paw.mtl",

			"../res/models2/bottom_body.mtl",

			"../res/models2/left_thigh.mtl",
			"../res/models2/left_calf.mtl",
			"../res/models2/left_foot.mtl",

			"../res/models2/right_thigh.mtl",
			"../res/models2/right_calf.mtl",
			"../res/models2/right_foot.mtl",

			"../res/models2/left_thumb_1.mtl",
			"../res/models2/left_thumb_2.mtl",
			"../res/models2/left_thumb_3.mtl",
			"../res/models2/left_index_1.mtl",
			"../res/models2/left_index_2.mtl",
			"../res/models2/left_index_3.mtl",
			"../res/models2/left_middle_1.mtl",
			"../res/models2/left_middle_2.mtl",
			"../res/models2/left_middle_3.mtl",
			"../res/models2/left_ring_1.mtl",
			"../res/models2/left_ring_2.mtl",
			"../res/models2/left_ring_3.mtl",
			"../res/models2/left_little_1.mtl",
			"../res/models2/left_little_2.mtl",
			"../res/models2/left_little_3.mtl",


			"../res/models2/right_thumb_1.mtl",
			"../res/models2/right_thumb_2.mtl",
			"../res/models2/right_thumb_3.mtl",
			"../res/models2/right_index_1.mtl",
			"../res/models2/right_index_2.mtl",
			"../res/models2/right_index_3.mtl",
			"../res/models2/right_middle_1.mtl",
			"../res/models2/right_middle_2.mtl",
			"../res/models2/right_middle_3.mtl",
			"../res/models2/right_ring_1.mtl",
			"../res/models2/right_ring_2.mtl",
			"../res/models2/right_ring_3.mtl",
			"../res/models2/right_little_1.mtl",
			"../res/models2/right_little_2.mtl",
			"../res/models2/right_little_3.mtl",
			});

		std::vector<std::string> objPaths({
			"../res/models2/top_body.obj", // 0

			"../res/models2/left_upper_arm.obj", // 1
			"../res/models2/left_lower_arm.obj", // 2
			"../res/models2/left_paw.obj",

			"../res/models2/head.obj", // 4

			"../res/models2/right_upper_arm.obj", // 5
			"../res/models2/right_lower_arm.obj", // 6
			"../res/models2/right_paw.obj",

			"../res/models2/bottom_body.obj", // 8

			"../res/models2/left_thigh.obj", // 9
			"../res/models2/left_calf.obj", // 10
			"../res/models2/left_foot.obj", // 11

			"../res/models2/right_thigh.obj", //12
			"../res/models2/right_calf.obj", // 13
			"../res/models2/right_foot.obj", // 14

			"../res/models2/left_thumb_1.obj", 
			"../res/models2/left_thumb_2.obj", 
			"../res/models2/left_thumb_3.obj", 
			"../res/models2/left_index_1.obj",
			"../res/models2/left_index_2.obj",
			"../res/models2/left_index_3.obj",
			"../res/models2/left_middle_1.obj",
			"../res/models2/left_middle_2.obj",
			"../res/models2/left_middle_3.obj",
			"../res/models2/left_ring_1.obj",
			"../res/models2/left_ring_2.obj",
			"../res/models2/left_ring_3.obj",
			"../res/models2/left_little_1.obj",
			"../res/models2/left_little_2.obj",
			"../res/models2/left_little_3.obj",


			"../res/models2/right_thumb_1.obj",
			"../res/models2/right_thumb_2.obj",
			"../res/models2/right_thumb_3.obj",
			"../res/models2/right_index_1.obj",
			"../res/models2/right_index_2.obj",
			"../res/models2/right_index_3.obj",
			"../res/models2/right_middle_1.obj",
			"../res/models2/right_middle_2.obj",
			"../res/models2/right_middle_3.obj",
			"../res/models2/right_ring_1.obj",
			"../res/models2/right_ring_2.obj",
			"../res/models2/right_ring_3.obj",
			"../res/models2/right_little_1.obj",
			"../res/models2/right_little_2.obj",
			"../res/models2/right_little_3.obj",

			});

		std::vector<std::vector<unsigned int>> relationship({
			{TOP_BODY, LEFT_UPPER_ARM, HEAD, RIGHT_UPPER_ARM, BOTTOM_BODY},
			{LEFT_UPPER_ARM, LEFT_LOWER_ARM},
			{LEFT_LOWER_ARM, LEFT_PAW},
			{LEFT_PAW, LEFT_THUMB_1, LEFT_INDEX_1, LEFT_MIDDLE_1, LEFT_RING_1, LEFT_LITTLE_1},
			{LEFT_THUMB_1, LEFT_THUMB_2},
			{LEFT_THUMB_2, LEFT_THUMB_3},
			{LEFT_INDEX_1, LEFT_INDEX_2},
			{LEFT_INDEX_2, LEFT_INDEX_3},
			{LEFT_MIDDLE_1, LEFT_MIDDLE_2},
			{LEFT_MIDDLE_2, LEFT_MIDDLE_3},
			{LEFT_RING_1, LEFT_RING_2},
			{LEFT_RING_2, LEFT_RING_3},
			{LEFT_LITTLE_1, LEFT_LITTLE_2},
			{LEFT_LITTLE_2, LEFT_LITTLE_3},
			{RIGHT_UPPER_ARM, RIGHT_LOWER_ARM},
			{RIGHT_LOWER_ARM, RIGHT_PAW},
			{RIGHT_PAW, RIGHT_THUMB_1, RIGHT_INDEX_1, RIGHT_MIDDLE_1, RIGHT_RING_1, RIGHT_LITTLE_1},
			{RIGHT_THUMB_1, RIGHT_THUMB_2},
			{RIGHT_THUMB_2, RIGHT_THUMB_3},
			{RIGHT_INDEX_1, RIGHT_INDEX_2},
			{RIGHT_INDEX_2, RIGHT_INDEX_3},
			{RIGHT_MIDDLE_1, RIGHT_MIDDLE_2},
			{RIGHT_MIDDLE_2, RIGHT_MIDDLE_3},
			{RIGHT_RING_1, RIGHT_RING_2},
			{RIGHT_RING_2, RIGHT_RING_3},
			{RIGHT_LITTLE_1, RIGHT_LITTLE_2},
			{RIGHT_LITTLE_2, RIGHT_LITTLE_3},
			{BOTTOM_BODY, LEFT_THIGH, RIGHT_THIGH},
			{LEFT_THIGH, LEFT_CALF},
			{LEFT_CALF, LEFT_FOOT},
			{RIGHT_THIGH, RIGHT_CALF},
			{RIGHT_CALF, RIGHT_FOOT}
			});

		robot.initialize(mtlPaths, objPaths);
		robot.setPartsRelationship(relationship);
		robot.gatherPartsData();

		float size = 100;
		float thick = size / 100.0;
		std::vector<glm::vec3> points({
			{-size,  thick,  size}, {-size, -thick,  size}, { size, -thick,  size}, { size,  thick,  size},
			{-size,  thick, -size}, {-size, -thick, -size}, {-size, -thick,  size}, {-size,  thick,  size},
			{ size,  thick,  size}, { size, -thick,  size}, { size, -thick, -size}, { size,  thick, -size},
			{ size,  thick, -size}, { size, -thick, -size}, {-size, -thick, -size}, {-size,  thick, -size},
			{-size,  thick, -size}, {-size,  thick,  size}, { size,  thick,  size}, { size,  thick, -size},
			{ size, -thick,  size}, {-size, -thick,  size}, {-size, -thick, -size}, { size, -thick, -size},
			});

		std::vector<glm::vec3> colors(points.size(), glm::vec3(0.3, 0.3, 0.3));

		ground.setShader("../res/shaders/Obj_Vertex.vp", "../res/shaders/Obj_Fragment.fp");
		ground.initialize(points, colors);
		ground.computeNormal(GL_QUADS);
		ground.gatherData();
		ground.setTranslate({0, -42, 0});
		/*std::vector<glm::vec3> instancingOffests = ground.getInstancingOffests();
		instancingOffests.push_back({100, 0, 0});
		ground.setInstancingOffests(instancingOffests);*/

		shadowSystem.setShader("../res/shaders/Shadow_Vertex.vp", "../res/shaders/Shadow_Fragment.fp");

		skyBox.getObject().setShader("../res/shaders/SkyBox_Vertex.vp", "../res/shaders/SkyBox_Fragment.fp");
		std::string fileType = ".jpg";
		std::string number = "3";
		skyBox.loadFaces({ // need follow the order
			"../res/skyBox/skyBox" + number + "_right" + fileType,
			"../res/skyBox/skyBox" + number + "_left"  + fileType,
			"../res/skyBox/skyBox" + number + "_up"	   + fileType,
			"../res/skyBox/skyBox" + number + "_down"  + fileType,
			"../res/skyBox/skyBox" + number + "_front" + fileType,
			"../res/skyBox/skyBox" + number + "_back"  + fileType
			});
		skyBox.updateDate();

		sphare.setShader("../res/shaders/Obj_EvnMap_Vertex.vp", "../res/shaders/Obj_EvnMap_Fragment.fp");
		sphare.generateSphere(5, 36, 36, { 0.0, 0.0, 0.0 });
		sphare.gatherData();
		sphare.setTranslate({ 0, 20, 0 });

		animator->target(&robot);
	}

	void MainScene::loadAnimation()
	{
		std::vector<std::vector<KeyFrame>> keyFrameBuffer;

		for (int i = 0; i < 45; i++)
		{
			keyFrameBuffer.emplace_back();
		}

		keyFrameBuffer[TOP_BODY].emplace_back(KeyFrame{ glm::vec3(0.000000, 0.000000, 0.000000) ,			glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_UPPER_ARM].emplace_back(KeyFrame{ glm::vec3(7.650000, 0.000000, 0.000000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_LOWER_ARM].emplace_back(KeyFrame{ glm::vec3(2.900000, -6.400000, 0.000000) ,	glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_PAW].emplace_back(KeyFrame{ glm::vec3(3.7, -8.6, 4),							glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[HEAD].emplace_back(KeyFrame{ glm::vec3(0.000000, 3.100000, 0.000000) ,				glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_UPPER_ARM].emplace_back(KeyFrame{ glm::vec3(-7.650000, 0.050000, 0.000000) ,	glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_LOWER_ARM].emplace_back(KeyFrame{ glm::vec3(-2.950000, -6.400000, 0.000000) ,	glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_PAW].emplace_back(KeyFrame{ glm::vec3(-3.55, -8.55, 4.05),						glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[BOTTOM_BODY].emplace_back(KeyFrame{ glm::vec3(0.000000, -5.050000, -1.450000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_THIGH].emplace_back(KeyFrame{ glm::vec3(3.250000, -6.650000, 2.200000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_CALF].emplace_back(KeyFrame{ glm::vec3(3.000000, -10.000000, 1.150000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_FOOT].emplace_back(KeyFrame{ glm::vec3(1.350000, -16.500000, -1.550000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_THIGH].emplace_back(KeyFrame{ glm::vec3(-3.250000, -6.650000, 2.200000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_CALF].emplace_back(KeyFrame{ glm::vec3(-3.350000, -10.000000, 1.150000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_FOOT].emplace_back(KeyFrame{ glm::vec3(-1.350000, -16.500000, -1.550000) ,		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });

		keyFrameBuffer[LEFT_THUMB_1].emplace_back(KeyFrame{ glm::vec3(0.25, -0.9, 1.2),			glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_THUMB_2].emplace_back(KeyFrame{ glm::vec3(0, -0.4, 1.05),			glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_THUMB_3].emplace_back(KeyFrame{ glm::vec3(0.05, -0.45, 0.85),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_INDEX_1].emplace_back(KeyFrame{ glm::vec3(0.85, -2.65, 1.85),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_INDEX_2].emplace_back(KeyFrame{ glm::vec3(0.25, -1.035, 0.510),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_INDEX_3].emplace_back(KeyFrame{ glm::vec3(0.05, -0.95, 0.35),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_MIDDLE_1].emplace_back(KeyFrame{ glm::vec3(0.9, -2.9, 1.3),			glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_MIDDLE_2].emplace_back(KeyFrame{ glm::vec3(0.25, -1.1, 0.4),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_MIDDLE_3].emplace_back(KeyFrame{ glm::vec3(0.05, -1.0, 0.35),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_RING_1].emplace_back(KeyFrame{ glm::vec3(0.9, -3.05, 0.75),			glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_RING_2].emplace_back(KeyFrame{ glm::vec3(0.25, -1.150, 0.35),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_RING_3].emplace_back(KeyFrame{ glm::vec3(0.05, -0.950, 0.3),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_LITTLE_1].emplace_back(KeyFrame{ glm::vec3(0.9, -3.150, 0.250),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_LITTLE_2].emplace_back(KeyFrame{ glm::vec3(0.2, -0.98, 0.23),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[LEFT_LITTLE_3].emplace_back(KeyFrame{ glm::vec3(0.05, -0.850, 0.15),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });


		keyFrameBuffer[RIGHT_THUMB_1].emplace_back(KeyFrame{ glm::vec3(-0.325, -0.9, 1.1),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_THUMB_2].emplace_back(KeyFrame{ glm::vec3(-0.025, -0.4, 1.05),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_THUMB_3].emplace_back(KeyFrame{ glm::vec3(0, -0.5, 0.85),			glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_INDEX_1].emplace_back(KeyFrame{ glm::vec3(-0.9, -2.650, 1.8),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_INDEX_2].emplace_back(KeyFrame{ glm::vec3(-0.250, -1.075, 0.5),	glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_INDEX_3].emplace_back(KeyFrame{ glm::vec3(-0.05, -0.95, 0.4),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_MIDDLE_1].emplace_back(KeyFrame{ glm::vec3(-0.9, -2.850, 1.250),	glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_MIDDLE_2].emplace_back(KeyFrame{ glm::vec3(-0.3, -1.15, 0.425),    glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_MIDDLE_3].emplace_back(KeyFrame{ glm::vec3(-0.1, -1.0, 0.4),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_RING_1].emplace_back(KeyFrame{ glm::vec3(-0.9, -3.050, 0.7),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_RING_2].emplace_back(KeyFrame{ glm::vec3(-0.3, -1.1, 0.33),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_RING_3].emplace_back(KeyFrame{ glm::vec3(-0.05, -0.95, 0.3),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_LITTLE_1].emplace_back(KeyFrame{ glm::vec3(-1, -3.1, 0.25),		glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_LITTLE_2].emplace_back(KeyFrame{ glm::vec3(-0.250, -0.95, 0.215),  glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
		keyFrameBuffer[RIGHT_LITTLE_3].emplace_back(KeyFrame{ glm::vec3(-0.05, -0.85, 0.2),     glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });


		animator->addClip("Stand", keyFrameBuffer);
		animator->setCurrentClip("Stand");
		animator->addClip("Hao Fire", "../res/animation/HaoFire.anim");
		animator->addClip("Lightning", "../res/animation/Lightning.anim");
		animator->addClip("Firework", "../res/animation/Firework.anim");


		// Add animation here
		/*
		animator->addClip("Walk", "../res/animation/walk.anim");	
		animator->addClip("Sit-Up", "../res/animation/sit-up.anim");
		animator->addClip("Push-Up", "../res/animation/push-up.anim");
		animator->addClip("Hopak", "../res/animation/hopak.anim");
		animator->addClip("APT", "../res/animation/apt.anim");
		animator->addClip("Shadow_Clone_Jutsu", "../res/animation/shadow_clone_jutsu.anim",
			{
				AnimationEvent{1, [=]() { robot.modifyInstance(1); }},
				AnimationEvent{8, [=]() { robot.modifyInstance(3); }}
			});
			*/
	}
}