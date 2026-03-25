#include "Wind.h"
#include "Mix.h"

void Wind::Initialize(const Vector3& center, const float radius, std::shared_ptr<DirectionalLight> directionalLight) {
	model_ = std::make_unique<Object>();
	//model_->Initialize(ModelManager::GetInstance()->GetModel());
	model_->SetDirectionalLight(directionalLight);
	model_->SetShininess(0);

	center_ = center;
	radius_ = radius;
	Vector3 transform = { center.x + GameEngine::randomFloat(0,radius),center.y, center.z + GameEngine::randomFloat(0, radius) };
	transform_ = { {1,1,1},{}, transform};
	model_->SetTransform(transform_);
}

void Wind::Update() {
	if (animationTime_ < kMaxAnimationTime_) {
		animationTime_ += 1.0f / 60.0f;
	}

	if (animationTime_ >= kMaxAnimationTime_) {
		isDead_ = true;
	} else {
		Vector3 vec = Vector3{ 0,0,0 };
		transform_.translate = Mix(transform_.translate, vec, center_, { 0,1,0 }, radius_, false);

		model_->SetTransform(transform_);
	}
}

void Wind::Draw() {
	model_->Draw3D();
}
