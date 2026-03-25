#pragma once
#include "GameEngine.h"

class Player;
class HUD {
public:
	void Initialize();
	void Update(Player* player);
	void Draw();

	void UpdateCharge(Player* player);

private:
	// エネルギー
	std::unique_ptr<Sprite> chargeBGSprite_ = nullptr;
	std::unique_ptr<Sprite> currentChargeSprite_ = nullptr;
	float kEnergyBarWidth = 800;
	Vector2 chargeLTPos_ = { 32,720 - 64 };
};

