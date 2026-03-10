#include "Human.h"
#include "Neck/Neck.h"
#include "Goal/Goal.h"
#include <Lerp.h>
#include <Collision.h>

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

	characterID_ = id_++;

	fallingSpeed_ = -kMinSpeed_;
	speed_ = 0.4f;

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

	if (isBreke_) {
		speed_ = Lerp(speed_, kMinSpeed_, 0.1f);
	} else {
		speed_ = Lerp(speed_, 0.4f, 0.1f);
	}

	//向いている向きに速度を向ける
	velocity_.translate = Vector3{ 0,0,1 } *rotateMatrix * speed_;

	if ((goal_->GetTransform().translate.y > transform_.translate.y) || goal_->GetHuman() == this) {
		if (!isTurnBack_) {
			noTargetMinNumber_ = uint32_t(necks_[selfNeckIndex_]->GetTransforms().size() - 1);
		}
		isTurnBack_ = true;
	} else {
		if (isTurnBack_) {
			noTargetMinNumber_ = uint32_t(necks_[selfNeckIndex_]->GetTransforms().size() - 1);
		}
		isTurnBack_ = false;
	}

	//巻き付いているとき
	if (isCoilAround_ && isDrifting_) {

		std::vector<SRT> neckTransforms = necks_[neckCoilAroundIndex_]->GetTransforms();

		//線形補間位置を加算する
		//下向き
 		if (RotateVector(Vector3{ 0,0,1 }, neckTransforms[neckCoilAroundNumber_].rotate).y >= 0.0f) {
			if (!isTurnBack_) {
				coilAroundDistance_ += 5.0f * speed_;
			} else {
				coilAroundDistance_ -= 5.0f * speed_;
			}
		} else {
			if (isTurnBack_) {
				coilAroundDistance_ += 5.0f * speed_;
			} else {
				coilAroundDistance_ -= 5.0f * speed_;
			}
		}
		while (coilAroundDistance_ >= 1.0f) {
			coilAroundDistance_ -= 1.0f;
			neckCoilAroundNumber_++;
			if (neckCoilAroundNumber_ >= neckTransforms.size()) neckCoilAroundNumber_ = int32_t(neckTransforms.size() - 1);
		}

		while (coilAroundDistance_ < 0.0f) {
			coilAroundDistance_ += 1.0f;
			neckCoilAroundNumber_--;
			if (neckCoilAroundNumber_ < 0) neckCoilAroundNumber_ = 0;
		}

		//軸になる位置
		SRT transform = transform_;
		//次の首がない場合先端の先に移動する
		if (neckCoilAroundNumber_ + 1 >= neckTransforms.size()) {
			//線形補間で回転中心のトランスフォームを求める
			transform.rotate = neckTransforms[neckCoilAroundNumber_].rotate;
			transform.translate = neckTransforms[neckCoilAroundNumber_].translate;

			//軸回転後位置
			if (!isTurnBack_) {
				Vector3 rotatePos = RotateVector(
					Vector3{ 0,0,-15 },
					transform.rotate);
				transform.translate += rotatePos;
				if (coilAroundEndTime_ < 1.0f) {
					coilAroundEndTime_ += 2.0f / 60.0f;
					if (coilAroundEndTime_ > 1.0f) {
						coilAroundEndTime_ = 1.0f;
					}
				}

				//巻き付き地点に向かう
				transform_.rotate = LookAt(transform_.translate, transform.translate);

				velocity_.translate = Normalize(transform.translate - transform_.translate) * speed_ * 5;

				rotatePos = RotateVector(
					Vector3{ 0,0,-5 },
					transform.rotate);
				if (coilAroundEndTime_ == 1.0f || neckTransforms[neckCoilAroundNumber_].translate.y + rotatePos.y > transform_.translate.y) {
					isCoilAround_ = false;
					isDrifting_ = false;
					unableDriftTimer_ = unableDriftTime_;
				}

			} else {
				Vector3 rotatePos = RotateVector(
					Vector3{ 0,0,-5 },
					transform.rotate);
				transform.translate += rotatePos;
				if (Length(transform_.translate - transform.translate) < 0.1f) {
					isCoilAround_ = false;
					isDrifting_ = false;
					unableDriftTimer_ = unableDriftTime_;
				}
			}

		} else {	//そうでないなら首の周囲を回る

			//線形補間で回転中心のトランスフォームを求める
			transform.rotate = Slerp(
				neckTransforms[neckCoilAroundNumber_].rotate,
				neckTransforms[neckCoilAroundNumber_ + 1].rotate,
				coilAroundDistance_);
			transform.translate = Lerp(
				neckTransforms[neckCoilAroundNumber_].translate,
				neckTransforms[neckCoilAroundNumber_ + 1].translate,
				coilAroundDistance_);

			//軸回転後位置
			//回り始めは余裕のある回転をする
			Vector3 rotateVector = (coilAroundRotatePos_ + coilAroundRotatePos_ * (1.0f - coilAroundStartTime_));
			rotateVector = RotateVector(rotateVector, MakeRotateAxisAngleQuaternion(Vector3{0,0,1}, std::numbers::pi_v<float> / 8 * ((neckCoilAroundNumber_ - coilAroundStartNumber_) % 16) + std::numbers::pi_v<float> / 4 * coilAroundDistance_));
			//首の方向向かせるより真下向かせた方がスペースは空く(おそらく余計に回転した位置に移動させてるからギリギリな回転になっていた)
			Vector3 rotatePos = RotateVector(rotateVector, MakeRotateAxisAngleQuaternion(Vector3{1,0,0}, std::numbers::pi_v<float> / 2));

			//軸回転後の正確な位置
  			transform.translate += rotatePos;

  			PrimitiveManager::GetInstance()->AddPoint(transform.translate);

			transform_.rotate = LookAt(transform_.translate, transform.translate);

			rotateMatrix = MakeRotateMatrix(transform_.rotate);

			if (coilAroundStartTime_ < 1.0f) {
				coilAroundStartTime_ += 4.0f / 60.0f;
				if (coilAroundStartTime_ > 1.0f) {
					coilAroundStartTime_ = 1.0f;
				}
			}

			//巻き付き地点に向かう
			velocity_.translate = Lerp((transform.translate - transform_.translate) / 4, transform.translate - transform_.translate, coilAroundStartTime_);

		}
		//向いている向きに速度を向ける
		//velocity_.translate = transform.translate - transform_.translate;

	} else {
		//巻き付いていないなら切る
		isCoilAround_ = false;
		coilAroundStartTime_ = 0;
		coilAroundEndTime_ = 0;
	}

	//とぐろ中(巻き付いていない)
	if (isDrifting_ && !isCoilAround_) {
		//落下速度を遅くする
		fallingSpeed_ = Lerp<float>(fallingSpeed_, kMinSpeed_, 0.1f);
		if (!isTurnBack_) {
			velocity_.translate += Vector3{ 0,-fallingSpeed_,0 };
		} else {
			velocity_.translate += Vector3{ 0,fallingSpeed_,0 };
		}

	} else {
		//上向き速度 * 重力」を落下速度に加える
		if (isTurnBack_) {
			fallingSpeed_ = min(fallingSpeed_ + kGravity_, maxRisingSpeed_);
		} else {
			fallingSpeed_ = max(fallingSpeed_ - kGravity_, -maxFallingSpeed_);
		}
		velocity_.translate += Vector3{ 0,fallingSpeed_,0 };
	}
	transform_.translate += velocity_.translate;

	// 速度が一定以下なら戻す
	if (speed_ < kDefaultSpeed_) {
		speed_ += 0.001f;
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
}

