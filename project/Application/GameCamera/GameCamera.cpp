#include "GameCamera.h"
#include "Operation/Operation.h"
#include "Lerp.h"
#include <numbers>

#pragma region 落下カメラ

void DownCamera::Initialize(std::shared_ptr<Input> input, Player* player) {
	input_ = input;
	player_ = player;

	//初期値として現在の向きを入れる
	transform_.scale = { 1,1,1 };
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	transform_.translate = player_->GetTransform().translate;
}
void DownCamera::Update() {

	Vector3 nextTranslate = player_->GetTransform().translate;

	// 折り返し基礎回転
	Quaternion nextRotate;
	nextRotate = MakeRotateAxisAngleQuaternion(
		Vector3{ 1,0,0 },
		-std::numbers::pi_v<float> / 2
	);

	transform_.rotate = Slerp(
		transform_.rotate,
		nextRotate,
		0.1f
	);

	nextTranslate += kCameraPos * MakeRotateMatrix(transform_.rotate);
	transform_.translate.y += player_->GetFallingSpeed() * 0.75f;

	transform_.translate = Lerp(
		transform_.translate,
		nextTranslate,
		0.1f
	);

}
#pragma endregion


#pragma region リザルトカメラ

void ResultCamera::Initialize(std::shared_ptr<Input> input, Player* player) {
	input_ = input;
	player_ = player;

	//初期値として現在の向きを入れる
	transform_.scale = { 1,1,1 };
	transform_.rotate = IdentityQuaternion();
	transform_.translate = kCameraPos;

	velocity_ = {};
	posY_ = 0.0f;
	mode_ = CameraMode::Automatic_Down;
	releaseKeyTime_ = 0.0f;
}
void ResultCamera::Update() {
	Keyboard keyboard = input_->GetKeyBoard();
	Pad pad = input_->GetPad(0);

	//カメラ移動
	Vector3 nextVelocity{};

	if (pad.LeftStick.magnitude > 0.2f) {//スティック操作
		nextVelocity = { pad.LeftStick.vector.x * pad.LeftStick.magnitude, pad.LeftStick.vector.y * pad.LeftStick.magnitude,0.0f };
	}

	//マニュアル移動
	if (keyboard.hold[DIK_W] || keyboard.hold[DIK_UP]) {
		nextVelocity += { 0,1,0 };
	}
	if (keyboard.hold[DIK_S] || keyboard.hold[DIK_DOWN]) {
		nextVelocity += { 0,-1,0 };
	}
	if (keyboard.hold[DIK_D] || keyboard.hold[DIK_RIGHT]) {
		nextVelocity += { 1,0,0 };
	}
	if (keyboard.hold[DIK_A] || keyboard.hold[DIK_LEFT]) {
		nextVelocity += { -1,0,0 };
	}

	//動かしていたらマニュアル移動
	if (Length(nextVelocity) > 0.0f) {
		nextVelocity = Normalize(nextVelocity) * 4.0f;
		mode_ = CameraMode::Manual;
		releaseKeyTime_ = 0.0f;
	} else {
	//動かしていない場合勝手にオートマに
		releaseKeyTime_ += 1.0f / 60.0f;
		if (releaseKeyTime_ > kMaxReleaseKeyTime && mode_ == CameraMode::Manual) {
			mode_ = CameraMode::Automatic_Down;
		}
	}

	switch (mode_)
	{
	case CameraMode::Automatic_Up:
		nextVelocity = { 0,1,0 };
		break;
	case CameraMode::Automatic_Down:
		nextVelocity = { 0,-1,0 };
		break;
	default:
		break;
	}

	velocity_ = Lerp(velocity_, nextVelocity, 0.1f);
	posY_ += velocity_.y;

	//位置を戻す
	if (posY_ > 0.0f) {
		posY_ = 0.0f;
		if (mode_ == CameraMode::Automatic_Up) {
			mode_ = CameraMode::Automatic_Down;
		}
	}
	if (posY_ < -32 * 12 * 3.0f + 16.0f * 3.0f) {
		posY_ = -32 * 12 * 3.0f + 16.0f * 3.0f;
		if (mode_ == CameraMode::Automatic_Down) {
			mode_ = CameraMode::Automatic_Up;
		}
	}

	//回転
	transform_.rotate = transform_.rotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, kCameraRotateSpeed);
	transform_.translate = kCameraPos * MakeRotateMatrix(transform_.rotate);
	//回転したxz平面のカメラ座標にYを加える
	transform_.translate.y = posY_;
	//ずらして見せる
	transform_.translate += velocity_ * MakeRotateMatrix(transform_.rotate) * 10;
}
#pragma endregion


