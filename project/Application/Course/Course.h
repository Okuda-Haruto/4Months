#pragma once
#include "GameEngine.h"
#include "Ring/Ring.h"
#include "Spike/Spike.h"
#include "OBB.h"

class Course {
public:
	Course();
	~Course();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw(const std::shared_ptr<DirectionalLight> directionalLight);

	// 衝突時
	void OnCollide();

	// Getter
	Vector2 GetColliderCenter() { return colliderCenter_; }
	float GetColliderRadius() { return radius_; }
	std::vector<OBB> GetWalls() { return walls_; }

	std::vector<Ring*> GetRings() {
		std::vector<Ring*> rings;
		for (auto& ring : rings_) {
			rings.push_back(ring.get());
		}
		return rings;
	}

	std::vector<Spike*> GetSpikes() {
		std::vector<Spike*> spikes;
		for (auto& spike : spikes_) {
			spikes.push_back(spike.get());
		}
		return spikes;
	}

private:
	// コース中心線上の座標
	Vector3 GetPoint(float t);

	// 外壁の配置
	Vector3 CatmullRomTangent(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);
	Vector3 GetTangent(float t);
	Quaternion FromRotationMatrix(const Matrix3x3& m);
	void CreateTubeCourse();
	float DistanceToT(float dist) const;

	// 読み込んで配置
	void ReadCSV();
	void AddRing(const Vector3& spawnPos, const float radius);
	void AddSpike(const Vector3& spawnPos, const float radius);

	// 衝突判定(円)
	Vector2 colliderCenter_ = {};
	float radius_ = 20;

	// 配置物
	int ringCount_ = 50;
	int spikeCount_ = 30;
	std::vector<std::unique_ptr<Ring>> rings_;
	std::vector<std::unique_ptr<Spike>> spikes_;

	// 配置物の読み込み関連
	const int kLayerCount_ = 10; // 何層に分けるか
	const int kCSVWidth_ = 10; // 横
	const int kCSVHeight_ = 10; // 縦

	// モデル
	std::unique_ptr<Object> model_;
	std::vector<std::unique_ptr<Object>> wallModel_;

	// トランスフォーム
	SRT transform_;

	// 板を並べて円形にする
	std::vector<OBB> walls_;
	const int wallCount_ = 32;
	Vector3 wallSize_ = { 1.0f,0.75f,0.1f };
	const float wallSpace_ = 1.5f;
	std::vector<Vector3> controlPoints_;

	const int sampleCount_ = 1000;
	std::vector<float> lengthTable_;
	std::vector<float> tTable_;
	float totalLength_ = 0.0f;
};


