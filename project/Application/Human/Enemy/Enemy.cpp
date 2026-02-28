#include "Enemy.h"
#include <numbers>
#include <algorithm>

void Enemy::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight) {
	//初期化
	Human::Initialize(position, directionalLight);
}

void Enemy::Update() {

	//基礎クォータニオン(真下)
	Quaternion NextRotate;

	if (!isTurnBack_) {
		NextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	} else {
		NextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, std::numbers::pi_v<float> / 2);
	}
	NextRotate = NextRotate * rollRotate_;
	//基礎クオータニオン分の回転行列
	Matrix4x4 rotateMatrix = MakeRotateMatrix(NextRotate);

	if (!isTurnBack_) {
		int index = -1;
		float length = -1;

		//近いリングを参照
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

	} else {

	}

	//現在の向きと次の向きの補完
	transform_.rotate = Slerp(transform_.rotate, NextRotate, 0.1f);

	//速度などを加算する
	Human::Update();
}

void Enemy::Draw() {
	Human::Draw();
}