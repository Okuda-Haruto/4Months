#pragma once
#include "Human/Human.h"

class Course;

class Player : public Human{
public:
	// 初期化
	void Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight, const std::shared_ptr<Camera> camera);

	// 更新
	void Update(const std::shared_ptr<Input> input, Course* course);

	// 描画
	void Draw();

private:
	float startTime_;
};

