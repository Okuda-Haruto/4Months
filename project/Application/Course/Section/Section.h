#pragma once
#include "GameEngine.h"
#include "GameTimer/GameTimer.h"

class Box;
class Voxel;
class Section {
public:
	// メインの区間
	void Initialize(int startChunkY, int endChunkY, float maxSeconds, int clearScore, int maxScore, Voxel* voxel);

	// ノルマなし区間
	void Initialize(int startChunkY, int endChunkY);

	void Update(float playerY);

	// スコア追加
	void AddBreak(int breakCount);

	// 現在の破壊率計算
	float GetBreakRate(std::vector<Box*> boxes);

	// ノルマなし区間か
	bool IsSubSection() { return isSubSection_; }
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
	// ゴール地点
	Vector3 GetEndPos() { return Vector3{ 0,endY_,0 }; }

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
	float scoreDelay_ = 0.8f;
	std::vector<Delay> delay_;

	// 区間
	float startY_ = 0;
	float endY_ = 0;
	float currentY_ = 0;

	int startChunkY_ = 0;
	int endChunkY_ = 0;

	// ノルマなしの区間
	bool isSubSection_ = false;

	std::unique_ptr<Audio> addScoreSE_ = nullptr;
	std::unique_ptr<Audio> breakSE_ = nullptr;
	float breakSETimer_ = 0;
	const float breakSEInterval_ = 0.075f;

	Voxel* voxel_ = nullptr;
	int startVoxelCount_ = 0;
};