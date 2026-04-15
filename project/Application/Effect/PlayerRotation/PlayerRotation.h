#pragma once
#include "GameEngine.h"

class PlayerRotation {
public:
	void Initialize(std::shared_ptr<DirectionalLight> directionalLight);
	void Update(const Vector3& position, const float radius, const float rotateY, const bool isCharging, const bool isChargeMax);
	void Draw();

private:
	void EmitRotationEffect();
	void EmitRotationEffectCharged();
	void EmitChargingEffect();
	void EmitChargingEffectCharged();
	void UpdateRotationEffect(const Vector3& position);
	void UpdateChargingEffect(const Vector3& position, const bool isChargeMax);

	float prevRotateY_ = 0.0f;
	Vector3 prevPosition_{};

	// 回転エフェクト
	static constexpr int kRotationEffectCount = 200; // 発生ポイントごとの最大数
	const float kRotationLifetime = 0.2f; // 消えるまでの時間
	float t = 0.5f; // 中間生成（0.5なら完全に半フレーム位置）
	struct RotationEffect {
		std::vector<std::unique_ptr<Object>> object;
		SRT transform[kRotationEffectCount];
		bool isActivated[kRotationEffectCount];
		float lifetime[kRotationEffectCount];
	};
	RotationEffect rotEffect_[4];

	// 溜めエフェクト
	static constexpr int kChargingEffectCount = 100; // 最大数
	static constexpr float kChargingLifetime = 0.25f; // 消えるまでの時間
	const int kChargingEmitTime = 1; // 間隔
	int chargingEmitTimer_ = 0; // 出現タイマー
	static constexpr float kChargingStartRadius = 6.0f; // 出現半径
	const float kChargingRadiusShrinkSpeed = kChargingStartRadius / (kChargingLifetime * 60.0f); // 半径が縮まる速度
	const float kChargingRotateSpeed = 0.02f; // 回転速度

	const int kMaxChargingEmitTime = 0; // 間隔
	const float kMaxChargingStartRadius = 12.0f;
	const float kMaxChargingRadiusShrinkSpeed = kMaxChargingStartRadius / (kChargingLifetime * 60.0f); // 半径が縮まる速度

	struct ChargingEffect {
		Vector3 spawnPoint;
		std::vector<std::unique_ptr<Object>> object;
		float radius[kChargingEffectCount];
		SRT transform[kChargingEffectCount];
		float rotate[kChargingEffectCount]{};
		bool isActivated[kChargingEffectCount];
		float lifetime[kChargingEffectCount];
	};
	ChargingEffect chargingEffect_;

	std::shared_ptr<DirectionalLight> directionalLight_ = nullptr;
};

