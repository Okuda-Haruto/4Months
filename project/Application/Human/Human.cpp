#include "Human.h"
#include "Neck/Neck.h"
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

	fallingSpeed_ = kMinSpeed_;
	speed_ = 0.2f;

	isDrifting_ = false;
	isCoilAround_ = false;
	coilAroundDistance_ = 0.0f;
}

void Human::Update() {

	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);

	//向いている向きに速度を向ける
	velocity_.translate = Vector3{ 0,0,1 } * rotateMatrix * speed_;

	if(isCoilAround_ && isDrifting_){

		std::vector<SRT> neckTransforms = neck_->GetTransforms();
		//線形補間位置を加算する
		coilAroundDistance_ += 1.0f;
		while (coilAroundDistance_ >= 1.0f) {
			coilAroundDistance_ -= 1.0f;
			neckCoilAroundIndex_++;
			if (neckCoilAroundIndex_ >= neckTransforms.size()) neckCoilAroundIndex_ = int32_t(neckTransforms.size() - 1);
		}

		//軸になる位置
		SRT transform = transform_;
		//次の首がない場合先端の先に移動する
		if (neckCoilAroundIndex_ + 1 >= neckTransforms.size()) {
			//線形補間で回転中心のトランスフォームを求める
			transform.rotate = neckTransforms[neckCoilAroundIndex_].rotate;
			transform.translate = neckTransforms[neckCoilAroundIndex_].translate;

			//軸回転後位置
			Vector3 rotatePos = RotateVector(
				Vector3{ 0,0,-15 },
				transform.rotate);
			transform.translate += rotatePos;

		} else {	//そうでないなら首の周囲を回る

			//線形補間で回転中心のトランスフォームを求める
			transform.rotate = Slerp(
				neckTransforms[neckCoilAroundIndex_].rotate,
				neckTransforms[neckCoilAroundIndex_ + 1].rotate,
				coilAroundDistance_);
			transform.translate = Lerp(
				neckTransforms[neckCoilAroundIndex_].translate,
				neckTransforms[neckCoilAroundIndex_ + 1].translate,
				coilAroundDistance_);

			//軸回転後位置
			Vector3 rotatePos = RotateVector(
					Vector3{ kCoilAroundRange_ * sinf(std::numbers::pi_v<float> / 8 * (neckCoilAroundIndex_ % 16) + std::numbers::pi_v<float> / 4 * coilAroundDistance_),
					kCoilAroundRange_* cosf(std::numbers::pi_v<float> / 8 * (neckCoilAroundIndex_ % 16) + std::numbers::pi_v<float> / 4 * coilAroundDistance_),0},
				transform.rotate);
			transform.translate += rotatePos;

			PrimitiveManager::GetInstance()->AddPoint(transform.translate);
		}

		transform_.rotate = LookAt(transform_.translate, transform.translate);

		rotateMatrix = MakeRotateMatrix(transform_.rotate);

		//向いている向きに速度を向ける
		velocity_.translate = Vector3{ 0,0,1 } *rotateMatrix * speed_ * 5;
		//velocity_ = {};
		//transform_.translate = transform.translate;

	} else {
		isCoilAround_ = false;
	}
	if (isDrifting_ && !isCoilAround_) {
		//落下速度を遅くする
		fallingSpeed_ = Lerp<float>(fallingSpeed_, kMinSpeed_, 0.1f);
		velocity_.translate += Vector3{ 0,-fallingSpeed_,0 };

		//近接判定
		Sphere humanNearSphere;
		humanNearSphere.center = transform_.translate;
		humanNearSphere.radius = kCanCoilAroundRange_;
		std::vector<SRT> neckTransforms = neck_->GetTransforms();

		//先端に近い順に
		for (int32_t i = 0; i < int32_t(neckTransforms.size()) - 1; i++) {
			if (IsCollision(humanNearSphere, neckTransforms[i].translate)) {
				//目標地点に向かう
				Vector3 toTarget = transform_.translate - neckTransforms[i].translate;
				Vector3 localDirection = RotateVector(toTarget,Inverse(neckTransforms[i].rotate));

				transform_.rotate = LookAt(transform_.translate, neckTransforms[i].translate);
				//近接判定に首が接触したなら巻き付く
				neckCoilAroundIndex_ = i;
				if (localDirection.x > 0.0f && localDirection.y < 0.0f) {
					coilAroundDistance_ = 0.0f;
				} else if (localDirection.x > 0.0f && localDirection.y > 0.0f) {
					coilAroundDistance_ = 4.0f;
				} else if (localDirection.x < 0.0f && localDirection.y > 0.0f) {
					coilAroundDistance_ = 8.0f;
				} else if (localDirection.x < 0.0f && localDirection.y < 0.0f) {
					coilAroundDistance_ = 12.0f;
				}
				isCoilAround_ = true;
				break;
			}
		}

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