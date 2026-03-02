#include "Spike.h"

void Spike::Initialize(const Vector3& spawnPos) {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/sphere", "sphere.obj"));
	model_->SetShininess(40.0f);

	transform_ = { {1,1,1},{},spawnPos };
	model_->SetTransform(transform_);

	// 衝突判定
	collider_.center = transform_.translate;
	collider_.radius = 0.5f;
}

void Spike::Update() {
	model_->SetTransform(transform_);
	collider_.center = transform_.translate;

	model_->SetColor({ 1, 0, 0, 1 });
}

void Spike::Draw(const std::shared_ptr<DirectionalLight> directionalLight) {
	model_->SetDirectionalLight(directionalLight);
	model_->Draw3D();
}

void Spike::OnCollide() {
	model_->SetColor({ 1, 1, 1, 1 });
}