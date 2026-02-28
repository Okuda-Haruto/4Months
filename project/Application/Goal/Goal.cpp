#include "Goal.h"
#include "ModelManager/ModelManager.h"
#include "Operation/Operation.h"
#include <imgui.h>

#include <numbers>

void Goal::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight) {
	object_ = make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/sphere", "sphere.obj"));
	object_->SetShininess(30.0f);

	transform_ = {};
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.translate = position;
	object_->SetTransform(transform_);
	object_->SetDirectionalLight(directionalLight);
}

void Goal::Update() {
#ifdef USE_IMGUI
	ImGui::Begin("ゴール");
	ImGui::DragFloat3("Translate", &transform_.translate.x);
	ImGui::End();
#endif // USE_IMGUI

	object_->SetTransform(transform_);
}

void Goal::Draw() {
	object_->Draw3D();
}
