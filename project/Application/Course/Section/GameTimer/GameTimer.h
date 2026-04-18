#pragma once
#include <algorithm>

// カウントダウンのタイマー
class GameTimer {
public:
	void Initialize(float maxTimer);
	void Update();

	float GetCurrent() { return currentTime_; }

private:
	float currentTime_ = 30;
	float maxTime_ = 30;
};

