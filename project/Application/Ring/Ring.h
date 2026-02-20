#pragma once
#include "GameEngine.h"

class Ring {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw(const std::shared_ptr<DirectionalLight> directionalLight);

	// Getter
	Vector3 GetColliderCenter() { return colliderCenter_; }
	float GetColliderRadius() { return colliderRadius_; }
	float GetColliderHeight() { return colliderHeight_; }

private:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;

	// トランスフォーム
	SRT transform_;

	// 衝突判定
	Vector3 colliderCenter_;
	float colliderRadius_;
	float colliderHeight_ = 1.0f;

	// サイズ範囲
	float radiusMin_ = 0.25f;
	float radiusMax_ = 3.0f;

	// 出現範囲
	float spawnRadius_ = 50;
	float spawnHeight_ = 100;
	Vector3 spawnCenter_ = { 0, -60, 0 };
};

