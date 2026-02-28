#pragma once
#include "GameEngine.h"
#include "OBB.h"
#include <deque>

static int id_ = 0;
const int kMaxCharacters = 4;

class Human {
public:
	// 初期化
	void Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update();

	// 描画
	void Draw();

	// ヒット時
	void OnHitRing(const float addSpeed);
	void OnHitSpike();
	void OnHitWall(OBB wallObb);

	SRT GetTransform() { return transform_; }
	Quaternion GetRollRotate() { return rollRotate_; }
	int GetID() { return characterID_; }
	bool IsTurnBack() { return isTurnBack_; }
	bool IsRewinding() { return rewindTimer_ > 0; }

protected:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;

	// トランスフォーム
	SRT transform_;
	SRT velocity_;
	//速度
	float speed_;
	const float kDefaultSpeed_ = 0.01f;

	//y軸回転
	Quaternion rollRotate_;

	//重力加速度
	const float kGravity_ = 0.098f;
	//落下最高速度
	const float kMaxFallingSpeed_ = 8.0f;
	const float kMaxRisingSpeed_ = 8.0f;
	//落下速度
	float fallingSpeed_;

	//折り返しているか
	bool isTurnBack_;

	//頭の進行角度
	Vector3 headRotate_;

	// 少し前までの位置履歴
	std::deque<Vector3> history_;
	const int maxHistory_ = 90;
	// 巻き戻しタイマー
	int rewindTimer_ = 0;
	const int kRewindTime_ = 30;

	// 無敵時間
	int invinsibleTimer_;
	const int invinsibleTimeOnHit_ = 90; // >= maxHistory

	// id
	int characterID_ = 0;
};