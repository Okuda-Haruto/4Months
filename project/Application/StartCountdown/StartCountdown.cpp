#include "StartCountdown.h"
#include "Mix.h"
#include <numbers>

void StartCountdown::Initialize(std::shared_ptr<DirectionalLight> directionalLight) {
	count_ = kMaxCount_;
	positions_.resize(count_ + 1);

	// CSV読み込み
	LoadCSV("resources/Countdown/Go.csv", directionalLight, 0);
	LoadCSV("resources/Countdown/1.csv", directionalLight, 1);
	LoadCSV("resources/Countdown/2.csv", directionalLight, 2);
	LoadCSV("resources/Countdown/3.csv", directionalLight, 3);
}

void StartCountdown::Update() {
	float rate = 0;
	float scale = 0;
	SRT transform{};

	switch (state_) {
	case State::PreStart:
		// 待機(スタート前カメラ)
		timer_ += 1.0f / 60.0f;
		timer_ = min(timer_, preStartTime_);
		break;
	case State::Start:
		// 初期配置につく
		timer_ += 1.0f / 60.0f;
		timer_ = min(timer_, startTime_);

		rate = timer_ / startTime_;
		rate = std::clamp(rate, 0.0f, 1.0f);
		scale = scale_ * rate;

		for (int i = 0; i < blocks_.size(); ++i) {
			transform = blocks_[i]->GetTransform();
			transform.scale = { scale, scale, scale };
			transform.translate = Lerp(startPos_[i], positions_.back()[i], rate);
			blocks_[i]->SetTransform(transform);
		}

		if (timer_ >= startTime_) {
			count_--;

			timer_ = 0;
			state_ = State::Wait;
		}
		break;
	case State::Wait:
		timer_ += 1.0f / 60.0f;

		timer_ = min(timer_, waitTime_);

		if (timer_ >= waitTime_) {
			// 次の形タイマーに変更
			timers_.clear();
			for (auto& pos : positions_[count_]) {
				timers_.push_back(-(pos.z - basePos_.z) / 50); // ここの調整で組みあがり時間差
			}

			timer_ = 0;
			state_ = State::Vacuum;
		}
		break;
	case State::Vacuum:
		timer_ += 1.0f / 60.0f;
		timer_ = min(timer_, vacuumEndTime_);

		for (int i = 0; i < blocks_.size(); ++i) {
			timers_[i] += 1.0f / 60.0f;
			timers_[i] = min(timers_[i], vacuumTime_);

			transform = blocks_[i]->GetTransform();
			rate = timers_[i] / vacuumTime_;
			rate = std::clamp(rate, 0.0f, 1.0f);
			// 移動
			Vector3 shapeVel = (positions_[count_][i] - transform.translate) * rate;
			Vector3 rotateVel = Mix2(transform.translate, basePos_, { 0,0,1 }, 6.0f, radius_, false);
			if (rotateVel.x < 0.3f) { rotateVel.x += 0.3f; } // 少ししか動かない違和感対策
			transform.translate += Lerp(rotateVel, shapeVel, rate);
			if (timers_[i] == vacuumTime_) {
				transform.translate = positions_[count_][i];
			}

			blocks_[i]->SetTransform(transform);
			blocks_[i]->Update();
		}

		if (timer_ >= vacuumEndTime_) {
			timer_ = 0;

			// 次のカウントに移動or終了
			if (count_ > 0) {
				count_--;
				state_ = State::Wait;
			} else {
				state_ = State::Spread;
			}
		}
		break;

	case State::Spread:
		timer_ += 1.0f / 60.0f;
		timer_ = min(timer_, spreadTime_);

		rate = timer_ / spreadTime_;
		rate = std::clamp(rate, 0.0f, 1.0f);
		scale = scale_ * (1.0f - rate);

		for (int i = 0; i < blocks_.size(); ++i) {
			transform = blocks_[i]->GetTransform();
			transform.scale = { scale, scale, scale };
			transform.translate = Lerp(positions_[0][i], startPos_[i], rate);
			blocks_[i]->SetTransform(transform);
		}

		if (timer_ == spreadTime_) {
			timer_ = 0;
			state_ = State::Stop;
		}

		break;

	case State::Stop:
		break;
	}
}

void StartCountdown::Draw() {
	for (auto& block : blocks_) {
		block->Draw3D();
	}
}

void StartCountdown::LoadCSV(std::string filename, std::shared_ptr<DirectionalLight> directionalLight, int countNumber) {
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

				positions_[countNumber].push_back(Vector3{ pos.x, 0, pos.y } + basePos_);
			}
		}
	}

	if (countNumber == kMaxCount_) {
		for (int i = 0; i < kMaxCount_; ++i) {
			maxBlocks_ = max(maxBlocks_, int(positions_[i].size()));
		}

		// 必要数のブロック
		for (int i = 0; i < maxBlocks_; ++i) {
			std::unique_ptr<Object> object = std::make_unique<Object>();
			object->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/Face", "Block.obj"));
			object->SetDirectionalLight(directionalLight);
			object->SetShininess(0);
			std::vector<Parts> parts = object->GetParts();
			parts[0].UVtransform.scale.x = 0.5f;
			parts[0].UVtransform.translate.x = 0.5f;
			object->SetParts(parts[0], 0);
			blocks_.push_back(std::move(object));
		}

		int index = 0;
		int side = int(ceil(cbrt(float(blocks_.size()))));
		int sideX = side;
		int sideY = side;
		int sideZ = int(ceil(float(blocks_.size()) / (sideX * sideY)));

		// 初期配置
		for (int z = 0; z < sideZ; z++) {
			for (int y = 0; y < sideY; y++) {
				for (int x = 0; x < sideX; x++) {

					if (index >= int(blocks_.size())) break;

					Vector3 pos;
					float spacing = blockSize_ * 15.0f;
					float half = (side - 1) * 0.5f;

					pos.x = (x - half) * spacing;
					pos.y = (y - half) * spacing;
					pos.z = z * spacing;

					SRT transform = blocks_[index]->GetTransform();
					transform.scale = { 0,0,0 };
					transform.translate = pos + basePos_;

					blocks_[index]->SetTransform(transform);
					startPos_.push_back(transform.translate);

					index++;
				}
			}
		}

		for (int i = 0; i < positions_.size(); ++i) {
			// 一番多いブロック数に合わせて余ったブロックの位置を設定
			int add = maxBlocks_ - int(positions_[i].size());
			if (add > 0) {
				for (int j = 0; j < add; ++j) {
					positions_[i].push_back(positions_[i][GameEngine::randomInt(0, int(positions_.size()) - 1)]);
				}
			}
		}
	}
}