#include "Human.h"
#include <Lerp.h>
#include <Collision.h>

#include <numbers>

void Human::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight, const std::shared_ptr<Camera> camera) {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/test", "Player.gltf"));
	model_->SetShininess(30.0f);
	bulletModel_ = make_unique<Object>();
	bulletModel_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player/Head", "beyblade.obj"));
	bulletModel_->SetShininess(30.0f);
	bulletModel_Break_ = make_unique<Object>();
	bulletModel_Break_->Initialize(ModelManager::GetInstance()->GetModel("resources/Title/StartAnim", "beyblade.obj"));
	bulletModel_Break_->SetShininess(30.0f);
	bulletModel_Break_->SetDirectionalLight(directionalLight);
	bulletModel_Break_->SetCamera(camera);

	int partsNum = int(bulletModel_Break_->GetParts().size());

	breakBulletTransform_.resize(partsNum);
	breakBulletDirection_.resize(partsNum);
	breakBulletRotateVelocity_.resize(partsNum);

	//カメラで使う
	transform_ = {};
	transform_.scale = { 2.5f,2.5f,2.5f };
	transform_.translate = position;
	model_->SetTransform(transform_);
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	model_->SetDirectionalLight(directionalLight);
	model_->SetIsUseAnimation(true);
	model_->SetAnimationIndex(7);
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
	velocity_ = {};
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

	velocity_.translate += Vector3{ 0,fallingSpeed_,0 } * GameEngine::GetDeltaTimeRate();

	//NANチェック
	float len = Length(knockBackAcceleration_);

	if (len > 1e-6f && std::isfinite(len)) {
		knockBackVelocity_ = Normalize(knockBackAcceleration_) * kNockBackSpeed_ * GameEngine::GetDeltaTimeRate();
	}

	knockBackAcceleration_ = {};

#ifdef USE_IMGUI

	ImGui::Begin("Human");
	ImGui::DragFloat3("速度", &velocity_.translate.x);
	ImGui::End();

#endif // USE_IMGUI

	if (!stop) {
		if (isCharging_) {
			transform_.translate += velocity_.translate / 4 + knockBackVelocity_;
		} else if (isResult_) {
			transform_.translate += velocity_.translate / 8;
		}
		else {
			transform_.translate += velocity_.translate + knockBackVelocity_;
		}
	}

	// リザルト中の場合
	if (isResult_) {
		// 範囲内でループ
		if (transform_.translate.y < resultLoopEndY) {
			transform_.translate.y = resultLoopStartY - (resultLoopEndY - transform_.translate.y);
		}
	}

	std::vector<Parts> parts = bulletModel_Break_->GetParts();
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

			model_->ResetAnimationTime();
			model_->SetAnimationIndex(7);
			model_->SetIsLoopAnimation(false);
		}
		break;
	case Break:
		headTransform_ = { headTransform_.scale,transform_.rotate, transform_.translate };
		headTransform_.translate.y += 15;
		for (int i = 0; i < parts.size(); i++) {
			//移動
			rotateMatrix = MakeRotateMatrix(breakBulletDirection_[i]);
			breakBulletTransform_[i].translate += Vector3{ 0,0,breakBulletSpeed_ } * rotateMatrix;
			//少しずつ下向きに
			breakBulletDirection_[i] = Slerp(breakBulletDirection_[i], MakeRotateAxisAngleQuaternion({ -1,0,0 }, std::numbers::pi_v<float> / 2), 0.1f);
			//回転
			breakBulletTransform_[i].rotate = breakBulletTransform_[i].rotate * breakBulletRotateVelocity_[i];

			*parts[i].transform = breakBulletTransform_[i];
			bulletModel_Break_->SetParts(parts[i], i);
		}

		break;
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

#ifdef USE_IMGUI

#endif

	model_->SetTransform(modelTransform);
	bulletModel_->SetTransform(headTransform_);

	model_->Update();
	wind_->Update();
}

