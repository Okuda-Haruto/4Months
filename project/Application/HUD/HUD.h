#pragma once
#include "GameEngine.h"

class Player;
class HUD {
public:
	void Initialize();
	void Update(Player* player);
	void Draw();

	void UpdateSpeed(Player* player);
	void UpdateEnergy(Player* player);

private:

	std::unique_ptr<Sprite> number_[3]{};
	std::unique_ptr<Sprite> speed_ = nullptr;
	Vector2 speedOffset_ = { 1000,640 };

	// エネルギー
	std::unique_ptr<Sprite> energyBGSprite_ = nullptr;
	std::unique_ptr<Sprite> currentEnergySprite_ = nullptr;
	float kEnergyBarWidth = 800;
	Vector2 energyLTPos_ = { 32,720 - 64 };
};

