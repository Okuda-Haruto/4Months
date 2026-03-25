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
	rollRotate_ = IdentityQuaternion();
	model_->SetTransform(transform_);
	model_->SetDirectionalLight(directionalLight);
	bulletModel_->SetTransform(transform_);
	bulletModel_->SetDirectionalLight(directionalLight);

	characterID_ = id_++;

	fallingSpeed_ = -kMinSpeed_;
	speed_ = 0.3f;

	isDrifting_ = false;
	isCoilAround_ = false;
	coilAroundDistance_ = 0.0f;

	noTargetMinNumber_ = 0;

	coilAroundStartTime_ = 0;
	coilAroundEndTime_ = 0;
	coilAroundRotatePos_ = {};
}

void Human::Update() {
	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);

	if (unableDriftTimer_ > 0) { unableDriftTimer_--; }

	if (cameraEffectTime_ > 0.0f) {
		cameraEffectTime_ -= 1.0f / 60.0f;
		if (cameraEffectTime_ < 0.0f) {
			cameraEffectTime_ = 0.0f;
		}
	}

	//向いている向きに速度を向ける
	velocity_.translate = Vector3{ 0,0,1 } *rotateMatrix * speed_;

	//とぐろ中(巻き付いていない)
	if (isDrifting_ && !isCoilAround_) {
		//落下速度を遅くする
		fallingSpeed_ = Lerp<float>(fallingSpeed_, kMinSpeed_, 0.1f);
		if (!isTurnBack_) {
			velocity_.translate += Vector3{ 0,-fallingSpeed_,0 };
		} else {
			velocity_.translate += Vector3{ 0,fallingSpeed_,0 };
		}
	}

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

	// 速度が一定以下なら戻す
	if (speed_ < kDefaultSpeed_) {
		speed_ += 0.0025f;
		speed_ = min(speed_, kDefaultSpeed_);
	}

	// 無敵タイマー
	if (invincibleTimer_) {
		invincibleTimer_--;
	}
	if (unableDriftTimer_) {
		unableDriftTimer_--;
	}
	if (knockbackTimer_) {
		knockbackTimer_--;
		transform_.translate += Vector3{ 0,0,1 } *rotateMatrix * 0.2f;
	}

#ifdef USE_IMGUI

#endif

	model_->SetTransform(transform_);
	bulletModel_->SetTransform(headTransform_);
}

void Human::Draw() {
	model_->Draw3D();
	if (vacuumState_ != None) {
		bulletModel_->Draw3D();
	}
}

void Human::OnHitVoxel() {
	Slowdown();

	//maxRisingSpeed_ = kDefaultMaxRisingSpeed_;
	//maxFallingSpeed_ = kDefaultMaxFallingSpeed_;
	invincibleTimer_ = invincibleTimeOnHit_;
	unableDriftTimer_ = unableDriftTime_;
}

bool Human::GetIsCoilAround() const {
	return isCoilAround_;
}

void Human::Throw() {
	headTransform_ = transform_;
	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);
	headDir_ = Vector3{ 0,0,1 } *rotateMatrix;
	headSpeed_ = headStartSpeed_;
	vacuumState_ = Going;
	fallingSpeed_ += bounceBackSpeed_;
	vacuumRadius_ = baseVacuumRadius_ + charge_;
	charge_ = 0;
}

void Human::Charge() {
	charge_ += kChargeSpeed_;
	charge_ = min(charge_, kMaxCharge_);
}

void Human::Slowdown() {
	speed_ -= 0.3f;
	speed_ = max(0, speed_);
}
