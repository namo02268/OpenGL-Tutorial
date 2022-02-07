#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class UI {
public:
	virtual void Update() = 0;
};

class UI1 : public UI {
public:
	void Update() override {
		ImGui::Begin("This is UI1");
		ImGui::Text("This is some useful text.");
		ImGui::End();
	}
};

class UI2 : public UI {
public:
	void Update() override {
		ImGui::Begin("This is UI2");
		ImGui::Text("This is some useful text.");
		ImGui::End();
	}
};