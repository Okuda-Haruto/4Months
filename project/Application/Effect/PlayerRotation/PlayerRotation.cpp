#include "PlayerRotation.h"

void PlayerRotation::Initialize(std::shared_ptr<DirectionalLight> directionalLight) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < kRotationEffectCount; ++j) {
			rotEffect_[i].isActivated[j] = false;
			rotEffect_[i].transform[j] = { {1,1,1},{},{} };

			std::unique_ptr<Object> model;
			model = std::make_unique<Object>();
			model->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/sphere", "sphere.obj"));
			model->SetDirectionalLight(directionalLight);
			model->SetShininess(0);
			model->Update();
			rotEffect_[i].object.push_back(std::move(model));
		}
	}

	for (int i = 0; i < kChargingEffectCount; ++i) {
		chargingEffect_.transform[i] = { {1,1,1},{},{} };
		std::unique_ptr<Object> model;
		model = std::make_unique<Object>();
		model->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/sphere", "sphere.obj"));
		model->SetDirectionalLight(directionalLight);
		model->SetShininess(0);
		chargingEffect_.object.push_back(std::move(model));
	}
}

void PlayerRotation::Update(const Vector3& position, const float radius, const float rotateY) {
	// 発生座標を決定
	Vector3 forward = {
	sin(rotateY),
	0,
	cos(rotateY)
	};

	Vector3 right = {
	cos(rotateY),
	0,
	-sin(rotateY)
	};

	rotEffect_[0].spawnPoint = position + forward * radius;  // 前
	rotEffect_[1].spawnPoint = position - forward * radius;  // 後
	rotEffect_[2].spawnPoint = position + right * radius;    // 右
	rotEffect_[3].spawnPoint = position - right * radius;    // 左

	for (int i = 0; i < 4; ++i) {
		// エフェクト発生
		for (int j = 0; j < kRotationEffectCount; ++j) {
			if (!rotEffect_[i].isActivated[j]) {
				rotEffect_[i].isActivated[j] = true;
				rotEffect_[i].lifetime[j] = kRotationLifetime;
				rotEffect_[i].transform[j].translate = rotEffect_[i].spawnPoint;
				rotEffect_[i].object[j]->SetTransform(rotEffect_[i].transform[j]);
				break;
			}
		}

		// 発生しているエフェクトの処理
		for (int j = 0; j < kRotationEffectCount; ++j) {
			if (rotEffect_[i].isActivated[j]) {
				// 移動
				rotEffect_[i].transform[j].translate.y += 1.0f;
				rotEffect_[i].object[j]->SetTransform(rotEffect_[i].transform[j]);

				rotEffect_[i].lifetime[j] -= 1.0f / 60.0f;
				if (rotEffect_[i].lifetime[j] <= 0) {
					rotEffect_[i].isActivated[j] = false;
				}
			}
		}
	}
}

void PlayerRotation::Draw() {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < kRotationEffectCount; ++j) {
			rotEffect_[i].object[j]->Draw3D();
		}
	}
}

