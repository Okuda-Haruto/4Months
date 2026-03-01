#pragma once

class Player;
class Course;
class Goal;

class CheckCollision {
public:
	// 初期化
	void Initialize(Player* player, Course* course, Goal* goal);
	// 更新
	void Update();

private:
	// プレイヤーとリングの判定
	void CheckRing();

	// プレイヤーととげの判定
	void CheckSpike();

	// プレイヤーと壁の判定
	void CheckWall();

	// プレイヤーと宝箱の判定
	void CheckGoal();

	Player* player_;
	Course* course_;
	Goal* goal_;
};

