#pragma once
#include "GameEngine.h"
#include "Voxel/Voxel.h"
#include "OBB.h"
#include "Section/Section.h"

class Course {
public:
	Course();
	~Course();

	// 初期化
	void Initialize(GameCamera* camera, std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update(float playerY);

	// 描画
	void Draw(const std::shared_ptr<DirectionalLight> directionalLight);
	// 描画
	void DrawAll(const std::shared_ptr<DirectionalLight> directionalLight);
	void DrawUp(const std::shared_ptr<DirectionalLight> directionalLight);

	// Getter
	Vector2 GetColliderCenter() { return colliderCenter_; }
	float GetColliderRadius() { return radius_; }
	std::vector<OBB> GetWalls() { return walls_; }

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

	void AddBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const int32_t maxHP);
	void AddSplitBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const int32_t maxHP);
	void SpawnBox();

	// 区間追加
	void AddSection(int startChunkY, int endChunkY, float maxSeconds, int clearScore, int maxScore);

	Section* GetCurrentSection() { return currentSection_; }
	int GetCurrentSectionNumber() { return currentSectionNum_; }
	bool isFailed() { return isFailed_; }
	bool isAllCleared() { return isAllCleared_; }
private:
	
	std::shared_ptr<DirectionalLight> directionalLight_;
	GameCamera* camera_;

	// 衝突判定(円)
	Vector2 colliderCenter_ = {};
	float radius_ = 3.0f * 16;

	std::unique_ptr<Voxel> voxel_;
	std::vector<std::unique_ptr<Box>> boxes_;
	std::vector<std::unique_ptr<Box>> spawnBoxes_;

	// 配置物の読み込み関連
	const int kLayerCount_ = 10; // 何層に分けるか
	const int kCSVWidth_ = 16; // 横
	const int kCSVHeight_ = 16; // 縦

	//チャンクデータ
	CSVData chunkData_;

	// トランスフォーム
	SRT transform_;

	// 板を並べて円形にする
	std::vector<OBB> walls_;
	const int wallCount_ = 32;
	Vector3 wallSize_ = { 1.0f,0.75f,0.1f };
	const float wallSpace_ = 1.5f;

	const int sampleCount_ = 1000;
	std::vector<float> lengthTable_;
	std::vector<float> tTable_;
	float totalLength_ = 0.0f;

	std::vector<std::unique_ptr<Section>> sections_;
	Section* currentSection_ = nullptr;
	int currentSectionNum_ = 0;

	bool isFailed_ = false;
	bool isAllCleared_ = false;
};


