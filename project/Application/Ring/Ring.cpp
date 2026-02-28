#include "Ring.h"

void Ring::Initialize() {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/sphere", "sphere.obj"));
	model_->SetShininess(40.0f);
	Vector3 spawnPos = {
		spawnCenter_.x + GameEngine::randomFloat(-spawnRadius_, spawnRadius_),
		spawnCenter_.y + GameEngine::randomFloat(-spawnHeight_, -50),
		spawnCenter_.z + GameEngine::randomFloat(-spawnRadius_, spawnRadius_),
	};
	transform_ = { {1,1,1},{},spawnPos };
	model_->SetTransform(transform_);

	// 衝突判定
	colliderCenter_ = transform_.translate;
	colliderRadius_ = GameEngine::randomFloat(radiusMin_, radiusMax_);

	transform_.scale = { colliderRadius_,colliderHeight_,colliderRadius_ };
}

void Ring::Update() {
	model_->SetTransform(transform_);
	colliderCenter_ = transform_.translate;

	model_->SetColor({ 0, 1, 1, 1 });
}

void Ring::Draw(const std::shared_ptr<DirectionalLight> directionalLight) {
	model_->SetDirectionalLight(directionalLight);
	model_->Draw3D();
}

void Ring::OnCollide() {
	model_->SetColor({ 1, 1, 1, 1 });
}