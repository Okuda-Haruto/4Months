#pragma once
#include "GameEngine.h"
#include "Math/Shape/Sphere.h"

class Human;

class Spike {
public:
	// 初期化
	void Initialize(const Vector3& spawnPos, const float radius);
	
	// 更新
	void Update();

	// 描画
	void Draw(const std::shared_ptr<DirectionalLight> directionalLight);

	// 触れたとき
	void OnCollide();

	//setter
	void SetTransform(SRT transform) { transform_ = transform; }
	void SetRotate(Quaternion rotate) { transform_.rotate = rotate; }
	void SetSpeed(float speed) { speed_ = speed; }
	void SetTarget(Human* target) { target_ = target; }

	// Getter
	Sphere GetCollider() { return collider_; }
	bool IsFire() { return speed_ > 0.0f; }
	bool IsDead() { return isDead_; }


private:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;

	// トランスフォーム
	SRT transform_;
	float speed_;

	Human* target_ = nullptr;

	// 衝突判定
	Sphere collider_;

	// 出現範囲
	float spawnRadius_ = 30;
	float spawnHeight_ = 300;
	Vector3 spawnCenter_ = { 0, -200, 0 };

	//発射時間
	float firingTime ;
	bool isDead_ = false;
	
};

