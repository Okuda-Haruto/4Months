#include "GameCamera.h"
#include "Operation/Operation.h"
#include "Lerp.h"
#include "Course/Course.h"
#include <numbers>

#pragma region 落下カメラ

void DownCamera::Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) {
	gameCamera_ = gameCamera;
	input_ = input;
	player_ = player;
	course_ = course;

	//初期値として現在の向きを入れる
	transform_.scale = { 1,1,1 };
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	transform_.translate = player_->GetTransform().translate + Vector3{ 0,50,0 };
}
void DownCamera::Update() {

	Vector3 nextTranslate = player_->GetTransform().translate;

	// 折り返し基礎回転
	Quaternion nextRotate;
	if (course_->InSubSection()) {
		nextRotate = MakeRotateAxisAngleQuaternion(
			Vector3{ 0,1,0 },
			std::numbers::pi_v<float>
		);

		float dt = GameEngine::GetDeltaTimeRate() / 60.0f;

		float rate = 1.0f - powf(0.5f, dt * 16.0f); // ← 減衰速度

		transform_.rotate = Slerp(
			transform_.rotate,
			nextRotate,
			rate
		);

		nextTranslate += (kCameraPos + Vector3{7.5f,0,0})* MakeRotateMatrix(transform_.rotate);
		transform_.translate.y = player_->GetTransform().translate.y + 3.0f;

		transform_.translate = Lerp(
			transform_.translate,
			nextTranslate,
			rate
		);
	} else {
		nextRotate = MakeRotateAxisAngleQuaternion(
			Vector3{ 1,0,0 },
			-std::numbers::pi_v<float> / 2
		);

		float dt = GameEngine::GetDeltaTimeRate() / 60.0f;

		float rate = 1.0f - powf(0.5f, dt * 16.0f); // ← 減衰速度

		transform_.rotate = Slerp(
			transform_.rotate,
			nextRotate,
			rate
		);

		nextTranslate += kCameraPos * MakeRotateMatrix(transform_.rotate);
		transform_.translate.y += player_->GetFallingSpeed() * 0.75f;

		transform_.translate = Lerp(
			transform_.translate,
			nextTranslate,
			rate
		);
	}
}
#pragma endregion


#pragma region リザルトカメラ

void ResultCamera::Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) {
	gameCamera_ = gameCamera;
	input_ = input;
	player_ = player;
	course_ = course;

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
		nextVelocity += { 0, 1, 0 };
	}
	if (keyboard.hold[DIK_S] || keyboard.hold[DIK_DOWN]) {
		nextVelocity += { 0, -1, 0 };
	}
	if (keyboard.hold[DIK_D] || keyboard.hold[DIK_RIGHT]) {
		nextVelocity += { 1, 0, 0 };
	}
	if (keyboard.hold[DIK_A] || keyboard.hold[DIK_LEFT]) {
		nextVelocity += { -1, 0, 0 };
	}

	//動かしていたらマニュアル移動
	if (Length(nextVelocity) > 0.0f) {
		nextVelocity = Normalize(nextVelocity) * 4.0f;
		mode_ = CameraMode::Manual;
		releaseKeyTime_ = 0.0f;
	} else {
		//動かしていない場合勝手にオートマに
		releaseKeyTime_ += GameEngine::GetDeltaTime();
		if (releaseKeyTime_ > kMaxReleaseKeyTime && mode_ == CameraMode::Manual) {
			mode_ = CameraMode::Automatic_Down;
		}
	}

	switch (mode_) {
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
	posY_ += velocity_.y * GameEngine::GetDeltaTimeRate();

	//位置を戻す
	if (posY_ > 0.0f) {
		posY_ = 0.0f;
		if (mode_ == CameraMode::Automatic_Up) {
			mode_ = CameraMode::Automatic_Down;
		}
	}
	if (posY_ < gameCamera_->GetCameraPosBottom() + 1 * 16 * 3) {
		posY_ = gameCamera_->GetCameraPosBottom() + 1 * 16 * 3;
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

#pragma region リビング全体を移すカメラ

void LivingCamera::Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) {
	transform_.scale = { 1,1,1 };
	transform_.translate = kCameraPos;
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>);
}

void LivingCamera::Update() {

}

#pragma endregion

#pragma region TVのみを写すカメラ

void TVCamera::Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) {
	transform_.scale = { 1,1,1 };
	transform_.translate = kCameraPos;
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>);
}

void TVCamera::Update() {

}

