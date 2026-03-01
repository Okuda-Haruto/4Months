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

	// 小数点
	dot_ = std::make_unique<Sprite>();
	dot_->Initialize("resources/HUD/Numbers/Dot.png");
	dot_->SetPosition(speedOffset_ + Vector2{ 84,16 });
	dot_->SetSize({ 48,48 });

	// 速度アイコン
	speed_ = std::make_unique<Sprite>();
	speed_->Initialize("resources/HUD/Speed/Speed.png");
	speed_->SetPosition(speedOffset_ + Vector2{ -128,0 });
	speed_->SetSize({ 128,64 });
}

void HUD::Update(Player* player) {
	float numberSize = 128;

	float x = player->GetSpeed();
	int firstDecimal = abs(static_cast<int>(floor(x))) % 10; // 一桁目
	number_[0]->SetTextureLeftTop({ numberSize * firstDecimal }); // 表示:二桁目

	firstDecimal = abs(static_cast<int>(floor(x * 10))) % 10; // 小数点第一位
	number_[1]->SetTextureLeftTop({ numberSize * firstDecimal }); // 表示:一桁目

	firstDecimal = abs(static_cast<int>(round(x * 100))) % 10; // 小数点第二位
	number_[2]->SetTextureLeftTop({ numberSize * firstDecimal }); // 表示:小数点第一位
	for (int i = 0; i < 3; ++i) {
		number_[i]->Update();
	}
	dot_->Update();
	speed_->Update();
}

void HUD::Draw() {
	for (int i = 0; i < 3; ++i) {
		number_[i]->Draw2D();
	}
	dot_->Draw2D();
	speed_->Draw2D();
}