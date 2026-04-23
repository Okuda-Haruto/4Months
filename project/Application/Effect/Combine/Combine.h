#pragma once
#include "GameEngine.h"

class Combine {
public:
	void InitializeTitle(std::shared_ptr<DirectionalLight> directionalLight);
	void InitializeGame(std::shared_ptr<DirectionalLight> directionalLight);
	void Update();
	void Draw();

	bool IsEnd() { return isEnd_; }

private:
	std::unique_ptr<Object> human_;
	std::unique_ptr<Object> beyblade_;

	bool isTitle_ = false;
	bool isEnd_ = false;

	// コマ
	float rotate_ = 0;
	float defaultY_ = 40;
	float setStart = 0.3f;
	float setCountdown_[8] = {
		0.7f,0.75f,0.8f,0.85f,5.0f,0.3f,0.3f,0.3f
	};
	float backAmount_ = -15.0f;
	Vector3 forward_ = { 0, 1, 0.3f };
	Vector3 backPos_{};
	Vector3 partsTranslate[8] = {
		{ 0,0,20 },
		{ 10,0,10 },
		{ 20,0,0 },
		{ 10,0,-10 },
		{ 0,defaultY_,0 }, // 見えない位置
		{ -10,0,-10 },
		{ -20,0,0 },
		{ -10,0,-10 }
	};

	enum class Phase {
		Dark,
		Fall,
		Set,
		Ride,
		Back,
		GameStart
	};
	Phase phase_ = Phase::Fall;

	float timer_ = 0;
	const float kDarkTime = 1.0f;
	const float kDarkWaitTime = 0.5f;
	const float kFallTime = 0.5f;
	const float kSetTime = 1.0f;
	const float kRideTime = 0.3f;
	const float kBackTime = 0.4f;
	const float kStartTime = 1.6f;

	std::unique_ptr<Sprite> black_ = nullptr;
};

