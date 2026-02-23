#include "Course.h"

Course::Course() {
	model_ = make_unique<Object>();
	model_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course", "Course.obj"));
	model_->SetShininess(40.0f);
	model_->SetColor({ 1,1,1,0.75f });
	transform_ = { {1,1,1},{},{0,-200,0} };
	model_->SetTransform(transform_);

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

	model_->SetDirectionalLight(directionalLight);
	model_->Draw3D();
}