#pragma once
#include "Human/Human.h"

class Player : public Human{
public:
	// 初期化
	void Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update(const std::shared_ptr<Input> input);

	// 描画
	void Draw();

private:

};

