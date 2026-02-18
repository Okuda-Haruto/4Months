#pragma once
#include "GameEngine.h"

class Player {
public:
	// 初期化
	void Initialize(const std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update(const std::shared_ptr<Input> input);

	// 描画
	void Draw();

private:
	// モデル
	std::unique_ptr<Object> model_ = nullptr;

	// トランスフォーム
	SRT transform_;
};

