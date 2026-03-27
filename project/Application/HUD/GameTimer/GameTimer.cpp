#include "GameTimer.h"

void GameTimer::Initialize() {
	currentTime_ = maxTime_;
}

void GameTimer::Update() {
	currentTime_ -= 1.0f / 60.0f;
}