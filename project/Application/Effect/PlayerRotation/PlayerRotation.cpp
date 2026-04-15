#include "PlayerRotation.h"
#include <numbers>

void PlayerRotation::Initialize(std::shared_ptr<DirectionalLight> directionalLight) {
	directionalLight_ = directionalLight_;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < kRotationEffectCount; ++j) {
			rotEffect_[i].isActivated[j] = false;
			rotEffect_[i].transform[j] = { {1,1,1},{},{} };

			std::unique_ptr<Object> model;
			model = std::make_unique<Object>();
			model->Initialize(ModelManager::GetInstance()->GetModel("resources/Effect/Rotate", "RotationEffect.obj"));
			model->SetDirectionalLight(directionalLight);
			model->SetShininess(0);
			rotEffect_[i].object.push_back(std::move(model));
		}
	}

	for (int i = 0; i < kChargingEffectCount; ++i) {
		chargingEffect_.transform[i] = { {1,1,1},{},{} };
		std::unique_ptr<Object> model;
		model = std::make_unique<Object>();
		model->Initialize(ModelManager::GetInstance()->GetModel("resources/Effect/Rotate", "ChargingEffect.obj"));
		model->SetDirectionalLight(directionalLight);
		model->SetShininess(0);
		model->SetTransform(chargingEffect_.transform[i]);
		model->SetColor({ 1.0f,0.5f,0,1 });
		chargingEffect_.object.push_back(std::move(model));
	}
}

void PlayerRotation::Update(const Vector3& position, const float radius, const float rotateY, const bool isCharging, const bool isChargeMax) {
	// 回転エフェクト
	Vector3 diff = position - prevPosition_;
	float dist = Length(diff);

	// 距離に応じて分割数を決める
	int steps = std::clamp((int)(dist / 0.1f), 1, 10);

	for (int k = 0; k < steps; ++k) {
		float t = (float)k / steps;

		Vector3 pos = prevPosition_ + diff * t;
		float ry = prevRotateY_ + (rotateY - prevRotateY_) * t;

		float r = -ry;
		Vector3 forward = { sin(r),0,cos(r) };
		Vector3 right = { cos(r),0,-sin(r) };

		Vector3 spawn[4];
		spawn[0] = pos + forward * radius;
		spawn[1] = pos - forward * radius;
		spawn[2] = pos + right * radius;
		spawn[3] = pos - right * radius;

		for (int i = 0; i < 4; ++i) {
			// エフェクト発生
			for (int j = 0; j < kRotationEffectCount; ++j) {
				if (!rotEffect_[i].isActivated[j]) {
					rotEffect_[i].isActivated[j] = true;
					rotEffect_[i].lifetime[j] = kRotationLifetime;
					rotEffect_[i].transform[j].scale = { 2,2,2 };
					float angle = atan2(spawn[i].x - pos.x, pos.z - spawn[i].z);
					Quaternion faceOut = MakeRotateAxisAngleQuaternion({ 0,1,0 }, angle);
					rotEffect_[i].transform[j].rotate = faceOut;
					rotEffect_[i].transform[j].translate = spawn[i];
					rotEffect_[i].object[j]->SetTransform(rotEffect_[i].transform[j]);
					break;
				}
			}
		}
	}

	UpdateRotationEffect(position);
	prevRotateY_ = rotateY;
	prevPosition_ = position;

	// 溜めエフェクト
	if (isCharging) {
		if (isChargeMax) {
			if (chargingEmitTimer_ >= kMaxChargingEmitTime) {
				chargingEmitTimer_ = 0;
				for (int i = 0; i < 2; ++i) {
					EmitChargingEffectCharged();
				}
			}
		} else {
			if (chargingEmitTimer_ >= kChargingEmitTime) {
				chargingEmitTimer_ = 0;
				for (int i = 0; i < 2; ++i) {
					EmitChargingEffect();
				}
			}
		}
		chargingEmitTimer_++;
	}

	// 発生しているエフェクトの処理
	UpdateChargingEffect(position, isChargeMax);

}

void PlayerRotation::Draw() {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < kRotationEffectCount; ++j) {
			if (rotEffect_[i].isActivated[j]) {
				rotEffect_[i].object[j]->Draw3D();
			}
		}
	}

	for (int i = 0; i < kChargingEffectCount; ++i) {
		if (chargingEffect_.isActivated[i]) {
			chargingEffect_.object[i]->Draw3D();
		}
	}
}

