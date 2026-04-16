#include "Human.h"
#include "Goal/Goal.h"
#include <Lerp.h>
#include <Collision.h>

#include <numbers>

void Human::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight, const std::shared_ptr<Camera> camera) {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player/Head", "Head.obj"));
	model_->SetShininess(30.0f);
	bulletModel_ = make_unique<Object>();
	bulletModel_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player/Head", "beyblade.obj"));
	bulletModel_->SetShininess(30.0f);
	//カメラで使う
	transform_ = {};
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	transform_.translate = position;
	model_->SetTransform(transform_);
	model_->SetDirectionalLight(directionalLight);
	headTransform_.scale = { 2.5f,2.5f,2.5f };
	headTransform_.translate = position;
	bulletModel_->SetTransform(headTransform_);
	bulletModel_->SetDirectionalLight(directionalLight);

	// エフェクト
	wind_ = make_unique<Wind>();
	wind_->Initialize(directionalLight);
	headRotateEffect_ = make_unique<PlayerRotation>();
	headRotateEffect_->Initialize(directionalLight,camera);

	fallingSpeed_ = -kMinSpeed_ * 0.3f;
	speed_ = 0.3f;
	knockBackAcceleration_ = {};
	knockBackVelocity_ = {};
	maxFallingSpeed_ = 3.75f;
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
	velocity_.translate += Vector3{ 0,0,1 } *rotateMatrix * speed_;

	// ★落下速度 1.5倍
	fallingSpeed_ = max(fallingSpeed_ - kGravity_ * 0.3375f, -maxFallingSpeed_);
	velocity_.translate += Vector3{ 0,fallingSpeed_,0 };

	//NANチェック
	float len = Length(knockBackAcceleration_);

	if (len > 1e-6f && std::isfinite(len)) {
		knockBackVelocity_ = Normalize(knockBackAcceleration_) * kNockBackSpeed_;
		// ★ジャンプ上限抑制
		knockBackVelocity_.y *= (kNockBackFallingSpeed_ / kNockBackSpeed_) * 0.7f;
	}

	knockBackAcceleration_ = {};

	if (!stop) {
		transform_.translate += velocity_.translate + knockBackVelocity_;
	}

	// 分離しているときの先頭
	switch (vacuumState_) {
	case None:
		headTransform_ = { headTransform_.scale,transform_.rotate, transform_.translate };
		break;

	case Going:
		headPrevTransform_ = headTransform_;
		headTransform_.translate += headDir_ * headSpeed_;
		headSpeed_ -= headDeceleration_;
		if (headSpeed_ <= 0) {
			headSpeed_ = 0;
			vacuumTimer_ = vacuumTime_;
			wind_->Set(headTransform_.translate, vacuumRadius_, float(vacuumTime_));
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
			headRotateEffect_->Catch();
		}
	}

	// 回転
	if (charge_ == kMaxCharge_) {
		headRotate_ += 0.2f;
	}
	else if (isCharging_ || vacuumState_ != None) {
		headRotate_ += 0.1f;
	}
	else {
		headRotate_ += 0.05f;
	}
	headTransform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, headRotate_);
	headRotateEffect_->Update(headTransform_.translate, headTransform_.scale.x, headRotate_, isCharging_, charge_ == kMaxCharge_);

	speed_ = Lerp(speed_, kDefaultSpeed_, 0.05f);

	knockBackVelocity_.x = Lerp(knockBackVelocity_.x, 0.0f, 0.1f);
	knockBackVelocity_.y = Lerp(knockBackVelocity_.y, 0.0f, 0.1f);
	knockBackVelocity_.z = Lerp(knockBackVelocity_.z, 0.0f, 0.1f);

	velocity_ = {};

#ifdef USE_IMGUI

#endif

	model_->SetTransform(transform_);
	bulletModel_->SetTransform(headTransform_);

	wind_->Update();
}

void Human::Draw() {
	model_->Draw3D();
	bulletModel_->Draw3D();
	headRotateEffect_->Draw();

	if (vacuumState_ == Vacuum) {
		wind_->Draw();
	}
}

void Human::OnHitVoxel(AABB aabb) {
	Slowdown();

	Vector3 closest;

	closest.x = std::clamp(transform_.translate.x, aabb.min.x, aabb.max.x) - transform_.translate.x;
	closest.y = std::clamp(transform_.translate.y, aabb.min.y, aabb.max.y) - transform_.translate.y;
	closest.z = std::clamp(transform_.translate.z, aabb.min.z, aabb.max.z) - transform_.translate.z;

	if (fabsf(closest.x) >= fabsf(closest.y) && fabsf(closest.x) >= fabsf(closest.z)) {
		knockBackAcceleration_.x -= fabsf(closest.x) / closest.x;
	}
	else if (fabsf(closest.y) >= fabsf(closest.x) && fabsf(closest.y) >= fabsf(closest.z)) {
		knockBackAcceleration_.y -= fabsf(closest.y) / closest.y;
	}
	else if (fabsf(closest.z) >= fabsf(closest.x) && fabsf(closest.z) >= fabsf(closest.y)) {
		knockBackAcceleration_.z -= fabsf(closest.z) / closest.z;
	}

	// ★ジャンプ上限さらに抑制
	fallingSpeed_ = 5.25f * 0.7f;
	speed_ = 1.0f;
}

void Human::Throw() {
	headTransform_ = { headTransform_.scale,transform_.rotate, transform_.translate };
	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);
	headDir_ = Vector3{ 0,0,1 } *rotateMatrix;
	headSpeed_ = headStartSpeed_;
	vacuumState_ = Going;

	// ★ジャンプ上限さらに抑制
	fallingSpeed_ = max(fallingSpeed_, bounceBackSpeed_ * min(0.225f + charge_ / kMaxCharge_ * 0.5f, 0.575f) * 0.7f);

	vacuumRadius_ = baseVacuumRadius_ + charge_;
	vacuumTime_ = int((charge_ / kMaxCharge_) * (kMaxVacuumTime - kMinVacuumTime) + kMinVacuumTime);
	returnTime_ = int((charge_ / kMaxCharge_) * (kMaxReturnTime - kMinReturnTime) + kMinReturnTime);
	charge_ = 0;
	headRotateEffect_->Shoot();
}

void Human::Charge() {
	charge_ += kChargeSpeed_;
	charge_ = min(charge_, kMaxCharge_);
	vacuumRadius_ = baseVacuumRadius_ + charge_;
	headStartSpeed_ = 2.5f + 1.5f * (charge_ / kMaxCharge_);

	vacuumStartPos_ = CalcVacuumPosition();
	isCharging_ = true;
}

void Human::Slowdown() {
	speed_ -= 0.3f;
	speed_ = max(0, speed_);
}

Vector3 Human::CalcVacuumPosition() {
	float v0 = headStartSpeed_;
	float a = headDeceleration_;
	float distance = (v0 * v0) / (2.0f * a);

	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);
	Vector3 dir = Vector3{ 0,0,1 } *rotateMatrix;

	return transform_.translate + dir * distance;
}