#pragma endregion

#pragma region スタジオ全体を移すカメラ

void StudioCamera::Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) {
	transform_.scale = { 1,1,1 };
	transform_.translate = kCameraPos;
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>);
}

void StudioCamera::Update() {

}

#pragma endregion

#pragma region ステージセレクトカメラ

void SelectCamera::Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) {
	transform_.scale = { 1,1,1 };
	transform_.translate = kCameraPos;
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 180 * 225);
}

void SelectCamera::Update() {

}

#pragma endregion

#pragma region エディターカメラ

#ifdef USE_IMGUI

void EditorCamera::Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player,Course* course) {
	gameCamera_ = gameCamera;
	input_ = input;
	player_ = player;
	course_ = course;

	//初期値として現在の向きを入れる
	transform_.scale = { 1,1,1 };
	transform_.rotate = IdentityQuaternion();

	cameraPos_ = { 0, 0, -150 };
	yaw_ = IdentityQuaternion();
	pitch_ = IdentityQuaternion();

	centerPoint_ = {};
}

void EditorCamera::Update() {
	Mouse mouse = input_->GetMouse();
	Keyboard key = input_->GetKeyBoard();

	//ホイールクリックで移動
	if (mouse.click[MOUSE_BOTTON_WHEEL].hold) {
		Vector3 XZMovement = { mouse.Movement.x,0.0f,mouse.Movement.y };

		centerPoint_.x += RotateVector(XZMovement / 30, transform_.rotate).x * GameEngine::GetDeltaTimeRate();;
		centerPoint_.z += -RotateVector(XZMovement / 30, transform_.rotate).z * GameEngine::GetDeltaTimeRate();;
	}

	//右クリック中は回転と拡縮
	if (mouse.click[MOUSE_BOTTON_RIGHT].hold) {
		yaw_ = yaw_ * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, -std::numbers::pi_v<float> / 360 * mouse.Movement.x);
		pitch_ = pitch_ * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 360 * mouse.Movement.y);

		//ホイール拡縮
		cameraPos_.z += mouse.Movement.z / 120 * GameEngine::GetDeltaTimeRate();
	} else {
		//ホイールy軸移動
		if (key.hold[DIK_LSHIFT] || key.hold[DIK_RSHIFT]) {
			centerPoint_.y += mouse.Movement.z / 40 * GameEngine::GetDeltaTimeRate();
		}
		else {
			centerPoint_.y += mouse.Movement.z / 120 * GameEngine::GetDeltaTimeRate();
		}
	}
	transform_.rotate = pitch_ * yaw_;
	transform_.translate = cameraPos_ * MakeRotateMatrix(transform_.rotate) + centerPoint_;
}

#endif // USE_IMGUI

#pragma endregion

void GameCamera::Initialize(std::shared_ptr<Camera> camera, std::unique_ptr<BaseCamera> nowCameraMode, std::shared_ptr<Input> input, Player* player, Course* course) {
	camera_ = camera;
	input_ = input;
	player_ = player;
	course_ = course;

	// 初期値
	nowCamera_ = move(nowCameraMode);
	nowCamera_->Initialize(this, input_, player, course);
}

