#include "Human.h"
#include <Lerp.h>
#include <Collision.h>

#include <numbers>

void Human::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight, const std::shared_ptr<Camera> camera) {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player", "Player.gltf"));
	model_->SetShininess(30.0f);
	bulletModel_ = make_unique<Object>();
	bulletModel_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player/Head", "beyblade.obj"));
	bulletModel_->SetShininess(30.0f);
	//カメラで使う
	transform_ = {};
	transform_.scale = { 2.5f,2.5f,2.5f };
	transform_.translate = position;
	model_->SetTransform(transform_);
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	model_->SetDirectionalLight(directionalLight);
	model_->SetIsUseAnimation(true);
	model_->SetAnimationIndex(28);
	model_->Update();

	headTransform_.scale = { 2.5f,2.5f,2.5f };
	headTransform_.translate = position;

	bulletModel_->SetTransform(headTransform_);
	bulletModel_->SetDirectionalLight(directionalLight);

	// エフェクト
	wind_ = make_unique<Wind>();
	wind_->Initialize(directionalLight);
	headRotateEffect_ = make_unique<PlayerRotation>();
	headRotateEffect_->Initialize(directionalLight, camera);

	fallingSpeed_ = -kMinSpeed_;
	speed_ = 0.3f;
	knockBackAcceleration_ = {};
	knockBackVelocity_ = {};

	shootSE_ = make_unique<Audio>();
	shootSE_->Initialize("resources/SE・BGM/Game/shot.mp3", 0.5f);
	catchSE_ = make_unique<Audio>();
	catchSE_->Initialize("resources/SE・BGM/Game/reload.mp3", 0.5f);
	chargeSE_ = make_unique<Audio>();
	chargeSE_->Initialize("resources/SE・BGM/Game/charge.mp3", 0.5f);
}

void Human::Update() {
	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);

	if (cameraEffectTime_ > 0.0f) {
		cameraEffectTime_ -= GameEngine::GetDeltaTime();
		if (cameraEffectTime_ < 0.0f) {
			cameraEffectTime_ = 0.0f;
		}
	}
	float time = GameEngine::GetDeltaTimeRate();

	//向いている向きに速度を向ける
	velocity_.translate += Vector3{ 0,0,1 } *rotateMatrix * speed_ * GameEngine::GetDeltaTimeRate();
	float gravity = kGravity_;
	float maxFall = maxFallingSpeed_;

	if (isResult_) {
		gravity *= 0.1f;
		maxFall *= 0.1f; // ← これが本命
	}

	// ★追加：ジャンピングフラッシュ風
	if (isJumpFlashMode_) {
		float jumpFlashGravity = gravity * 0.4f;
		float jumpFlashMaxFall = maxFall * 0.8f;
		fallingSpeed_ = max(fallingSpeed_ - jumpFlashGravity * GameEngine::GetDeltaTimeRate(), -jumpFlashMaxFall);
	}
	else {
		fallingSpeed_ = max(fallingSpeed_ - gravity * GameEngine::GetDeltaTimeRate(), -maxFall);
	}

	velocity_.translate += Vector3{ 0,fallingSpeed_,0 } *GameEngine::GetDeltaTimeRate();

	//NANチェック
	float len = Length(knockBackAcceleration_);

	if (len > 1e-6f && std::isfinite(len)) {
		knockBackVelocity_ = Normalize(knockBackAcceleration_) * kNockBackSpeed_ * GameEngine::GetDeltaTimeRate();
		knockBackVelocity_.y *= kNockBackFallingSpeed_ / kNockBackSpeed_;
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
		headTransform_.translate += headDir_ * headSpeed_ * GameEngine::GetDeltaTimeRate();
		headSpeed_ -= headDeceleration_ * GameEngine::GetDeltaTimeRate();
		if (headSpeed_ <= 0) {
			headSpeed_ = 0;
			vacuumTimer_ = vacuumTime_;
			wind_->Set(headTransform_.translate, vacuumRadius_, vacuumTime_);
			vacuumState_ = Vacuum;
		}
		break;

	case Vacuum:
		vacuumTimer_ -= GameEngine::GetDeltaTime();

		if (vacuumTimer_ <= 0) {
			vacuumState_ = Return;
			returnTimer_ = returnTime_;
			vacuumStartPos_ = headTransform_.translate;
		}
		break;

	case Return:
		headPrevTransform_ = headTransform_;
		headTransform_.translate = Lerp(vacuumStartPos_, transform_.translate, (1.0f - float(returnTimer_) / float(returnTime_)));
		headSpeed_ += headDeceleration_ * GameEngine::GetDeltaTimeRate();

		returnTimer_ -= GameEngine::GetDeltaTime();
		if (returnTimer_ <= 0) {
			headSpeed_ = 0;
			vacuumState_ = None;
			headRotateEffect_->Catch();
			catchSE_->SoundPlayWave();

			model_->SetAnimationIndex(28);
		}
	}

	// 回転
	if (charge_ == kMaxCharge_) {
		headRotate_ += 0.2f * GameEngine::GetDeltaTimeRate();
	}
	else if (isCharging_ || vacuumState_ != None) {
		headRotate_ += 0.1f * GameEngine::GetDeltaTimeRate();
	}
	else {
		headRotate_ += 0.05f * GameEngine::GetDeltaTimeRate();
	}

	Vector3 forward = { 0,0,1 }; // or {0,0,-1}
	forward = TransformNormal(forward, MakeRotateMatrix(transform_.rotate));

	SRT modelTransform = transform_;
	modelTransform.rotate = MakeRotateAxisAngleQuaternion({ 1,0,0 }, std::numbers::pi_v<float> / 2) * transform_.rotate;
	headTransform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, headRotate_) * modelTransform.rotate;

	Vector3 up = RotateVector({ 0,1,0 }, modelTransform.rotate);
	up.x *= -1; up.z *= -1;
	modelTransform.translate = transform_.translate + up * humanFootOffset;
	
	headRotateEffect_->Update(transform_.translate, headTransform_.translate, headTransform_.scale.x, headRotate_, isCharging_, charge_ == kMaxCharge_);

	float dt = GameEngine::GetDeltaTimeRate() / 60.0f;

	float rate = 1.0f - powf(0.5f, dt * 10.0f); // ← 減衰速度
	speed_ = Lerp(speed_, kDefaultSpeed_, rate);

	knockBackVelocity_.x = Lerp(knockBackVelocity_.x, 0.0f, rate);
	knockBackVelocity_.y = Lerp(knockBackVelocity_.y, 0.0f, rate);
	knockBackVelocity_.z = Lerp(knockBackVelocity_.z, 0.0f, rate);

	velocity_ = {};

