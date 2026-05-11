#pragma once
#include "GameEngine.h"
#include "Effect/Wind/Wind.h"
#include <vector>

class TitleMoji {
public:
	void Initialize(std::shared_ptr<DirectionalLight> directionalLight, std::shared_ptr<Input> input);
	void Update();
	void Draw();

	bool IsEnd() { return state_ == State::End; }
private:
	void LoadCSV(std::string filename, std::shared_ptr<DirectionalLight> directionalLight);

	std::vector<std::unique_ptr<Object>> blocks_;
	std::vector<Vector3> positions_;
	std::vector<float> timers_;
	std::vector<Vector3> spreadVel_;
	float blockSize_ = 1.0f;
	float scale_ = 0.5f;

	float timer_ = 0.0f;
	const float waitTime_ = 0.5f;
	const float vacuumTime_ = 2.0f;
	const float vacuumEndTime_ = 3.0f;
	const float stopTime_ = 0.3f;
	const float spreadTime_ = 0.5f;

	std::shared_ptr<Input> input_;
	//ボタンを押すまでStop状態から変化させない
	bool isStop_;

	enum class State {
		Wait,
		Vacuum,
		Stop,
		Spread,
		End
	};
	State state_ = State::Wait;

	std::unique_ptr<Wind> wind_ = nullptr;
	float radius_ = 300;

	std::unique_ptr<Audio> explosionSE_ = nullptr;
};

