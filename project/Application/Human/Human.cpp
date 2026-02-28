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

	fallingSpeed_ = 0.0f;
	speed_ = kDefaultSpeed_;

	characterID_ = id_++;
}

void Human::Update() {
	if (!rewindTimer_) { // 巻き戻し中でなければ移動

		Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);

		//向いている向きに速度を向ける
		velocity_.translate = Vector3{ 0,0,1 } *rotateMatrix;

		if (!isTurnBack_) {
			//「下向き速度 * 重力」を落下速度に加える
			fallingSpeed_ = min(fallingSpeed_ + kGravity_, kMaxFallingSpeed_);

			velocity_.translate *= fallingSpeed_;
		} else {
			//「下向き速度 * 重力」を落下速度に加える
			fallingSpeed_ = min(fallingSpeed_ + kGravity_, kMaxRisingSpeed_);

			velocity_.translate *= fallingSpeed_;
		}
		transform_.translate += velocity_.translate * speed_;

		// 位置を記録
		history_.push_back(transform_.translate);
		if (history_.size() > maxHistory_) { // 後ろの記録を削除
			history_.pop_front();
		}

		// 無敵タイマー
		if (invinsibleTimer_) {
			invinsibleTimer_--;
		}

	} else {
		rewindTimer_--;
		if (rewindTimer_ <= 0) {
			rewindTimer_ = 0;
		}
		transform_.translate = history_[int(float(rewindTimer_) / float(kRewindTime_) * maxHistory_)];
	}


#ifdef USE_IMGUI

#endif

	model_->SetTransform(transform_);
}

void Human::Draw() {
	model_->Draw3D();
}

void Human::OnHitRing(const float addSpeed) {
	speed_ += addSpeed;
}

void Human::OnHitSpike() {
	if (invinsibleTimer_ <= 0) {
		rewindTimer_ = kRewindTime_;
		speed_ = kDefaultSpeed_;
		invinsibleTimer_ = invinsibleTimeOnHit_;
	}
}

void Human::OnHitWall(OBB wallObb) {
	// 壁の向きをrotateにコピー
	Vector3 right = wallObb.orientations[0];
	Vector3 up = wallObb.orientations[1];
	Vector3 forward = wallObb.orientations[2];

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