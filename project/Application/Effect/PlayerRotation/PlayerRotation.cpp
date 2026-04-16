#include "PlayerRotation.h"
#include "HUD/HUD.h"
#include <numbers>

void PlayerRotation::Initialize(std::shared_ptr<DirectionalLight> directionalLight, std::shared_ptr<Camera> camera) {
	directionalLight_ = directionalLight_;
	camera_ = camera;

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
		model->SetColor({ 1.0f,0.5f,0,0.8f });
		chargingEffect_.object.push_back(std::move(model));
	}

	ring_ = std::make_unique<Sprite>();
	ring_->Initialize("resources/Effect/Rotate/ring.png");
	ring_->SetAnchorPoint({ 0.5f,0.5f });
	ring_->SetColor({ 1,1,1,0.7f });
	ring_->Update();
}

void PlayerRotation::Update(const Vector3& position, const float radius, const float rotateY, const bool isCharging, const bool isChargeMax) {
	// 回転エフェクト
	EmitRotationEffect(position, rotateY, radius);
	UpdateRotationEffect(position);
	prevRotateY_ = rotateY;
	prevPosition_ = position;

	// 溜めエフェクト
	if (isCharging) {
		if (isChargeMax) {
			if (chargingEmitTimer_ >= kMaxChargingEmitTime) {
				chargingEmitTimer_ = 0;
				EmitChargingEffect(isChargeMax);
			}
		} else {
			if (chargingEmitTimer_ >= kChargingEmitTime) {
				chargingEmitTimer_ = 0;
				EmitChargingEffect(isChargeMax);
			}
		}
		chargingEmitTimer_++;
	}

	if (isShooting_) {
		afterShootTimer_ -= 1.0f / 60.0f;
		if (afterShootTimer_ <= 0) {
			isShooting_ = false;

			for (int i = 0; i < kChargingEffectCount; ++i) {
				chargingEffect_.isActivated[i] = false;
			}
		}
	}

	// 発生しているエフェクトの処理
	UpdateChargingEffect(position, isChargeMax);


	if (isCatching_) {
		Vector2 size = ring_->GetSize();
		size = size - Vector2{ringShrinkSpeed_, ringShrinkSpeed_};
		ring_->SetPosition(ToScreen(camera_,position));
		ring_->SetSize(size);
		ring_->Update();

		if (size.x <= 0) {
			isCatching_ = false;
		}
	}
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

	if (isCatching_) {
		ring_->Draw2D();
	}
}

void PlayerRotation::Shoot() {
	isShooting_ = true;
	afterShootTimer_ = kPulseTime;

	// 一斉にエフェクト
	for (int i = 0; i < kChargingEffectCount; ++i) {
		chargingEffect_.isActivated[i] = false;
	}

	for (int i = 0; i < kPulseEmitCount; ++i) {
		EmitChargingEffect(true);
	}
}

void PlayerRotation::Catch() {
	isCatching_ = true;
	ring_->SetSize(ringStartScale_);
}

void PlayerRotation::EmitRotationEffect(const Vector3& position, const float rotateY, const float radius) {
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
}

void PlayerRotation::EmitChargingEffect(bool isChargeMax) {
	for (int i = 0; i < kChargingEffectCount; ++i) {
		// エフェクト発生
		if (!chargingEffect_.isActivated[i]) {
			// 初期回転位置
			if (isShooting_) {
				chargingEffect_.radius[i] = 0;
			}else if (isChargeMax) {
				chargingEffect_.radius[i] = kMaxChargingStartRadius;
			} else {
				chargingEffect_.radius[i] = kChargingStartRadius;
			}
			chargingEffect_.rotate[i] = GameEngine::randomFloat(0.0f, 2.0f * float(std::numbers::pi));
			float rotate = chargingEffect_.rotate[i];
			Vector3 dir = { sin(rotate),0,cos(rotate) };

			chargingEffect_.isActivated[i] = true;
			float size = 0.65f;
			chargingEffect_.transform[i].scale = { size,size,size * 4 };
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
			float r = chargingEffect_.rotate[i];

			// 円の基準ベクトル
			Vector3 radial = { sin(r), 0, cos(r) };        // 外向き
			Vector3 tangent = { cos(r), 0, -sin(r) };      // 回転方向（90度ずらし）

			// 半径変化量（内向き）
			float shrink = 0;
			if (isShooting_) {
				shrink = kPulseSpeed;
			} else if (isChargeMax) {
				shrink = kMaxChargingRadiusShrinkSpeed;
			} else {
				shrink = kChargingRadiusShrinkSpeed;
			}

			// 回転速度
			float rotSpeed = 0;
			if (isShooting_) {
				rotSpeed = kPulseRotateSpeed;
			} else if(isChargeMax){
				rotSpeed = -kChargingRotateSpeed * 1.5f;
			} else {
				rotSpeed = -kChargingRotateSpeed;
			}

			// 進行方向
			Vector3 velocity = Normalize(tangent * rotSpeed * chargingEffect_.radius[i] - radial * shrink);

			// 正規化
			velocity = Normalize(velocity);

			// 位置更新（今まで通り）
			chargingEffect_.radius[i] -= shrink;
			chargingEffect_.rotate[i] += rotSpeed;

			Vector3 dir = { sin(chargingEffect_.rotate[i]), 0, cos(chargingEffect_.rotate[i]) };
			chargingEffect_.transform[i].translate = position + dir * chargingEffect_.radius[i];

			// 進行方向を向かせる
			float angle = atan2(velocity.x, -velocity.z);
			chargingEffect_.transform[i].rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, angle);

			// サイズ
			chargingEffect_.object[i]->SetTransform(chargingEffect_.transform[i]);

			if (chargingEffect_.radius[i] <= 0.05f) {
				chargingEffect_.isActivated[i] = false;
				continue;
			}
		}
	}
}
