#include "Goal.h"
#include "ModelManager/ModelManager.h"
#include "Operation/Operation.h"
#include <imgui.h>

#include <numbers>

void Goal::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight) {
	object_ = make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel("resources/Goal", "Goal.obj"));
	object_->SetShininess(30.0f);

	transform_ = {};
	transform_.scale = { 2.0f,2.0f,2.0f };
	transform_.translate = position;
	object_->SetTransform(transform_);
	object_->SetDirectionalLight(directionalLight);

	goalCoolTime_ = 0.0f;
}

void Goal::Update() {
#ifdef USE_IMGUI
	ImGui::Begin("ゴール");
	ImGui::DragFloat3("Translate", &transform_.translate.x);
	ImGui::End();
#endif // USE_IMGUI

	if (goalCoolTime_ > 0.0f) {
		goalCoolTime_ -= 1.0f / 60.0f;
	}

	//誰かが取得済みなら追従する
	if (human_) {
		velocity_.translate.y = 0.0f;
		SRT transform = human_->GetTransform();
		//頭からの距離
		Vector3 haveLength = Vector3(0, 0, 3.2f) * MakeRotateMatrix(transform.rotate);
		transform.translate += haveLength;
		transform.scale = transform_.scale;
		transform_ = transform;
	} else if (transform_.translate.y > kBaseHeight_) {
		velocity_.translate.y += -0.01f;
		if (velocity_.translate.y < -0.2f) {
			velocity_.translate.y = -0.2f;
		}
		transform_.translate.y += velocity_.translate.y;
		transform_.rotate = IdentityQuaternion();
	}

	object_->SetTransform(transform_);
}

void Goal::Draw() {
	object_->Draw3D();
}
