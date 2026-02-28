#pragma once
#include "GameEngine.h"
#include "Human/Human.h"

class Ring {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw(const std::shared_ptr<DirectionalLight> directionalLight);

	// 触れたとき
	void OnCollide(const int id);

	// Getter
	Vector3 GetColliderCenter() { return colliderCenter_; }
	float GetColliderRadius() { return colliderRadius_; }
	float GetColliderHeight() { return colliderHeight_; }
	float GetBoostAmount() { return boostAmount_; }
	bool IsCoolDown(const int id);

private:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;

	// トランスフォーム
	SRT transform_;

	// 衝突判定
	Vector3 colliderCenter_;
	float colliderRadius_;
	float colliderHeight_ = 0.5f;

	// サイズ範囲
	float radiusMin_ = 0.5f;
	float radiusMax_ = 3.0f;

	// 出現範囲
	float spawnRadius_ = 10;
	float spawnHeight_ = 100;
	Vector3 spawnCenter_ = { 0, -60, 0 };

	// 速度増加量
	float boostAmount_ = 0.01f;
	// クールダウン
	const int boostCoolDown_ = 20;

	// 触れた対象を記録して触れないようにする
	int characterCoolDown[kMaxCharacters]{};
};

