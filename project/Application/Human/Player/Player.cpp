#include "Player.h"
#include <numbers>

void Player::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight) {
	//初期化
	Human::Initialize(position, directionalLight);
	color_ = { 0,1,0,1 };   //首の色　緑
}

void Player::Update(const std::shared_ptr<Input> input) {
	Keyboard keyboard = input->GetKeyBoard();
	Pad pad = input->GetPad(0);

	//基礎クォータニオン(真下)
	Quaternion NextRotate;

	NextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	NextRotate = NextRotate * rollRotate_;
	//基礎クオータニオン分の回転行列
	Matrix4x4 rotateMatrix = MakeRotateMatrix(NextRotate);

	//パッド操作
	if (pad.isConnected) {

		//上下左右キー
		if (pad.LeftStick.magnitude > 0.2f) {
			NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, std::numbers::pi_v<float> / 4 * pad.LeftStick.vector.y * pad.LeftStick.magnitude);
			NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4 * pad.LeftStick.vector.x * pad.LeftStick.magnitude);
		}
		if (isDrifting_) {
			unableDriftTimer_ = unableDriftTime_;
		}
		isDrifting_ = false;

		if (vacuumState_ == None) {
			if (pad.Button[PAD_BUTTON_Y].hold) {
				Charge();
			}

			if (pad.Button[PAD_BUTTON_Y].release) {
				Throw();
			}
		}
	} else {

		//上下左右キー
		if (keyboard.hold[DIK_UP] || keyboard.hold[DIK_W] || pad.Button[PAD_BUTTON_UP].hold) {
			NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, std::numbers::pi_v<float> / 4);
		}
		if (keyboard.hold[DIK_DOWN] || keyboard.hold[DIK_S] || pad.Button[PAD_BUTTON_DOWN].hold) {
			NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4);
		}
		if (keyboard.hold[DIK_RIGHT] || keyboard.hold[DIK_D] || pad.Button[PAD_BUTTON_RIGHT].hold) {
			NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4);
		}
		if (keyboard.hold[DIK_LEFT] || keyboard.hold[DIK_A] || pad.Button[PAD_BUTTON_LEFT].hold) {
			NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, std::numbers::pi_v<float> / 4);
		}
		if (vacuumState_ == None) {
			if ((keyboard.hold[DIK_Q] || keyboard.hold[DIK_SPACE])) {
				Charge();
			}
			if ((keyboard.release[DIK_Q] || keyboard.release[DIK_SPACE])) {
				Throw();
			}
		}
		if (isDrifting_) {
			unableDriftTimer_ = unableDriftTime_;
		}
		isDrifting_ = false;
	}

	//現在の向きと次の向きの補完
	transform_.rotate = Slerp(transform_.rotate, NextRotate, 0.1f);

	//速度などを加算する
	Human::Update();

#ifdef USE_IMGUI
	ImGui::Begin("プレイヤー");
	ImGui::DragFloat3("速度", &velocity_.translate.x);
	if (ImGui::Button("折り返し")) {
		isTurnBack_ = !isTurnBack_;
	}
	ImGui::DragInt("まぜる時間", &vacuumTime_);
	ImGui::DragInt("戻る時間", &returnTime_);
	ImGui::DragFloat("発射時の速度", &headStartSpeed_, 0.1f);

	ImGui::End();
#endif

}

void Player::Draw() {
	Human::Draw();
}
bool Player::GetIsDrifting() const {
	return isDrifting_;
}