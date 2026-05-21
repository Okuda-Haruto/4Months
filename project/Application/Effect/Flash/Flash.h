#pragma once
#include "GameEngine.h"
class Flash {
public:
	void Initialize();
	void Update();
	void Draw();

	void Set();

private:
	std::unique_ptr<Sprite> flash_ = nullptr;

	float timer_ = 0;
	float time_ = 1.5f;
	bool isStart_ = false;
};

