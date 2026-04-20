#pragma once
#include "GameEngine.h"
#include "GameTimer/GameTimer.h"

class Section {
public:
	void Initialize(int startChunkY, int endChunkY, float maxSeconds, int clearScore, int maxScore);
	void Update(float playerY);

	// スコア追加
	void AddBreak(int breakCount) {
		// 同じ秒数があれば一つにまとめる
		for (auto& d : delay_) {
			if (fabs(d.time - scoreDelay_) < 0.0001f) {
				d.score += breakCount;
				return;
			}
		}
		// なければ新規追加
		delay_.push_back(Delay{ scoreDelay_, breakCount });
	}

	// この区間に入っているか
	bool IsEnter(float y) { return startY_ >= y && y > endY_; }
	// この区間を通りすぎたか
	bool IsOver(float y) { return endY_ > y; }
	// クリアスコアに達したか
	bool IsCleared() { return breakScore_ > clearBreakScore_; }
	// タイマー
	GameTimer* GetTimer() { return timer_.get(); }

	// 必要スコア
	int GetClearScore() { return clearBreakScore_; }
	// 最大スコア
	int GetMaxScore() { return maxBreakScore_; }
	// 現在スコア
	int GetCurrentScore() { return breakScore_; }

	// 区間終わりまでの進み具合
	float GetPositionRate() { return clamp((currentY_ - startY_) / (endY_ - startY_), 0.0f, 1.0f); }

	// スタート地点
	Vector3 GetStartPos() { return Vector3{ 0,startY_,0 }; }

private:
	// 区間タイム
	std::unique_ptr<GameTimer> timer_;

	// 破壊ノルマ
	int breakScore_ = 0;
	int clearBreakScore_ = 0;
	int maxBreakScore_ = 0;

	struct Delay {
		float time;
		int score;
	};
	// スコアに反映までの時間
	float scoreDelay_ = 1.0f + (2.0f / 3.0f);
	std::vector<Delay> delay_;

	// 区間
	float startY_ = 0;
	float endY_ = 0;
	float currentY_ = 0;
};

