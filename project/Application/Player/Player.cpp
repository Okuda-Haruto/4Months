#include "Player.h"

void Player::Initialize(const std::shared_ptr<DirectionalLight> directionalLight) {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/sphere", "sphere.obj"));
	model_->SetShininess(30.0f);
	transform_ = { {1,1,1},{},{} };
	model_->SetTransform(transform_);
	model_->SetDirectionalLight(directionalLight);
}

void Player::Update(const std::shared_ptr<Input> input) {
	Keybord keybord = input->GetKeyBord();
	Pad pad = input->GetPad(0);

	if (keybord.hold[DIK_UP] || pad.Button[PAD_BUTTON_UP].hold) {
			transform_.translate.y += 0.1f;
	}
	if (keybord.hold[DIK_DOWN] || pad.Button[PAD_BUTTON_DOWN].hold) {
			transform_.translate.y -= 0.1f;
	}
	if (keybord.hold[DIK_RIGHT] || pad.Button[PAD_BUTTON_RIGHT].hold) {
			transform_.translate.x += 0.1f;
	}
	if (keybord.hold[DIK_LEFT] || pad.Button[PAD_BUTTON_LEFT].hold) {
			transform_.translate.x -= 0.1f;
	}
	model_->SetTransform(transform_);
}

void Player::Draw() {
	model_->Draw3D();
}