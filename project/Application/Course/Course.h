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

	// Getter
	std::vector<Ring*> GetRings() {
		std::vector<Ring*> rings;
		for (auto& ring : rings_) {
			rings.push_back(ring.get());
		}
		return rings;
	}

	std::vector<Spike*> GetSpikes() {
		std::vector<Spike*> spikes;
		for (auto& spike : spikes_) {
			spikes.push_back(spike.get());
		}
		return spikes;
	}

private:
	// 衝突判定(円)
	Vector2 colliderCenter_ = {};
	float colliderRadius_ = 10;

	// 配置物
	int ringCount_ = 20;
	int spikeCount_ = 30;
	std::vector<std::unique_ptr<Ring>> rings_;
	std::vector<std::unique_ptr<Spike>> spikes_;
};


