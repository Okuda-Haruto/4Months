#include "HUD.h"
#include "Human/Player/Player.h"
#include "Course/Course.h"

void HUD::Initialize(Input* input) {
	input_ = input;

	// チャージ背景
	chargeBGSprite_ = std::make_unique<Sprite>();
	chargeBGSprite_->Initialize("./resources/DebugResources/white2x2.png");
	chargeBGSprite_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	chargeBGSprite_->SetSize({ kEnergyBarWidth, 32.0f });
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
	currentBreakSprite_->SetSize({ kBreakBarWidth, 32.0f });
	currentBreakSprite_->SetPosition(breakLTPos_);

	bonusBreakSprite_ = std::make_unique<Sprite>();
	bonusBreakSprite_->Initialize("./resources/DebugResources/white2x2.png");
	bonusBreakSprite_->SetColor({ 1.0f, 0.6f, 0.6f, 1.0f });
	bonusBreakSprite_->SetSize({ kBreakBarWidth, 32.0f });
	bonusBreakSprite_->SetPosition(breakLTPos_);
	// 時間
	timeBGSprite_ = std::make_unique<Sprite>();
	timeBGSprite_->Initialize("./resources/DebugResources/white2x2.png");
	timeBGSprite_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	timeBGSprite_->SetSize({ kTimeBarWidth, 32.0f });
	timeBGSprite_->SetPosition(timeLTPos_);

	currentTimeSprite_ = std::make_unique<Sprite>();
	currentTimeSprite_->Initialize("./resources/DebugResources/white2x2.png");
	currentTimeSprite_->SetColor({ 0.0f, 1.0f, 1.0f, 1.0f });
	currentTimeSprite_->SetSize({ kTimeBarWidth, 32.0f });
	currentTimeSprite_->SetPosition(timeLTPos_);

	// 区間
	sectionSprite_ = std::make_unique<Sprite>();
	sectionSprite_->Initialize("./resources/DebugResources/white2x2.png");
	sectionSprite_->SetColor({ 0.2f,0.2f,0.2f, 1.0f });
	sectionSprite_->SetSize(sectionBarSize_);
	sectionSprite_->SetPosition(sectionLTPos_);
	sectionSprite_->Update();
	progressSprite_ = std::make_unique<Sprite>();
	progressSprite_->Initialize("./resources/DebugResources/white2x2.png");
	progressSprite_->SetColor({ 1.0f,1.0f,1.0f, 1.0f });
	progressSprite_->SetSize({ sectionBarSize_.x,0 });
	progressSprite_->SetPosition(sectionLTPos_);
	progressSprite_->Update();

	//情報
	infoSprite_ = std::make_unique<Sprite>();
	infoSprite_->Initialize("./resources/HUD/Info.png");
	infoSprite_->SetSize(Vector2{ 471.0f,62.0f });
	infoSprite_->SetTextureSize(Vector2{ 471.0f,62.0f });
	infoSprite_->SetPosition(infoLTPos_);

	startNumSprite_ = std::make_unique<Sprite>();
	startNumSprite_->Initialize("./resources/HUD/Start_Nums.png");
	startNumSprite_->SetSize(Vector2{ 50,61.0f });
	startNumSprite_->SetTextureSize(Vector2{ 50.0f,61.0f });
	startNumSprite_->SetPosition(startNumPos_);
	startNumSprite_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
	startNumIsDraw_ = true;

	canShoot_ = std::make_unique<Sprite>();
	canShoot_->Initialize("./resources/HUD/CanShoot.png");
	canShoot_->SetSize(Vector2{ 180.0f,40.0f });
	canShoot_->SetTextureSize(Vector2{ 180.0f,40.0f });
	canShoot_->SetPosition({640,300});
	canShoot_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
}

void HUD::Update(Player* player, Course* course, GameTimer* timer, int startNum, std::shared_ptr<Camera> camera) {
	UpdateCharge(player);
	UpdateScore(course);
	UpdateTimer(course);
	UpdateSection(player, course);
	UpdateInfo();
	UpdateStartNum(startNum);
	UpdateReload(player,camera);
}

void HUD::Draw() {
	chargeBGSprite_->Draw2D();
	currentChargeSprite_->Draw2D();
	breakBGSprite_->Draw2D();
	bonusBreakSprite_->Draw2D();
	currentBreakSprite_->Draw2D();


	// 時間
	timeBGSprite_->Draw2D();
	currentTimeSprite_->Draw2D();

	// 区間
	sectionSprite_->Draw2D();
	progressSprite_->Draw2D();

	infoSprite_->Draw2D();
	canShoot_->Draw2D();

	if (startNumIsDraw_) {
		startNumSprite_->Draw2D();
	}
}

void HUD::SetPauseDisplay(bool isOn) {
	if (isOn) {
		Vector4 color;
		color = canShoot_->GetColor();
		color.w = 0.0f;
		canShoot_->SetColor(color);
	} else {
		Vector4 color;
		color = canShoot_->GetColor();
		color.w = 1.0f;
		canShoot_->SetColor(color);
	}
}

