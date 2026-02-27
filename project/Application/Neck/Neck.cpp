#include "Neck.h"
#include "GameEngine.h"
#include "Operation/Operation.h"

std::weak_ptr<DirectionalLight> Neck::directionalLight_;

void Neck::Initialize(Human* human, const std::shared_ptr<DirectionalLight> directionalLight) {
	human_ = human;
	directionalLight_ = directionalLight;

	//共通モデル
	model_ = ModelManager::GetInstance()->GetModel("resources/Player/Neck", "Neck.obj");
	
	//初期地点の首
	std::unique_ptr<Object> object = std::make_unique<Object>();
	object->Initialize(model_.lock());
	object->SetTransform(human_->GetTransform());
	object->SetDirectionalLight(directionalLight_.lock());
	objects_.push_back(move(object));

	transforms_.push_back(human_->GetTransform());

	lastPoint_ = human_->GetTransform().translate;
}

void Neck::Update() {
	SRT playerTransform = human_->GetTransform();

	//最終地点とプレイヤー位置の距離がある程度あるならオブジェクト生成
	Vector3 diff = playerTransform.translate - lastPoint_;
	while (Length(diff) > 1.0f) {
		//初期地点の首
		std::unique_ptr<Object> object = std::make_unique<Object>();
		object->Initialize(model_.lock());

		SRT transform;
		transform.scale = { 1,1,1 };
		transform.rotate = LookAt(lastPoint_,playerTransform.translate);
		transform.translate = lastPoint_ + Normalize(diff);
		object->SetTransform(transform);
		object->SetDirectionalLight(directionalLight_.lock());
		objects_.push_back(move(object));
		transforms_.push_back(transform);

		lastPoint_ = transform.translate;
		diff = playerTransform.translate - lastPoint_;
	}
}

void Neck::Draw() {
	for (auto& object : objects_) {
		object->Draw3D();
	}
}