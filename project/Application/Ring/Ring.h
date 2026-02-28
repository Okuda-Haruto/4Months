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

	// 触れたとき
	void OnCollide();

	// Getter
	Vector3 GetColliderCenter() { return colliderCenter_; }
	float GetColliderRadius() { return colliderRadius_; }
	float GetColliderHeight() { return colliderHeight_; }

	//ソート用
	bool operator<(const Ring& another) const
	{
		return transform_.translate.y < another.transform_.translate.y;
	};

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
	float radiusMin_ = 0.5f;
	float radiusMax_ = 3.0f;

	// 出現範囲
	float spawnRadius_ = 30;
	float spawnHeight_ = 300;
	Vector3 spawnCenter_ = { 0, -60, 0 };
};

