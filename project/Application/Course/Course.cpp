#include "Course.h"
#include <numbers>

Course::Course() {
}

Course::~Course() {
}

void Course::Initialize(std::shared_ptr<DirectionalLight> directionalLight) {
	directionalLight_ = directionalLight;

	voxel_ = std::make_unique<Voxel>();
	voxel_->Initialize(this, ModelManager::GetInstance()->GetModel("resources/Course/Face", "Face.obj"), directionalLight_);
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
		if (!box) {
			std::cout << "NULL検出\n";
			continue;
		}
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

void Course::AddBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const float radius, const int32_t maxHP) {
	std::unique_ptr box = std::make_unique<Box>();
	box->Initialize(this, transform, velocity, number, vacuumSensitivity, radius, maxHP, directionalLight_);
	boxes_.push_back(std::move(box));
}

void Course::AddSplitBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const float radius, const int32_t maxHP) {
	std::unique_ptr box = std::make_unique<Box>();
	box->Initialize(this, transform, velocity, number, vacuumSensitivity, radius, maxHP, directionalLight_);
	spawnBoxes_.push_back(std::move(box));
}

void Course::SpawnBox() {
	size_t count = spawnBoxes_.size();

	for (size_t i = 0; i < count; i++) {
		boxes_.push_back(move(spawnBoxes_[i]));
	}

	spawnBoxes_.clear();

}