#pragma once
#include "GameEngine.h"
#include "OBB.h"
#include "Sphere.h"
#include "Effect/Wind/Wind.h"
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
	void OnHitVoxel(Vector3 translate);
	void StopBullet(const Vector3& hitPos) { headTransform_.translate = hitPos; headSpeed_ = 0; returnTime_ /= 3; }

	SRT GetTransform() { return transform_; }
	float GetSpeed() { return speed_; }

	bool IsInvincible() { return invincibleTimer_ > 0; }
	bool IsVacuuming() { return vacuumState_ == Vacuum; }
	bool IsShooting() { return vacuumState_ == Going; }
	bool IsCharging() { return isCharging_; }

	//ドリフト中か
	bool isDrifting_ = false;

	//setter
	void SetCameraEffectTime(float cameraEffectTime) { cameraEffectTime_ = cameraEffectTime; }

	//getter
	float GetCameraEffectTime() { return cameraEffectTime_; }
	Sphere GetVacuumSphere() { return Sphere(headTransform_.translate,vacuumRadius_); }
	Sphere GetPrevVacuumSphere() { return Sphere(headPrevTransform_.translate,vacuumRadius_); }
	float GetCharge() { return charge_; }
	float GetMaxCharge() { return kMaxCharge_; }
	Vector3 GetVacuumStartPos() { return vacuumStartPos_; }
	float GetFallingSpeed() { return fallingSpeed_; }

protected:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;
	std::unique_ptr<Object> bulletModel_ = nullptr;

	// トランスフォーム
	SRT transform_;
	SRT velocity_;
	//速度
	float speed_;

	//重力加速度
	const float kGravity_ = 0.25f;
	//落下最高速度
	const float kMinSpeed_ = 0.05f;
	float maxFallingSpeed_ = 4.5f;
	//落下速度
	float fallingSpeed_;

	//頭の進行角度
	Vector3 headRotate_;

	// 無敵時間
	int invincibleTimer_;
	const int invincibleTimeOnHit_ = 45;

	//カメラ演出(プレイヤー用)
	const float kMaxCameraEffectTime_ = 1.0f;
	float cameraEffectTime_ = 0.0f;
	// 最低移動速度
	const float kDefaultSpeed_ = 0.6f;

	// ノックバック中
	int knockbackTimer_;
	const int kKnockbackTime_ = 20;

	// 吸引
	enum VacuumState {
		None,
		Going,
		Vacuum,
		Return,
	};
	VacuumState vacuumState_;

	// 先頭をとばすとき
	SRT headTransform_;
	SRT headPrevTransform_;
	Vector3 headDir_;
	float headSpeed_;
	float headStartSpeed_ = 3.0f;
	float headDeceleration_ = 0.08f;
	int returnTime_ = 30;
	const float kMinReturnTime = 15;
	const float kMaxReturnTime = 70;
	int returnTimer_ = 0;
	float bounceBackSpeed_ = 11.0f;

	// 吸引
	int vacuumTime_ = 30;
	int vacuumTimer_ = 30;
	float vacuumRadius_ = 0.0f; 
	const float baseVacuumRadius_ = 7.0f; 
	Vector3 vacuumStartPos_;
	const int kMinVacuumTime = 20;
	const int kMaxVacuumTime = 40;

	float charge_ = 0;
	const float kChargeSpeed_ = 1.5f;
	const float kMaxCharge_ = 100;
	bool isCharging_ = false;

	std::unique_ptr<Wind> wind_ = nullptr;

	void Throw();
	void Charge();
	void Slowdown();
	Vector3 CalcVacuumPosition();
};