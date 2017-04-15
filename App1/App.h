#pragma once

#include "AppTime.h"
#include "Shader.h"
#include "Common\ShaderStructures.h"
#include "DeferredRenderer.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "ModelData.h"
#include "PostProcessing.h"
#include "GammaPostProcessing.h"
#include "Camera.h"

class App
{
public:
	App();
	~App();
	bool Initialize(int width, int height, const char* title);
	void GameLoop();
	void LoadContent();
	void UnloadContent();
	void FixedUpdate(const AppTime & time);
	void Update(const AppTime & time);
	void Draw(const AppTime & time);
private:
	static void StaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static App* currentApp;

	GLFWwindow* window;
	bool glfwInitialized = false;
	std::unique_ptr<Camera> camera;
	std::unique_ptr<DeferredRenderer> deferredRenderer;
	std::unique_ptr<PostProcessing> postProcessing;
	std::vector<std::unique_ptr<ILight>> lights;

	std::shared_ptr<Model<VertexPositionTextureNTB>> ground;
	std::shared_ptr<Model<VertexPositionTextureNTB>> nanosuit;

	std::shared_ptr<GammaPostProcessing> gammaPostProc;
};

