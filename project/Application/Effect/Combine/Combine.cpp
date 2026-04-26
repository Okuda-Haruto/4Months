#include "Combine.h"
#include <numbers>

static float EaseOutCubic(float t) {
	return 1.0f - powf(1.0f - t, 3.0f);
}

Quaternion QuaternionFromMatrix(const Matrix4x4& m) {
	float trace = m.m[0][0] + m.m[1][1] + m.m[2][2];

	Quaternion q;

	if (trace > 0.0f) {
		float s = sqrtf(trace + 1.0f) * 2.0f;
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

Quaternion MakeLookRotation(const Vector3& forward, const Vector3& up) {
	Vector3 f = Normalize(forward);
	Vector3 r = Normalize(Cross(up, f));
	Vector3 u = Cross(f, r);

	Matrix4x4 m = {
		r.x, r.y, r.z, 0,
		u.x, u.y, u.z, 0,
		f.x, f.y, f.z, 0,
		0,   0,   0,   1
	};

	return QuaternionFromMatrix(m);
}

void Combine::InitializeTitle(std::shared_ptr<DirectionalLight> directionalLight, shared_ptr<Camera> camera) {
	camera_ = camera;

	human_ = std::make_unique<Object>();
	human_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player", "Player.gltf"));
	human_->SetDirectionalLight(directionalLight);
	human_->SetShininess(0);
	SRT transform = human_->GetTransform();
	transform.scale = { 1.0f / 0.13f,1.0f / 0.13f ,1.0f / 0.13f };
	transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, std::numbers::pi_v<float>);
	transform.translate = { 0,defaultY_,0 };
	human_->SetTransform(transform);
	human_->SetIsUseAnimation(true);
	human_->SetAnimationIndex(6);

	beyblade_ = std::make_unique<Object>();
	beyblade_->Initialize(ModelManager::GetInstance()->GetModel("resources/Title/startAnim", "beyblade.obj"));
	beyblade_->SetDirectionalLight(directionalLight);
	beyblade_->SetShininess(0);
	beyblade_->SetTransform({ {1,1,1},{},Vector3{ 0,defaultY_,0 } });

	// 初期配置
	auto parts = beyblade_->GetParts();
	for (int i = 0; i < 8; i++) {
		parts[i].transform->translate = partsTranslate[i];
		beyblade_->SetParts(parts[i], i);
	}

	isTitle_ = true;
	phase_ = Phase::Black;

	black_ = std::make_unique<Sprite>();
	black_->Initialize("resources/Effect/Combine/hole.png");
	black_->SetSize({});
	black_->SetAnchorPoint({ 0.5f, 0.5f });
	black_->SetPosition({ 640,360 });
}

void Combine::InitializeGame(std::shared_ptr<DirectionalLight> directionalLight, std::shared_ptr<Camera> camera) {
	camera_ = camera;

	human_ = std::make_unique<Object>();
	human_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player", "Player.gltf"));
	human_->SetDirectionalLight(directionalLight);
	human_->SetShininess(0);
	SRT transform = human_->GetTransform();
	transform.scale = { 1.0f / 0.13f,1.0f / 0.13f ,1.0f / 0.13f };
	Quaternion rot = MakeRotateAxisAngleQuaternion({ 0,1,0 }, std::numbers::pi_v<float>) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_);
	transform.rotate = rot;
	transform.translate = { 0,0, backAmount_ };
	human_->SetTransform(transform);
	human_->SetIsUseAnimation(true);
	human_->SetAnimationIndex(9);
	human_->Update();

	Vector3 translate = transform.translate;
	beyblade_ = std::make_unique<Object>();
	beyblade_->Initialize(ModelManager::GetInstance()->GetModel("resources/Title/startAnim", "beyblade.obj"));
	beyblade_->SetDirectionalLight(directionalLight);
	beyblade_->SetShininess(0);
	transform = beyblade_->GetTransform();
	transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_);
	transform.translate = translate;
	beyblade_->SetTransform(transform);

	isTitle_ = false;

	phase_ = Phase::Wait;

	black_ = std::make_unique<Sprite>();
	black_->Initialize("resources/Effect/Combine/hole.png");
	black_->SetAnchorPoint({ 0.5f, 0.5f });

	SRT uv = black_->GetUVTransform();
	uv.scale = { 0.1f, 0.1f, 1.0f };
	black_->SetUVTransform(uv);
}

