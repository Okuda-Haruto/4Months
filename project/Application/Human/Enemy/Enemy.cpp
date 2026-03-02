#include "Enemy.h"
#include <Collision.h>
#include "Goal/Goal.h"

#include <numbers>
#include <algorithm>

void Enemy::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight) {
	//初期化
	Human::Initialize(position, directionalLight);
	color_ = { 1,0,0,1 };   //  //首の色　赤
}

void Enemy::Update() {

	//基礎クォータニオン(真下)
	Quaternion NextRotate;

	if (fallingSpeed_ < 0.0f) {
		NextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	} else {
		NextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, std::numbers::pi_v<float> / 2);
	}
	NextRotate = NextRotate * rollRotate_;
	//基礎クオータニオン分の回転行列
	Matrix4x4 rotateMatrix = MakeRotateMatrix(NextRotate);

	if (fallingSpeed_ < 0.0f) {

		//先にゴールが近いか判定
		bool isNearGoal = false;
		float length = -1;

		if (goal_->GetHuman() != this) {
			Sphere nearSpehre;
			nearSpehre.center = transform_.translate;
			nearSpehre.radius = 50.0f;
			if (IsCollision(nearSpehre, goal_->GetTransform().translate)) {
				//目標地点に向かう距離
				Vector3 toTarget = goal_->GetTransform().translate - transform_.translate;

				isNearGoal = true;
				length = Length(toTarget);
			}
		}

		//近いリングを参照
		int index = -1;

		for (int i = int(rings_.size()) - 1; i >= 0;i--) {
			//リング位置
			Vector3 center = rings_[i]->GetColliderCenter();

			if (center.y < transform_.translate.y) {

				//目標地点に向かう距離
				Vector3 toTarget = center - transform_.translate;

				//一番近いリングを捜索
				if (length == -1 || (length > Length(toTarget))) {
					index = i;
					length = Length(toTarget);
				}
			}
		}

		//とりあえず愚直に向かうように
		if (index != -1) {

			//目標地点に向かう距離
			Vector3 toTarget = rings_[index]->GetColliderCenter() - transform_.translate;

			//半直線
			Ray ray;
			ray.origin = transform_.translate;
			ray.diff = Normalize(toTarget);

			bool isRayCollision = false;
			//全障害物と判定
			for (int i = 0; i < spikes_.size(); i++) {
				if (IsCollision(spikes_[i]->GetCollider(), ray)) {
					//真下だとぶつかりそうな場合
					if (fabsf(ray.diff.x) < 0.5f && fabsf(ray.diff.z) < 0.1f) {
						toTarget = Vector3(0.0f, transform_.translate.y + 10.0f, 0.0f) - transform_.translate;
					} else {
						isRayCollision = true;
					}
					break;
				}
			}

			int neckIndex = -1;
			float neckLength = -1;
			std::vector<SRT> transforms = neck_->GetTransforms();
			for (int i = 0; i < transforms.size(); i++) {

				//プレイヤー(ある程度の距離まで判定)
				Sphere playerCollisder;
				playerCollisder.center = transform_.translate;
				playerCollisder.radius = 1.0f;
				//首
				Sphere neckCollider;
				neckCollider.center = transforms[i].translate;
				neckCollider.radius = 1.0f;

				//ある程度近いなら
				if (IsCollision(playerCollisder, neckCollider)) {
					if (neckCollider.center.y < playerCollisder.center.y &&
						(neckIndex == -1 || (neckLength == -1 || Length(neckCollider.center - playerCollisder.center) < neckLength))) {

						neckIndex = i;
						neckLength = Length(neckCollider.center - playerCollisder.center);
					}
				}

			}

			//近くに首があるなら
			if (neckIndex != -1) {
				//首の方向
				toTarget = transforms[neckIndex].translate - transform_.translate;

				toTarget = toTarget * MakeRotateMatrix(MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> * 3 / 4));
			}

			if (!isRayCollision) {
				if (toTarget.x > 0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4);
				}
				if (toTarget.x < -0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, std::numbers::pi_v<float> / 4);
				}
				if (toTarget.z > 0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, std::numbers::pi_v<float> / 4);
				}
				if (toTarget.z < -0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4);
				}
			}
		} else if (isNearGoal) {

			//目標地点に向かう距離
			Vector3 toTarget = goal_->GetTransform().translate - transform_.translate;

			//半直線
			Ray ray;
			ray.origin = transform_.translate;
			ray.diff = Normalize(toTarget);

			bool isRayCollision = false;
			//全障害物と判定
			for (int i = 0; i < spikes_.size(); i++) {
				if (IsCollision(spikes_[i]->GetCollider(), ray)) {
					//真下だとぶつかりそうな場合
					if (fabsf(ray.diff.x) < 0.5f && fabsf(ray.diff.z) < 0.1f) {
						toTarget = Vector3(0.0f, transform_.translate.y + 10.0f, 0.0f) - transform_.translate;
					} else {
						isRayCollision = true;
					}
					break;
				}
			}

			int neckIndex = -1;
			float neckLength = -1;
			std::vector<SRT> transforms = neck_->GetTransforms();
			for (int i = 0; i < transforms.size(); i++) {

				//プレイヤー(ある程度の距離まで判定)
				Sphere playerCollisder;
				playerCollisder.center = transform_.translate;
				playerCollisder.radius = 1.0f;
				//首
				Sphere neckCollider;
				neckCollider.center = transforms[i].translate;
				neckCollider.radius = 1.0f;

				//ある程度近いなら
				if (IsCollision(playerCollisder, neckCollider)) {
					if (neckCollider.center.y < playerCollisder.center.y &&
						(neckIndex == -1 || (neckLength == -1 || Length(neckCollider.center - playerCollisder.center) < neckLength))) {

						neckIndex = i;
						neckLength = Length(neckCollider.center - playerCollisder.center);
					}
				}

			}

			//近くに首があるなら
			if (neckIndex != -1) {
				//首の方向
				toTarget = transforms[neckIndex].translate - transform_.translate;

				toTarget = toTarget * MakeRotateMatrix(MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> * 3 / 4));

			}

			if (!isRayCollision) {
				if (toTarget.x > 0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4);
				}
				if (toTarget.x < -0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, std::numbers::pi_v<float> / 4);
				}
				if (toTarget.z > 0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, std::numbers::pi_v<float> / 4);
				}
				if (toTarget.z < -0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4);
				}
			}
		}

	} else {
		//先にゴールが近いか判定
		bool isNearGoal = false;
		float length = -1;

		if (goal_->GetHuman() != this) {
			Sphere nearSpehre;
			nearSpehre.center = transform_.translate;
			nearSpehre.radius = 50.0f;
			if (IsCollision(nearSpehre, goal_->GetTransform().translate)) {
				//目標地点に向かう距離
				Vector3 toTarget = goal_->GetTransform().translate - transform_.translate;

				isNearGoal = true;
				length = Length(toTarget);
			}
		}

		//近いリングを参照
		int index = -1;

		for (int i = 0; i < rings_.size(); i++) {
			//リング位置
			Vector3 center = rings_[i]->GetColliderCenter();

			if (center.y > transform_.translate.y) {

				//目標地点に向かう距離
				Vector3 toTarget = center - transform_.translate;

				//一番近いリングを捜索
				if (length == -1 || (length > Length(toTarget))) {
					index = i;
					length = Length(toTarget);
				}
			}
		}

		//とりあえず愚直に向かうように
		if (index != -1) {

			//目標地点に向かう距離
			Vector3 toTarget = rings_[index]->GetColliderCenter() - transform_.translate;

			//半直線
			Ray ray;
			ray.origin = transform_.translate;
			ray.diff = Normalize(toTarget);

			bool isRayCollision = false;
			//全障害物と判定
			for (int i = 0; i < spikes_.size(); i++) {
				if (IsCollision(spikes_[i]->GetCollider(),ray)) {
					//真下だとぶつかりそうな場合
					if (fabsf(ray.diff.x) < 0.5f && fabsf(ray.diff.z) < 0.1f) {
						toTarget = Vector3(0.0f, transform_.translate.y + 10.0f, 0.0f) - transform_.translate;
					} else {
						isRayCollision = true;
					}
					break;
				}
			}

			int neckIndex = -1;
			float neckLength = -1;
			std::vector<SRT> transforms = neck_->GetTransforms();
			for (int i = 0; i < transforms.size(); i++) {

				//プレイヤー(ある程度の距離まで判定)
				Sphere playerCollisder;
				playerCollisder.center = transform_.translate;
				playerCollisder.radius = 0.5f;
				//首
				Sphere neckCollider;
				neckCollider.center = transforms[i].translate;
				neckCollider.radius = 1.0f;

				//ある程度近いなら
				if (IsCollision(playerCollisder, neckCollider)) {
					if (neckCollider.center.y > playerCollisder.center.y &&
						(neckIndex == -1 || (neckLength == -1 || Length(neckCollider.center - playerCollisder.center) < neckLength))) {

						neckIndex = i;
						neckLength = Length(neckCollider.center - playerCollisder.center);
					}
				}

			}

			//近くに首があるなら
			if (neckIndex != -1) {
				//首の方向
				toTarget = transforms[neckIndex].translate - transform_.translate;

				toTarget = toTarget * MakeRotateMatrix(MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> * 3 / 4));

			}

			if (!isRayCollision) {
				if (toTarget.x > 0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4);
				}
				if (toTarget.x < -0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, std::numbers::pi_v<float> / 4);
				}
				if (toTarget.z > 0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4);
				}
				if (toTarget.z < -0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, std::numbers::pi_v<float> / 4);
				}
			}
		} else if (isNearGoal) {

			//目標地点に向かう距離
			Vector3 toTarget = goal_->GetTransform().translate - transform_.translate;

			//半直線
			Ray ray;
			ray.origin = transform_.translate;
			ray.diff = Normalize(toTarget);

			bool isRayCollision = false;
			//全障害物と判定
			for (int i = 0; i < spikes_.size(); i++) {
				if (IsCollision(spikes_[i]->GetCollider(), ray)) {
					//真下だとぶつかりそうな場合
					if (fabsf(ray.diff.x) < 0.5f && fabsf(ray.diff.z) < 0.1f) {
						toTarget = Vector3(0.0f, transform_.translate.y + 10.0f, 0.0f) - transform_.translate;
					} else {
						isRayCollision = true;
					}
					break;
				}
			}

			int neckIndex = -1;
			float neckLength = -1;
			std::vector<SRT> transforms = neck_->GetTransforms();
			for (int i = 0; i < transforms.size(); i++) {

				//プレイヤー(ある程度の距離まで判定)
				Sphere playerCollisder;
				playerCollisder.center = transform_.translate;
				playerCollisder.radius = 0.5f;
				//首
				Sphere neckCollider;
				neckCollider.center = transforms[i].translate;
				neckCollider.radius = 1.0f;

				//ある程度近いなら
				if (IsCollision(playerCollisder, neckCollider)) {
					if (neckCollider.center.y > playerCollisder.center.y &&
						(neckIndex == -1 || (neckLength == -1 || Length(neckCollider.center - playerCollisder.center) < neckLength))) {

						neckIndex = i;
						neckLength = Length(neckCollider.center - playerCollisder.center);
					}
				}

			}

			//近くに首があるなら
			if (neckIndex != -1) {
				//首の方向
				toTarget = transforms[neckIndex].translate - transform_.translate;

				toTarget = toTarget * MakeRotateMatrix(MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> *3 / 4));

			}

			if (!isRayCollision) {
				if (toTarget.x > 0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4);
				}
				if (toTarget.x < -0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 } *rotateMatrix, std::numbers::pi_v<float> / 4);
				}
				if (toTarget.z > 0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, -std::numbers::pi_v<float> / 4);
				}
				if (toTarget.z < -0.2f) {
					NextRotate = NextRotate * MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 } *rotateMatrix, std::numbers::pi_v<float> / 4);
				}
			}
		}
	}

	//現在の向きと次の向きの補完
	transform_.rotate = Slerp(transform_.rotate, NextRotate, 0.1f);

	//速度などを加算する
	Human::Update();
}

void Enemy::Draw() {
	Human::Draw();
}