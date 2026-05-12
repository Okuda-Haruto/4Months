#pragma once
#include "GameEngine.h"
#include "Voxel/Voxel.h"
#include "OBB.h"
#include "Section/Section.h"
#include "GoalBarrier/GoalBarrier.h"

//コースファイルデータ
struct CourseData {
	CSVData data;
	std::string fileName;
};

class Course {
public:
	Course();
	~Course();

	// 初期化
	void Initialize(CSVData chunkData, GameCamera* camera, std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update(Human* player);

	// 描画
	void Draw(const std::shared_ptr<DirectionalLight> directionalLight);
	// 描画
	void Draw(AABB drawRange, const std::shared_ptr<DirectionalLight> directionalLight);
	// 描画
	void DrawAll(const std::shared_ptr<DirectionalLight> directionalLight);
	void DrawUp(const std::shared_ptr<DirectionalLight> directionalLight);

	void DrawGoalBarrier();

	// Getter
	Vector2 GetColliderCenter() { return colliderCenter_; }
	float GetColliderRadius() { return radius_; }

	std::vector<Box*> GetBoxes() {
		std::vector<Box*> boxes;
		for (auto& box : boxes_) {
			boxes.push_back(box.get());
		}
		return boxes;
	}

	std::vector<Box*> GetSpawnBoxes() {
		std::vector<Box*> boxes;
		for (auto& box : spawnBoxes_) {
			boxes.push_back(box.get());
		}
		return boxes;
	}

	Voxel* GetVoxel() { return voxel_.get(); }
	CSVData GetChunkData() { return chunkData_; }

	void AddBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const float maxHP);
	void AddSplitBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const float maxHP);
	void SpawnBox();

	// 区間追加
	void AddSection(int startChunkY, int endChunkY, float maxSeconds, int clearScore, int maxScore);
	void AddSubSection(int startChunkY, int endChunkY);

	Section* GetCurrentSection() { return currentSection_; }
	int GetCurrentSectionNumber() { return currentSectionNum_; }
	bool isEnd() { return isEnd_; }
	bool isAllCleared() { return isAllCleared_; }
	bool InSubSection();

	std::vector<Vector3> GetBreakPos() { return breakPos_; }

	bool isNoNormaMode_ = false;

	// 直前の区間の破壊率
	float GetPrevBreakRate() { return sections_[currentSectionNum_ - 1]->GetBreakRate(GetBoxes()); }
	float GetCurrBreakRate() { return sections_[currentSectionNum_]->GetBreakRate(GetBoxes()); }
public:
	void SetNoNormaMode(bool flag) { isNoNormaMode_ = flag; }
	bool GetNoNormaMode() const { return isNoNormaMode_; }
private:
	
	std::shared_ptr<DirectionalLight> directionalLight_;
	GameCamera* camera_;

	// 衝突判定(円)
	Vector2 colliderCenter_ = {};
	float radius_ = 3.0f * 16;

	std::unique_ptr<Voxel> voxel_;
	std::vector<std::unique_ptr<Box>> boxes_;
	std::vector<std::unique_ptr<Box>> spawnBoxes_;

	std::vector<std::unique_ptr<GoalBarrier>> goalBarriers_;

	//チャンクデータ
	CSVData chunkData_;

	// トランスフォーム
	SRT transform_;

	std::vector<std::unique_ptr<Section>> sections_;
	Section* currentSection_ = nullptr;
	int currentSectionNum_ = 0;

	bool isEnd_ = false;
	bool isAllCleared_ = false;

	// 破壊された場所
	std::vector<Vector3> breakPos_;

	// 区間データ
	struct SectionData {
		bool isFailed = false;
	};
	std::vector<SectionData> sectionsData_;

	std::unique_ptr<Audio> failSE_;
};