void GameCamera::Update() {
	//次がないなら現在のカメラで更新する
	if (!nextCamera_) {
		nowCamera_->Update();
		camera_->Update(nowCamera_->GetTransform());

		// カメラシェイク
		if (shakeTime_ > 0.0f) {
			shakeTime_ -= GameEngine::GetDeltaTime();
			if (shakeTime_ < 0.0f) {
				shakeTime_ = 0.0f;
			}

			float amp = amplitude_ * shakeTime_ / shakeEndTime_;

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
			changeCameraTime_ += GameEngine::GetDeltaTime();
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
			if (shakeTime_ > 0.0f) {
				shakeTime_ -= GameEngine::GetDeltaTime();
				if (shakeTime_ < 0.0f) {
					shakeTime_ = 0.0f;
				}

				float amp = amplitude_ * shakeTime_ / shakeEndTime_;

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
		lerpTransform.translate = Lerp(nowTransform.translate, nextTransform.translate, 1.0f - powf(1.0f - changeCameraTime_ / maxChangeCameraTime_, 2));

		camera_->Update(lerpTransform);
		// カメラシェイク
		if (shakeTime_ > 0.0f) {
			shakeTime_ -= GameEngine::GetDeltaTime();
			if (shakeTime_ < 0.0f) {
				shakeTime_ = 0.0f;
			}

			float amp = amplitude_ * shakeTime_ / shakeEndTime_;

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
	nextCamera_->Initialize(this, input_, player_, course_);

	//カメラ遷移時間
	maxChangeCameraTime_ = changeCameraTime;
	changeCameraTime_ = 0.0f;
}

void GameCamera::StartShake(float amplitude, float time) {
	amplitude_ = amplitude;
	shakeTime_ = time;
	shakeEndTime_ = time;
}

#pragma region ゲーム開始前カメラ

void StartCamera::Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player,Course* course) {
	gameCamera_ = gameCamera;
	input_ = input;
	player_ = player;
	course_ = course;

	transform_.scale = { 1,1,1 };
}

void StartCamera::Update() {
	timer_ += GameEngine::GetDeltaTime();
	float t = 0;
	Vector3 target{0,0,0};
	float radius = 0;
	float angle = 0;
	Vector3 offset{};
	float height = 0;
	Vector3 finalPos{};
	Quaternion finalRot{};

	t = timer_ / risingTime_;
	t = std::clamp(t, 0.0f, 1.0f);

	radius = 250.0f;
	angle = t * std::numbers::pi_v<float> *2.0f - std::numbers::pi_v<float> / 2.0f;

	offset = {
		cosf(angle) * radius,
		0,
		sinf(angle) * radius
	};

	height = Lerp(-1000.0f, 0.0f, t); // 移動範囲

	Vector3 basePos = target + offset;
	basePos.y += height;

	transform_.translate = basePos;

	// 回転（補間あり）
	Quaternion targetRot = LookAt(transform_.translate, target);

	transform_.rotate = Slerp(
		transform_.rotate,
		targetRot,
		0.5f
	);
}

#pragma endregion

Quaternion StartCamera::LookAt(const Vector3& eye, const Vector3& target) {
	Vector3 forward = Normalize(target - eye);

	Vector3 up = { 0,1,0 };

	if (fabs(Dot(forward, up)) > 0.99f) {
		up = { 0,0,1 };
	}

	Vector3 right = Normalize(Cross(up, forward));
	Vector3 newUp = Cross(forward, right);

	Matrix4x4 m;
	m.m[0][0] = right.x;   m.m[0][1] = right.y;   m.m[0][2] = right.z;
	m.m[1][0] = newUp.x;   m.m[1][1] = newUp.y;   m.m[1][2] = newUp.z;
	m.m[2][0] = forward.x; m.m[2][1] = forward.y; m.m[2][2] = forward.z;

	return Normalize(MatrixToQuaternion(m));
}

Quaternion StartCamera::MatrixToQuaternion(const Matrix4x4& m) {
	Quaternion q{};
	float trace = m.m[0][0] + m.m[1][1] + m.m[2][2];

	if (trace > 0.0f) {
		float s = sqrtf(trace + 1.0f) * 2.0f; // S=4*qw
		q.w = 0.25f * s;
		q.x = (m.m[2][1] - m.m[1][2]) / s;
		q.y = (m.m[0][2] - m.m[2][0]) / s;
		q.z = (m.m[1][0] - m.m[0][1]) / s;
	} else if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2]) {
		float s = sqrtf(1.0f + m.m[0][0] - m.m[1][1] - m.m[2][2]) * 2.0f;
		q.w = (m.m[2][1] - m.m[1][2]) / s;
		q.x = 0.25f * s;
		q.y = (m.m[0][1] + m.m[1][0]) / s;
		q.z = (m.m[0][2] + m.m[2][0]) / s;
	} else if (m.m[1][1] > m.m[2][2]) {
		float s = sqrtf(1.0f + m.m[1][1] - m.m[0][0] - m.m[2][2]) * 2.0f;
		q.w = (m.m[0][2] - m.m[2][0]) / s;
		q.x = (m.m[0][1] + m.m[1][0]) / s;
		q.y = 0.25f * s;
		q.z = (m.m[1][2] + m.m[2][1]) / s;
	} else {
		float s = sqrtf(1.0f + m.m[2][2] - m.m[0][0] - m.m[1][1]) * 2.0f;
		q.w = (m.m[1][0] - m.m[0][1]) / s;
		q.x = (m.m[0][2] + m.m[2][0]) / s;
		q.y = (m.m[1][2] + m.m[2][1]) / s;
		q.z = 0.25f * s;
	}

	return Normalize(q);
}