#include "GameCamera.h"
#include "Operation/Operation.h"

void GameCamera::Initialize(std::shared_ptr<Camera> camera) {
	camera_ = camera;

}

void GameCamera::Update() {
	SRT transform = playerTransform_;
	transform.translate += MakeRotateMatrix(transform.rotate) * Vector3 { 0, 2.5f, -20 };
	camera_->Update(transform);
}