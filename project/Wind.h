#pragma once
#include "GameEngine.h"

class Wind {
public:
	void Initialize(const Vector3& center, const float radius, std::shared_ptr<DirectionalLight> directionalLight);
	void Update();
	void Draw();

private:
	std::unique_ptr<Object> model_ = nullptr;
	SRT transform_;
	Vector3 center_;
	float radius_;

	const float kMaxAnimationTime_ = 1.0f;
	float animationTime_ = 0;
	bool isDead_ = false;
};

