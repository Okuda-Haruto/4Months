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

	// 区間の設定(上~下)
	AddSection(0, 2, 30, 2000, 4000, RankBorders{{ 80, 40 }, { 3000, 2000 }, { 10, 0 }});
	AddSection(4, 10, 40, 12000, 15000, RankBorders{ { 60, 30 }, { 14000, 12000 }, { 10, 0 } });
	AddSection(12, 20, 60, 20000, 25000, RankBorders{ { 40, 20 }, { 23000, 20000 }, { 15, 0 } });
	goalBarriers_.clear();
	for (int i = 0; i < sections_.size(); ++i) {
		float y = sections_[i]->GetEndPos().y;

		std::unique_ptr<GoalBarrier> barrier = std::make_unique<GoalBarrier>();
		barrier->Initialize(y, camera_->GetCamera());

		goalBarriers_.push_back(std::move(barrier));
	}

	currentSection_ = sections_[0].get();

	resultFlash_ = std::make_unique<Flash>();
	resultFlash_->Initialize();

	failSE_ = std::make_unique<Audio>();
	failSE_->Initialize("resources/SE・BGM/Game/failure.mp3", 0.5f);

	gameover_ = std::make_unique<GameOver>();
	gameover_->Initialize(directionalLight_);
}

void Course::Update(Human* player) {
	breakPos_.clear();
	
	// 今いる区間
	for (int i = currentSectionNum_; i < sections_.size(); ++i) { // 今より上に行っても区間は戻らない
		if (sections_[i]->IsEnter(player->GetTransform().translate.y)) {
			currentSectionNum_ = i;
			currentSection_ = sections_[i].get();

			if (!sections_[i]->IsSubSection()) {
				resultState_ = ResultState::RotateIn;
				resultFlash_->Set();
			}
		}

		if (!sections_[i]->IsSubSection()) {
			// 失敗時
			if ((!sections_[i]->IsCleared() &&
				sections_[i]->IsOver(player->GetTransform().translate.y)) ||
				sections_[i]->GetTimer()->GetCurrent() <= 0) {

				if (!sectionsData_[i].isFailed) {
					sectionsData_[i].isFailed = true;

					isSectionFailed_ = true;

					if (!player->IsBreak()) {
						player->BreakSpinner();

						if (sections_.size() > i + 2) {
							// 次の区間に入る
							currentSectionNum_+= 2;
							currentSection_ = sections_[currentSectionNum_].get();
						}
					}

					failSE_->SoundPlayWave();
				}

				//これ以上セクション移動がない場合エラーが出る
				if (sections_.size() < i) {
					if (sections_[i + 1]) {
						// 次の地点に移動
						player->SetTranslate(sections_[i + 1]->GetStartPos());
					}
				}
			}
		}

		if (currentSection_->IsSubSection() && !isSectionFailed_) {
			resultFlash_->Update();
		}
	}

	if (isSectionFailed_ && player->IsBreak()) {
		gameover_->Update();
	}

	currentSection_->Update(player->GetTransform().translate.y);

	// クリア条件
	for (int i = 0; i < sections_.size(); ++i) {
		if (sections_[i]->IsCleared()) {
			if (i < goalBarriers_.size()) {
				goalBarriers_[i]->Clear();
			}
		}
	}

	// コース終了
	if (sections_.back()->IsOver(player->GetTransform().translate.y)) {
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

	for (auto& barrier : goalBarriers_) {
		barrier->Update(camera_);
	}
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

	if (isSectionFailed_) {
		for (auto& barrier : goalBarriers_) {
			barrier->Draw();
		}
	} else {
		for (int i = currentSectionNum_; i < goalBarriers_.size(); ++i) {
			if (!sections_[i]->IsSubSection()) {
				goalBarriers_[i]->Draw();
			}
		}
	}

	if (currentSection_->IsSubSection()) {
		resultFlash_->Draw();
	}
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

	for (auto& barrier : goalBarriers_) {
		barrier->Draw();
	}

	voxel_->Draw(drawRange);

	if (currentSection_->IsSubSection()) {
		resultFlash_->Draw();
	}
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
	if (currentSection_->IsSubSection()) {
		resultFlash_->Draw();
	}
}

void Course::DrawUp(const std::shared_ptr<DirectionalLight> directionalLight) {
	std::list<Object*> boxObjects;
	for (auto& box : boxes_) {
		boxObjects.push_back(box->GetObjectData());
	}
	if (!boxObjects.empty()) {
		GameEngine::DrawInstancingObject_3D(boxObjects, directionalLight, nullptr, nullptr);
	}

	for (auto& barrier : goalBarriers_) {
		barrier->Draw();
	}

	voxel_->DrawUp();
	if (currentSection_->IsSubSection()) {
		resultFlash_->Draw();
	}
}

void Course::DrawGoalBarrier() {
	//for (auto& barrier : goalBarriers_) {
	//	barrier->Draw();
	//}
}

void Course::DrawGameOver() {
	gameover_->Draw();
}

void Course::AddBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const float maxHP) {
	if (boxes_.size() >= 65536)return;
	std::unique_ptr box = std::make_unique<Box>();
	box->Initialize(this, transform, velocity, number, vacuumSensitivity, maxHP, directionalLight_);
	boxes_.push_back(std::move(box));
}

void Course::AddSplitBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const float maxHP) {
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

void Course::AddSection(int startChunkY, int endChunkY, float maxSeconds, int clearScore, int maxScore, const RankBorders& rankBorders) {
	std::unique_ptr<Section> newSection = std::make_unique<Section>();
	newSection->Initialize(startChunkY, endChunkY, maxSeconds, clearScore, maxScore, voxel_.get(), rankBorders);

	// 間が空いていればノルマなし区間を挿入
	if (!sections_.empty()) {
		float blockScale = 3.0f;
		float chunkSize = blockScale * 2 * 16;
		float endPos = sections_.back()->GetEndPos().y;
		int prevEnd = int(endPos / chunkSize);
		if (startChunkY > -prevEnd) {
			AddSubSection(-prevEnd, startChunkY - 1);
		}
	}

	sections_.push_back(std::move(newSection));
	sectionsData_.push_back(SectionData());
}

void Course::AddSubSection(int startChunkY, int endChunkY) {
	std::unique_ptr<Section> newSection = std::make_unique<Section>();
	newSection->Initialize(startChunkY, endChunkY);
	sections_.push_back(std::move(newSection));
	sectionsData_.push_back(SectionData());
}

bool Course::InSubSection() {
	if (currentSection_) {
		return currentSection_->IsSubSection();
	}
	return false;
}
