#pragma once
#include <algorithm>

// カウントダウンのタイマー
class GameTimer {
public:
	void Initialize(float maxTimer);
	void Update();

	float GetTimeRate() { return std::clamp(currentTime_ / maxTime_,0.0f,1.0f); }

private:
	float currentTime_ = 30;
	float maxTime_ = 30;
};

