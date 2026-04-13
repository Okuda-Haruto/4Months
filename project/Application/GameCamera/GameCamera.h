#pragma once
#include "Camera/Camera.h"
#include "Human/Player/Player.h"

#include <numbers>

class BaseCamera {
protected:
	SRT transform_;
	Player* player_;
	std::shared_ptr<Input> input_;
public:
	//初期化
	virtual void Initialize(std::shared_ptr<Input> input, Player* player) = 0;
	//更新処理
	virtual void Update() = 0;
	//Transform
	SRT GetTransform() { return transform_; }
};

//落下カメラ
class DownCamera : public BaseCamera {
private:
	//回転がない場合のカメラ座標
	const Vector3 kCameraPos = { 0, 0, -30 };

public:
	//初期化
	void Initialize(std::shared_ptr<Input> input, Player* player) override;
	//更新処理
	void Update() override;
};

//リザルトカメラ
class ResultCamera : public BaseCamera {
private:
	//回転がない場合のカメラ座標
	const Vector3 kCameraPos = { 0, 0, -300 };

	//カメラ回転速度
	const float kCameraRotateSpeed = std::numbers::pi_v<float> / 180;

	//オートマになるまでの時間
	const float kMaxReleaseKeyTime = 1.0f;

	//カメラ移動モード
	enum class CameraMode {
		Manual,			//手動
		Automatic_Up,	//自動上昇
		Automatic_Down,	//自動下降
	};
	CameraMode mode_;

	//カメラ速度
	Vector3 velocity_;

	//カメラY座標
	float posY_;

	//マニュアル移動してないならオートマにする
	float releaseKeyTime_;
	
public:
	//初期化
	void Initialize(std::shared_ptr<Input> input, Player* player) override;
	//更新処理
	void Update() override;
};

#ifdef USE_IMGUI
//エディターカメラ
class EditorCamera : public BaseCamera {
private:
	//回転がない場合のカメラ座標
	Vector3 cameraPos_;

	Quaternion yaw_;
	Quaternion pitch_;

	//カメラY座標
	Vector3 centerPoint_;

public:
	//初期化
	void Initialize(std::shared_ptr<Input> input, Player* player) override;
	//更新処理
	void Update() override;
};
#endif // USE_IMGUI



class GameCamera {
private:
	//使用するカメラ
	std::shared_ptr<Camera> camera_;
	//入力
	std::shared_ptr<Input> input_;
	//プレイヤー位置
	Player* player_;
	//現在のカメラ
	std::unique_ptr<BaseCamera> nowCamera_;
	//遷移する先のカメラ
	std::unique_ptr<BaseCamera> nextCamera_;

	//カメラ遷移時間
	float maxChangeCameraTime_;
	float changeCameraTime_;

	// シェイク
	Vector3 shake_{};
	int shakeFrame_ = 0;
	int shakeEndFrame_;
	float amplitude_ = 0;

	//チャンクのサイズ
	int32_t chunkHeight_;

	//getter用
	SRT transform_;
public:
	//初期化
	void Initialize(std::shared_ptr<Camera> camera, const std::unique_ptr<BaseCamera>& nowCameraMode, std::shared_ptr<Input> input, Player* player);
	//更新処理
	void Update();

	//カメラ遷移
	void ChangeCamera(const std::unique_ptr<BaseCamera>& nextCamera, float changeCameraTime);

	// シェイク
	void StartShake(float amplitude, int frame);

	SRT GetTransform() { return transform_; }

	void SetChunkHeight(int32_t chunkHeight) { chunkHeight_ = chunkHeight; }

	std::shared_ptr<Camera> GetCamera() { return camera_; }
};