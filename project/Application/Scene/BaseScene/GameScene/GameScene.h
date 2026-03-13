#pragma once
#include <GameEngine.h>
#include "../BaseScene.h"
#include <WindowsAPI/WindowsAPI.h>
#include "Human/Player/Player.h"
#include "Human/Enemy/Enemy.h"
#include "Neck/Neck.h"
#include "Goal/Goal.h"
#include "Course/Course.h"
#include "HUD/HUD.h"

#include "CheckCollision/CheckCollision.h"
#include "GameCamera/GameCamera.h"

class GameScene : public BaseScene{
private:

	// カメラ
	SRT cameraTransform_{};
	std::shared_ptr<Camera> defaultCamera_ = nullptr;
	std::unique_ptr<GameCamera> gameCamera_;


	//デバッグカメラ
	std::shared_ptr<DebugCamera> debugCamera_ = nullptr;
	//デバッグカメラを使用するか
	bool isUseDebugCamera_ = false;

	//光源
	DirectionalLightElement directionalLightElement_{};
	std::shared_ptr<DirectionalLight> directionalLight_ = nullptr;
	UINT reflection = REFLECTION_HalfLambert;
	float shininess_ = 40.0f;

	// プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	//首
	std::vector<std::shared_ptr<Neck>> necks_;

	std::unique_ptr<Goal> goal_;

	// コース
	std::unique_ptr<Course> course_ = nullptr;

	// 当たり判定
	std::unique_ptr<CheckCollision> checkCollision_ = nullptr;

	// HUD
	std::unique_ptr<HUD> hud_ = nullptr;

public:
	//初期化
	void Initialize(std::shared_ptr<Input> input) override;
	//終了処理
	void Finalize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};