#include "Section.h"
#include "Voxel/Voxel.h"
#include "Box/Box.h"

void Section::Initialize(int startChunkY, int endChunkY, float maxSeconds, int clearScore, int maxScore, Voxel* voxel, const RankBorders& rankBorders) {
	startChunkY_ = startChunkY;
	endChunkY_ = endChunkY;

	float blockScale = 3.0f;
	float chunkSize = blockScale * 2 * 16;
	startY_ = -startChunkY * chunkSize;
	endY_ = -(endChunkY + 1) * chunkSize;
	clearBreakScore_ = clearScore;
	maxBreakScore_ = maxScore;
	isSubSection_ = false;

	voxel_ = voxel;
	startVoxelCount_ = voxel->CountObjects(startChunkY, endChunkY);

	timer_ = std::make_unique<GameTimer>();
	timer_->Initialize(maxSeconds);

	addScoreSE_ = std::make_unique<Audio>();
	addScoreSE_->Initialize("resources/SE・BGM/Game/star.mp3", 0.7f);
	breakSE_ = std::make_unique<Audio>();
	breakSE_->Initialize("resources/SE・BGM/Game/destruction.mp3", 0.7f);

	rankBorders_ = rankBorders;
}

void Section::Initialize(int startChunkY, int endChunkY) {
	float blockScale = 3.0f;
	float chunkSize = blockScale * 2 * 16;
	startY_ = -startChunkY * chunkSize;
	endY_ = -(endChunkY + 1) * chunkSize;

	isSubSection_ = true;

	addScoreSE_ = std::make_unique<Audio>();
	addScoreSE_->Initialize("resources/SE・BGM/Game/star.mp3", 0.7f);
	breakSE_ = std::make_unique<Audio>();
	breakSE_->Initialize("resources/SE・BGM/Game/destruction.mp3", 0.7f);
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

	breakSETimer_ -= GameEngine::GetDeltaTime();
}

int Section::JudgeRank(std::vector<Box*> boxes) {
	int breakRate = int(std::roundf(GetBreakRate(boxes)));
	if (breakRate_ > rankBorders_.rate.aScore) {
		rankItems_.rateRank = A;
	} else if (breakRate_ > rankBorders_.rate.bScore) {
		rankItems_.rateRank = B;
	} else {
		rankItems_.rateRank = C;
	}

	if (breakScore_ > rankBorders_.count.aScore) {
		rankItems_.countRank = A;
	} else if (breakScore_ > rankBorders_.count.bScore) {
		rankItems_.countRank = B;
	} else {
		rankItems_.countRank = C;
	}

	if (timer_->GetCurrent() > rankBorders_.time.aScore) {
		rankItems_.countRank = A;
	}else if (timer_->GetCurrent() > rankBorders_.time.bScore) {
		rankItems_.countRank = B;
	} else {
		rankItems_.countRank = C;
	}

	return int(std::roundf(float(rankItems_.rateRank + rankItems_.countRank + rankItems_.timeRank) / 3.0f));
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

	if (breakSETimer_ <= 0) {
		breakSE_->SoundPlayWave();
		breakSETimer_ = breakSEInterval_;
	}

}

float Section::GetBreakRate(std::vector<Box*> boxes) {
	int boxCount = 0;
	for (auto box : boxes) {
		if (box->GetTransform().scale.x >= 3.0f) {
			float boxY = box->GetTransform().translate.y;
			if (boxY < startY_ && boxY > endY_) {
				boxCount++;
			}
		}
	}

	return 1.0f - (float(voxel_->CountObjects(startChunkY_, endChunkY_) + boxCount) / startVoxelCount_);
}
