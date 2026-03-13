#pragma once
#include "GameEngine.h"

class Player;
class HUD {
public:
	void Initialize();
	void Update(Player* player);
	void Draw();

private:

	std::unique_ptr<Sprite> number_[3]{};
	std::unique_ptr<Sprite> dot_ = nullptr;
	std::unique_ptr<Sprite> speed_ = nullptr;
	Vector2 speedOffset_ = { 1000,640 };

	std::unique_ptr<Sprite> staminaBack_;
	std::unique_ptr<Sprite> staminaGauge_;
};

