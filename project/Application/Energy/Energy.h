#pragma once
#include "GameEngine.h"
#include "Human/Human.h"
#include "Math/Shape/Sphere.h"
class Energy {
public:
	// 初期化
	void Initialize(const Vector3& spawnPos, const float radius);

	// 更新
	void Update();

	// 描画
	void Draw(const std::shared_ptr<DirectionalLight> directionalLight);

	// 触れたとき
	void OnCollide(const int id);

	// 移動
	void Move(const Vector3& velocity);

	// Getter
	Sphere GetCollider() { return collider_; }
	float GetHealAmount() { return healAmount_; }
	bool IsCoolDown(const int id);

	//ソート用
	bool operator<(const Energy& another) const {
		return transform_.translate.y < another.transform_.translate.y;
	};

private:

	// モデル
	std::unique_ptr<Object> model_ = nullptr;

	// トランスフォーム
	SRT transform_;

	// 衝突判定
	Sphere collider_;

	// 回復量
	float healAmount_ = 10;

	// クールダウン
	const int boostCoolDown_ = 20;

	// 触れた対象を記録して触れないようにする
	int characterCoolDown[kMaxCharacters]{};
};


