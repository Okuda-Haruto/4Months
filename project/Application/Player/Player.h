#pragma once
#include "GameEngine.h"

class Player {
public:
	// 初期化
	void Initialize(const std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update(const std::shared_ptr<Input> input);

	// 描画
	void Draw();

	SRT GetTransform() { return transform_; }

private:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;

	// トランスフォーム
	SRT transform_;
	SRT velocity_;

	const float kGravity_ = 0.098f;
	const float kMaxFallingSpeed_ = -4.0f;
	float fallingSpeed_;

	//頭の進行角度
	Vector3 headRotate_;
};

