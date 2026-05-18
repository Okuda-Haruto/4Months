#pragma once
#include "Camera/Camera.h"
#include "Human/Player/Player.h"

#include <numbers>
class Course;
class GameCamera;

Quaternion MatrixToQuaternion(const Matrix4x4& m);

class BaseCamera {
protected:
	SRT transform_;
	Player* player_;
	Course* course_;
	std::shared_ptr<Input> input_;
	GameCamera* gameCamera_;
public:
	//初期化
	virtual void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) = 0;
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

	// リザルト演出用
	float resultInTimer_ = 0;
	const float kResultInTime = 2.5f;
	const float kResultSetTime = 0.12f;
	const float kResultOutTime = 1.0f;
	SRT resultInTransform = {};
	SRT setTransform = { {1,1,1}, {-0.057f ,0.919f ,-0.27f, 0.146f }, { 0.89f, 7.0f, 12.8f } };
	Vector3 setRot{};

public:
	//初期化
	void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) override;
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
	void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) override;
	//更新処理
	void Update() override;
};

//リビング全体を写すカメラ
class LivingCamera : public BaseCamera {
private:
	//カメラ座標
	const Vector3 kCameraPos = { 0, 0, 80 };
public:
	//初期化
	void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) override;
	//更新処理
	void Update() override;
};

//TVのみを写すカメラ
class TVCamera : public BaseCamera {
private:
	//カメラ座標
	const Vector3 kCameraPos = { 6.5f, 3.5f, 25.0f };
public:
	//初期化
	void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) override;
	//更新処理
	void Update() override;
};

//スタジオ全体を写すカメラ
class StudioCamera : public BaseCamera {
private:
	//カメラ座標
	const Vector3 kCameraPos = { 0,35, 120 };
public:
	//初期化
	void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) override;
	//更新処理
	void Update() override;
};

//ステージセレクトカメラ
class SelectCamera : public BaseCamera {
private:
	//カメラ座標
	const Vector3 kCameraPos = { -35, 40, 20 };
public:
	//初期化
	void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) override;
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
	void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) override;
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
	// コース(渡す用)
	Course* course_;
	//現在のカメラ
	std::unique_ptr<BaseCamera> nowCamera_;
	//遷移する先のカメラ
	std::unique_ptr<BaseCamera> nextCamera_;

	//カメラ遷移時間
	float maxChangeCameraTime_;
	float changeCameraTime_;

	// シェイク
	Vector3 shake_{};
	float shakeTime_ = 0;
	float shakeEndTime_;
	float amplitude_ = 0;

	//チャンクのサイズ
	int32_t chunkHeight_;

	//getter用
	SRT transform_;

	//リザルトカメラで行けるY座標の最低値
	float cameraPosBottom_ = 0;

public:
	//初期化
	void Initialize(std::shared_ptr<Camera> camera, std::unique_ptr<BaseCamera> nowCameraMode, std::shared_ptr<Input> input, Player* player, Course* course);
	//更新処理
	void Update();

	//カメラ遷移
	void ChangeCamera(const std::unique_ptr<BaseCamera>& nextCamera, float changeCameraTime);

	// シェイク
	void StartShake(float amplitude, float time);

	SRT GetTransform() { return transform_; }

	void SetChunkHeight(int32_t chunkHeight) { chunkHeight_ = chunkHeight; }

	void SetCameraPosBottom(float cameraPosBottom) { cameraPosBottom_ = cameraPosBottom; }

	std::shared_ptr<Camera> GetCamera() { return camera_; }
	
	float GetCameraPosBottom() { return cameraPosBottom_; }

	bool IsEndChangeCamera() { return nextCamera_ == nullptr; }
};

class StartCamera : public BaseCamera{
private:
	const float risingTime_ = 3.0f;
	float timer_ = 0;

	// カメラ回転速度
	const float kCameraRotateSpeed = std::numbers::pi_v<float> / 180;

	// 上昇速度
	const float kRiseSpeed = 1.0f;
	
public:
	//初期化
	void Initialize(GameCamera* gameCamera, std::shared_ptr<Input> input, Player* player, Course* course) override;
	//更新処理
	void Update() override;
	Quaternion LookAt(const Vector3& eye, const Vector3& target);
};