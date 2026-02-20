#include "Course.h"

Course::Course() {
	// 配置物の設置
	for (int i = 0; i < ringCount_; ++i) {
		rings_.push_back(std::make_unique<Ring>());
	}
	for (int i = 0; i < spikeCount_; ++i) {
		spikes_.push_back(std::make_unique<Spike>());
	}
}

void Course::Initialize() {
	for (auto& ring : rings_) {
		ring->Initialize();
	}

	for (auto& spike : spikes_) {
		spike->Initialize();
	}
}

void Course::Update() {
	for (auto& ring : rings_) {
		ring->Update();
	}

	for (auto& spike : spikes_) {
		spike->Update();
	}
}

void Course::Draw(const std::shared_ptr<DirectionalLight> directionalLight) {
	for (auto& ring : rings_) {
		ring->Draw(directionalLight);
	}

	for (auto& spike : spikes_) {
		spike->Draw(directionalLight);
	}
}