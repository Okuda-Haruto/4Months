#pragma once
#include <vector>

class Course;
class Goal;
class Human;
class Neck;
class GameCamera;

class CheckCollision {
public:
	// 初期化
	void Initialize(Course* course, Goal* goal, std::vector<Neck*> necks, GameCamera* gameCamera);
	// 更新
	void Update(Human* human);
	void UpdateImGui();

	bool IsGoal() { return isGoal_; };

private:
	// プレイヤーとリングの判定
	void CheckRing(Human* human);

	// プレイヤーととげの判定
	void CheckSpike(Human* human);

	// プレイヤーとエネルギーの判定
	void CheckEnergy(Human* human);

	// プレイヤーと壁(ボクセル)の判定
	void CheckVoxel(Human* human);

	// プレイヤーと軌跡の判定
	void CheckNeck(Human* human);

	// プレイヤーと宝箱の判定
	void CheckGoal(Human* human);

	// プレイヤーの吸い込み判定
	void CheckVacuum(Human* human);

	Course* course_;
	Goal* goal_;
	std::vector<Neck*> necks_;
	GameCamera* gameCamera_;
	bool isGoal_ = false;
	int mixType_ = 0;

	//吸い込み速度
	const float baseVacuumSpeed_ = 3.0f;
};

