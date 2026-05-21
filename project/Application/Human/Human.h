#pragma once
#include "GameEngine.h"
#include "OBB.h"
#include "Sphere.h"
#include "Effect/Wind/Wind.h"
#include "Effect/PlayerRotation/PlayerRotation.h"
#include <deque>

static int id_ = 0;
const int kMaxCharacters = 4;

class Human {
public:
	// 初期化
	void Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight, const std::shared_ptr<Camera> camera);

	// 更新
	void Update();

	// 描画
	void Draw();

	// ヒット時
	void OnHitVoxel(AABB aabb);
	void StopBullet(const Vector3& hitPos) { headTransform_.translate = hitPos; headSpeed_ = 0; returnTime_ /= 3; }

	void BreakSpinner();

	SRT GetTransform() { return transform_; }
	float GetSpeed() { return speed_; }
	Vector3 GetVelocity() { return velocity_.translate + knockBackVelocity_; }

	bool IsInvincible() { return invincibleTimer_ > 0; }
	bool IsVacuuming() { return vacuumState_ == Vacuum; }
	bool IsShooting() { return vacuumState_ == Going; }
	bool CanShoot() { return vacuumState_ == None; }
	bool IsCharging() { return isCharging_; }

	//setter
	void SetCameraEffectTime(float cameraEffectTime) { cameraEffectTime_ = cameraEffectTime; }
	void ResetPos(const Vector3& pos) { transform_.translate = pos; vacuumState_ = None; }
	void SetTranslate(const Vector3& pos) { transform_.translate = pos; vacuumState_ = None; }
	void ApproachCenter(const Vector2& center);
	void RepairSpinner() { isBreak_ = false; }

	//getter
	float GetCameraEffectTime() { return cameraEffectTime_; }
	Sphere GetVacuumSphere() { return Sphere(headTransform_.translate,vacuumRadius_); }
	Sphere GetPrevVacuumSphere() { return Sphere(headPrevTransform_.translate,vacuumRadius_); }
	float GetCharge() { return charge_; }
	float GetMaxCharge() { return kMaxCharge_; }
	Vector3 GetVacuumStartPos() { return vacuumStartPos_; }
	float GetFallingSpeed() { return fallingSpeed_; }
	bool IsResult() { return isResult_; }
	bool IsBreak() { return isBreak_; }

	// ★これ追加
	void SetResult(bool flag);
	void SetCanSkipResult(bool canSkip) { canSkipResult_ = canSkip; }
	bool isAutoBurst_ = true;
	bool isJumpFlashMode_ = false;
protected:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;
	std::unique_ptr<Object> bulletModel_ = nullptr;

	std::unique_ptr<Object> bulletModel_Break_ = nullptr;
	//壊れたパーツごとのtransform
	std::vector<SRT> breakBulletTransform_;
	//壊れたパーツが動く向き
	std::vector<Quaternion> breakBulletDirection_;
	float breakBulletSpeed_ = 1.0f;
	//壊れたパーツの回転
	std::vector<Quaternion> breakBulletRotateVelocity_;

	// トランスフォーム
	SRT transform_;
	SRT velocity_;
	//速度
	float speed_;

	//重力加速度
	const float kGravity_ = 0.1f;
	//落下最高速度
	const float kMinSpeed_ = 0.05f;
	float maxFallingSpeed_ = 5.0f;
	//落下速度
	float fallingSpeed_;

	// 無敵時間
	int invincibleTimer_;
	const int invincibleTimeOnHit_ = 45;

	//カメラ演出(プレイヤー用)
	const float kMaxCameraEffectTime_ = 1.0f;
	float cameraEffectTime_ = 0.0f;
	// 最低移動速度
	const float kDefaultSpeed_ = 0.4f;

	// ノックバック中
	Vector3 knockBackAcceleration_;
	Vector3 knockBackVelocity_;
	const float kNockBackSpeed_ = 1.5f;
	const float kNockBackFallingSpeed_ = 4.0f;

	// 吸引
	enum VacuumState {
		None,
		Going,
		Vacuum,
		Return,
		Break,
	};
	VacuumState vacuumState_;

	// 先頭をとばすとき
	SRT headTransform_;
	SRT headPrevTransform_;
	Vector3 headDir_;
	float headSpeed_;
	float headStartSpeed_ = 3.0f;
	float headDeceleration_ = 0.08f;
	float returnTime_ = 30 / 60.0f;
	const float kMinReturnTime = 15 / 60.0f;
	const float kMaxReturnTime = 70 / 60.0f;
	float returnTimer_ = 0;
	float bounceBackSpeed_ = 5.0f;

	// 飛ばすコマの回転
	std::unique_ptr<PlayerRotation> headRotateEffect_ = nullptr;
	float headRotate_ = 0;

	// 吸引
	float vacuumTime_ = 30 / 60.0f;
	float vacuumTimer_ = 30 / 60.0f;
	float vacuumRadius_ = 0.0f; 
	const float baseVacuumRadius_ = 7.0f; 
	Vector3 vacuumStartPos_;
	const float kMinVacuumTime = 20 / 60.0f;
	const float kMaxVacuumTime = 40 / 60.0f;

	float charge_ = 0;
	const float kChargeSpeed_ = 1.0f;
	const float kMaxCharge_ = 100;
	bool isCharging_ = false;

	std::unique_ptr<Wind> wind_ = nullptr;

	void Throw();
	void Charge();
	void Slowdown();
	Vector3 CalcVacuumPosition();

	bool stop;
	bool isResult_;
	bool canSkipResult_;

	float humanFootOffset = 2.4f;

	// リザルト中のループ地点
	float resultLoopStartY = 0;
	float resultLoopEndY = 0;

	//ゲームオーバー演出
	bool isBreak_ = false;
	
	// SE
	std::unique_ptr<Audio> shootSE_ = nullptr;
	std::unique_ptr<Audio> catchSE_ = nullptr;
	std::unique_ptr<Audio> chargeSE_ = nullptr;
};