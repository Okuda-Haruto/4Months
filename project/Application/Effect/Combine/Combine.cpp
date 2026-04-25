#include "Combine.h"
#include <numbers>

static float EaseOutCubic(float t) {
	return 1.0f - powf(1.0f - t, 3.0f);
}

void Combine::InitializeTitle(std::shared_ptr<DirectionalLight> directionalLight) {
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
	black_->SetPosition({ 0,0 });
}

void Combine::InitializeGame(std::shared_ptr<DirectionalLight> directionalLight) {
	human_ = std::make_unique<Object>();
	human_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player", "Player.gltf"));
	human_->SetDirectionalLight(directionalLight);
	human_->SetShininess(0);
	SRT transform = human_->GetTransform();
	transform.scale = { 1.0f / 0.13f,1.0f / 0.13f ,1.0f / 0.13f };
	Quaternion rot = MakeRotateAxisAngleQuaternion({ 0,1,0 }, std::numbers::pi_v<float>) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, 1);
	transform.rotate = rot;
	transform.translate = Normalize(forward_) * backAmount_;
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
	transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, 1);
	transform.translate = translate;
	beyblade_->SetTransform(transform);

	isTitle_ = false;

	forward_ = Normalize(TransformNormal({ 0,0,1 }, MakeRotateMatrix(LookAt(translate, { 0, 0, -250 }))));
	phase_ = Phase::GameStart;

	black_ = std::make_unique<Sprite>();
	black_->Initialize("resources/Effect/Combine/hole.png");
	black_->SetSize({ 1280,720 });
	black_->SetAnchorPoint({ 0.5f, 0.5f });
	black_->SetPosition({ 0,0 });
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
		transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, 1);
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
		transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, 1);
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
		transformHuman.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, std::numbers::pi_v<float>) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, 1);
		transformHuman.translate = parts[4].transform->translate;
		human_->SetTransform(transformHuman);
		human_->Update();

		auto transform = beyblade_->GetTransform();
		transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, 1);
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
		transformHuman.translate = TransformNormal(Normalize(forward_), MakeRotateMatrix(transformHuman.rotate)) * (t * backAmount_);
		human_->SetTransform(transformHuman);
		human_->Update();

		auto transform = beyblade_->GetTransform();
		transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, 1);
		transform.translate = transformHuman.translate;
		beyblade_->SetTransform(transform);

		if (timer_ / kBackTime >= 1.0f) {
			isEnd_ = true;
		}
		break;
	}
	case Phase::GameStart:
	{
		rotate_ += 0.15f;

		float t = min(timer_, kStartTime) / kStartTime;
		black_->SetSize(Vector2{ 1280,720 } *t * 30);
		Vector2 screenSize = { float(GameEngine::GetWindowWidth()), float(GameEngine::GetWindowHeight()) };
		black_->SetPosition(screenSize * 0.5f);

		auto transformHuman = human_->GetTransform();
		transformHuman.translate += forward_ * (-backAmount_ / 6.0f);
		transformHuman.translate.y = 0;
		human_->SetTransform(transformHuman);
		human_->Update();

		auto transform = beyblade_->GetTransform();
		transform.rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, 1);
		transform.translate = transformHuman.translate;
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

	if(phase_ > Phase::Black)
	human_->Draw3D();
	beyblade_->Draw3D();
}