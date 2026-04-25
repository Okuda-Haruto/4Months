#pragma once
#include "../BaseScene.h"

#include <Sprite/Sprite.h>
#include "GameCamera/GameCamera.h"
#include "TitleMoji/TitleMoji.h"
#include "TitleMoji/TitleScreen/TitleScreen.h"
#include "Fade/Fade.h"
#include "Effect/Combine/Combine.h"

class TitleScene : public BaseScene {
private:

	// HUD
	std::unique_ptr<Sprite> title_Sprite_ = nullptr;
	std::unique_ptr<TitleScreen> titleScreen_ = nullptr;

	// ブロック
	std::unique_ptr<TitleMoji> logo_ = nullptr;

	//オブジェクト
	std::unique_ptr<Object> livingRoom_ = nullptr;
	std::unique_ptr<Object> studio_ = nullptr;
	std::array<std::unique_ptr<Object>, 6> humans_;
	std::array<SRT, 6> human_Tranaforms_;

	std::unique_ptr<Fade> fade_;

	//光源
	DirectionalLightElement directionalLightElement_{};
	std::shared_ptr<DirectionalLight> directionalLight_ = nullptr;
	UINT reflection = REFLECTION_HalfLambert;
	float shininess_ = 40.0f;

	// カメラ
	std::shared_ptr<Camera> defaultCamera_ = nullptr;

	//スタジオからの視点
	std::shared_ptr<Camera> studioCamera_ = nullptr;
	std::shared_ptr<GameCamera> studioGameCamera_ = nullptr;
	bool isChangeStudioCamera_ = false;

	// 居間からの視点
	std::shared_ptr<Camera> livingRoomCamera_ = nullptr;
	std::shared_ptr<GameCamera> livingGameCamera_ = nullptr;

	// 遷移演出
	std::unique_ptr<Combine> combine_ = nullptr;
	bool sceneChange_ = false;

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