#pragma once
#include "GameEngine.h"
#include <vector>
class StartCountdown {
public:
	void Initialize(std::shared_ptr<DirectionalLight> directionalLight);
	void Update();
	void Draw();

	bool IsEnd() { return count_ == 0 && state_ == State::Stop; }
	bool IsPreStart() { return state_ == State::PreStart; }
	bool IsDownCameraTime() {
		if (state_ == State::PreStart && timer_ >= preStartTime_) {
			timer_ = -2.5f;
			state_ = State::Start;
			return true;
		}
		return false;
	}
	void SkipPreStart() { timer_ = -0.5f; state_ = State::Start; }
	void SkipAll() { count_ = 0; state_ = State::Stop; }
private:
	void LoadCSV(std::string filename, std::shared_ptr<DirectionalLight> directionalLight, int countNumber);

	std::vector<std::unique_ptr<Object>> blocks_;
	std::vector<std::vector<Vector3>> positions_;
	std::vector<float> timers_;
	float blockSize_ = 0.3f;
	float scale_ = 0.15f;

	float timer_ = 0.0f;
	const float preStartTime_ = 3.0f;
	const float startTime_ = 0.4f;
	const float waitTime_ = 0.15f;
	const float vacuumTime_ = 0.4f;
	const float vacuumEndTime_ = 0.5f;
	const float spreadTime_ = 0.3f;

	enum class State {
		PreStart,
		Start,
		Wait,
		Vacuum,
		Spread,
		Stop,
	};
	State state_ = State::PreStart;
	int count_ = 3;
	const int kMaxCount_ = 3;

	int maxBlocks_ = 0;
	float radius_ = 200;
	
	Vector3 basePos_ = { 0,200.0f, 5};
	std::vector<Vector3> startPos_;

	std::unique_ptr<Audio> countSE_ = nullptr;
	std::unique_ptr<Audio> startSE_ = nullptr;
};

