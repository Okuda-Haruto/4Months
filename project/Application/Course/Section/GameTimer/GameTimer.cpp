#include "GameTimer.h"

void GameTimer::Initialize(float maxTime) {
	maxTime_ = maxTime;
	currentTime_ = maxTime_;
}

void GameTimer::Update() {
	currentTime_ -= 1.0f / 60.0f;
}