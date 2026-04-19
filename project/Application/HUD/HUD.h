#pragma once
#include "GameEngine.h"
#include "Effect/Stars/Stars.h"

class Player;
class Course;
class GameTimer;
class HUD {
public:
	void Initialize(Input* input, std::shared_ptr<Camera> camera);
	void Update(Player* player, Course* course, GameTimer* timer, int startNum, std::shared_ptr<Camera> camera);
	void Draw();

	void SetPauseDisplay(bool isOn);
private:
	void UpdateCharge(Player* player);
	void UpdateScore(Course* course);
	void UpdateTimer(Course* course);
	void UpdateSection(Player* player, Course* course);
	void UpdateInfo();
	void UpdateStartNum(int num);
	void UpdateReload(Player* player, std::shared_ptr<Camera> camera);

	// エネルギー
	std::unique_ptr<Sprite> chargeBGSprite_ = nullptr;
	std::unique_ptr<Sprite> currentChargeSprite_ = nullptr;
	float kEnergyBarWidth = 256.0f;
	Vector2 chargeLTPos_ = { 640 - 128,720 - 64 };

	// 壊した量
	std::unique_ptr<Sprite> breakBGSprite_ = nullptr;
	std::unique_ptr<Sprite> currentBreakSprite_ = nullptr;
	std::unique_ptr<Sprite> bonusBreakSprite_ = nullptr;
	float kBreakBarWidth = 1280 - 64;
	float bonusRate_ = 0.6f; // ボーナス部分の見た目の長さ
	Vector2 breakLTPos_ = { 32, 48 };

	// タイマー
	std::unique_ptr<Sprite> currentTimeSprite_[4]{};
	Vector2 currentTimeSpriteSize_ = { 64,64 };
	Vector2 kTimeNumSize = { 256,256 };
	Vector2 timePos_[4] = { {60,60},{60 + 35,60},{60 + 70,60},{60 + 105,60}};

	// 区間の進度
	std::unique_ptr<Sprite> sectionSprite_ = nullptr;
	std::unique_ptr<Sprite> progressSprite_ = nullptr;
	Vector2 sectionBarSize_ = { 32,450 };
	Vector2 sectionLTPos_ = { 1280 - 48, 150};

	// エネルギー
	std::unique_ptr<Sprite> infoSprite_ = nullptr;
	Vector2 infoLTPos_ = { 12,720 - (62 + 12) };

	// 目的
	std::unique_ptr<Sprite> objective_[2]{};
	Sprite* currentObjective_ = nullptr;
	Vector2 objectivePos_ = { 640, 110 };
	Vector2 objectiveSize_ = { 480,120 };

	std::unique_ptr<Sprite> startNumSprite_ = nullptr;
	Vector2 startNumPos_ = { 640,360 };
	bool startNumIsDraw_;

	std::unique_ptr<Sprite> canShoot_ = nullptr;
	bool drawCanShoot_ = false;

	Input* input_;

	// ゲージ増加エフェクト
	std::unique_ptr<Stars> stars_;
};

inline Vector4 Transform(const Vector4& vector, const Matrix4x4& matrix);
inline Vector2 ToScreen(std::shared_ptr<Camera> camera, Vector3 worldPos);