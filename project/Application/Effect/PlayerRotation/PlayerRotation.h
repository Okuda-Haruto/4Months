#pragma once
#include "GameEngine.h"

class PlayerRotation {
public:
	void Initialize(std::shared_ptr<DirectionalLight> directionalLight, std::shared_ptr<Camera> camera);
	void Update(const Vector3& playerPos,const Vector3& position, const float radius, const float rotateY, const bool isCharging, const bool isChargeMax);
	void Draw();

	// 撃ったとき
	void Shoot();

	// 帰ってきたとき
	void Catch();

private:
	void EmitRotationEffect(const Vector3& position, const float rotateY, const float radius);
	void EmitChargingEffect(bool isChargeMax);
	void UpdateRotationEffect(const Vector3& position);
	void UpdateChargingEffect(const Vector3& position, const bool isChargeMax);

	float prevRotateY_ = 0.0f;
	Vector3 prevPosition_{};

	// 回転エフェクト
	static constexpr int kRotationEffectCount = 200; // 発生ポイントごとの最大数
	const float kRotationLifetime = 0.5f; // 消えるまでの時間

	struct RotationEffect {
		std::vector<std::unique_ptr<Object>> object;
		SRT transform[kRotationEffectCount];
		bool isActivated[kRotationEffectCount];
		float lifetime[kRotationEffectCount];
	};
	RotationEffect rotEffect_[4];

	// 溜めエフェクト
	static constexpr int kChargingEffectCount = 100; // 最大数
	const float kChargingEmitTime = 0.05f; // 間隔
	float chargingEmitTimer_ = 0; // 出現タイマー
	static constexpr float kChargingStartRadius = 8.0f; // 出現半径
	const float kChargingRadiusShrinkSpeed = 0.15f * 60.0f; // 半径が縮まる速度
	const float kChargingRotateSpeed = 0.05f * 60.0f; // 回転速度

	const float kMaxChargingEmitTime = 0.04f; // 間隔
	const float kMaxChargingStartRadius = 12.0f;
	const float kMaxChargingRadiusShrinkSpeed = 0.25f * 60.0f; // 半径が縮まる速度

	// 撃った直後
	bool isShooting_ = false;
	float afterShootTimer_ = 0;
	const int kPulseEmitCount = 30;
	const float kPulseTime = 0.6f;
	const float kPulseSpeed = -0.5f * 60.0f;
	const float kPulseRotateSpeed = 0.6f * 60.0f;

	struct ChargingEffect {
		Vector3 spawnPoint;
		std::vector<std::unique_ptr<Object>> object;
		float radius[kChargingEffectCount];
		SRT transform[kChargingEffectCount];
		float rotate[kChargingEffectCount]{};
		bool isActivated[kChargingEffectCount];
	};
	ChargingEffect chargingEffect_;

	// 戻ってくる時
	bool isCatching_ = false;
	std::unique_ptr<Sprite> ring_;
	float ringShrinkSpeed_ = 80.0f * 60.0f;
	Vector2 ringStartScale_ = { 2000,2000 };

	std::shared_ptr<Camera> camera_ = nullptr;
	std::shared_ptr<DirectionalLight> directionalLight_ = nullptr;
};

