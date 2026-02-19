#pragma once
#include <GameEngine.h>
#include <Input/Input.h>
#include <WindowsAPI/WindowsAPI.h>
#include "Player/Player.h"

class GameScene {
private:

	std::shared_ptr<Input> input_;

	// カメラ
	SRT cameraTransform_{};
	std::shared_ptr<Camera> defaultCamera_ = nullptr;

	//デバッグカメラ
	std::shared_ptr<DebugCamera> debugCamera_ = nullptr;
	//デバッグカメラを使用するか
	bool isUseDebugCamera_ = false;

	//光源
	DirectionalLightElement directionalLightElement_;
	std::shared_ptr<DirectionalLight> directionalLight_ = nullptr;
	UINT reflection = REFLECTION_HalfLambert;
	float shininess_ = 40.0f;

	// プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
};


