#include "Section.h"

void Section::Initialize(int startChunkY, int endChunkY, float maxSeconds, int clearScore, int maxScore) {
	float blockScale = 3.0f;
	float chunkSize = blockScale * 2 * 16;
	startY_ = -startChunkY * chunkSize;
	endY_ = -(endChunkY + 1) * chunkSize;
	clearBreakScore_ = clearScore;
	maxBreakScore_ = maxScore;

	timer_ = std::make_unique<GameTimer>();
	timer_->Initialize(maxSeconds);
}

void Section::Update(float playerY) {
	timer_->Update();

	currentY_ = playerY;
}
