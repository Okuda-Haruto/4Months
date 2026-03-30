#include "HitPreview.h"
#include "Human/Player/Player.h"
#include "CheckCollision/CheckCollision.h"
#include <numbers>

void HitPreview::Initialize(const std::shared_ptr<DirectionalLight> directionalLight) {
	circle_ = make_unique<Object>();
	circle_->Initialize(ModelManager::GetInstance()->GetModel("resources/HitPreview/Circle", "circle.obj"));
	circle_->SetShininess(30.0f);
	circle_->SetDirectionalLight(directionalLight);
	circle_->SetColor({ 1,1,1,0.5f });

	for (int i = 0; i < rotateCount_; ++i) {
		std::unique_ptr<Object> radModel = make_unique<Object>();
		radModel->Initialize(ModelManager::GetInstance()->GetModel("resources/Effect/Wind/Spiral", "Spiral.obj"));
		radModel->SetShininess(30.0f);
		radModel->SetDirectionalLight(directionalLight);
		radModel_.push_back(std::move(radModel));

		float t = float(i) / rotateCount_;
		float angle = t * float(std::numbers::pi) * 2.0f;

		radRotate_.push_back(angle); // 等間隔角度

		Quaternion rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, angle);
		radModel_[i]->SetTransform(SRT({ 1,1,1 }, rotate, {}));
		radModel_[i]->SetColor({ 1,1,1,0.5f });
	}
}


void HitPreview::Update(Player* player, CheckCollision* checkCollision) {
	if (player->IsCharging()) {
		Simulate(player, checkCollision);
		canDraw_ = true;

		for (int i = 0; i < rotateCount_; ++i) {
			SRT transform = radModel_[i]->GetTransform();
			radRotate_[i] -= rotateSpeed_;
			Quaternion rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, radRotate_[i]);
			float radius = player->GetVacuumSphere().radius;
			radius /= 3.0f;
			transform.scale = { radius,1,radius };
			transform.rotate = rotate;
			transform.translate = { hitPos_.x, hitPos_.y, hitPos_.z };
			radModel_[i]->SetTransform(transform);
		}

	} else {
		canDraw_ = false;
	}
}

void HitPreview::Draw() {
	if (canDraw_) {
		circle_->Draw3D();

		for (auto& model : radModel_) {
			model->Draw3D();
		}
	}
}

void HitPreview::Simulate(Player* player, CheckCollision* checkCollision) {
	// 途中で当たるか判定
	hitPos_ = checkCollision->HitPreview(player);

	// 表示設定
	circle_->SetTransform(SRT{ {1,1,1},{},hitPos_});
	for (auto& model : radModel_) {
		model->SetTransform(SRT{ {1,1,1},{},hitPos_ });
	}
}
