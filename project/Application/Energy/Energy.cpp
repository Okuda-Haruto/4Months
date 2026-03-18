#include "Energy.h"

void Energy::Initialize(const Vector3& spawnPos, const float radius) {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/Spike", "Spike.obj"));
	model_->SetShininess(40.0f);

	transform_ = { {radius,radius,radius},{},spawnPos };
	model_->SetTransform(transform_);

	// 衝突判定
	collider_.center = transform_.translate;
	collider_.radius = radius;
}

void Energy::Update() {
	model_->SetTransform(transform_);
	collider_.center = transform_.translate;

	model_->SetColor({ 1, 1, 0, 1 });

	// クールダウン
	for (int i = 0; i < kMaxCharacters; ++i) {
		if (characterCoolDown[i] > 0) { characterCoolDown[i]--; }
	}
}

void Energy::Draw(const std::shared_ptr<DirectionalLight> directionalLight) {
	model_->SetDirectionalLight(directionalLight);
	model_->Draw3D();
}

void Energy::OnCollide(const int id) {
	model_->SetColor({ 1, 1, 1, 1 });
	characterCoolDown[id] += boostCoolDown_;
}

bool Energy::IsCoolDown(int id) {
	if (characterCoolDown[id] > 0) {
		return true;
	}
	return false;
}

void Energy::Move(const Vector3& velocity) {
	transform_.translate += velocity;
}