#pragma once
#include "../BaseScene.h"

#include <Sprite/Sprite.h>
#include "TitleMoji/TitleMoji.h"

class TitleScene : public BaseScene {
private:

	// HUD
	std::unique_ptr<Sprite> title_Sprite_ = nullptr;

	// ブロック
	std::unique_ptr<TitleMoji> logo_ = nullptr;

	//光源
	DirectionalLightElement directionalLightElement_{};
	std::shared_ptr<DirectionalLight> directionalLight_ = nullptr;
	UINT reflection = REFLECTION_HalfLambert;
	float shininess_ = 40.0f;

	// カメラ
	SRT cameraTransform_{};
	std::shared_ptr<Camera> defaultCamera_ = nullptr;

	//デバッグカメラ
	std::shared_ptr<DebugCamera> debugCamera_ = nullptr;
	//デバッグカメラを使用するか
	bool isUseDebugCamera_ = false;

	// 音声
	std::unique_ptr<Audio> bgm_ = nullptr;
	std::unique_ptr<Audio> selectSE_ = nullptr;
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