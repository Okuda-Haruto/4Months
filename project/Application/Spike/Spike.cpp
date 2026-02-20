#include "Spike.h"

void Spike::Initialize() {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/sphere", "sphere.obj"));
	model_->SetShininess(30.0f);
	Vector3 spawnPos = {
		spawnCenter_.x + GameEngine::randomFloat(-spawnRadius_, spawnRadius_),
		spawnCenter_.y + GameEngine::randomFloat(-spawnHeight_ / 2, spawnHeight_ / 2),
		spawnCenter_.z + GameEngine::randomFloat(-spawnRadius_, spawnRadius_),
	};
	transform_ = { {1,1,1},{},spawnPos };
	model_->SetTransform(transform_);

	// 衝突判定
	collider_.center = transform_.translate;
	collider_.radius = 0.5f;

	transform_.scale.x = collider_.radius * 2;
}

void Spike::Update() {
	model_->SetTransform(transform_);
	collider_.center = transform_.translate;
}

void Spike::Draw(const std::shared_ptr<DirectionalLight> directionalLight) {
	model_->SetColor({ 1, 0, 0, 1 });

	model_->SetDirectionalLight(directionalLight);
	model_->Draw3D();
}