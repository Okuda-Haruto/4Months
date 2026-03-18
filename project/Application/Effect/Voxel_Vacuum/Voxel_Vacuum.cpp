#include "Voxel_Vacuum.h"
#include "Human/Player/Player.h"

void Voxel_Vacuum::Initialize(SRT transform, Player* player, std::shared_ptr<DirectionalLight> directionalLight) {
	player_ = player;

	object_ = std::make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/Block","Block.obj"));
	object_->SetDirectionalLight(directionalLight);
	object_->SetShininess(0);

	transform_ = transform;
	object_->SetTransform(transform_);

	animationTime_ = 0.0f;
	isDead_ = false;
}

void Voxel_Vacuum::Update() {
	if (animationTime_ < kMaxAnimationTime_) {
		animationTime_ += 1.0f / 60.0f;
	}


	if (animationTime_ >= kMaxAnimationTime_) {
		isDead_ = true;
	} else {

		transform_.translate = Lerp(transform_.translate, player_->GetVacuumSphere().center, 0.1f);
		transform_.scale = Lerp(transform_.scale, Vector3{0,0,0}, 0.1f);

		object_->SetTransform(transform_);
	}
}

void Voxel_Vacuum::Draw() {
	object_->Draw3D();

}