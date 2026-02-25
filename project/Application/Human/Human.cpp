#include "Human.h"
#include <numbers>

void Human::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight) {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player/Head", "Head.obj"));
	model_->SetShininess(30.0f);
	//カメラで使う
	transform_ = {};
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	transform_.translate = position;
	rollRotate_ = IdentityQuaternion();
	model_->SetTransform(transform_);
	model_->SetDirectionalLight(directionalLight);

	fallingSpeed_ = 0.05f;
	speed_ = 0.2f;

	isDrifting_ = false;
}

void Human::Update() {

	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);

	//向いている向きに速度を向ける
	velocity_.translate = Vector3{ 0,0,1 } * rotateMatrix * speed_;

	if (isDrifting_) {

		fallingSpeed_ *= 0.9f;
		if (fallingSpeed_ < 0.05f) {
			fallingSpeed_ = 0.05f;
		}

		velocity_.translate += Vector3{ 0,-fallingSpeed_,0 };

	} else if (!isTurnBack_) {
		//「下向き速度 * 重力」を落下速度に加える
		fallingSpeed_ = min(fallingSpeed_ + kGravity_, kMaxFallingSpeed_);

		velocity_.translate += Vector3{ 0,-fallingSpeed_,0 };
	} else {
		//上向き速度 * 重力」を落下速度に加える
		fallingSpeed_ = min(fallingSpeed_ + kGravity_, kMaxRisingSpeed_);

		velocity_.translate += Vector3{ 0,fallingSpeed_,0 };
	}
	transform_.translate += velocity_.translate;

#ifdef USE_IMGUI

#endif

	model_->SetTransform(transform_);
}

void Human::Draw() {
	model_->Draw3D();
}