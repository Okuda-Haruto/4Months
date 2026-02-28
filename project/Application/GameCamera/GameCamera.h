#pragma once
#include "Camera/Camera.h"
#include "Human/Player/Player.h"

class BaseCamera {
protected:
	SRT transform_;
	Quaternion rollRotate_;
	Player* player_;
public:
	//初期化
	virtual void Initialize(Player* player) = 0;
	//更新処理
	virtual void Update() = 0;
	//Transform
	SRT GetTransform() { return transform_; }
};

//落下カメラ
class DownCamera : public BaseCamera {
private:
	//回転がない場合のカメラ座標
	const Vector3 kCameraPos = { 0, 3, -20 };
public:
	//初期化
	void Initialize(Player* player) override;
	//更新処理
	void Update() override;
};

class GameCamera {
private:
	//使用するカメラ
	std::shared_ptr<Camera> camera_;
	//プレイヤー位置
	Player* player_;
	//現在のカメラ
	std::unique_ptr<BaseCamera> nowCamera_;
	//遷移する先のカメラ
	std::unique_ptr<BaseCamera> nextCamera_;
public:
	//初期化
	void Initialize(std::shared_ptr<Camera> camera, Player* player);
	//更新処理
	void Update();
};