#include "Wind.h"
#include "Mix.h"
#include <numbers>

void Wind::Initialize(std::shared_ptr<DirectionalLight> directionalLight) {
	for (int i = 0; i < dustCount_; ++i) {
		std::unique_ptr<Object> model;
		model = std::make_unique<Object>();
		model->Initialize(ModelManager::GetInstance()->GetModel("resources/Effect/Wind/Dust", "Dust.obj"));
		model->SetDirectionalLight(directionalLight);
		model->SetShininess(0);
		dust_.push_back(std::move(model));
		velocity_.push_back({});
	}

	for (int i = 0; i < spiralCount_; ++i) {
		std::unique_ptr<Object> model;
		model = std::make_unique<Object>();
		model->Initialize(ModelManager::GetInstance()->GetModel("resources/Effect/Wind/Spiral", "Spiral.obj"));
		model->SetDirectionalLight(directionalLight);
		model->SetShininess(0);
		spiral_.push_back(std::move(model));
		spiralRadius_.push_back({});
		spiralRotate_.push_back({});
		spiralRotateSpeed_.push_back({});
	}
}

void Wind::Set(const Vector3& center, const float radius, const float animationTime) {
	// 吸い込み開始時
	center_ = center;
	radius_ = radius;
	maxAnimationTime_ = animationTime / 60.0f;
	animationTime_ = 0;

	activeDustCount_ = int(radius) - 7;
	activeDustCount_ = min(activeDustCount_, dustCount_);
	for (int i = 0; i < activeDustCount_; ++i) {
		Vector3 translate = { center.x + GameEngine::randomFloat(-radius,radius),center.y, center.z + GameEngine::randomFloat(-radius, radius) };
		dust_[i]->SetTransform(SRT({ 0.2f,0.2f,0.2f }, {}, translate));
		dust_[i]->SetColor({ 0.5f,0.5f,0.5f,0.5f });
	}

	activeSpiralCount_ = int(radius) / 2 + 1;
	activeSpiralCount_ = min(activeSpiralCount_, spiralCount_);
	for (int i = 0; i < activeSpiralCount_; ++i) {
		Vector3 translate = { center.x,center.y + GameEngine::randomFloat(-radius, radius), center.z };
		spiralRotate_[i] = GameEngine::randomFloat(0, float(std::numbers::pi) * 2.0f);
		spiralRotateSpeed_[i] = GameEngine::randomFloat(0.15f, 0.3f);
		Quaternion spiralRotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, spiralRotate_[i]);
		spiralRadius_[i] = GameEngine::randomFloat(0, radius / 2);
		spiral_[i]->SetTransform(SRT({ 0,0,0 }, spiralRotate, translate));
		spiral_[i]->SetColor({ 0.8f,0.8f,0.8f,0.8f });
	}
}

void Wind::Update() {
	if (animationTime_ < maxAnimationTime_) {
		animationTime_ += 1.0f / 60.0f;
	}

	if (animationTime_ >= maxAnimationTime_) {
		isDead_ = true;
	} else {
		const Vector3 axis = Vector3(0, 1, 0);
		const float baseVacuumSpeed = 3.0f;
		const float vacuumSensitivity = 0.1f;

		// ちり
		for (int i = 0; i < activeDustCount_; ++i) {
			SRT transform = dust_[i]->GetTransform();

			float length = Length(center_ - transform.translate);

			float vacuumLate = length / radius_;
			float speed = (1.0f - vacuumLate) * baseVacuumSpeed;
			speed += 0.25f;
			Vector3 vec = {};

			Vector3 velTarget = Mix(transform.translate, vec, center_, axis, radius_, false) * speed;

			velocity_[i] = Lerp(velocity_[i], velTarget, vacuumSensitivity);
			velocity_[i] = Lerp(velocity_[i], Vector3{ 0,0,0 }, 0.1f);

			transform.translate += velocity_[i];
			dust_[i]->SetTransform(transform);
		}

		// 旋風
		for (int i = 0; i < activeSpiralCount_; ++i) {
			SRT transform = spiral_[i]->GetTransform();
			spiralRotate_[i] -= spiralRotateSpeed_[i];
			Quaternion rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, spiralRotate_[i]);
			transform.rotate = rotate;
			spiral_[i]->SetTransform(transform);
		}

		float time = 0.1f;
		float perFrame = 60.0f * time;
		if (animationTime_ < time) {
			// 開始時
			for (int i = 0; i < activeSpiralCount_; ++i) {
				SRT transform = spiral_[i]->GetTransform();
				transform.scale.x += spiralRadius_[i] / perFrame;
				transform.scale.y += 1.0f / perFrame;
				transform.scale.z += spiralRadius_[i] / perFrame;
				spiral_[i]->SetTransform(transform);
			}

		} else if (maxAnimationTime_ - animationTime_ < time) {
			// 終了時
			for (int i = 0; i < activeSpiralCount_; ++i) {
				SRT transform = spiral_[i]->GetTransform();
				transform.scale.x -= spiralRadius_[i] / perFrame;
				transform.scale.y -= 1.0f / perFrame;
				transform.scale.z -= spiralRadius_[i] / perFrame;
				spiral_[i]->SetTransform(transform);
			}
		}
	}
}

void Wind::Draw() {
	if (animationTime_ < maxAnimationTime_) {
		for (int i = 0; i < activeDustCount_; ++i) {
			dust_[i]->Draw3D();
		}
		for (int i = 0; i < activeSpiralCount_; ++i) {
			spiral_[i]->Draw3D();
		}
	}
}
