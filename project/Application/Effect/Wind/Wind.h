#pragma once
#include "GameEngine.h"

class Wind {
public:
	void Initialize(std::shared_ptr<DirectionalLight> directionalLight);
	void Set(const Vector3& center, const float radius, const float animationTime);
	void Update();
	void Draw();

private:
	Vector3 center_;
	float radius_;

	std::vector<std::unique_ptr<Object>> dust_;
	std::vector<Vector3> velocity_;
	const int dustCount_ = 100;
	int activeDustCount_ = 100;

	std::vector<std::unique_ptr<Object>> spiral_;
	std::vector<float> spiralRadius_;
	std::vector<float> spiralRotate_;
	std::vector<float> spiralRotateSpeed_;
	const int spiralCount_ = 50;
	int activeSpiralCount_ = 0;
	float maxAnimationTime_ = 1.0f;
	float animationTime_ = 1.0f;
	bool isDead_ = false;
};

