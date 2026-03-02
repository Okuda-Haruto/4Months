#pragma once
#include "../BaseScene.h"

#include <Sprite/Sprite.h>

class TitleScene : public BaseScene {
private:

	// HUD
	std::unique_ptr<Sprite> title_Sprite_ = nullptr;

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