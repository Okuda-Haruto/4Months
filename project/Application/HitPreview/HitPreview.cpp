#include "HitPreview.h"
#include "Human/Player/Player.h"
#include "CheckCollision/CheckCollision.h"
#include <numbers>

void HitPreview::Initialize(const std::shared_ptr<DirectionalLight> directionalLight) {
	circle_ = make_unique<Object>();
	circle_->Initialize(ModelManager::GetInstance()->GetModel("resources/HitPreview/LineCircle", "Circle.obj"));
	circle_->SetShininess(30.0f);
	circle_->SetDirectionalLight(directionalLight);
	circle_->SetColor({ 1,1,1,0.8f });

	radModel_ = make_unique<Object>();
	radModel_->Initialize(ModelManager::GetInstance()->GetModel("resources/HitPreview/Circle", "Circle.obj"));
	radModel_->SetShininess(30.0f);
	radModel_->SetDirectionalLight(directionalLight);
	radModel_->SetColor({ 1,0,0,0.2f });

	for (int i = 0; i < rotateCount_; ++i) {
		std::unique_ptr<Object> rotateModel = make_unique<Object>();
		rotateModel->Initialize(ModelManager::GetInstance()->GetModel("resources/Effect/Wind/Spiral", "Spiral.obj"));
		rotateModel->SetShininess(30.0f);
		rotateModel->SetDirectionalLight(directionalLight);
		rotateModel_.push_back(std::move(rotateModel));

		float t = float(i) / rotateCount_;
		float angle = t * float(std::numbers::pi) * 2.0f;

		rotate_.push_back(angle); // 等間隔角度

		Quaternion rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, angle);
		rotateModel_[i]->SetTransform(SRT({ 1,1,1 }, rotate, {}));
		rotateModel_[i]->SetColor({ 1,1,1,1.0f });
	}
}


void HitPreview::Update(Player* player, CheckCollision* checkCollision) {
	if (player->IsCharging()) {
		Simulate(player, checkCollision);
		canDraw_ = true;

		for (int i = 0; i < rotateCount_; ++i) {
			SRT transform = rotateModel_[i]->GetTransform();
			rotate_[i] -= rotateSpeed_;
			Quaternion rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_[i]);
			float radius = player->GetVacuumSphere().radius;
			radius /= 3.0f;
			transform.scale = { radius,1,radius };
			transform.rotate = rotate;
			transform.translate = { hitPos_.x, hitPos_.y, hitPos_.z };
			rotateModel_[i]->SetTransform(transform);
		}

		if (player->GetCharge() == player->GetMaxCharge()) {
			radModel_->SetColor({ 1, 1, 0, 0.2f });
		} else {
			radModel_->SetColor({ 1, 0, 0, 0.2f });
		}
	} else {
		canDraw_ = false;
	}
}

void HitPreview::Draw() {
	if (canDraw_) {
		radModel_->Draw3D();

		for (auto& model : rotateModel_) {
			model->Draw3D();
		}

		circle_->Draw3D();
	}
}

void HitPreview::Simulate(Player* player, CheckCollision* checkCollision) {
	// 途中で当たるか判定
	hitPos_ = checkCollision->HitPreview(player);

	// 表示設定
	isHit_ = checkCollision->IsPreviewHit();
	if (isHit_) {
		circle_->SetColor({ 1,0,0,1 });
	} else {
		circle_->SetColor({ 1,1,1,1 });
	}
	circle_->SetTransform(SRT{ {3,1,3},{},hitPos_ });

	float radius = player->GetVacuumSphere().radius;
	radius /= 3.0f;
	radModel_->SetTransform(SRT{ { radius,1,radius },{},hitPos_ });

	for (auto& model : rotateModel_) {
		model->SetTransform(SRT{ {1,1,1},{},hitPos_ });
	}
}
