#include "Player.h"
#include <Course/Course.h>
#include <StartCountdown/StartCountdown.h>
#include <numbers>

void Player::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight, const std::shared_ptr<Camera> camera) {
	//初期化
	Human::Initialize(position, directionalLight, camera);

	startTime_ = 0.1f;
}

void Player::Update(const std::shared_ptr<Input> input, Course* course, StartCountdown* countdown) {
	if (isFinalResult_) {
		
		SRT modelTransform = transform_;
		modelTransform.rotate = MakeRotateAxisAngleQuaternion({ 1,0,0 }, 0);
		Vector3 up = RotateVector({ 0,1,0 }, modelTransform.rotate);
		up.x *= -1; up.z *= -1;
		modelTransform.translate = transform_.translate + up * humanFootOffset;
		model_->SetTransform(modelTransform);
		model_->Update();

		headTransform_ = { headTransform_.scale,transform_.rotate, transform_.translate };
		headTransform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, headRotate_) * modelTransform.rotate;
		bulletModel_->SetTransform(headTransform_);

		Quaternion NextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
		transform_.rotate = Slerp(transform_.rotate, NextRotate, 0.1f * GameEngine::GetDeltaTimeRate());
		return;
	}


	//基礎クォータニオン(真下)
	Quaternion NextRotate;

	NextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	//基礎クオータニオン分の回転行列
	Matrix4x4 rotateMatrix = MakeRotateMatrix(NextRotate);

	Vector2 vector{};

	Keyboard keyboard = input->GetKeyBoard();
	Pad pad = input->GetPad(0);

	// ★ここから入力を止める
	if (!isSectionResult_ && !isBreak_) {

		isCharging_ = false;
		if (keyboard.trigger[DIK_L]) {
			isAutoBurst_ = !isAutoBurst_;
		}
		if (keyboard.trigger[DIK_P]) {
			isJumpFlashMode_ = !isJumpFlashMode_;
		}
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
		isEndSectionResult_ = false;

	} else if (vacuumState_ != Break) {
		transform_.translate.x = 0;
		transform_.translate.z = 0;

		// 簡易リザルト終了
		if (canSkipResult_ && (keyboard.release[DIK_SPACE] || pad.Button[PAD_BUTTON_B].release)) {
			float blockSize = 3.0f;
			float offset = transform_.translate.y - std::floor(transform_.translate.y / blockSize) * blockSize;
			transform_.translate.y = resultLoopEndY + offset;
			isSectionResult_ = false;
			isEndSectionResult_ = true;
		}
	} else {
		// 簡易リザルト終了
		if (keyboard.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_B].trigger) {
			float blockSize = 3.0f;
			float offset = transform_.translate.y - std::floor(transform_.translate.y / blockSize) * blockSize;
			transform_.translate = { 0,resultLoopEndY + offset,0 };
			isSectionResult_ = false;
			isBreak_ = false;
			vacuumState_ = None;

			//やられモーションから治す
			model_->ResetAnimationTime();
			model_->SetAnimationIndex(7);
			model_->SetIsLoopAnimation(false);

			course->ResetFailed();
			countdown->Reset(transform_.translate);
		}
	}

	// ★入力が無い場合でも回転処理はそのまま通る
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
	if (!isBreak_) {
		Human::Update();
	}

#ifdef USE_IMGUI
	ImGui::Begin("プレイヤー");
	ImGui::DragFloat("まぜる時間", &vacuumTime_);
	ImGui::DragFloat("戻る時間", &returnTime_);
	ImGui::DragFloat("発射時の速度", &headStartSpeed_, 0.1f);

	if (!isSectionResult_) {
		Keyboard keyboard = input->GetKeyBoard();
		Pad pad = input->GetPad(0);

		if ((keyboard.release[DIK_G] || pad.Button[PAD_BUTTON_LSTICK].release)) {
			stop = !stop;
		}
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