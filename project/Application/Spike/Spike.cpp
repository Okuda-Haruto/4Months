#include "Spike.h"

void Spike::Initialize(const Vector3& spawnPos, const float radius) {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/Spike", "Spike.obj"));
	model_->SetShininess(40.0f);

	transform_ = { {radius,radius,radius},{},spawnPos };
	model_->SetTransform(transform_);

	// 衝突判定
	collider_.center = transform_.translate;
	collider_.radius = radius;
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