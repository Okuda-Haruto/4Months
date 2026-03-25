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
		}
		break;

	case Return:
		headDir_ = Normalize(transform_.translate - headTransform_.translate);
		headTransform_.translate += headDir_ * headSpeed_;
		headSpeed_ += headDeceleration_;

		if (headTransform_.translate.y > transform_.translate.y) {
			headSpeed_ = 0;
			vacuumState_ = None;
		}

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

void Human::OnHitNeck(const Vector3& pos) {
	// 減速
	if (isTurnBack_) {
		fallingSpeed_ += 0.3f;
	} else {
		fallingSpeed_ -= 0.3f;
	}

	isCoilAround_ = false;
	isDrifting_ = false;
	unableDriftTimer_ = unableDriftTime_;

	Slowdown();

	//maxRisingSpeed_ = kDefaultMaxRisingSpeed_;
	//maxFallingSpeed_ = kDefaultMaxFallingSpeed_;
	invincibleTimer_ = invincibleTimeOnHit_;
	unableDriftTimer_ = unableDriftTime_;
	knockbackTimer_ = kKnockbackTime_;

	// 反射方向の計算
	Vector3 normal = Normalize(transform_.translate - pos);
	Vector3 reflect = velocity_.translate - /*normal*/Vector3{} * 2.0f * Dot(velocity_.translate, /*normal*/Vector3{});
	reflect = Normalize(reflect);
	Vector3 forward = reflect;

	Vector3 up = { 0, 1, 0 };
	if (fabs(Dot(forward, up)) > 0.99f)
		up = { 1, 0, 0 };

	Vector3 right = Normalize(Cross(up, forward));
	up = Cross(forward, right);

	Matrix3x3 rot;
	rot.m[0][0] = right.x;   rot.m[0][1] = right.y;   rot.m[0][2] = right.z;
	rot.m[1][0] = up.x;      rot.m[1][1] = up.y;      rot.m[1][2] = up.z;
	rot.m[2][0] = forward.x; rot.m[2][1] = forward.y; rot.m[2][2] = forward.z;

	Quaternion q;
	float trace = rot.m[0][0] + rot.m[1][1] + rot.m[2][2];
	if (trace > 0.0f) {
		float s = sqrtf(trace + 1.0f) * 2.0f;
		q.w = 0.25f * s;
		q.x = (rot.m[2][1] - rot.m[1][2]) / s;
		q.y = (rot.m[0][2] - rot.m[2][0]) / s;
		q.z = (rot.m[1][0] - rot.m[0][1]) / s;
	} else if (rot.m[0][0] > rot.m[1][1] && rot.m[0][0] > rot.m[2][2]) {
		float s = sqrtf(1.0f + rot.m[0][0] - rot.m[1][1] - rot.m[2][2]) * 2.0f;
		q.w = (rot.m[2][1] - rot.m[1][2]) / s;
		q.x = 0.25f * s;
		q.y = (rot.m[0][1] + rot.m[1][0]) / s;
		q.z = (rot.m[0][2] + rot.m[2][0]) / s;
	} else if (rot.m[1][1] > rot.m[2][2]) {
		float s = sqrtf(1.0f + rot.m[1][1] - rot.m[0][0] - rot.m[2][2]) * 2.0f;
		q.w = (rot.m[0][2] - rot.m[2][0]) / s;
		q.x = (rot.m[0][1] + rot.m[1][0]) / s;
		q.y = 0.25f * s;
		q.z = (rot.m[1][2] + rot.m[2][1]) / s;
	} else {
		float s = sqrtf(1.0f + rot.m[2][2] - rot.m[0][0] - rot.m[1][1]) * 2.0f;
		q.w = (rot.m[1][0] - rot.m[0][1]) / s;
		q.x = (rot.m[0][2] + rot.m[2][0]) / s;
		q.y = (rot.m[1][2] + rot.m[2][1]) / s;
		q.z = 0.25f * s;
	}

	transform_.rotate = Normalize(q);
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
