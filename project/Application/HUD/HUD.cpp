#include "HUD.h"
#include "Human/Player/Player.h"

void HUD::Initialize() {
	// 数字
	for (int i = 0; i < 3; ++i) {
		number_[i] = std::make_unique<Sprite>();
		number_[i]->Initialize("resources/HUD/Numbers/Numbers.png");
		number_[i]->SetTextureSize({ 128,220 });
		number_[i]->SetSize({ 48,48 });
	}
	number_[0]->SetPosition(speedOffset_ + Vector2{ 0,16 });
	number_[1]->SetPosition(speedOffset_ + Vector2{ 48,16 });
	number_[2]->SetPosition(speedOffset_ + Vector2{ 96,16 });

	// 速度アイコン
	speed_ = std::make_unique<Sprite>();
	speed_->Initialize("resources/HUD/Speed/Speed.png");
	speed_->SetPosition(speedOffset_ + Vector2{ -128,0 });
	speed_->SetSize({ 128,64 });


	// エネルギー背景
	energyBGSprite_ = std::make_unique<Sprite>();
	energyBGSprite_->Initialize("./resources/DebugResources/white2x2.png");
	energyBGSprite_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	energyBGSprite_->SetSize({kEnergyBarWidth, 32.0f });
	energyBGSprite_->SetPosition(energyLTPos_);

	// 現在エネルギー
	currentEnergySprite_ = std::make_unique<Sprite>();
	currentEnergySprite_->Initialize("./resources/DebugResources/white2x2.png");
	currentEnergySprite_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
	currentEnergySprite_->SetSize({ kEnergyBarWidth, 32.0f });
	currentEnergySprite_->SetPosition(energyLTPos_);

}

void HUD::Update(Player* player) {
	UpdateSpeed(player);
	UpdateEnergy(player);
}

void HUD::Draw() {
	for (int i = 0; i < 3; ++i) {
		number_[i]->Draw2D();
	}
	speed_->Draw2D();

	energyBGSprite_->Draw2D();
	currentEnergySprite_->Draw2D();
}

void HUD::UpdateSpeed(Player* player) {
	float numberSize = 128;

	float x = player->GetSpeed();
	int firstDecimal = abs(static_cast<int>(floor(x))) % 10;
	number_[0]->SetTextureLeftTop({ numberSize * firstDecimal });

	firstDecimal = abs(static_cast<int>(floor(x * 10))) % 10;
	number_[1]->SetTextureLeftTop({ numberSize * firstDecimal });

	firstDecimal = abs(static_cast<int>(round(x * 100))) % 10;
	number_[2]->SetTextureLeftTop({ numberSize * firstDecimal });
	for (int i = 0; i < 3; ++i) {
		number_[i]->Update();
	}
	speed_->Update();

}
void HUD::UpdateEnergy(Player* player) {
	float current = player->GetCurrentEnergy();
	float max = player->GetMaxEnergy();
	if (current < 0) return;

	// 割合を求める
	float rate = current / max;
	// HP量に応じてスプライトのサイズ変更
	float length = kEnergyBarWidth * rate;
	currentEnergySprite_->SetSize({ length, currentEnergySprite_->GetSize().y});
	energyBGSprite_->Update();
	currentEnergySprite_->Update();
}