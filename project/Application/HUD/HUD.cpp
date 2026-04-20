#include "HUD.h"
#include "Human/Player/Player.h"
#include "Course/Course.h"

void HUD::Initialize(Input* input, std::shared_ptr<Camera> camera) {
	input_ = input;
	stars_ = std::make_unique<Stars>();
	stars_->Initialize(camera);

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
	for (int i = 0; i < 4; ++i) {
		currentTimeSprite_[i] = std::make_unique<Sprite>();
		currentTimeSprite_[i]->Initialize("./resources/HUD/Numbers/Number.png");
		currentTimeSprite_[i]->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		currentTimeSprite_[i]->SetSize(currentTimeSpriteSize_);
		currentTimeSprite_[i]->SetAnchorPoint({ 0.5f,0.5f });
		currentTimeSprite_[i]->SetPosition(timePos_[i]);
		currentTimeSprite_[i]->SetTextureSize(kTimeNumSize);
	}

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

	// 現時点の目的
	objective_[0] = std::make_unique<Sprite>();
	objective_[0]->Initialize("./resources/HUD/norma_Dontclear.png");
	objective_[1] = std::make_unique<Sprite>();
	objective_[1]->Initialize("./resources/HUD/norma_clear.png");
	for (int i = 0; i < 2; ++i) {
		objective_[i]->SetAnchorPoint({ 0.5f,0.5f });
		objective_[i]->SetPosition(objectivePos_);
		objective_[i]->SetSize(objectiveSize_);
		objective_[i]->Update();
	}


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
	canShoot_->SetPosition({ 640,300 });
	canShoot_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
}

void HUD::Update(Player* player, Course* course, GameTimer* timer, int startNum, std::shared_ptr<Camera> camera) {
	UpdateCharge(player);
	UpdateScore(course);
	UpdateTimer(course);
	UpdateSection(player, course);
	UpdateInfo();
	UpdateStartNum(startNum);
	UpdateReload(player, camera);

	// エフェクト
	auto pos = course->GetBreakPos();
	for (int i = 0; i < int(pos.size()); i += 800) {
		stars_->AddStar(pos[i]);
	}
	Vector2 center = currentBreakSprite_->GetSize();
	center.x += bonusBreakSprite_->GetSize().x;
	center.y /= 2.0f;
	stars_->Update(breakLTPos_ + center);
}

void HUD::Draw() {
	chargeBGSprite_->Draw2D();
	currentChargeSprite_->Draw2D();

	if (canDrawScore_) {
		// 区間
		breakBGSprite_->Draw2D();
		bonusBreakSprite_->Draw2D();
		currentBreakSprite_->Draw2D();

		sectionSprite_->Draw2D();
		progressSprite_->Draw2D();

		// 時間
		for (int i = 0; i < 4; ++i) {
			currentTimeSprite_[i]->Draw2D();
		}

		// 目的
		currentObjective_->Draw2D();
	}

	infoSprite_->Draw2D();
	canShoot_->Draw2D();

	if (startNumIsDraw_) {
		startNumSprite_->Draw2D();
	}

	// エフェクト
	stars_->Draw();
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

	// 区間などの情報を描画するかどうか
	canDrawScore_ = !currentSection->IsSubSection();

	if (canDrawScore_) {
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
		if (currentSection->IsCleared()) {
			rate = float(current - clear) / float(max);
			rate = clamp(rate, 0.0f, 1.0f);
		}
		// 最大スコアに応じてスプライトのサイズ変更
		length = kBreakBarWidth * rate * bonusRate_;
		bonusBreakSprite_->SetSize({ length, bonusBreakSprite_->GetSize().y });
		bonusBreakSprite_->SetPosition({ breakLTPos_.x + kBreakBarWidth * (1 - bonusRate_), breakLTPos_.y });
		bonusBreakSprite_->Update();

		// ノルマ達成/未達成
		if (currentSection->IsCleared()) {
			currentObjective_ = objective_[1].get();
		} else {
			currentObjective_ = objective_[0].get();
		}
	}
}

void HUD::UpdateTimer(Course* course) {
	Section* currentSection = course->GetCurrentSection();
	if (!currentSection->IsSubSection()) {
		int time = int(currentSection->GetTimer()->GetCurrent());
		int min = time / 60;
		int sec = time % 60;

		int num[3] = { min, sec / 10, sec % 10 };
		for (int i = 0; i < 3; ++i) {
			if (num[i] == 0) {
				num[i] = 9;
			} else {
				num[i]--;
			}
		}

		currentTimeSprite_[0]->SetTextureLeftTop({ num[0] * kTimeNumSize.x, 0 });
		currentTimeSprite_[1]->SetTextureLeftTop({ 10.0f * kTimeNumSize.x, 0 });
		currentTimeSprite_[2]->SetTextureLeftTop({ num[1] * kTimeNumSize.x, 0 });
		currentTimeSprite_[3]->SetTextureLeftTop({ num[2] * kTimeNumSize.x, 0 });
		for (int i = 0; i < 4; ++i) {
			currentTimeSprite_[i]->Update();
		}
	}
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

	Vector2 screen = ToScreen(camera, player->GetTransform().translate);

	drawCanShoot_ = player->CanShoot();
	if (drawCanShoot_) {
		canShoot_->SetTextureLeftTop({ 0,0 });
	} else {
		canShoot_->SetTextureLeftTop({ 0,40 });
	}
	canShoot_->SetPosition(screen + Vector2{ 0,-50 });
	canShoot_->Update();
}

inline Vector4 Transform(const Vector4& vector, const Matrix4x4& matrix) {
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

inline Vector2 ToScreen(std::shared_ptr<Camera> camera, Vector3 worldPos) {
	Matrix4x4 viewProjection = camera->GetViewMatrix() * camera->GetProjectionMatrix();
	Vector3 pos = worldPos;
	Vector4 world = { pos.x, pos.y, pos.z, 1.0f };
	Vector4 clipPos = Transform(world, viewProjection);

	// w除算
	Vector3 ndc;
	ndc.x = clipPos.x / clipPos.w;
	ndc.y = clipPos.y / clipPos.w;
	ndc.z = clipPos.z / clipPos.w;

	// スクリーン変換
	Vector2 screen;
	screen.x = (ndc.x + 1.0f) * 0.5f * 1280;
	screen.y = (1.0f - ndc.y) * 0.5f * 720;

	return screen;
}
