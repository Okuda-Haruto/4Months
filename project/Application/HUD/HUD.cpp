#include "HUD.h"
#include "Human/Player/Player.h"
#include "Course/Course.h"
#include "GameTimer/GameTimer.h"

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

	// ノルマの線背景
	//clearBarSprite_ = std::make_unique<Sprite>();
	//clearBarSprite_->Initialize("./resources/DebugResources/white2x2.png");
	//clearBarSprite_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	//clearBarSprite_->SetSize({ kBreakBarWidth, 32.0f });
	//clearBarSprite_->SetPosition(breakLTPos_);

	//// ボーナス背景
	//bonusBreakSprite_ = std::make_unique<Sprite>();
	//bonusBreakSprite_->Initialize("./resources/DebugResources/white2x2.png");
	//bonusBreakSprite_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	//bonusBreakSprite_->SetSize({ kBreakBarWidth, 32.0f });
	//bonusBreakSprite_->SetPosition(breakLTPos_);

	// 現在破壊量
	currentBreakSprite_ = std::make_unique<Sprite>();
	currentBreakSprite_->Initialize("./resources/DebugResources/white2x2.png");
	currentBreakSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	currentBreakSprite_->SetSize({ kBreakBarWidth, 32.0f });
	currentBreakSprite_->SetPosition(breakLTPos_);

	// 時間
	accBGSprite_ = std::make_unique<Sprite>();
	accBGSprite_->Initialize("./resources/DebugResources/white2x2.png");
	accBGSprite_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	accBGSprite_->SetSize({ kTimeBarWidth, 32.0f });
	accBGSprite_->SetPosition(timeLTPos_);

	currentAccSprite_ = std::make_unique<Sprite>();
	currentAccSprite_->Initialize("./resources/DebugResources/white2x2.png");
	currentAccSprite_->SetColor({ 0.0f, 1.0f, 1.0f, 1.0f });
	currentAccSprite_->SetSize({ kTimeBarWidth, 32.0f });
	currentAccSprite_->SetPosition(timeLTPos_);
}

void HUD::Update(Player* player, Course* course, GameTimer* timer) {
	UpdateCharge(player);
	UpdateScore(course);
	UpdateAcceleration(player);
}

void HUD::Draw() {
	chargeBGSprite_->Draw2D();
	currentChargeSprite_->Draw2D();
	breakBGSprite_->Draw2D();
	currentBreakSprite_->Draw2D();

	// 時間
	accBGSprite_->Draw2D();
	currentAccSprite_->Draw2D();
}

void HUD::UpdateCharge(Player* player) {
	float current = player->GetCharge();
	float max = player->GetMaxCharge();
	if (current < 0) return;

	// 割合を求める
	float rate = current / max;
	// 溜め量に応じてスプライトのサイズ変更
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
	// スコアに応じてスプライトのサイズ変更
	float length = kBreakBarWidth * rate;
	currentBreakSprite_->SetSize({ length, currentBreakSprite_->GetSize().y });
	breakBGSprite_->Update();
	currentBreakSprite_->Update();
}

void HUD::UpdateAcceleration(Player* player) {
	float current = player->GetAcc();
	float max = player->GetMaxAcc();
	if (current < 0) return;

	// 割合を求める
	float rate = current / max;
	rate = min(rate, 1.0f);
	// 残り時間に応じてスプライトのサイズ変更
	float length = kTimeBarWidth * rate;
	currentAccSprite_->SetSize({ length, currentAccSprite_->GetSize().y });
	accBGSprite_->Update();
	currentAccSprite_->Update();
}