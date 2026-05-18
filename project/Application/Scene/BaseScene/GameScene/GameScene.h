#pragma once
#include <GameEngine.h>
#include "../BaseScene.h"
#include <WindowsAPI/WindowsAPI.h>
#include "Human/Player/Player.h"
#include "Course/Course.h"
#include "HUD/HUD.h"
#include "HitPreview/HitPreview.h"
#include "StartCountdown/StartCountdown.h"
#include "CheckCollision/CheckCollision.h"
#include "GameCamera/GameCamera.h"
#include "Fade/Fade.h"
#include "Effect/Combine/Combine.h"

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

	std::unique_ptr<Object> skydome_;

	// プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	// コース
	std::unique_ptr<Course> course_ = nullptr;

	// 当たり判定
	std::unique_ptr<CheckCollision> checkCollision_ = nullptr;

	// HUD
	std::unique_ptr<HUD> hud_ = nullptr;

	// 予測表示
	std::unique_ptr<HitPreview> hitPreview_ = nullptr;

	// 開始カウントダウン
	std::unique_ptr<StartCountdown> startCountdown_ = nullptr;

	std::unique_ptr<Fade> fade_;

	// タイトルからの遷移
	std::unique_ptr<Combine> gameTransition = nullptr;

	//クリアしているか
	bool isClear_ = false;
	//クリアカメラの座標
	SRT clearCameraTransform_;
	//クリアカメラのY位座標
	float clearY_ = 0;
	//ループカメラで上昇中か
	bool isUp_ = false;

	int32_t chunkHeight_;

	float skipHold_ = 0;

	CourseData courseData_;

	// BGM
	std::unique_ptr<Audio> bgm_ = nullptr;

public:
	//初期化
	void Initialize(std::shared_ptr<Input> input) override;
	//終了処理
	void Finalize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;

private:
	//コースを読む
	void LoadCourse(std::string filePath);
};