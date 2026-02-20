#pragma once

class Player;
class Course;

class CheckCollision {
public:
	// 初期化
	void Initialize(Player* player, Course* course);
	// 更新
	void Update();

private:
	// プレイヤーとリングの判定
	void CheckRing();

	// プレイヤーととげの判定
	void CheckSpike();

	Player* player_;
	Course* course_;
};

