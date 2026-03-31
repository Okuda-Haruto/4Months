#include "TitleMoji.h"
#include "Mix.h"

void TitleMoji::Initialize(std::shared_ptr<DirectionalLight> directionalLight) {
	LoadCSV("resources/Title/logo.csv", directionalLight);
	timer_ = 0;

	wind_ = std::make_unique<Wind>();
	wind_->Initialize(directionalLight);
}

void TitleMoji::Update() {
	switch (state_) {
	case State::Wait:
		timer_ += 1.0f / 60.0f;

		timer_ = min(timer_, waitTime_);

		if (timer_ == waitTime_) {
			timer_ = 0;
			wind_->SetTitle({}, radius_ / 2.0f, vacuumTime_ * 60);
			state_ = State::Vacuum;
		}
		break;
	case State::Vacuum:
		timer_ += 1.0f / 60.0f;
		timer_ = min(timer_, vacuumEndTime_);

		for (int i = 0; i < blocks.size(); ++i) {
			timers_[i] += 1.0f / 60.0f;
			timers_[i] = min(timers_[i], vacuumTime_);

			SRT transform = blocks[i]->GetTransform();
			// 移動
			Vector3 logoVel = Normalize(positions_[i] - transform.translate) * 1.2f;
			if (timers_[i] > 1.0f) { Vector3 logoVel = Normalize(positions_[i] - transform.translate) * 3.0f; }
			Vector3 rotateVel = Mix2(transform.translate, {}, { 0,1,0 }, 4.5f, radius_, false);
			float rate = timers_[i] / vacuumTime_;
			transform.translate += Lerp(rotateVel, logoVel, rate);
			if (rate == 1) { transform.translate = positions_[i]; }

			blocks[i]->SetTransform(transform);
			blocks[i]->Update();
		}

		wind_->Update();

		if (timer_ == vacuumEndTime_) {
			timer_ = 0;
			state_ = State::Stop;
		}
		break;
	case State::Stop:
		break;
	}
}

void TitleMoji::Draw() {
	for (auto& block : blocks) {
		block->Draw3D();
	}

	wind_->Draw();
}

void TitleMoji::LoadCSV(std::string filename, std::shared_ptr<DirectionalLight> directionalLight) {
	ifstream file(filename);

	// 全行読み込み
	std::vector<std::string> lines;
	std::string line;
	while (getline(file, line)) {
		lines.push_back(line);
	}

	int height = int(lines.size());
	int width = 0;

	for (const auto& l : lines) {
		if (l.length() > width) {
			width = int(l.length());
		}
	}

	// 中心を0にするためのオフセット
	Vector2 offset{ (width - 1) * 0.5f, (height - 1) * 0.5f };

	// 配置
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < lines[y].length(); x++) {
			char c = lines[y][x];

			if (c == '#') {
				Vector2 pos{
				(x - offset.x) * blockSize_,
				-(y - offset.y) * blockSize_
				};

				positions_.push_back({ pos.x, pos.y, 0 });

				float range = 5.0f; // 散らばり具合

				std::unique_ptr<Object> object = std::make_unique<Object>();
				object->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/Face", "Block.obj"));
				object->SetDirectionalLight(directionalLight);
				object->SetShininess(0);
				std::vector<Parts> parts = object->GetParts();
				parts[0].UVtransform.scale.x = 0.5f;
				parts[0].UVtransform.translate.x = 0.5f;
				object->SetParts(parts[0], 0);
				blocks.push_back(std::move(object));
				timers_.push_back(0 - pos.y / 40); // ここの調整で組みあがり速度
			}
		}
	}

	int index = 0;

	int side = int(ceil(cbrt(float(blocks.size()))));
	int sideX = side;
	int sideY = side;
	int sideZ = int(ceil(float(blocks.size()) / (sideX * sideY)));

	for (int z = 0; z < sideZ; z++) {
		for (int y = 0; y < sideY; y++) {
			for (int x = 0; x < sideX; x++) {

				if (index >= int(blocks.size())) break;

				Vector3 pos;
				float spacing = blockSize_ * 1.2f;
				float half = (side - 1) * 0.5f;

				pos.x = (x - half) * spacing;
				pos.y = (y - half) * spacing;
				pos.z = (z - half) * spacing;

				SRT transform = blocks[index]->GetTransform();
				transform.scale = { scale_, scale_, scale_ };
				transform.translate = pos;
				blocks[index]->SetTransform(transform);

				index++;
			}
		}
	}
}