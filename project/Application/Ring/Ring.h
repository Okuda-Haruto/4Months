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

private:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;

	// トランスフォーム
	SRT transform_;

	// 出現範囲
	float spawnRadius_ = 50;
	float spawnHeight_ = 100;
	Vector3 spawnCenter_ = { 0, -60, 0 };
};

