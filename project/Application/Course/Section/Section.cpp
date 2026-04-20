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

	for (auto it = delay_.begin(); it != delay_.end(); ) {
		it->time -= GameEngine::GetDeltaTime();

		if (it->time <= 0) {
			breakScore_ += it->score;
			it = delay_.erase(it);
		} else {
			++it;
		}
	}
}
