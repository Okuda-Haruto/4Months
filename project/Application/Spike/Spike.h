#pragma once
#include "GameEngine.h"
#include "Math/Shape/Sphere.h"

class Spike {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw(const std::shared_ptr<DirectionalLight> directionalLight);

	// 触れたとき
	void OnCollide();

	// Getter
	Sphere GetCollider() { return collider_; }

private:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;

	// トランスフォーム
	SRT transform_;

	// 衝突判定
	Sphere collider_;

	// 出現範囲
	float spawnRadius_ = 30;
	float spawnHeight_ = 300;
	Vector3 spawnCenter_ = { 0, -200, 0 };
};

