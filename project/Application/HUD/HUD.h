#pragma once
#include "GameEngine.h"
#include "Effect/Stars/Stars.h"

class Player;
class Course;
class GameTimer;
class HUD {
public:
	void Initialize(Input* input, std::shared_ptr<Camera> camera, std::shared_ptr<DirectionalLight> directionalLight);
	void Update(Player* player, Course* course, GameTimer* timer, int startNum, std::shared_ptr<Camera> camera);
	void Draw();

	void SetPauseDisplay(bool isOn);
private:
	void UpdateCharge(Player* player);
	void UpdateScore(Course* course);
	void UpdateTimer(Course* course);
	void UpdateBreakRate(Course* course);
	void UpdateBreakAmount(Course* course);
	void UpdateInfo();
	void UpdateStartNum(int num);
	void UpdateReload(Player* player, std::shared_ptr<Camera> camera);
	void UpdateResult(Course* course);

	// カメラ
	std::shared_ptr<Camera> camera_ = nullptr;
	SRT cameraTransform_;

	// 壊した量
	std::unique_ptr<Sprite> breakBGSprite_ = nullptr;
	std::unique_ptr<Sprite> currentBreakSprite_ = nullptr;
	const Vector2 breakSpriteSize = { 5300,800 };
	Vector2 kBreakBarSize = { 120 * (breakSpriteSize.x / breakSpriteSize.y), 120.0f};
	Vector2 breakLTPos_ = { 230, 0 };


	bool canDrawScore_ = false;
	bool isSectionFailed_ = false;

	bool canDrawPlayingInfo_ = false;


	// 数字ひとつ分の画像内サイズ
	Vector2 kNumberSize = { 256,256 };
	// タイマー
	std::unique_ptr<Sprite> currentTimeSprite_[4]{};
	Vector2 currentTimeSpriteSize_ = { 64,64 };
	Vector2 timePos_[4] = { {60,60},{60 + 35,60},{60 + 70,60},{60 + 105,60} };

	std::unique_ptr<Sprite> infoSprite_ = nullptr;
	Vector2 infoLTPos_ = { 12,720 - (62 + 12) };

	// 目的
	std::unique_ptr<Sprite> objective_[2]{};
	Sprite* currentObjective_ = nullptr;
	Vector2 objectivePos_ = { 640, 110 };
	Vector2 objectiveSize_ = { 480,120 };

	std::unique_ptr<Sprite> startNumSprite_ = nullptr;
	Vector2 startNumPos_ = { 640,360 };
	bool startNumIsDraw_;

	// 発射可能
	std::unique_ptr<Sprite> canShoot_ = nullptr;
	bool drawCanShoot_ = false;

	// 区間の結果
	std::unique_ptr<Object> sectionResult_[3]{};
	Vector3 resultPos_[3] = { {4.5f,2.0f,17.5f},{4.5f,-1.5f,17.5f}, {4.5f,-5.0f,17.5f} };
	Vector3 resultSize_ = { 1,1,1 };

	struct HUDObject {
		std::unique_ptr<Object> object;
		Vector3 scale;
		Vector3 pos;
	};

	struct RankObject {
		std::unique_ptr<Object> object[3];
		Vector3 scale;
		Vector3 pos;
	};
	// ランク
	RankObject sectionRank_ = {
		.scale = {2,2,2},
		.pos = {-4.7f,-2.8f,17.5f}
	};
	int rank_ = 0;

	struct NumberDisplay2D {
		std::vector<std::unique_ptr<Sprite>> sprite;
		Vector2 size;
		Vector2 pos;
		int digitCount;
		float spacing;
	};

	struct NumberDisplay3D {
		std::vector<std::unique_ptr<Object>> object;
		Vector3 scale;
		Vector3 pos;
		int digitCount;
		float spacing;
	};

	// タイマー
	NumberDisplay2D timer_ = {
		.size = {64,64},
		.pos = {40,60},
		.digitCount = 5,
		.spacing = 35
	};
	int lastTime_ = 0;

	// ブロック破壊率
	NumberDisplay3D breakRate_ = {
		.scale = {1.2f,1.2f,1.2f},
		.pos = {5,-0.7f,17.5f},
		.digitCount = 4,
		.spacing = 0.9f
	};

	// ブロック破壊個数
	NumberDisplay3D breakCount_ = {
		.scale = {1.2f,1.2f,1.2f},
		.pos = {5,2.8f,17.5f},
		.digitCount = 6,
		.spacing = 0.8f
	};

	// リザルト時タイマー
	NumberDisplay3D sectionTime_ = {
		.scale = {1.2f,1.2f,1.2f},
		.pos = {5,-4.2f,17.5f},
		.digitCount = 6,
		.spacing = 0.75f
	};

	Quaternion objectRot_ = {};

	int unuseDigitCountBreakRate_ = 0;
	int unuseDigitCountBreakAmount_ = 0;
	bool useMinusSectionTime_ = false;

	struct ChargeUI {
		struct ChargeBar {
			std::unique_ptr<Sprite> back;
			std::unique_ptr<Sprite> light;
			std::unique_ptr<Sprite> frame;

			float length = 0;
			const Vector2 defaultPos = { 640 - 160, 600 };
			const Vector2 defaultSize = {3.2f * 100, 100};
			Vector2 pos = defaultPos;
			Vector2 size = defaultSize;
			
			// 発射後のイージング
			const float kInactiveEaseTime = 0.75f;
			float inactiveEaseTimer = 0;
			// 再発射可能
			const float kReactiveEaseTime = 0.3f;
			float reactiveEaseTimer = 0;

			float chargeAtShoot = 0;
		};
		struct ChargeIcon {
			std::unique_ptr<Sprite> frame;
			std::unique_ptr<Sprite> light;
			std::unique_ptr<Sprite> icon;
			const Vector2 defaultPos = { 640 - 160 + 290 + 50, 600 + 50 };
			const Vector2 defaultSize = { 100, 100 };
			Vector2 pos = defaultPos;
			Vector2 size = defaultSize;
			float rot = 0;
		};

		ChargeBar bar;
		ChargeIcon icon;
	};
	ChargeUI charge_;

	float resultTimer_ = 0;

	Input* input_;

	// ゲージ増加エフェクト
	std::unique_ptr<Stars> stars_;
};

inline Vector4 Transform(const Vector4& vector, const Matrix4x4& matrix);
inline Vector2 ToScreen(std::shared_ptr<Camera> camera, Vector3 worldPos);
Vector3 CameraLocalToWorld(Vector3 local, Vector3 camPos, Vector3 right, Vector3 up, Vector3 forward);
int ConvertPartNumber(int num);
std::unique_ptr<Sprite> InitSprite(const std::string& path, const Vector2& pos, const Vector2& size, const Vector2& anchorPoint);