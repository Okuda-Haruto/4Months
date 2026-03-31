#include "Human.h"
#include "Goal/Goal.h"
#include <Lerp.h>
#include <Collision.h>

#include <numbers>

void Human::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight) {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player/Head", "Head.obj"));
	model_->SetShininess(30.0f);
	bulletModel_ = make_unique<Object>();
	bulletModel_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player/Head", "Head.obj"));
	bulletModel_->SetShininess(30.0f);
	//カメラで使う
	transform_ = {};
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	transform_.translate = position;
	model_->SetTransform(transform_);
	model_->SetDirectionalLight(directionalLight);
	bulletModel_->SetTransform(transform_);
	bulletModel_->SetDirectionalLight(directionalLight);

	// エフェクト
	wind_ = make_unique<Wind>();
	wind_->Initialize(directionalLight);

	fallingSpeed_ = -kMinSpeed_;
	speed_ = 0.3f;
}

void Human::Update() {
	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);

	if (cameraEffectTime_ > 0.0f) {
		cameraEffectTime_ -= 1.0f / 60.0f;
		if (cameraEffectTime_ < 0.0f) {
			cameraEffectTime_ = 0.0f;
		}
	}

	//向いている向きに速度を向ける
	velocity_.translate = Vector3{ 0,0,1 } *rotateMatrix * speed_;

	fallingSpeed_ = max(fallingSpeed_ - kGravity_, -maxFallingSpeed_);
	velocity_.translate += Vector3{ 0,fallingSpeed_,0 };

	transform_.translate += velocity_.translate;

	// 分離しているときの先頭
	switch (vacuumState_) {
	case None:
		headTransform_ = transform_;
		break;

	case Going:
		headPrevTransform_ = headTransform_;
		headTransform_.translate += headDir_ * headSpeed_;
		headSpeed_ -= headDeceleration_;
		if (headSpeed_ <= 0) {
			headSpeed_ = 0;
			vacuumTimer_ = vacuumTime_;
			wind_->Set(headTransform_.translate, vacuumRadius_,float(vacuumTime_));
			vacuumState_ = Vacuum;
		}
		break;

	case Vacuum:
		vacuumTimer_--;

		if (vacuumTimer_ <= 0) {
			vacuumState_ = Return;
			returnTimer_ = returnTime_;
			vacuumStartPos_ = headTransform_.translate;
		}
		break;

	case Return:
		headPrevTransform_ = headTransform_;
		headTransform_.translate = Lerp(vacuumStartPos_, transform_.translate, (1.0f - float(returnTimer_) / float(returnTime_)));
		headSpeed_ += headDeceleration_;

		returnTimer_--;
		if (returnTimer_ <= 0) {
			headSpeed_ = 0;
			vacuumState_ = None;
		}
	}

	speed_ = Lerp(speed_, kDefaultSpeed_, 0.05f);
	


	if (knockbackTimer_) {
		knockbackTimer_--;
		transform_.translate += Vector3{ 0,0,1 } *rotateMatrix * 0.2f;
	}

#ifdef USE_IMGUI

#endif

	model_->SetTransform(transform_);
	bulletModel_->SetTransform(headTransform_);

	wind_->Update();
}

void Human::Draw() {
	model_->Draw3D();
	if (vacuumState_ != None) {
		bulletModel_->Draw3D();
	}
	if (vacuumState_ == Vacuum) {
		wind_->Draw();
	}
}

void Human::OnHitVoxel(Vector3 translate) {
	Slowdown();

	transform_.rotate = LookAt(translate, transform_.translate);
	fallingSpeed_ = 1.0f;
	speed_ = 1.0f;
}

void Human::Throw() {
	headTransform_ = transform_;
	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);
	headDir_ = Vector3{ 0,0,1 } *rotateMatrix;
	headStartSpeed_ = 2.5f + 1.5f * (charge_ / kMaxCharge_);
	headSpeed_ = headStartSpeed_;
	vacuumState_ = Going;
	fallingSpeed_ += bounceBackSpeed_ * min(0.25f + charge_ / kMaxCharge_, 1.0f);
	vacuumRadius_ = baseVacuumRadius_ + charge_;
	vacuumTime_ = int((charge_ / kMaxCharge_) * (kMaxVacuumTime - kMinVacuumTime) + kMinVacuumTime);
	returnTime_ = int((charge_ / kMaxCharge_) * (kMaxReturnTime - kMinReturnTime) + kMinReturnTime);
	charge_ = 0;
}

void Human::Charge() {
	charge_ += kChargeSpeed_;
	charge_ = min(charge_, kMaxCharge_);
	vacuumRadius_ = baseVacuumRadius_ + charge_;

	vacuumStartPos_ = CalcVacuumPosition();
	isCharging_ = true;
}

void Human::Slowdown() {
	speed_ -= 0.3f;
	speed_ = max(0, speed_);
}

Vector3 Human::CalcVacuumPosition() {
	// 初速
	float v0 = headStartSpeed_;

	// 減速
	float a = headDeceleration_;

	// 移動距離
	float distance = (v0 * v0) / (2.0f * a);

	// 向き（Throwと同じ）
	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);
	Vector3 dir = Vector3{ 0,0,1 } *rotateMatrix;

	// 最終位置
	return transform_.translate + dir * distance;
}