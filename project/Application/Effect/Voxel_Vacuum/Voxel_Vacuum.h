#pragma once
#include "GameEngine.h"

class Player;

class Voxel_Vacuum {
private:

	std::unique_ptr<Object> object_;

	SRT transform_;

	const float kMaxAnimationTime_ = 1.0f;
	float animationTime_;
	bool isDead_;

	Player* player_;

public:

	void Initialize(SRT transform, Player* player, std::shared_ptr<DirectionalLight> directionalLight);

	void Update();

	void Draw();

	bool IsDead() { return isDead_; }
};
