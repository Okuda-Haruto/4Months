#pragma once
#include <vector>

class Course;
class Goal;
class Human;
class Neck;

class CheckCollision {
public:
	// 初期化
	void Initialize(Course* course, Goal* goal, std::vector<Neck*> necks);
	// 更新
	void Update(Human* human);

private:
	// プレイヤーとリングの判定
	void CheckRing(Human* human);

	// プレイヤーととげの判定
	void CheckSpike(Human* human);

	// プレイヤーと壁の判定
	void CheckWall(Human* human);

	// プレイヤーと軌跡の判定
	void CheckNeck(Human* human);

	// プレイヤーと宝箱の判定
	void CheckGoal(Human* human);

	Course* course_;
	Goal* goal_;
	std::vector<Neck*> necks_;
};

