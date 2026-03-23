#include "Box.h"
#include "Course/Course.h"

void Box::Initialize(Course* course, SRT transform, Vector3 velocity, const float radius, const int32_t maxHP, std::shared_ptr<DirectionalLight> directionalLight) {

	course_ = course;
	MaxHP_ = maxHP;
	HP_ = MaxHP_;
	velocity_ = velocity;

	object_ = std::make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/Block", "Block.obj"));
	object_->SetDirectionalLight(directionalLight);
	object_->SetShininess(0);

	transform_ = transform;
	object_->SetTransform(transform_);

	// 衝突判定
	collider_.center = transform_.translate;
	collider_.radius = radius;

	deathTimer_ = 2.0f;
	isDead_ = false;
}

void Box::Update() {

	if (HP_ <= 0) {
		if (collider_.radius >= 0.1f) {
			for (int i = 0; i < 2; i++) {
				SRT newTransform = transform_;
				newTransform.scale /= 2;
				newTransform.translate += Vector3{
					GameEngine::randomFloat(-newTransform.scale.x * 2,newTransform.scale.x * 2),
					GameEngine::randomFloat(-newTransform.scale.y * 2,newTransform.scale.y * 2),
					GameEngine::randomFloat(-newTransform.scale.z * 2,newTransform.scale.z * 2)
				};
				course_->AddSplitBox(newTransform, velocity_ + Normalize(newTransform.translate - transform_.translate) * 2, collider_.radius / 2, MaxHP_);
			}
		}
		isDead_ = true;
	}

	deathTimer_ -= 1.0f / 60.0f;

	if (deathTimer_ < 0.0f) {
		isDead_ = true;
	}

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