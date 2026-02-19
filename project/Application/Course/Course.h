#pragma once
#include "GameEngine.h"
#include "Ring/Ring.h"
#include "Spike/Spike.h"

class Course {
public:
	Course();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw(const std::shared_ptr<DirectionalLight> directionalLight);

private:

	// 配置物
	int ringCount_ = 20;
	int spikeCount_ = 30;
	std::vector<std::unique_ptr<Ring>> rings_;
	std::vector<std::unique_ptr<Spike>> spikes_;
};