void Combine::Update() {

	switch (phase_) {
	case Phase::Black:
	{
		float t = min(timer_, kDarkTime) / kDarkTime;
		if (t <= kDarkTime) {
			black_->SetSize(Vector2{ 1280,720 } *max((1 - t), 0.03f) * 40);
			Vector2 screenSize = { float(GameEngine::GetWindowWidth()), float(GameEngine::GetWindowHeight()) };
			black_->SetPosition(screenSize * 0.5f);

			SRT uv = black_->GetUVTransform();
			float uvScale = 1.0f - 0.85f * (1 - t);
			if (1 - t <= 0) {
				black_->SetAnchorPoint({});
				black_->SetSize(Vector2{ 1280,720 } *4);
				black_->SetPosition({});
			}
			uv.scale = { uvScale, uvScale, 1.0f };
			uv.translate = { 0.5f - 0.5f * uvScale, 0.5f - 0.5f * uvScale, 0.0f };
			black_->SetUVTransform(uv);
		}

		if (timer_ / (kDarkTime + kDarkWaitTime) >= 1.0f) {
			timer_ = 0;
			phase_ = Phase::Fall;
		}
		break;
	}
	case Phase::Fall:
	{
		rotate_ += 0.1f;

		// バラバラのパーツが降下
		float t = min(timer_ / kFallTime, 1.0f);
		auto transform = beyblade_->GetTransform();
		transform.translate = Lerp(Vector3{ 0,defaultY_,0 }, Vector3{ 0,0,0 }, t);
		transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_);
		beyblade_->SetTransform(transform);

		if (t == 1.0f) {
			timer_ = 0;
			phase_ = Phase::Set;
		}
	}

	break;
	case Phase::Set:
	{
		rotate_ += 0.15f;

		// パーツごとに合体
		float deltaTime = GameEngine::GetDeltaTime();
		auto parts = beyblade_->GetParts();
		for (int i = 0; i < 8; i++) {
			// パーツごとのタイマー
			setCountdown_[i] -= deltaTime;

			if (setCountdown_[i] <= 0.2f) {
				float t = 1.0f - max(setCountdown_[i] / 0.2f, 0.0f);
				t = EaseOutCubic(t);
				parts[i].transform->translate = Lerp(partsTranslate[i], Vector3{}, t);
				beyblade_->SetParts(parts[i], i);
			}
		}

		auto transform = beyblade_->GetTransform();
		transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_);
		beyblade_->SetTransform(transform);

		if (timer_ / kSetTime >= 1.0f) {
			timer_ = 0;
			phase_ = Phase::Ride;
		}
	}
	break;

	case Phase::Ride:
	{
		rotate_ += 0.1f;

		// 人と最終パーツ落下
		float deltaTime = GameEngine::GetDeltaTime();
		float t = min(timer_, kRideTime) / kRideTime;
		auto parts = beyblade_->GetParts();
		parts[4].transform->translate = Lerp(partsTranslate[4], Vector3{}, t);
		beyblade_->SetParts(parts[4], 4);

		auto transformHuman = human_->GetTransform();
		transformHuman.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, std::numbers::pi_v<float>) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_);
		transformHuman.translate = parts[4].transform->translate;
		human_->SetTransform(transformHuman);
		human_->Update();

		auto transform = beyblade_->GetTransform();
		transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_);
		beyblade_->SetTransform(transform);

		if (timer_ / kRideTime >= 1.0f) {
			timer_ = 0;
			phase_ = Phase::Back;
		}
		break;
	}
	case Phase::Back:
	{
		rotate_ += 0.05f;

		float t = min(timer_, kBackTime) / kBackTime;
		auto transformHuman = human_->GetTransform();
		transformHuman.translate = Vector3{ 0,0,1 } *(t * backAmount_);
		human_->SetTransform(transformHuman);
		human_->Update();

		auto transform = beyblade_->GetTransform();
		transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_);
		transform.translate = transformHuman.translate;
		beyblade_->SetTransform(transform);

		if (timer_ / kBackTime >= 1.0f) {
			isEnd_ = true;
		}
		break;
	}
	case Phase::Wait:
	{
		Matrix4x4 view = camera_->GetViewMatrix();
		Matrix4x4 cameraWorld = Inverse(view);

		Vector3 cameraPos = { cameraWorld.m[3][0], cameraWorld.m[3][1], cameraWorld.m[3][2] };
		Vector3 cameraForward = { cameraWorld.m[2][0], cameraWorld.m[2][1], cameraWorld.m[2][2] };
		cameraForward = Normalize(cameraForward);
		Vector3 startPos = cameraPos + cameraForward * backAmount_;

		// キャラをカメラ方向に向ける
		Quaternion lookRot = MakeLookRotation(cameraPos - startPos, Vector3{ 0,1,0 });

		SRT h = human_->GetTransform();
		h.translate = startPos;
		h.rotate = lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_);
		human_->SetTransform(h);
		human_->Update();

		auto transform = beyblade_->GetTransform();
		transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * (lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_));
		transform.translate = startPos;
		beyblade_->SetTransform(transform);
	}

	if (timer_ / kBackTime >= 1.0f) {
		timer_ = 0;
		phase_ = Phase::GameStart;
	}
	break;
	case Phase::GameStart:
	{
		// カメラのワールド行列を取得（ビュー行列の逆）
		Matrix4x4 view = camera_->GetViewMatrix();
		Matrix4x4 cameraWorld = Inverse(view);

		// カメラ位置と前方ベクトル（ワールド空間）
		Vector3 cameraPos = { cameraWorld.m[3][0], cameraWorld.m[3][1], cameraWorld.m[3][2] };
		Vector3 cameraForward = { cameraWorld.m[2][0], cameraWorld.m[2][1], cameraWorld.m[2][2] };
		cameraForward = Normalize(cameraForward);

		rotate_ += 0.15f;

		float t = min(timer_, kStartTime) / kStartTime;
		if (timer_ > kStartIrisIn) {
			SRT uv = black_->GetUVTransform();
			uv.scale = { 1, 1, 1 };
			black_->SetUVTransform(uv);

			float inT = min(timer_ - kStartIrisIn, kStartTime - kStartIrisIn) / (kStartTime - kStartIrisIn);
			black_->SetSize(Vector2{ 1280,720 } *inT * 100);
			Vector2 screenSize = { float(GameEngine::GetWindowWidth()), float(GameEngine::GetWindowHeight()) };
			black_->SetPosition(screenSize * 0.5f);
		}

		// カメラの相対位置
		float ease = EaseOutCubic(t);
		Vector3 startPos = cameraPos + cameraForward * backAmount_;
		Vector3 endPos = cameraPos + cameraForward * -5.0f;
		Vector3 worldPos = Lerp(startPos, endPos, ease);

		// キャラをカメラ方向に向ける
		Quaternion lookRot = MakeLookRotation(cameraPos - worldPos, Vector3{ 0,1,0 });

		SRT h = human_->GetTransform();
		h.translate = worldPos;
		h.rotate = lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_);
		human_->SetTransform(h);
		human_->Update();

		auto transform = beyblade_->GetTransform();
		transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * (lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_));
		transform.translate = worldPos;
		beyblade_->SetTransform(transform);

		if (timer_ / kStartTime >= 1.0f) {
			isEnd_ = true;
		}
		break;
	}
	}

	timer_ += GameEngine::GetDeltaTime();
}

void Combine::Draw() {
	black_->Update();
	black_->Draw2D();

	if (phase_ > Phase::Black)
		human_->Draw3D();
	beyblade_->Draw3D();
}