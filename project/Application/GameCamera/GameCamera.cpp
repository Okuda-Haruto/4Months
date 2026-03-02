#include "GameCamera.h"
#include "Operation/Operation.h"
#include "Lerp.h"
#include <numbers>

#pragma region 落下カメラ

void DownCamera::Initialize(Player* player) {
	player_ = player;
	//初期値として現在の向きを入れる
	transform_.scale = { 1,1,1 };
	transform_.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	rollRotate_ = player_->GetRollRotate();
	transform_.rotate = transform_.rotate * rollRotate_;
	transform_.translate = player_->GetTransform().translate;
}
void DownCamera::Update() {

    Vector3 nextTranslate = player_->GetTransform().translate;

    bool isCoil = player_->GetIsCoilAround();

    // 巻き付きに入った瞬間を検出
    if (isCoil && !wasCoilAround_) {
        coilLockRotate_ = transform_.rotate;  // 今の回転を保存
    }

    if (isCoil) {
        // 巻き付き中は回転固定
        transform_.rotate = coilLockRotate_;
    }
    else {
        // 折り返し基礎回転
        Quaternion baseRotate;
        if (!player_->GetIsTurnBack()) {
            baseRotate = MakeRotateAxisAngleQuaternion(
                Vector3{ 1,0,0 },
                -std::numbers::pi_v<float> / 2
            );
        }
        else {
            baseRotate = MakeRotateAxisAngleQuaternion(
                Vector3{ 1,0,0 },
                std::numbers::pi_v<float> / 2
            );
        }

        rollRotate_ = Slerp(
            rollRotate_,
            player_->GetRollRotate(),
            0.1f
        );

        Quaternion nextRotate = baseRotate * rollRotate_;

        transform_.rotate = Slerp(
            transform_.rotate,
            nextRotate,
            0.1f
        );
    }

    nextTranslate += kCameraPos * MakeRotateMatrix(transform_.rotate);

    transform_.translate = Lerp(
        transform_.translate,
        nextTranslate,
        0.1f
    );

    wasCoilAround_ = isCoil;
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

		// カメラシェイク
		if (shakeFrame_ > 0) {
			shakeFrame_--;

			float amp = amplitude_ * (float(shakeFrame_) / float(shakeEndFrame_));

			shake_ = {
				GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
				GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
				GameEngine::randomFloat(-amp / 2.0f, amp / 2.0f),
			};
		} else {
			shake_ = {};
			amplitude_ = 0;
		}
		SRT shakedTransform = nowCamera_->GetTransform();
		shakedTransform.translate += shake_;
		// 通常カメラのビュー
		camera_->SetViewMatrix(Inverse(MakeQuaternionMatrix(shakedTransform.scale, shakedTransform.rotate, shakedTransform.translate)));
	} else {
		//あとで
	}
}

void GameCamera::StartShake(float amplitude, int frame) {
	amplitude_ = amplitude;
	shakeFrame_ = frame;
	shakeEndFrame_ = frame;
}