#ifdef USE_IMGUI

#endif

	model_->SetTransform(modelTransform);
	bulletModel_->SetTransform(headTransform_);

	model_->Update();
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

	// ★追加：ジャンピングフラッシュ風
	if (isJumpFlashMode_) {
		fallingSpeed_ = bounceBackSpeed_ * 0.33f;
	}
	else {
		fallingSpeed_ = 0.0f;
	}

	speed_ = 1.0f;
	if (vacuumState_ == None) {
		if (isAutoBurst_) {
			charge_ = max(kMaxCharge_ * 0.1f, charge_ * 0.5f);
			Throw();
			headRotateEffect_->Shoot();
		}
	}
}

void Human::ApproachCenter(const Vector2& center) {
	Vector3 target = { center.x,transform_.translate.y, center.y };
	if (Length(transform_.translate - target) < 0.5f) {
		transform_.translate = target;
	}
	else {
		Vector3 dir = Normalize(target - transform_.translate);
		transform_.translate += (speed_ * 0.5f) * dir;
	}
}

void Human::Throw() {
	headTransform_ = { headTransform_.scale,transform_.rotate, transform_.translate };
	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);
	headDir_ = Vector3{ 0,0,1 } *rotateMatrix;
	headSpeed_ = headStartSpeed_;
	vacuumState_ = Going;
	fallingSpeed_ += 7 + bounceBackSpeed_ * min(0.25f + charge_ / kMaxCharge_, 1.0f);
	vacuumRadius_ = baseVacuumRadius_ + charge_;
	vacuumTime_ = (charge_ / kMaxCharge_) * (kMaxVacuumTime - kMinVacuumTime) + kMinVacuumTime;
	returnTime_ = (charge_ / kMaxCharge_) * (kMaxReturnTime - kMinReturnTime) + kMinReturnTime;
	charge_ = 0;
	headRotateEffect_->Shoot();

	if (!shootSE_->IsSoundPlayingWave()) {
		shootSE_->SoundPlayWave();
	}
	if (chargeSE_->IsSoundPlayingWave()) {
		chargeSE_->SoundEndWave();
	}

	model_->ResetAnimationTime();
	model_->SetAnimationIndex(24);
}

void Human::Charge() {
	if (charge_ == 0) {
		model_->ResetAnimationTime();
		model_->SetAnimationIndex(28);

		chargeSE_->SoundPlayWave();
	}

	charge_ += kChargeSpeed_ * GameEngine::GetDeltaTimeRate();
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
void Human::SetResult(bool flag) {
	isResult_ = flag;
}