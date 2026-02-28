#include "GameCamera.h"
#include "Operation/Operation.h"
#include <numbers>

#pragma region 落下カメラ

void DownCamera::Initialize(Player* player) {
	player_ = player;
	//初期値として現在の向きを入れる
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	rollRotate_ = player_->GetRollRotate();
	transform_.rotate = transform_.rotate * rollRotate_;
	transform_.translate = player_->GetTransform().translate;
}

void DownCamera::Update() {
	//プレイヤー座標を基にする
	Vector3 nextTranslate = player_->GetTransform().translate;
	rollRotate_ = Slerp(rollRotate_, player_->GetRollRotate(), 0.1f);
	//プレイヤーのロール分回転した位置に移動
	nextTranslate += kCameraPos * MakeRotateMatrix(transform_.rotate);
	transform_.translate = Lerp(transform_.translate, nextTranslate, 0.1f);
	//常に下を向く
	Quaternion nextRotate;
	if (!player_->GetIsTurnBack()) {
		nextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	} else {
		nextRotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, std::numbers::pi_v<float> / 2);
	}
	nextRotate = nextRotate * rollRotate_;

	//現在の向きと次の向きの補完
	transform_.rotate = Slerp(transform_.rotate, nextRotate, 0.1f);
}

#pragma endregion



void GameCamera::Initialize(std::shared_ptr<Camera> camera, Player* player) {
	camera_ = camera;
	player_ = player;

	//一旦落下カメラを初期値に
	nowCamera_ = std::make_unique<DownCamera>();
	nowCamera_->Initialize(player);
}

void GameCamera::Update() {
	//次がないなら現在のカメラで更新する
	if (!nextCamera_) {
		nowCamera_->Update();
		camera_->Update(nowCamera_->GetTransform());
	} else {
		//あとで
	}
}