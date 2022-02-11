#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Model.h"

class UI {
public:
	virtual void Update(unsigned int& width, unsigned int& height) = 0;
};

class ModelUI : public UI {
public:
	ModelUI(const std::shared_ptr<Model>& model) { this->model = model; }

	void Update(unsigned int& width, unsigned int& height) override {
		ImGui::SetNextWindowPos(ImVec2(width - 300, 0.0));
		ImGui::SetNextWindowSize(ImVec2(300, height));
		ImGui::Begin("Model", &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::PushID((model->name).c_str());
		if (ImGui::CollapsingHeader((model->name).c_str()))
		{
			ImGui::Text("Transform");
			ImGui::DragFloat3("Position", &model->position.x, 0.01f);
			ImGui::DragFloat3("Scale", &model->scale.x, 0.01f);
			ImGui::DragFloat3("Rotation", &model->rotation.x, 1.0f);
		}
		ImGui::PopID();
		ImGui::End();
	}

private:
	std::shared_ptr<Model> model;
	bool open;
};
