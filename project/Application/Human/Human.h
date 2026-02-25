#pragma once
#include "GameEngine.h"

class Human {
public:
	// 初期化
	void Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update();

	// 描画
	void Draw();

	SRT GetTransform() { return transform_; }
	Quaternion GetRollRotate() { return rollRotate_; }

	//ドリフト中か
	bool isDrifting_ = false;

protected:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;

	// トランスフォーム
	SRT transform_;
	SRT velocity_;
	//速度
	float speed_;

	//y軸回転
	Quaternion rollRotate_;

	//重力加速度
	const float kGravity_ = 0.001f;
	//落下最高速度
	const float kMaxFallingSpeed_ = 0.1f;
	const float kMaxRisingSpeed_ = 0.1f;
	//落下速度
	float fallingSpeed_;

	//折り返しているか
	bool isTurnBack_;

	//頭の進行角度
	Vector3 headRotate_;
};