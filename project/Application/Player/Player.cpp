#include "Player.h"
#include <numbers>

void Player::Initialize(const std::shared_ptr<DirectionalLight> directionalLight) {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player/Head", "Head.obj"));
	model_->SetShininess(30.0f);
	transform_ = {};
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	model_->SetTransform(transform_);
	model_->SetDirectionalLight(directionalLight);

	fallingSpeed_ = 0.0f;
}

void Player::Update(const std::shared_ptr<Input> input) {
	Keyboard keyboard = input->GetKeyBoard();
	Pad pad = input->GetPad(0);

	//基礎クォータニオン(真下)
	Quaternion NextRotate;

	if (!isTurnBack_) {
		NextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	} else {
		NextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, std::numbers::pi_v<float> / 2);
	}
	//基礎クオータニオン分の回転行列
	Matrix4x4 rotateMatrix = MakeRotateMatrix(NextRotate);

	if (keyboard.hold[DIK_UP] || keyboard.hold[DIK_W] || pad.Button[PAD_BUTTON_UP].hold) {
		NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } * rotateMatrix, std::numbers::pi_v<float> / 4);
	}
	if (keyboard.hold[DIK_DOWN] || keyboard.hold[DIK_S] || pad.Button[PAD_BUTTON_DOWN].hold) {
		NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } * rotateMatrix, -std::numbers::pi_v<float> / 4);
	}
	if (keyboard.hold[DIK_RIGHT] || keyboard.hold[DIK_D] || pad.Button[PAD_BUTTON_RIGHT].hold) {
		NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } * rotateMatrix, -std::numbers::pi_v<float> / 4);
	}
	if (keyboard.hold[DIK_LEFT] || keyboard.hold[DIK_A] || pad.Button[PAD_BUTTON_LEFT].hold) {
		NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } * rotateMatrix, std::numbers::pi_v<float> / 4);
	}

	//現在の向きと次の向きの補完
	transform_.rotate = Slerp(transform_.rotate, NextRotate, 0.25f);
	rotateMatrix = MakeRotateMatrix(transform_.rotate);

	//向いている向きに速度を向ける
	velocity_.translate = Vector3{ 0,0,1 } * rotateMatrix;

	//「下向き速度 * 重力」を落下速度に加える
	fallingSpeed_ = min(max(fallingSpeed_ + velocity_.translate.y * kGravity_, kMaxFallingSpeed_), kMaxRisingSpeed_);

	velocity_.translate.y = fallingSpeed_;

	static float speed = 1.0f / 100;
	transform_.translate += velocity_.translate * speed;

#ifdef USE_IMGUI
	ImGui::Begin("プレイヤー");
	ImGui::DragFloat3("速度", &velocity_.translate.x);
	ImGui::DragFloat("speed", &speed, 0.001f);
	if (ImGui::Button("折り返し")) {
		isTurnBack_ = !isTurnBack_;
	}
	ImGui::End();
#endif

	model_->SetTransform(transform_);
}

void Player::Draw() {
	model_->Draw3D();
}