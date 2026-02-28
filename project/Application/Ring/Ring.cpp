#include "Ring.h"

void Ring::Initialize() {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/Ring", "Ring.obj"));
	model_->SetShininess(40.0f);
	Vector3 spawnPos = {
		spawnCenter_.x + GameEngine::randomFloat(-spawnRadius_ / 2, spawnRadius_ / 2),
		spawnCenter_.y + GameEngine::randomFloat(-spawnHeight_ / 2, spawnHeight_ / 2),
		spawnCenter_.z + GameEngine::randomFloat(-spawnRadius_ / 2, spawnRadius_ / 2),
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

	// クールダウン
	for (int i = 0; i < kMaxCharacters; ++i) {
		if (characterCoolDown[i] > 0) { characterCoolDown[i]--; }
	}
}

void Ring::Draw(const std::shared_ptr<DirectionalLight> directionalLight) {
	model_->SetDirectionalLight(directionalLight);
	model_->Draw3D();
}

void Ring::OnCollide(const int id) {
	model_->SetColor({ 1, 1, 1, 1 });
	characterCoolDown[id] += boostCoolDown_;
}

bool Ring::IsCoolDown(int id) {
	if (characterCoolDown[id] > 0) {
		return true;
	}
	return false;
}
