#include "HUD.h"
#include "Human/Player/Player.h"
#include "Course/Course.h"

void HUD::Initialize() {
	// チャージ背景
	chargeBGSprite_ = std::make_unique<Sprite>();
	chargeBGSprite_->Initialize("./resources/DebugResources/white2x2.png");
	chargeBGSprite_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	chargeBGSprite_->SetSize({kEnergyBarWidth, 32.0f });
	chargeBGSprite_->SetPosition(chargeLTPos_);

	// 現在チャージ量
	currentChargeSprite_ = std::make_unique<Sprite>();
	currentChargeSprite_->Initialize("./resources/DebugResources/white2x2.png");
	currentChargeSprite_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
	currentChargeSprite_->SetSize({ kEnergyBarWidth, 32.0f });
	currentChargeSprite_->SetPosition(chargeLTPos_);


	// 破壊量背景
	breakBGSprite_ = std::make_unique<Sprite>();
	breakBGSprite_->Initialize("./resources/DebugResources/white2x2.png");
	breakBGSprite_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	breakBGSprite_->SetSize({ kBreakBarWidth, 32.0f });
	breakBGSprite_->SetPosition(breakLTPos_);

	// 現在破壊量
	currentBreakSprite_ = std::make_unique<Sprite>();
	currentBreakSprite_->Initialize("./resources/DebugResources/white2x2.png");
	currentBreakSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	currentBreakSprite_->SetSize({ kBreakBarWidth * 0.4f, 32.0f });
	currentBreakSprite_->SetPosition(breakLTPos_);

	bonustBreakSprite_ = std::make_unique<Sprite>();
	bonustBreakSprite_->Initialize("./resources/DebugResources/white2x2.png");
	bonustBreakSprite_->SetColor({ 1.0f, 0.6f, 0.6f, 1.0f });
	bonustBreakSprite_->SetSize({ kBreakBarWidth * 0.6f, 32.0f });
	bonustBreakSprite_->SetPosition(Vector2{ breakLTPos_.x + kBreakBarWidth / 10 * 4,breakLTPos_.y });
}

void HUD::Update(Player* player, Course* course) {
	UpdateCharge(player);
	UpdateScore(course);
}

void HUD::Draw() {
	chargeBGSprite_->Draw2D();
	currentChargeSprite_->Draw2D();
	breakBGSprite_->Draw2D();
	currentBreakSprite_->Draw2D();
	bonustBreakSprite_->Draw2D();
}

void HUD::UpdateCharge(Player* player) {
	float current = player->GetCharge();
	float max = player->GetMaxCharge();
	if (current < 0) return;

	// 割合を求める
	float rate = current / max;
	// HP量に応じてスプライトのサイズ変更
	float length = kEnergyBarWidth * rate;
	currentChargeSprite_->SetSize({ length, currentChargeSprite_->GetSize().y});
	chargeBGSprite_->Update();
	currentChargeSprite_->Update();
}

void HUD::UpdateScore(Course* course) {
	int current = course->GetBreakScore();
	int max = course->GetMaxBreakScore();
	if (current < 0) return;

	// 割合を求める
	float rate = float(current) / float(max);
	rate = min(rate, 1.0f);
	// HP量に応じてスプライトのサイズ変更
	float length = kBreakBarWidth * 0.4f * rate;
	currentBreakSprite_->SetSize({ length, currentBreakSprite_->GetSize().y });
	breakBGSprite_->Update();
	currentBreakSprite_->Update();

	// 割合を求める
	rate = float(current - max) / (float(max) / 4 * 6);
	rate = min(rate, 1.0f);
	if (rate < 0) return;
	// HP量に応じてスプライトのサイズ変更
	length = kBreakBarWidth * 0.6f * rate;

	bonustBreakSprite_->SetSize({ length, bonustBreakSprite_->GetSize().y });
	bonustBreakSprite_->Update();
}