void Human::Draw() {
	model_->Draw3D();
}

void Human::StartDrifting() {
	isDrifting_ = true;

	//近接判定
	Sphere humanNearSphere;
	humanNearSphere.center = transform_.translate;
	humanNearSphere.radius = kCanCoilAroundRange_;

	//一番近い首との距離
	float neckNearLength = -1;
	//首(一本)の番号
	int neckIndex = -1;
	//首(単体)の番号
	int neckNumber = -1;
	for (int32_t i = 0; i < int32_t(necks_.size()); i++) {
		std::vector<SRT> neckTransforms = necks_[i]->GetTransforms();

		//先端に近い順に
		for (uint32_t j = 0; j < uint32_t(neckTransforms.size()) - 1; j++) {

			//自分の首かつ、巻き付き不可範囲ならbreak
			if (i == selfNeckIndex_ && j > noTargetMinNumber_) break;

			//首がある程度近いか
			if (IsCollision(humanNearSphere, neckTransforms[j].translate)) {

				Vector3 toTarget = transform_.translate - neckTransforms[j].translate;

				if (neckNearLength == -1 || Length(toTarget) < neckNearLength) {
					neckNearLength = Length(toTarget);
					neckNumber = j;
					neckIndex = i;
				}
			}
		}
	}

	if (neckIndex != -1) {
		SRT neckTransform = necks_[neckIndex]->GetTransforms()[neckNumber];
		//目標地点に向かう
		Vector3 neckToHuman = transform_.translate - neckTransform.translate;
		Vector3 localDirection = RotateVector(neckToHuman, Inverse(neckTransform.rotate));
		localDirection.z = 0;

		transform_.rotate = LookAt(transform_.translate, neckTransform.translate);
		//近接判定に首が接触したなら巻き付く
		neckCoilAroundNumber_ = neckNumber;
		neckCoilAroundIndex_ = neckIndex;
		coilAroundRotatePos_ = Normalize(localDirection) * kCoilAroundRange_;	//開始地点での回転
		coilAroundStartNumber_ = neckNumber;
		coilAroundDistance_ = 0;
		isCoilAround_ = true;
	}
}

void Human::OnHitRing(const float addSpeed, const float addMaxSpeed) {
	speed_ += addSpeed;
	maxRisingSpeed_ += addMaxSpeed;
	maxFallingSpeed_ += addMaxSpeed;
	cameraEffectTime_ = kMaxCameraEffectTime_;
}

void Human::OnHitSpike(const Vector3& pos) {
	OnHitNeck(pos);
}

void Human::OnHitWall(OBB wallObb) {
	// 壁の向きをrotateにコピー
	Vector3 right = wallObb.orientations[0];
	Vector3 up = wallObb.orientations[1];
	Vector3 forward = wallObb.orientations[2];

	isCoilAround_ = false;
	isDrifting_ = false;
	unableDriftTimer_ = unableDriftTime_;

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

	speed_ -= 0.1f;
	if (speed_ < 0) { speed_ = 0.1f; }

	maxRisingSpeed_ = kDefaultMaxRisingSpeed_;
	maxFallingSpeed_ = kDefaultMaxFallingSpeed_;
	invincibleTimer_ = invincibleTimeOnHit_;
	unableDriftTimer_ = unableDriftTime_;
	knockbackTimer_ = kKnockbackTime_;

	// 反射方向の計算
	Vector3 normal = Normalize(transform_.translate - pos);
	Vector3 reflect = velocity_.translate - normal * 2.0f * Dot(velocity_.translate, normal);
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