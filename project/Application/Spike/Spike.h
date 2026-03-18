#pragma once
#include "GameEngine.h"
#include "Math/Shape/Sphere.h"

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

	// Getter
	Sphere GetCollider() { return collider_; }

	void Move(const Vector3& velocity);

private:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;

	// トランスフォーム
	SRT transform_;

	// 衝突判定
	Sphere collider_;
};

