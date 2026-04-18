#include "Course.h"
#include <numbers>

Course::Course() {
}

Course::~Course() {
}

void Course::Initialize(CSVData chunkData, GameCamera* camera, std::shared_ptr<DirectionalLight> directionalLight) {
	chunkData_ = chunkData;
	camera_ = camera;
	directionalLight_ = directionalLight; chunkData_;

	voxel_ = std::make_unique<Voxel>();
	voxel_->Initialize(this, ModelManager::GetInstance()->GetModel("resources/Course/Face", "Face.obj"), chunkData_, camera_, directionalLight_);

	goalBarrier_ = std::make_unique<GoalBarrier>();
	goalBarrier_->Initialize(-3.0f * 2.0f * 16 * 19, camera_->GetCamera());

	// 区間の設定(上~下)
	AddSection(0, 18, 54, 50000, 75000);
	currentSection_ = sections_[0].get();
}

void Course::Update(float playerY) {
	breakPos_.clear();

	// 今いる区間
	for (int i = currentSectionNum_; i < sections_.size(); ++i) { // 今より上に行っても区間は戻らない
		if (sections_[i]->IsEnter(playerY)) {
			currentSectionNum_ = i;
			currentSection_ = sections_[i].get();
		}

		if ((!sections_[i]->IsCleared() &&
			sections_[i]->IsOver(playerY)) ||
			sections_[i]->GetTimer()->GetCurrent() == 0) {
			isFailed_ = true;
		}
	}
	currentSection_->Update(playerY);
	// クリア条件
	if (sections_.back()->IsCleared()){
		goalBarrier_->Clear();
	}
	if (sections_.back()->IsCleared() && sections_.back()->IsOver(playerY)) {
		isAllCleared_ = true;
	}

	for (auto& box : boxes_) {
		if (box->IsDead()) {
			sections_[currentSectionNum_]->AddBreak(1);
			breakPos_.push_back(box->GetTransform().translate);
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

	goalBarrier_->Update(camera_);
}

void Course::Draw(const std::shared_ptr<DirectionalLight> directionalLight) {
	std::list<Object*> boxObjects;
	for (auto& box : boxes_) {
		boxObjects.push_back(box->GetObjectData());
	}
	if (!boxObjects.empty()) {
		GameEngine::DrawInstancingObject_3D(boxObjects, directionalLight, nullptr, nullptr);
	}

	goalBarrier_->Draw();

	voxel_->Draw();
}

// 描画
void Course::Draw(AABB drawRange, const std::shared_ptr<DirectionalLight> directionalLight) {
	std::list<Object*> boxObjects;
	for (auto& box : boxes_) {
		boxObjects.push_back(box->GetObjectData());
	}
	if (!boxObjects.empty()) {
		GameEngine::DrawInstancingObject_3D(boxObjects, directionalLight, nullptr, nullptr);
	}

	goalBarrier_->Draw();

	voxel_->Draw(drawRange);
}

void Course::DrawAll(const std::shared_ptr<DirectionalLight> directionalLight) {
	std::list<Object*> boxObjects;
	for (auto& box : boxes_) {
		boxObjects.push_back(box->GetObjectData());
	}
	if (!boxObjects.empty()) {
		GameEngine::DrawInstancingObject_3D(boxObjects, directionalLight, nullptr, nullptr);
	}

	goalBarrier_->Draw();

	voxel_->DrawAll();
}

void Course::DrawUp(const std::shared_ptr<DirectionalLight> directionalLight) {
	std::list<Object*> boxObjects;
	for (auto& box : boxes_) {
		boxObjects.push_back(box->GetObjectData());
	}
	if (!boxObjects.empty()) {
		GameEngine::DrawInstancingObject_3D(boxObjects, directionalLight, nullptr, nullptr);
	}

	goalBarrier_->Draw();

	voxel_->DrawUp();
}

void Course::DrawGoalBarrier() {
	goalBarrier_->Draw();
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

void Course::AddSection(int startChunkY, int endChunkY, float maxSeconds, int clearScore, int maxScore) {
	std::unique_ptr<Section> newSection = std::make_unique<Section>();
	newSection->Initialize(startChunkY, endChunkY, maxSeconds, clearScore, maxScore);
	sections_.push_back(std::move(newSection));
}
