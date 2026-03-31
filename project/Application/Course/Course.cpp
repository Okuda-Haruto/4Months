#include "Course.h"
#include <numbers>

Course::Course() {
}

Course::~Course() {
}

void Course::Initialize(GameCamera* camera, std::shared_ptr<DirectionalLight> directionalLight) {
	camera_ = camera;
	directionalLight_ = directionalLight;

	chunkData_.size = { 2,12,2 };
	chunkData_.directoryPath = "resources/CSV";

	voxel_ = std::make_unique<Voxel>();
	voxel_->Initialize(this, ModelManager::GetInstance()->GetModel("resources/Course/Face", "Face.obj"), chunkData_, camera_, directionalLight_);
	sections_.push_back({ 0,-32 * 4 * 3.0f });
	sections_.push_back({ -32 * 4 * 3.0f, -1000 });

}

void Course::Update() {
	for (auto& box : boxes_) {
		if (box->IsDead()) {
			breakScore_++;
		}
	}

	std::erase_if(boxes_, [](const auto& box) {
		return box->IsDead();
		});

	for (auto& box : boxes_) {
		box->Update();
	}
	SpawnBox();

	voxel_->Update();
}

void Course::Draw(const std::shared_ptr<DirectionalLight> directionalLight) {
	std::list<Object*> boxObjects;
	for (auto& box : boxes_) {
		boxObjects.push_back(box->GetObjectData());
	}
	if (!boxObjects.empty()) {
		GameEngine::DrawInstancingObject_3D(boxObjects, directionalLight, nullptr, nullptr);
	}

	voxel_->Draw();
}

void Course::DrawAll(const std::shared_ptr<DirectionalLight> directionalLight) {
	std::list<Object*> boxObjects;
	for (auto& box : boxes_) {
		boxObjects.push_back(box->GetObjectData());
	}
	if (!boxObjects.empty()) {
		GameEngine::DrawInstancingObject_3D(boxObjects, directionalLight, nullptr, nullptr);
	}

	voxel_->DrawAll();
}

void Course::AddBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const int32_t maxHP) {
	std::unique_ptr box = std::make_unique<Box>();
	box->Initialize(this, transform, velocity, number, vacuumSensitivity, maxHP, directionalLight_);
	boxes_.push_back(std::move(box));
}

void Course::AddSplitBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const int32_t maxHP) {
	std::unique_ptr box = std::make_unique<Box>();
	box->Initialize(this, transform, velocity, number, vacuumSensitivity, maxHP, directionalLight_);
	spawnBoxes_.push_back(std::move(box));
}

void Course::SpawnBox() {
	size_t count = spawnBoxes_.size();

	for (size_t i = 0; i < count; i++) {
		boxes_.push_back(move(spawnBoxes_[i]));
	}

	spawnBoxes_.clear();

}