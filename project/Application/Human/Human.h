#pragma once
#include "GameEngine.h"
#include "OBB.h"
#include <deque>

static int id_ = 0;
const int kMaxCharacters = 4;

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

	// ヒット時
	void OnHitRing(const float addSpeed, const float addMaxSpeed);
	void OnHitSpike(const Vector3& pos);
	void OnHitWall(OBB wallObb);
	void OnHitNeck(const Vector3& pos);

	bool GetIsCoilAround() const;

	SRT GetTransform() { return transform_; }
	Quaternion GetRollRotate() { return rollRotate_; }
	int GetID() { return characterID_; }
	float GetSpeed() { return fabsf(velocity_.translate.y); }
	//首の色取得
	const Vector4& GetColor() const { return color_; }

	bool IsTurnBack() { return isTurnBack_; }
	bool IsCoilAround() { return isCoilAround_; }
	bool IsInvincible() { return invincibleTimer_ > 0; }

	//ドリフト中か
	bool isDrifting_ = false;

	//setter
	void SetNecks(std::vector<std::shared_ptr<Neck>> necks) { necks_ = necks; }
	void SetGoal(Goal* goal) { goal_ = goal; }
	void SetSelfNeckIndex(int selfNeckIndex) { selfNeckIndex_ = selfNeckIndex; }
	void SetCameraEffectTime(float cameraEffectTime) { cameraEffectTime_ = cameraEffectTime; }

	//getter
	float GetCameraEffectTime() { return cameraEffectTime_; }

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
	const float kGravity_ = 0.005f;
	//落下最高速度
	const float kMinSpeed_ = 0.05f;
	float maxFallingSpeed_ = 0.2f;
	float maxRisingSpeed_ = 0.2f;
	const float kDefaultMaxFallingSpeed_ = 0.2f;
	const float kDefaultMaxRisingSpeed_ = 0.2f;
	//落下速度
	float fallingSpeed_;

	//折り返しているか
	bool isTurnBack_;

	//頭の進行角度
	Vector3 headRotate_;

	// 無敵時間
	int invincibleTimer_;
	const int invincibleTimeOnHit_ = 60;

	// ドリフト、巻きつき不可時間
	int unableDriftTimer_;
	const int unableDriftTime_ = 120;

	//カメラ演出(プレイヤー用)
	const float kMaxCameraEffectTime_ = 1.0f;
	float cameraEffectTime_ = 0.0f;
	// 最低移動速度
	const float kDefaultSpeed_ = 0.2f;

	// ノックバック中
	int knockbackTimer_;
	const int kKnockbackTime_ = 20;

	// id
	int characterID_ = 0;

	//ゴール
	Goal* goal_ = nullptr;

	//軸になる可能性のある首
	std::vector<std::shared_ptr<Neck>> necks_;
	//自分の首の番号
	int selfNeckIndex_ = 0;
	//自分の首が対象にならない範囲の下限
	uint32_t noTargetMinNumber_;

	//巻き付き可能距離
	const float kCanCoilAroundRange_ = 5.0f;
	//巻き付く場合の首の中心からの距離
	const float kCoilAroundRange_ = 2.0f;
	//巻き付いているか
	bool isCoilAround_ = false;
	//現在巻き付いている首(一本)の番号
	int32_t neckCoilAroundIndex_;
	//現在巻き付いている首(単体)の番号
	int32_t neckCoilAroundNumber_;
	//線形補間位置
	float coilAroundDistance_;

	//巻き付き始点移動時間
	float coilAroundStartTime_;
	//巻き付き終端移動時間
	float coilAroundEndTime_;
	//巻き付き開始位置
	Vector3 coilAroundRotatePos_;
	//巻き付き開始番号
	int32_t coilAroundStartNumber_;

	//首の色
	Vector4 color_ = { 1,1,1,1 };
};