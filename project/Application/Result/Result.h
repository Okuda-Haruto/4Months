#pragma once
#include "GameEngine.h"

enum class ResultPhase {
	Set,
	Rise,
	ZoomOut,
	ResultSet,
	DisplayResult,
	ResultOut,
	Clap
};

class Player;
class Course;
class Result {
public:
	void Initialize(std::shared_ptr<DirectionalLight> directionalLight, int stageNum);
	void Update(Player* player, Input* input, Course* course);
	void Draw();


	ResultPhase GetPhase() { return phase_; }
	float CurrentTimeRate() { return clamp(timer_[int(phase_)] / kTime[int(phase_)],0.0f,1.0f); }
	bool IsEnd() { return isEnd_; }
private:
	void UpdateTimer(Course* course);
	void UpdateBreakAmount(Course* course);
	void UpdateBreakRate(Course* course);

	ResultPhase phase_ = ResultPhase::Set;

	const float kTime[7]{
		1.5f,
		5.0f,
		2.0f,
		1.5f,
		2.0f,
		1.0f,
		2.0f
	};
	float timer_[7]{};

	std::unique_ptr<Object> studio_;

	float startY_ = 0;
	float endY_ = 155;
	bool isEnd_ = false;

	SRT studioTransform_ = { {0.13f,0.13f,0.13f}, {}, {0,150,2} };

	const int kScoreItemCount = 15;

	// リザルト項目
	const float kScoreTime[15]{
		// ステージ評価枠
		0.3f,
		0.5f,
		0.7f,

		// ステージNo
		0.3f,
		0.5f,
		0.7f,

		// ステージ評価
		1.1f,
		1.4f,
		1.7f,

		// 数
		2.0f,
		// 率
		2.2f,
		// 時間
		2.4f,

		// 総合評価枠
		2.6f,
		// 総合評価
		3.3f,

		// 上の枠
		0.0f
	};
	float scoreTimer_[15];
	std::vector<std::unique_ptr<Object>> scores_;

	Vector3 euler[15]{};
	SRT scoreTransform_[15]{
		// ステージごと枠
		{{0.45f,0.45f,0.45f},{0,2.89f,0},{-8.531f,156.3f,-0.153f}},
		{{0.45f,0.45f,0.45f},{0,2.89f,0},{-8.997f,156.3f,1.663f}},
		{{0.45f,0.45f,0.45f},{0,2.89f,0},{-9.463f,156.3f,3.478f}},

		// ステージナンバー
		{{0.15f,0.15f,0.15f},{0,2.89f,0},{}},
		{{0.15f,0.15f,0.15f},{0,2.89f,0},{}},
		{{0.15f,0.15f,0.15f},{0,2.89f,0},{}},

		// ステージ評価
		{{0.2f,0.2f,0.2f},{},{-8.505f,156.6f,-0.156f}},
		{{0.2f,0.2f,0.2f},{},{-8.978f,156.6f,1.684f}},
		{{0.2f,0.2f,0.2f},{},{-9.439f,156.6f,3.476f}},

		// 数
		{{0.2f,0.2f,0.2f},{},{-9.407f,155.4f,3.054f}},
		// 率
		{{0.2f,0.2f,0.2f},{},{-9.407f,154.8f,3.054f}},
		// 時間
		{{0.2f,0.2f,0.2f},{},{-9.407f,154.21f,3.054f}},

		// 総合枠
		{{0.75f,0.65f,0.75f},{0,2.89f,0},{-8.6f,154.9f,0}},
		// 総合評価
		{{0.65f,0.65f,0.65f},{0,2.89f,0},{-8.6f,155.1f,0}},

		// 上枠
		{{0.5f,0.5f,0.25f},{}, {-9.082f,156.3f, 1.813f}}
	};

	Vector3 stageNumPos[3][3] = {
		{{-8.479f,156.77f, -0.159f},{-8.758f,156.9f,1.731f},{-9.39f,157.05f,3.488f} },
		{{-8.479f,157.22f, -0.159f},{-8.758f,157.34f,1.731f},{-9.39f,157.5f,3.488f} },
		{{-8.479f,157.66f, -0.159f},{-8.758f,156.8f,1.731f},{-9.39f,157.98f,3.488f} },
	};

	bool endTimerIsActive_ = false;
	float endTimer_ = 1.0f;



	struct NumberDisplay3D {
		std::vector<std::unique_ptr<Object>> object;
		int digitCount;
	};

	// ブロック破壊率
	NumberDisplay3D breakRate_ = {
		.digitCount = 4,
	};

	// ブロック破壊個数
	NumberDisplay3D breakCount_ = {
		.digitCount = 6,
	};

	// リザルト時タイマー
	NumberDisplay3D sectionTime_ = {
		.digitCount = 6,
	};

	std::unique_ptr<Audio> ascend_;
	std::unique_ptr<Audio> voice_;
	std::unique_ptr<Audio> space_;
};