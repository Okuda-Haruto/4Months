#pragma once
#include "Human/Human.h"
#include "Course/Course.h"
#include "Neck/Neck.h"

class Enemy : public Human {
public:
	// 初期化
	void Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update();

	// 描画
	void Draw();

	//setter
	void SetRings(std::vector<Ring*> rings) { rings_ = rings; }
	void SetSpikes(std::vector<Spike*> spikes) { spikes_ = spikes; }
	void SetNeck(std::vector<std::shared_ptr<Neck>> necks) { necks_ = necks; }

private:
	//コース
	std::vector<Ring*> rings_;
	std::vector<Spike*> spikes_;

	std::vector<std::shared_ptr<Neck>> necks_;
};