void PlayerRotation::EmitChargingEffect() {
	for (int i = 0; i < kChargingEffectCount; ++i) {
		// エフェクト発生
		if (!chargingEffect_.isActivated[i]) {
			// 初期回転位置
			chargingEffect_.radius[i] = kChargingStartRadius;
			chargingEffect_.rotate[i] = GameEngine::randomFloat(0.0f, 2.0f * float(std::numbers::pi));
			float rotate = chargingEffect_.rotate[i];
			Vector3 dir = { sin(rotate),0,cos(rotate) };

			chargingEffect_.isActivated[i] = true;
			chargingEffect_.lifetime[i] = kChargingLifetime;
			chargingEffect_.transform[i].scale = { 1,1,1 };
			chargingEffect_.transform[i].translate = chargingEffect_.spawnPoint + dir * chargingEffect_.radius[i];
			chargingEffect_.object[i]->SetTransform(chargingEffect_.transform[i]);
			break;
		}
	}
}

void PlayerRotation::EmitChargingEffectCharged() {
	for (int i = 0; i < kChargingEffectCount; ++i) {
		// エフェクト発生
		if (!chargingEffect_.isActivated[i]) {
			// 初期回転位置
			chargingEffect_.radius[i] = kMaxChargingStartRadius;
			chargingEffect_.rotate[i] = GameEngine::randomFloat(0.0f, 2.0f * float(std::numbers::pi));
			float rotate = chargingEffect_.rotate[i];
			Vector3 dir = { sin(rotate),0,cos(rotate) };

			chargingEffect_.isActivated[i] = true;
			chargingEffect_.lifetime[i] = kChargingLifetime;
			chargingEffect_.transform[i].scale = { 1.5f,1.5f,1.5f };
			chargingEffect_.transform[i].translate = chargingEffect_.spawnPoint + dir * chargingEffect_.radius[i];
			chargingEffect_.object[i]->SetTransform(chargingEffect_.transform[i]);
			break;
		}
	}
}

void PlayerRotation::UpdateRotationEffect(const Vector3& position) {
	// 発生しているエフェクトの処理
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < kRotationEffectCount; ++j) {
			if (rotEffect_[i].isActivated[j]) {
				// 移動
				rotEffect_[i].transform[j].translate.y = position.y + (1.0f - rotEffect_[i].lifetime[j] / kRotationLifetime) * 0.05f; // プレイヤーに近い高さにする
				rotEffect_[i].object[j]->SetTransform(rotEffect_[i].transform[j]);
				rotEffect_[i].object[j]->SetColor({ 1,1,0,rotEffect_[i].lifetime[j] / kRotationLifetime });
				rotEffect_[i].lifetime[j] -= 1.0f / 60.0f;
				if (rotEffect_[i].lifetime[j] <= 0) {
					rotEffect_[i].isActivated[j] = false;
				}
			}
		}
	}
}

void PlayerRotation::UpdateChargingEffect(const Vector3& position, const bool isChargeMax) {
	for (int i = 0; i < kChargingEffectCount; ++i) {
		if (chargingEffect_.isActivated[i]) {
			// 半径を縮める
			if (isChargeMax) {
				chargingEffect_.radius[i] -= kMaxChargingRadiusShrinkSpeed;
			} else {
				chargingEffect_.radius[i] -= kChargingRadiusShrinkSpeed;
			}

			// 移動
			chargingEffect_.rotate[i] += kChargingRotateSpeed;
			float rotate = chargingEffect_.rotate[i];
			Vector3 dir = { sin(rotate),0,cos(rotate) };
			chargingEffect_.transform[i].translate = position + dir * chargingEffect_.radius[i];

			// サイズ
			float size = 1;
			if (chargingEffect_.lifetime[i] < 0.2f) {
				size = 1.0f * (chargingEffect_.lifetime[i] / 0.2f);
				chargingEffect_.transform[i].scale = { size,size,size };
			} else if (chargingEffect_.lifetime[i] > 0.8f) {
				float t = (chargingEffect_.lifetime[i] - 0.8f) / 0.2f;
				size = 1.0f - t;
			}
			chargingEffect_.transform[i].scale = { size,size,size };
			chargingEffect_.object[i]->SetTransform(chargingEffect_.transform[i]);

			chargingEffect_.lifetime[i] -= 1.0f / 60.0f;
			if (chargingEffect_.lifetime[i] <= 0) {
				chargingEffect_.isActivated[i] = false;
			}
		}
	}
}
