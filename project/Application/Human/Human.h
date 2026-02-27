#pragma once
#include "GameEngine.h"

class Neck;
class Goal;

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

	//setter
	void SetNeck(Neck* neck) { neck_ = neck; }
	void SetGoal(Goal* goal) { goal_ = goal; }

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
	const float kGravity_ = 0.02f;
	//落下最高速度
	const float kMinSpeed_ = 0.05f;
	const float kMaxFallingSpeed_ = 0.2f;
	const float kMaxRisingSpeed_ = 0.2f;
	//落下速度
	float fallingSpeed_;

	//折り返しているか
	bool isTurnBack_;

	//頭の進行角度
	Vector3 headRotate_;

	//軸になる可能性のある首
	Neck* neck_;

	//ゴール
	Goal* goal_ = nullptr;

	//巻き付き可能距離
	const float kCanCoilAroundRange_ = 5.0f;
	//巻き付く場合の首の中心からの距離
	const float kCoilAroundRange_ = 4.0f;
	//巻き付いているか
	bool isCoilAround_ = false;
	//現在巻き付いている首の番号
	int32_t neckCoilAroundIndex_;
	//線形補間位置
	float coilAroundDistance_;

};