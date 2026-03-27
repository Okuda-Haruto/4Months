#pragma once
#include "GameEngine.h"

class Player;
class Course;
class GameTimer;
class HUD {
public:
	void Initialize();
	void Update(Player* player,Course* course,GameTimer* timer);
	void Draw();

private:
	void UpdateCharge(Player* player);
	void UpdateScore(Course* course);
	void UpdateAcceleration(Player* player);

	// エネルギー
	std::unique_ptr<Sprite> chargeBGSprite_ = nullptr;
	std::unique_ptr<Sprite> currentChargeSprite_ = nullptr;
	float kEnergyBarWidth = 1280 - 64;
	Vector2 chargeLTPos_ = { 32,720 - 64 };

	// 壊した量
	std::unique_ptr<Sprite> breakBGSprite_ = nullptr;
	std::unique_ptr<Sprite> currentBreakSprite_ = nullptr;
	float kBreakBarWidth = 1280 - 64;
	Vector2 breakLTPos_ = {32, 48 };

	// タイマー
	std::unique_ptr<Sprite> accBGSprite_ = nullptr;
	std::unique_ptr<Sprite> currentAccSprite_ = nullptr;
	float kTimeBarWidth = 1280 - 64;
	Vector2 timeLTPos_ = { 32, 16 };

	std::unique_ptr<Sprite> clearBarSprite_ = nullptr;
	std::unique_ptr<Sprite> bonusBreakSprite_ = nullptr;

};

