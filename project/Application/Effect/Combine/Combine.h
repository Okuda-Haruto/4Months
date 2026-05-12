#pragma once
#include "GameEngine.h"

class Combine {
public:
	void InitializeTitle(std::shared_ptr<DirectionalLight> directionalLight, shared_ptr<Camera> camera);
	void InitializeGame(std::shared_ptr<DirectionalLight> directionalLight, std::shared_ptr<Camera> camera);
	void Update();
	void Draw();

	bool IsEnd() { return isEnd_; }
	bool IsPlaying() { return phase_ > Phase::Black; }

private:
	std::unique_ptr<Object> human_;
	std::unique_ptr<Object> beyblade_;

	std::shared_ptr<Camera> camera_;
	bool isTitle_ = false;
	bool isEnd_ = false;

	// コマ
	float rotate_ = 0;
	float defaultHY_ = 70;
	float defaultBY_ = 40;
	float endY_ = 7.5f;
	float setStart = 0.3f;
	float setCountdown_[8] = {
		0.7f,0.7f,0.9f,0.9f,5.0f,0.4f,0.4f,0.4f
	};
	float backAmount_ = 50.0f;
	const float gameDefaultZ_ = 50.0f;
	float angle_ = 0.5f;
	Vector3 partsTranslate[8] = {
		{ 20,0,0 },
		{ -20,0,0 },
		{ 0,0,-20 },
		{ 0,0,20 },
		{ 0,defaultHY_,0 }, // 見えない位置
		{ 10,0,10 },
		{ -10,0,10 },
		{ -10,0,-10 }
	};

	enum class Phase {
		Black,
		Fall,
		Set,
		Ride,
		Back,
		Wait,
		GameStart
	};
	Phase phase_ = Phase::Fall;

	float timer_ = 0;
	const float kDarkTime = 1.0f;
	const float kDarkWaitTime = 0.2f;
	const float kFallTime = 0.4f;
	const float kSetTime = 1.0f;
	const float kRideTime = 0.3f;
	const float kBackTime = 0.5f;
	const float kWaitTime = 0.8f;
	const float kStartTime = 1.0f;

	const float kStartIrisIn = 0.5f;

	std::unique_ptr<Sprite> black_ = nullptr;
};

