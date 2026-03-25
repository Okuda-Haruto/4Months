#pragma once
#include "GameEngine.h"

class Player;
class Course;
class HUD {
public:
	void Initialize();
	void Update(Player* player,Course* course);
	void Draw();

	void UpdateCharge(Player* player);
	void UpdateScore(Course* course);

private:
	// エネルギー
	std::unique_ptr<Sprite> chargeBGSprite_ = nullptr;
	std::unique_ptr<Sprite> currentChargeSprite_ = nullptr;
	float kEnergyBarWidth = 1280 - 64;
	Vector2 chargeLTPos_ = { 32,720 - 64 };

	// 壊した量
	std::unique_ptr<Sprite> breakBGSprite_ = nullptr;
	std::unique_ptr<Sprite> currentBreakSprite_ = nullptr;
	float kBreakBarWidth = 1280 - 128;
	Vector2 breakLTPos_ = { 32, 16 };
};

