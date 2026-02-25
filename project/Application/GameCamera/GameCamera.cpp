#include "GameCamera.h"
#include "Operation/Operation.h"
#include <numbers>

void GameCamera::Initialize(std::shared_ptr<Camera> camera) {
	camera_ = camera;

}

void GameCamera::Update() {
	SRT transform = playerTransform_;
	transform.translate += Vector3 { 0, 20.0f, 0 };
	transform.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2);
	camera_->Update(transform);
}