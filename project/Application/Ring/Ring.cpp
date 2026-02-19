#include "Ring.h"

void Ring::Initialize() {
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
}

void Ring::Update() {
}

void Ring::Draw(const std::shared_ptr<DirectionalLight> directionalLight) {
	model_->SetColor({ 0, 1, 1, 1 });

	model_->SetDirectionalLight(directionalLight);
	model_->Draw3D();
}