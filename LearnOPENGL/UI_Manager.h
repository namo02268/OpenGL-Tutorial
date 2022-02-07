#pragma once

#include <GLFW/glfw3.h>

#include <vector>

#include "UI.h"

class UIManager
{
private:
	std::vector<std::shared_ptr<UI>> UIs;

public:
	UIManager(GLFWwindow* window);
	~UIManager();
	void Add(std::shared_ptr<UI> ui);
	void Update();
	void Draw();

private:
	void Init(GLFWwindow* window);
};

UIManager::UIManager(GLFWwindow* window) {
	Init(window);
}

UIManager::~UIManager() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UIManager::Add(std::shared_ptr<UI> ui) {
	UIs.push_back(ui);
}

void UIManager::Update() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	for (auto& UI : UIs)
		UI->Update();
}

void UIManager::Draw() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::Init(GLFWwindow* window) {
	// init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	const char* glsl_version = "#version 330";
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}