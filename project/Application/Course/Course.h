#pragma once
#include "GameEngine.h"
#include "Ring/Ring.h"
#include "Spike/Spike.h"
#include "OBB.h"

class Course {
public:
	Course();

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
	Vector3 GetPoint(float t);
	Vector3 CatmullRomTangent(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);
	Vector3 GetTangent(float t);
	Quaternion FromRotationMatrix(const Matrix3x3& m);
	void CreateTubeCourse();

	// 衝突判定(円)
	Vector2 colliderCenter_ = {};
	float radius_ = 10;

	// 配置物
	int ringCount_ = 3;
	int spikeCount_ = 30;
	std::vector<std::unique_ptr<Ring>> rings_;
	std::vector<std::unique_ptr<Spike>> spikes_;

	// モデル
	std::vector<std::unique_ptr<Object>> wallModel_;

	// トランスフォーム
	SRT transform_;

	// 板を並べて円形にする
	std::vector<OBB> walls_;
	const int wallCount_ = 32;
	Vector3 wallSize_ = { 1.0f,0.75f,0.1f };
	const float wallSpace_ = 1.5f;
	std::vector<Vector3> controlPoints_;
};


