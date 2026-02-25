#pragma once
#include "Human/Human.h"

class Enemy : public Human {
public:
	// 初期化
	void Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update();

	// 描画
	void Draw();

private:

};