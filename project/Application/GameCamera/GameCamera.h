#pragma once
#include "Camera/Camera.h"

class GameCamera {
private:
	//使用するカメラ
	std::shared_ptr<Camera> camera_;
	//プレイヤー位置
	SRT playerTransform_;
public:
	//初期化
	void Initialize(std::shared_ptr<Camera> camera);
	//更新処理
	void Update();

	//プレイヤー位置
	void SetPlayerTransform(SRT transform) { playerTransform_ = transform; }
};