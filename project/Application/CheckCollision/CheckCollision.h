#pragma once

class Course;
class Goal;
class Human;

class CheckCollision {
public:
	// 初期化
	void Initialize(Course* course, Goal* goal);
	// 更新
	void Update(Human* human);

private:
	// プレイヤーとリングの判定
	void CheckRing(Human* human);

	// プレイヤーととげの判定
	void CheckSpike(Human* human);

	// プレイヤーと壁の判定
	void CheckWall(Human* human);

	// プレイヤーと宝箱の判定
	void CheckGoal(Human* human);

	Course* course_;
	Goal* goal_;
};

