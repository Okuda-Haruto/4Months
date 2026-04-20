#include "GameTimer.h"
#include "GameEngine.h"

void GameTimer::Initialize(float maxTime) {
	maxTime_ = maxTime;
	currentTime_ = maxTime_;
}

void GameTimer::Update() {
	currentTime_ -= GameEngine::GetDeltaTime();
}