void Human::Draw() {
	model_->Draw3D();

	if (vacuumState_ != Break) {
		bulletModel_->Draw3D();
		headRotateEffect_->Draw();
	}
	else {
		bulletModel_Break_->Draw3D();
	}

	if (vacuumState_ == Vacuum) {
		wind_->Draw();
	}
}

void Human::OnHitVoxel(AABB aabb) {
	if (!isBreak_) {
		Slowdown();

		Vector3 closest;

		closest.x = std::clamp(transform_.translate.x, aabb.min.x, aabb.max.x) - transform_.translate.x;
		closest.y = std::clamp(transform_.translate.y, aabb.min.y, aabb.max.y) - transform_.translate.y;
		closest.z = std::clamp(transform_.translate.z, aabb.min.z, aabb.max.z) - transform_.translate.z;

		if (fabsf(closest.x) >= fabsf(closest.y) && fabsf(closest.x) >= fabsf(closest.z)) {
			knockBackAcceleration_.x -= fabsf(closest.x) / closest.x;
		}
		else if (fabsf(closest.y) >= fabsf(closest.x) && fabsf(closest.y) >= fabsf(closest.z)) {
			//knockBackAcceleration_.y -= fabsf(closest.y) / closest.y;
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

		if (vacuumState_ == None) {
			if (isAutoBurst_) {
				charge_ = max(kMaxCharge_ * 0.1f, charge_ * 0.5f);
				Throw();
				headRotateEffect_->Shoot();
			}
		}
	}
}

void Human::BreakSpinner() {
	vacuumState_ = Break;
	fallingSpeed_ = 2.5f;
	speed_ = 0.0f;

	transform_.translate -= velocity_.translate;
	resultLoopStartY = transform_.translate.y;
	resultLoopEndY = resultLoopStartY - 9;

	velocity_.translate.y = 0.0f;
	isResult_ = false;
	model_->SetIsLoopAnimation(true);
	model_->SetAnimationIndex(2);

	bulletModel_Break_->SetTransform(headTransform_);

	for (int i = 0; i < breakBulletDirection_.size();i++) {
		breakBulletTransform_[i] = { {0.125f,0.125f,0.125f},IdentityQuaternion(), {0,0,0} };

		// 30~60
		breakBulletDirection_[i] = MakeRotateAxisAngleQuaternion({ 1,0,0 }, GameEngine::randomFloat(std::numbers::pi_v<float> * 70.0f / 180.0f, std::numbers::pi_v<float> * 85.0f / 180.0f));
		// 0~360
		breakBulletDirection_[i] = breakBulletDirection_[i] * MakeRotateAxisAngleQuaternion({ 0,1,0 }, GameEngine::randomFloat(0.0f, std::numbers::pi_v<float> * 2.0f));

		//乱数
		breakBulletRotateVelocity_[i] = Normalize(
			Quaternion{ GameEngine::randomFloat(0.0f, 1.0f),
			GameEngine::randomFloat(0.0f, 1.0f),
			GameEngine::randomFloat(0.0f, 1.0f),
			GameEngine::randomFloat(0.0f, 1.0f)
			});
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
	fallingSpeed_ += 5 + bounceBackSpeed_ * min(0.25f + charge_ / kMaxCharge_, 1.0f);
	speed_ = 0.0f;
	velocity_.translate.y = 0.0f;
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
	model_->SetAnimationIndex(11);
	model_->SetIsLoopAnimation(false);
}

void Human::Charge() {
	if (charge_ == 0) {
		model_->ResetAnimationTime();
		model_->SetAnimationIndex(12);
		model_->SetIsLoopAnimation(false);

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
	if (!isResult_ && flag) {
		resultLoopStartY = transform_.translate.y;
		resultLoopEndY = resultLoopStartY - 12;
	}

	isResult_ = flag;

	if (vacuumState_ == Break && fallingSpeed_ < 0.0f && transform_.translate.y < resultLoopStartY) {
		isBreak_ = true;
	}
}