#include "GoalBarrier.h"
#include <ModelManager/ModelManager.h>
#include <numbers>
#include <Operation/Operation.h>
#include "Lerp.h"

void GoalBarrier::Initialize(float position, std::shared_ptr<Camera> camera) {
	camera_ = camera;

	transform_.translate.y = position;
	transform_.scale = { 1.0f,1.0f,1.0f };

	baseObject_ = std::make_unique<Object>();
	baseObject_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/GoalBarrier", "GoalBarrier.obj"));
	baseObject_->SetReflection(REFLECTION_None);
	baseObject_->SetCamera(camera_);
	baseObject_->SetTransform(transform_);
	baseObject_->SetColor(Vector4{ 0.5f ,0.0f,0.0f,1.0f });

	transform_.translate.y += 1.0f;

	smokeObject_ = std::make_unique<Object>();
	smokeObject_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/GoalBarrier", "GoalBarrier_Smoke.obj"));
	smokeObject_->SetReflection(REFLECTION_None);
	smokeObject_->SetCamera(camera_);
	smokeObject_->SetTransform(transform_);
	smokeObject_->SetColor(Vector4{ 1.0f ,1.0f,1.0f,0.5f });

	std::vector<Parts> parts = smokeObject_->GetParts();
	parts[0].UVtransform.scale = { 4.0f,4.0f ,4.0f };
	smokeObject_->SetParts(parts[0], 0);

	smokeObjectRotate_ = std::make_unique<Object>();
	smokeObjectRotate_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/GoalBarrier", "GoalBarrier_Smoke.obj"));
	smokeObjectRotate_->SetReflection(REFLECTION_None);
	smokeObjectRotate_->SetCamera(camera_);
	smokeObjectRotate_->SetTransform(transform_);
	smokeObjectRotate_->SetColor(Vector4{ 1.0f ,1.0f,1.0f,0.5f });

	parts = smokeObjectRotate_->GetParts();
	parts[0].UVtransform.scale = { 2.0f,2.0f ,2.0f };
	smokeObjectRotate_->SetParts(parts[0], 0);

	smokeObjectScale_ = std::make_unique<Object>();
	smokeObjectScale_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/GoalBarrier", "GoalBarrier_Smoke.obj"));
	smokeObjectScale_->SetReflection(REFLECTION_None);
	smokeObjectScale_->SetCamera(camera_);
	smokeObjectScale_->SetTransform(transform_);
	smokeObjectScale_->SetColor(Vector4{ 1.0f ,1.0f,1.0f,0.5f });

	parts = smokeObjectScale_->GetParts();
	smokeObjectScale_->SetParts(parts[0], 0);

	time_ = 0.0f;
	clearTimer_ = kMaxClearTimer_;
	isClear_ = false;
}

void GoalBarrier::Update(GameCamera* gameCamera) {
	time_ += 1.0f / 60.0f;

	if (isClear_ && clearTimer_ > 0.0f) {
		clearTimer_ -= 1.0f / 60.0f;
		if (clearTimer_ < 0.0f)clearTimer_ = 0.0f;
		float a = Lerp(0.0f, 0.5f, powf(clearTimer_,2) / kMaxClearTimer_);
		float scale = Lerp(2.5f, 1.0f, powf(clearTimer_,2) / kMaxClearTimer_);
		transform_.scale = { scale,scale,scale };

		baseObject_->SetColor(Vector4{ 1.0f ,1.0f,1.0f,a * 2 });
		smokeObject_->SetColor(Vector4{ 1.0f ,1.0f,1.0f,a });
		smokeObjectRotate_->SetColor(Vector4{ 1.0f ,1.0f,1.0f,a });
		smokeObjectScale_->SetColor(Vector4{ 1.0f ,1.0f,1.0f,a });
	}


	std::vector<Parts> parts = smokeObject_->GetParts();
	parts[0].UVtransform.translate = { time_,time_ ,0 };
	smokeObject_->SetParts(parts[0], 0);

	parts = smokeObjectRotate_->GetParts();
	parts[0].UVtransform.rotate = MakeRotateAxisAngleQuaternion(Vector3{ 0,0,1 }, std::numbers::pi_v<float> * time_);
	smokeObjectRotate_->SetParts(parts[0], 0);

	parts = smokeObjectScale_->GetParts();
	parts[0].UVtransform.scale = { sinf(std::numbers::pi_v<float> * time_ * 2),sinf(std::numbers::pi_v<float> *time_) ,0 };
	smokeObjectScale_->SetParts(parts[0], 0);

	Vector3 cameraTranslate = gameCamera->GetTransform().translate;

	transform_.translate = { cameraTranslate.x,transform_.translate.y,cameraTranslate.z };

	baseObject_->SetTransform(transform_);
	smokeObject_->SetTransform(transform_);
	smokeObjectRotate_->SetTransform(transform_);
	smokeObjectScale_->SetTransform(transform_);
}

void GoalBarrier::Draw() {
	if (clearTimer_ > 0.0f) {
		baseObject_->Draw3D();
		smokeObjectRotate_->Draw3D();
		smokeObjectScale_->Draw3D();
		smokeObject_->Draw3D();
	}
}