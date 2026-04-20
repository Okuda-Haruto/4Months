#include "Player.h"
#include <numbers>

void Player::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight, const std::shared_ptr<Camera> camera) {
	//初期化
	Human::Initialize(position, directionalLight,camera);

	startTime_ = 0.1f;
}

void Player::Update(const std::shared_ptr<Input> input) {
	Keyboard keyboard = input->GetKeyBoard();
	Pad pad = input->GetPad(0);
	isCharging_ = false;

	//基礎クォータニオン(真下)
	Quaternion NextRotate;

	NextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	//基礎クオータニオン分の回転行列
	Matrix4x4 rotateMatrix = MakeRotateMatrix(NextRotate);

	//パッド操作

	Vector2 vector{};

		//スティック操作
		if (pad.LeftStick.magnitude > 0.2f) {
			vector = { pad.LeftStick.vector.x * pad.LeftStick.magnitude, pad.LeftStick.vector.y * pad.LeftStick.magnitude };
		}

		//上下左右キー
		if (keyboard.hold[DIK_UP] || keyboard.hold[DIK_W] || pad.Button[PAD_BUTTON_UP].hold) {
			vector.y += 1.0f;
		}
		if (keyboard.hold[DIK_DOWN] || keyboard.hold[DIK_S] || pad.Button[PAD_BUTTON_DOWN].hold) {
			vector.y -= 1.0f;
		}
		if (keyboard.hold[DIK_RIGHT] || keyboard.hold[DIK_D] || pad.Button[PAD_BUTTON_RIGHT].hold) {
			vector.x += 1.0f;
		}
		if (keyboard.hold[DIK_LEFT] || keyboard.hold[DIK_A] || pad.Button[PAD_BUTTON_LEFT].hold) {
			vector.x -= 1.0f;
		}
		if (vacuumState_ == None && startTime_ <= 0.0f) {
			if ((keyboard.hold[DIK_SPACE] || pad.Button[PAD_BUTTON_B].hold)) {
				Charge();
			}
			if ((keyboard.release[DIK_SPACE] || pad.Button[PAD_BUTTON_B].release)) {
				Throw();
			}
		}

		if (Length(vector) > 0.0f) {
			if (Length(vector) > 1.0f) {
				vector = Normalize(vector);
			}
			
			NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, std::numbers::pi_v<float> / 4 * vector.y);
			rotateMatrix = MakeRotateMatrix(NextRotate);
			NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4 * vector.x);
		}
		

	//現在の向きと次の向きの補完
	transform_.rotate = Slerp(transform_.rotate, NextRotate, 0.1f * GameEngine::GetDeltaTimeRate());

	//速度などを加算する
	Human::Update();

#ifdef USE_IMGUI
	ImGui::Begin("プレイヤー");
	ImGui::DragFloat3("速度", &velocity_.translate.x);
	ImGui::DragFloat("まぜる時間", &vacuumTime_);
	ImGui::DragFloat("戻る時間", &returnTime_);
	ImGui::DragFloat("発射時の速度", &headStartSpeed_, 0.1f);

	if ((keyboard.release[DIK_G] || pad.Button[PAD_BUTTON_LSTICK].release)) {
		stop = !stop;
	}

	ImGui::End();
#endif

	if (startTime_ > 0.0f) {
		startTime_ -= GameEngine::GetDeltaTime();
	}

}

void Player::Draw() {
	Human::Draw();
}