void HUD::UpdateCharge(Player* player) {
	float current = player->GetCharge();
	float max = player->GetMaxCharge();
	if (current < 0) return;

	// 割合を求める
	float rate = current / max;
	// 溜め量に応じてスプライトのサイズ変更
	float length = kEnergyBarWidth * rate;
	currentChargeSprite_->SetSize({ length, currentChargeSprite_->GetSize().y });
	chargeBGSprite_->Update();
	currentChargeSprite_->Update();
}

void HUD::UpdateScore(Course* course) {
	Section* currentSection = course->GetCurrentSection();
	int current = currentSection->GetCurrentScore();
	int clear = currentSection->GetClearScore();
	int max = currentSection->GetMaxScore();
	if (current < 0) return;

	// 割合を求める
	float rate = float(current) / float(clear);
	rate = clamp(rate, 0.0f, 1.0f);
	// 必要スコアに応じてスプライトのサイズ変更
	float length = kBreakBarWidth * rate * (1.0f - bonusRate_);
	currentBreakSprite_->SetSize({ length, currentBreakSprite_->GetSize().y });
	currentBreakSprite_->SetPosition({ breakLTPos_.x, breakLTPos_.y });
	breakBGSprite_->Update();
	currentBreakSprite_->Update();

	// 割合を求める
	rate = 0;
	if(currentSection->IsCleared()) {
		rate = float(current - clear) / float(max);
		rate = clamp(rate, 0.0f, 1.0f);
	}
	// 最大スコアに応じてスプライトのサイズ変更
	length = kBreakBarWidth * rate * bonusRate_;
	bonusBreakSprite_->SetSize({ length, bonusBreakSprite_->GetSize().y });
	bonusBreakSprite_->SetPosition({ breakLTPos_.x + kBreakBarWidth * (1 - bonusRate_), breakLTPos_.y });
	bonusBreakSprite_->Update();
}

void HUD::UpdateTimer(Course* course) {
	Section* currentSection = course->GetCurrentSection();
	float rate = currentSection->GetTimer()->GetTimeRate();

	// 残り時間に応じてスプライトのサイズ変更
	float length = kTimeBarWidth * rate;
	currentTimeSprite_->SetSize({ length, currentTimeSprite_->GetSize().y });
	timeBGSprite_->Update();
	currentTimeSprite_->Update();
}

void HUD::UpdateSection(Player* player, Course* course) {
	Section* currentSection = course->GetCurrentSection();
	float rate = currentSection->GetPositionRate();

	// 進度に応じてスプライトのサイズ変更
	float length = sectionBarSize_.y * rate;
	progressSprite_->SetSize({ sectionBarSize_.x, length });
	sectionSprite_->Update();
	progressSprite_->Update();
}

void HUD::UpdateInfo() {
	if (input_->GetPad(0).isConnected) {
		infoSprite_->SetTextureLeftTop(Vector2{ 0.0f,62.0f });
	} else {
		infoSprite_->SetTextureLeftTop(Vector2{ 0.0f,0.0f });
	}
	infoSprite_->Update();
}

void HUD::UpdateStartNum(int num) {

	if (num == 0) {
		startNumIsDraw_ = false;
		return;
	}

	startNumSprite_->SetTextureLeftTop(Vector2{ 150.0f - 50.0f * num,0.0f });
	startNumSprite_->Update();
}

void HUD::UpdateReload(Player* player, std::shared_ptr<Camera> camera) {
	Matrix4x4 viewProjection = camera->GetViewMatrix() * camera->GetProjectionMatrix();

	// 修正後
	Vector3 pos = player->GetTransform().translate;
	Vector4 worldPos = { pos.x, pos.y, pos.z, 1.0f };
	Vector4 clipPos = Transform(worldPos, viewProjection);

	// w除算
	Vector3 ndc;
	ndc.x = clipPos.x / clipPos.w;
	ndc.y = clipPos.y / clipPos.w;
	ndc.z = clipPos.z / clipPos.w;

	// スクリーン変換
	Vector2 screen;
	screen.x = (ndc.x + 1.0f) * 0.5f * 1280;
	screen.y = (1.0f - ndc.y) * 0.5f * 720;

	drawCanShoot_ = player->CanShoot();
	if (drawCanShoot_) {
		canShoot_->SetTextureLeftTop({ 0,0 });
	} else {
		canShoot_->SetTextureLeftTop({ 0,40 });
	}
	canShoot_->SetPosition(screen + Vector2{0,-50});
	canShoot_->Update();
}

Vector4 HUD::Transform(const Vector4& vector, const Matrix4x4& matrix) {
	// 座標変換した結果
	Vector4 transform;

	transform.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + vector.w * matrix.m[3][0];
	transform.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + vector.w * matrix.m[3][1];
	transform.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + vector.w * matrix.m[3][2];
	transform.w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + vector.w * matrix.m[3][3];

	assert(transform.w != 0.0f);
	transform.x /= transform.w;
	transform.y /= transform.w;
	transform.z /= transform.w;
	transform.w /= transform.w;

	return transform;
}