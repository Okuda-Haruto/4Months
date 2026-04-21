#include "Section.h"

void Section::Initialize(int startChunkY, int endChunkY, float maxSeconds, int clearScore, int maxScore) {
	float blockScale = 3.0f;
	float chunkSize = blockScale * 2 * 16;
	startY_ = -startChunkY * chunkSize;
	endY_ = -(endChunkY + 1) * chunkSize;
	clearBreakScore_ = clearScore;
	maxBreakScore_ = maxScore;
	isSubSection_ = false;

	timer_ = std::make_unique<GameTimer>();
	timer_->Initialize(maxSeconds);

	addScoreSE_ = std::make_unique<Audio>();
	addScoreSE_->Initialize("resources/DebugResources/mokugyo.wav", 0.5f);
	breakSE_ = std::make_unique<Audio>();
	breakSE_->Initialize("resources/DebugResources/mokugyo.wav", 0.5f);
}

void Section::Initialize(int startChunkY, int endChunkY) {
	float blockScale = 3.0f;
	float chunkSize = blockScale * 2 * 16;
	startY_ = -startChunkY * chunkSize;
	endY_ = -(endChunkY + 1) * chunkSize;

	isSubSection_ = true;

	addScoreSE_ = std::make_unique<Audio>();
	addScoreSE_->Initialize("resources/DebugResources/mokugyo.wav", 0.5f);
	breakSE_ = std::make_unique<Audio>();
	breakSE_->Initialize("resources/DebugResources/mokugyo.wav", 0.5f);
}

void Section::Update(float playerY) {
	if (timer_) {
		timer_->Update();
	}

	currentY_ = playerY;

	for (auto it = delay_.begin(); it != delay_.end(); ) {
		it->time -= GameEngine::GetDeltaTime();

		if (it->time <= 0) {
			breakScore_ += it->score;
			addScoreSE_->SoundPlayWave();
			it = delay_.erase(it);
		} else {
			++it;
		}
	}
}

void Section::AddBreak(int breakCount) {
	// 同じ秒数があれば一つにまとめる
	for (auto& d : delay_) {
		if (fabs(d.time - scoreDelay_) < 0.0001f) {
			d.score += breakCount;
			return;
		}
	}
	// なければ新規追加
	delay_.push_back(Delay{ scoreDelay_, breakCount });

	if (breakSE_->IsSoundPlayingWave()) {
		breakSE_->SoundEndWave();
	}
	breakSE_->SoundPlayWave();

}
