#pragma once
#include "GameEngine.h"

class PlayerRotation {
public:
	void Initialize(std::shared_ptr<DirectionalLight> directionalLight);
	void Update(const Vector3& position, const float radius, const float rotateY);
	void Draw();

private:
	// 回転エフェクト
	static const int kRotationEffectCount = 10; // 発生ポイントごとの最大数
	const float kRotationLifetime = 1.0f;
	struct RotationEffect {
		Vector3 spawnPoint;
		std::vector<std::unique_ptr<Object>> object;
		SRT transform[kRotationEffectCount];
		bool isActivated[kRotationEffectCount];
		float lifetime[kRotationEffectCount];
	};
	RotationEffect rotEffect_[4]; // こまの4箇所から発生

	// 溜めエフェクト
	static const int kChargingEffectCount = 100; // 最大数
	const float kChargingLifetime = 1.0f;
	struct ChargingEffect {
		Vector3 spawnPoint;
		std::vector<std::unique_ptr<Object>> object;
		SRT transform[kChargingEffectCount];
		bool isActivated[kChargingEffectCount];
		float lifeTime[kChargingEffectCount];
	};
	ChargingEffect chargingEffect_;
};

