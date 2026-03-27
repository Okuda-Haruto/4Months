#pragma once
class GameTimer {
public:
	void Initialize();
	void Update();

	float GetCurrentGameTime() { return currentTime_; }
	float GetMaxTime() { return maxTime_; }

private:
	float currentTime_ = 30;
	const float maxTime_ = 30;
};

