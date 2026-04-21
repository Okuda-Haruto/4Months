#pragma once
#include <Object/Object.h>
#include "GameCamera/GameCamera.h"

class GoalBarrier {
private:
	//基礎
	std::unique_ptr<Object> skydome_;
	std::unique_ptr<Object> baseObject_;
	std::unique_ptr<Object> smokeObject_;
	std::unique_ptr<Object> smokeObjectRotate_;
	std::unique_ptr<Object> smokeObjectScale_;

	std::shared_ptr<Camera> camera_;
	std::unique_ptr<Audio> clearSE_;

	SRT transform_;

	float time_;

	const float kMaxClearTimer_ = 1.0f;
	float clearTimer_;
	bool isClear_;

public:

	void Initialize(float position, std::shared_ptr<Camera> camera);

	void Update(GameCamera* gameCamera);

	void Draw();

	void Clear();
};