#pragma once
#include "GameEngine.h"
#include "Effect/Wind/Wind.h"
#include <vector>

class TitleMoji {
public:
	void Initialize(std::shared_ptr<DirectionalLight> directionalLight);
	void Update();
	void Draw();
private:
	void LoadCSV(std::string filename, std::shared_ptr<DirectionalLight> directionalLight);

	std::vector<std::unique_ptr<Object>> blocks_;
	std::vector<Vector3> positions_;
	std::vector<float> timers_;
	float blockSize_ = 1.0f;
	float scale_ = 0.5f;

	float timer_ = 0.0f;
	const float waitTime_ = 0.5f;
	const float vacuumTime_ = 2.0f;
	const float vacuumEndTime_ = 3.0f;

	enum class State {
		Wait,
		Vacuum,
		Stop
	};
	State state_ = State::Wait;

	std::unique_ptr<Wind> wind_ = nullptr;
	float radius_ = 300;
};