void GameCamera::Initialize(std::shared_ptr<Camera> camera, std::shared_ptr<Input> input, Player* player) {
	camera_ = camera;
	input_ = input;
	player_ = player;

	//一旦落下カメラを初期値に
	nowCamera_ = std::make_unique<DownCamera>();
	nowCamera_->Initialize(input_, player);
}

void GameCamera::Update() {
	//次がないなら現在のカメラで更新する
	if (!nextCamera_) {
		nowCamera_->Update();
		camera_->Update(nowCamera_->GetTransform());

		// カメラシェイク
		if (shakeFrame_ > 0) {
			shakeFrame_--;

			float amp = amplitude_ * (float(shakeFrame_) / float(shakeEndFrame_));

			shake_ = {
				GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
				GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
				GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
			};
		} else {
			shake_ = {};
			amplitude_ = 0;
		}
		SRT shakedTransform = nowCamera_->GetTransform();
		shakedTransform.translate += shake_;

		transform_ = shakedTransform;
		// 通常カメラのビュー
		camera_->SetViewMatrix(Inverse(MakeQuaternionMatrix(shakedTransform.scale, shakedTransform.rotate, shakedTransform.translate)));
	//カメラ遷移
	} else {
		//遷移タイマー
		if (changeCameraTime_ < maxChangeCameraTime_) {
			changeCameraTime_ += 1.0f / 60.0f;
		} else {
			//遷移
			if (nowCamera_) {
				nowCamera_.reset();
			}
			nowCamera_ = move(nextCamera_);
			nextCamera_.reset();

			nowCamera_->Update();
			camera_->Update(nowCamera_->GetTransform());

			// カメラシェイク
			if (shakeFrame_ > 0) {
				shakeFrame_--;

				float amp = amplitude_ * (float(shakeFrame_) / float(shakeEndFrame_));

				shake_ = {
					GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
					GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
					GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
				};
			} else {
				shake_ = {};
				amplitude_ = 0;
			}
			SRT shakedTransform = nowCamera_->GetTransform();
			shakedTransform.translate += shake_;

			transform_ = shakedTransform;
			// 通常カメラのビュー
			camera_->SetViewMatrix(Inverse(MakeQuaternionMatrix(shakedTransform.scale, shakedTransform.rotate, shakedTransform.translate)));
			return;
		}  
		//遷移中なら両方を更新
		nowCamera_->Update();

		nextCamera_->Update();

		SRT lerpTransform;
		SRT nowTransform = nowCamera_->GetTransform();
		SRT nextTransform = nextCamera_->GetTransform();
		lerpTransform.scale = { 1,1,1 };
		lerpTransform.rotate = Slerp(nowTransform.rotate, nextTransform.rotate, 1.0f - powf(1.0f - changeCameraTime_ / maxChangeCameraTime_, 2));
		lerpTransform.translate = Lerp(nowTransform.translate, nextTransform.translate, 1.0f - powf(1.0f - changeCameraTime_ / maxChangeCameraTime_,2));

		camera_->Update(lerpTransform);
		// カメラシェイク
		if (shakeFrame_ > 0) {
			shakeFrame_--;

			float amp = amplitude_ * (float(shakeFrame_) / float(shakeEndFrame_));

			shake_ = {
				GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
				GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
				GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
			};
		} else {
			shake_ = {};
			amplitude_ = 0;
		}
		lerpTransform.translate += shake_;

		transform_ = lerpTransform;
		// 通常カメラのビュー
		camera_->SetViewMatrix(Inverse(MakeQuaternionMatrix(lerpTransform.scale, lerpTransform.rotate, lerpTransform.translate)));
	}
}

void GameCamera::ChangeCamera(const std::unique_ptr<BaseCamera>& nextCamera, float changeCameraTime) {
	nextCamera_ = std::move(const_cast<std::unique_ptr<BaseCamera>&>(nextCamera));
	nextCamera_->Initialize(input_, player_);
	
	//カメラ遷移時間
	maxChangeCameraTime_ = changeCameraTime;
	changeCameraTime_ = 0.0f;

}

void GameCamera::StartShake(float amplitude, int frame) {
	amplitude_ = amplitude;
	shakeFrame_ = frame;
	shakeEndFrame_ = frame;
}