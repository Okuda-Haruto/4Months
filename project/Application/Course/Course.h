#pragma once
#include "GameEngine.h"
#include "Voxel/Voxel.h"
#include "OBB.h"

class Course {
public:
	Course();
	~Course();

	// 初期化
	void Initialize(GameCamera* camera, std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update();

	// 描画
	void Draw(const std::shared_ptr<DirectionalLight> directionalLight);

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

	Voxel* GetVoxel() { return voxel_.get(); }

	void AddBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const float radius, const int32_t maxHP);
	void AddSplitBox(const SRT& transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const float radius, const int32_t maxHP);
	void SpawnBox();

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
};


