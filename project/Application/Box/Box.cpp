#include "Box.h"

void Box::Initialize(SRT transform, const float radius, std::shared_ptr<DirectionalLight> directionalLight) {

	object_ = std::make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/Block", "Block.obj"));
	object_->SetDirectionalLight(directionalLight);
	object_->SetShininess(0);

	transform_ = transform;
	object_->SetTransform(transform_);

	// 衝突判定
	collider_.center = transform_.translate;
	collider_.radius = radius;
}

void Box::Update() {
	velocity_ = Lerp(velocity_, Vector3{0,0,0}, 0.1f);

	transform_.translate += velocity_;

	object_->SetTransform(transform_);
	collider_.center = transform_.translate;
}

void Box::Draw() {
	object_->Draw3D();

}

void Box::Move(const Vector3& velocity) {
	velocity_ = Lerp(velocity_,velocity,0.5f);
}