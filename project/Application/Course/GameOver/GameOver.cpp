#include "GameOver.h"
#include <ModelManager/ModelManager.h>
#include <Operation/Operation.h>

void GameOver::Initialize(std::shared_ptr<DirectionalLight> directionalLight) {

	camera_ = std::make_shared<Camera>();
	camera_->Initialize(GameEngine::GetDirectXCommon());

	flowerGarden_Sprite_ = std::make_unique<Sprite>();
	flowerGarden_Sprite_->Initialize("resources/GameOver/GameOver_BackGround.png");
	flowerGarden_Sprite_->SetPosition(Vector2{ 640,360 });
	flowerGarden_Sprite_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
	flowerGarden_Sprite_->SetSize(Vector2{ 1280,720 });

	videoDistorted_Sprite_ = std::make_unique<Sprite>();
	videoDistorted_Sprite_->Initialize("resources/GameOver/VideoDistorted.png");
	videoDistorted_Sprite_->SetSize(Vector2{ 330,70 });
	videoDistorted_Sprite_->SetPosition(Vector2{ 640,70 });
	videoDistorted_Sprite_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
	videoDistorted_Sprite_->SetColor(Vector4{ 1,1,1,0 });

	normaOver_Sprite_ = std::make_unique<Sprite>();
	normaOver_Sprite_->Initialize("resources/GameOver/NormaOver.png");
	normaOver_Sprite_->SetSize(Vector2{ 660,140 });
	normaOver_Sprite_->SetPosition(Vector2{ 640,200 });
	normaOver_Sprite_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
	normaOver_Sprite_->SetColor(Vector4{ 1,1,1,0 });

	pressToNext_Sprite_ = std::make_unique<Sprite>();
	pressToNext_Sprite_->Initialize("resources/GameOver/Press_to_Next.png");
	pressToNext_Sprite_->SetPosition(Vector2{ 640,560 });
	pressToNext_Sprite_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
	pressToNext_Sprite_->SetColor(Vector4{ 1,1,1,0 });

	for (int i = 0; i < 4; i++) {
		butterfly_[i] = std::make_unique<Object>();
		butterfly_[i]->Initialize(ModelManager::GetInstance()->GetModel("resources/GameOver/", "butterfly.gltf"));
		butterfly_[i]->SetCamera(camera_);
		butterfly_[i]->SetDirectionalLight(directionalLight);
		butterfly_[i]->SetIsUseAnimation(true);
		butterfly_[i]->SetIsLoopAnimation(true);
	}

	SRT transform;
	transform.scale = { 1,1,1 };
	transform.rotate = MakeRotateAxisAngleQuaternion(RotateVector(Vector3{ 0,0,1 }, transform.rotate), 30.0f * std::numbers::pi_v<float> / 180);
	transform.rotate = IdentityQuaternion();
	transform.translate = { 22,2,70 };
	butterfly_[0]->SetTransform(transform);
	butterfly_[0]->SetAnimationIndex(0);
	transform.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, 7 * std::numbers::pi_v<float> / 180);
	transform.rotate = transform.rotate * MakeRotateAxisAngleQuaternion(RotateVector(Vector3{ 0,1,0 }, transform.rotate), 158 * std::numbers::pi_v<float> / 180);
	transform.rotate = transform.rotate * MakeRotateAxisAngleQuaternion(RotateVector(Vector3{ 0,0,1 }, transform.rotate), 38 * std::numbers::pi_v<float> / 180);
	transform.translate = { -12,0,24 };
	butterfly_[1]->SetTransform(transform);
	butterfly_[1]->SetAnimationIndex(0);
	butterfly_[1]->SetAnimationTime(0.2f);
	transform.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -16 * std::numbers::pi_v<float> / 180);
	transform.rotate = transform.rotate * MakeRotateAxisAngleQuaternion(RotateVector(Vector3{ 0,1,0 }, transform.rotate), -50 * std::numbers::pi_v<float> / 180);
	transform.rotate = transform.rotate * MakeRotateAxisAngleQuaternion(RotateVector(Vector3{ 0,0,1 }, transform.rotate), 22 * std::numbers::pi_v<float> / 180);
	transform.translate = { 11,-8,54 };
	butterfly_[2]->SetTransform(transform);
	butterfly_[2]->SetAnimationIndex(1);
	transform.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, 30 * std::numbers::pi_v<float> / 180);
	transform.rotate = transform.rotate * MakeRotateAxisAngleQuaternion(RotateVector(Vector3{ 0,0,1 }, transform.rotate), 45 * std::numbers::pi_v<float> / 180);
	transform.translate = { -15,14,100 };
	butterfly_[3]->SetTransform(transform);
	butterfly_[3]->SetAnimationIndex(0);
	butterfly_[3]->SetAnimationTime(0.15f);

	eventTime_ = 0.0f;
	plessToNextColor_ = 0.0f;
}

void GameOver::Update() {

	if (eventTime_ < kMaxEventTime_) {
		eventTime_ += GameEngine::GetDeltaTime();
		if (eventTime_ >= kMaxEventTime_) {
			eventTime_ = kMaxEventTime_;
		}
	}

	if (eventTime_ < 2.0f) {
		videoDistorted_Sprite_->SetColor(Vector4{ 1,1,1,sinf(std::numbers::pi_v<float> *12.5f * (1.0f - powf(1.0f - eventTime_ / (kMaxEventTime_ - 2.0f),2.0f))) });
		normaOver_Sprite_->SetColor(Vector4{ 1,1,1,0 });
	}
	else {
		videoDistorted_Sprite_->SetColor(Vector4{ 1,1,1,1 });
		normaOver_Sprite_->SetColor(Vector4{ 1,1,1,sinf(std::numbers::pi_v<float> *12.5f * (1.0f - powf(1.0f - (eventTime_ - 2.0f) / (kMaxEventTime_ - 2.0f),2.0f))) });
	}

	if (eventTime_ >= kMaxEventTime_) {
		plessToNextColor_ += GameEngine::GetDeltaTime();
		if (plessToNextColor_ > 1.0f) {
			plessToNextColor_ = 1.0f;
		}
	}
	pressToNext_Sprite_->SetColor({ 1,1,1,plessToNextColor_ });

	flowerGarden_Sprite_->Update();
	videoDistorted_Sprite_->Update();
	pressToNext_Sprite_->Update();
	normaOver_Sprite_->Update();

	for (int i = 0; i < 4; i++) {
		butterfly_[i]->Update();
	}

}

void GameOver::Draw() {
	flowerGarden_Sprite_->Draw2D();

	for (int i = 0; i < 4; i++) {
		butterfly_[i]->Draw3D();
	}

	videoDistorted_Sprite_->Draw2D();

	pressToNext_Sprite_->Draw2D();

	normaOver_Sprite_->Draw2D();
}