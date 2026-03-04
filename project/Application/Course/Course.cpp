#include "Course.h"

Course::Course() {
	// 制御点
	controlPoints_.push_back({});
	controlPoints_.push_back({});
	controlPoints_.push_back({ 0,-50,0 });
	controlPoints_.push_back({ 0,-150,0 });
	controlPoints_.push_back({ 0,-170,5 });
	controlPoints_.push_back({ 0,-190,10 });
	controlPoints_.push_back({ 0,-210,15 });
	controlPoints_.push_back({ 0,-230,20 });
	controlPoints_.push_back({ 0,-250,25 });
	controlPoints_.push_back({ 0,-270,30 });
	controlPoints_.push_back({ 0,-370,30 });
	controlPoints_.push_back({ 0,-390,25 });
	controlPoints_.push_back({ 0,-410,20 });
	controlPoints_.push_back({ 0,-430,15 });
	controlPoints_.push_back({ 0,-450,10 });
	controlPoints_.push_back({ 0,-470,5 });
	controlPoints_.push_back({ 0,-490,0 });
	controlPoints_.push_back({ 0,-590,0 });
	controlPoints_.push_back({ 0,-590,0 });

	// 壁配置
	CreateTubeCourse();

	model_ = std::make_unique<Object>();
	model_->Initialize(
		ModelManager::GetInstance()
		->GetModel("resources/Course", "Course.obj"));

	model_->SetShininess(40.0f);
	model_->SetColor({ 1,1,1,1 });
	model_->SetTransform({ {200,200,200},{0,0,0},{0,-150,0} });
}

Course::~Course() {
	for (auto& model_ : wallModel_) {
		model_.reset();
	}
	wallModel_.clear();
}

void Course::Initialize() {
	ReadCSV();
}

void Course::Update() {
	for (auto& model : wallModel_) {
		model->SetColor({ 1,1,1,0.75f });
	}

	for (auto& ring : rings_) {
		ring->Update();
	}

	for (auto& spike : spikes_) {
		spike->Update();
	}

	model_->Update();

#ifdef USE_IMGUI
	ImGui::Begin("Wall");
	ImGui::Text("x = %f", wallModel_[0]->GetTransform().translate.x);
	ImGui::Text("y = %f", wallModel_[0]->GetTransform().translate.y);
	ImGui::Text("z = %f", wallModel_[0]->GetTransform().translate.z);
	ImGui::DragFloat3("controlPoint0", &controlPoints_[0].x, 0.01f);
	ImGui::DragFloat3("controlPoint1", &controlPoints_[1].x, 0.01f);
	ImGui::DragFloat3("controlPoint2", &controlPoints_[2].x, 0.01f);
	ImGui::DragFloat3("controlPoint3", &controlPoints_[3].x, 0.01f);
	ImGui::End();
#endif
}

void Course::Draw(const std::shared_ptr<DirectionalLight> directionalLight) {
	for (auto& ring : rings_) {
		ring->Draw(directionalLight);
	}

	for (auto& spike : spikes_) {
		spike->Draw(directionalLight);
	}

	model_->SetDirectionalLight(directionalLight);
	model_->Draw3D();

	std::list<Object*> objects;
	for (std::unique_ptr<Object>& model : wallModel_) {
		objects.push_back(model.get());
	}
	//Object::InstancingDraw3D(objects, directionalLight, nullptr, nullptr);
}

void Course::OnCollide() {
	for (auto& model : wallModel_) {
		model->SetColor({ 1,0,0,0.75f });
	}
}

Vector3 Course::GetPoint(float t) {
	assert(controlPoints_.size() >= 4);

	int segmentCount = int(controlPoints_.size()) - 3;
	float scaledT = t * segmentCount;

	int index = (int)scaledT;
	if (index >= segmentCount)
		index = segmentCount - 1;

	float localT = scaledT - index;

	return Spline(
		controlPoints_[index],
		controlPoints_[index + 1],
		controlPoints_[index + 2],
		controlPoints_[index + 3],
		localT
	);
}

Vector3 Course::CatmullRomTangent(
	const Vector3& p0,
	const Vector3& p1,
	const Vector3& p2,
	const Vector3& p3,
	float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	return 0.5f * (
		(2.0f * p1) +
		(-p0 + p2) * t +
		(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
		);
}

Vector3 Course::GetTangent(float t) {
	float delta = 0.001f;
	Vector3 p1 = GetPoint(t);
	Vector3 p2 = GetPoint((std::min)(t + delta, 1.0f));
	return Normalize(p2 - p1);
}

Quaternion Course::FromRotationMatrix(const Matrix3x3& m) {
	Quaternion q;

	float trace = m.m[0][0] + m.m[1][1] + m.m[2][2];

	if (trace > 0.0f) {
		float s = sqrtf(trace + 1.0f) * 2.0f;
		q.w = 0.25f * s;
		q.x = (m.m[2][1] - m.m[1][2]) / s;
		q.y = (m.m[0][2] - m.m[2][0]) / s;
		q.z = (m.m[1][0] - m.m[0][1]) / s;
	} else if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2]) {
		float s = sqrtf(1.0f + m.m[0][0] - m.m[1][1] - m.m[2][2]) * 2.0f;
		q.w = (m.m[2][1] - m.m[1][2]) / s;
		q.x = 0.25f * s;
		q.y = (m.m[0][1] + m.m[1][0]) / s;
		q.z = (m.m[0][2] + m.m[2][0]) / s;
	} else if (m.m[1][1] > m.m[2][2]) {
		float s = sqrtf(1.0f + m.m[1][1] - m.m[0][0] - m.m[2][2]) * 2.0f;
		q.w = (m.m[0][2] - m.m[2][0]) / s;
		q.x = (m.m[0][1] + m.m[1][0]) / s;
		q.y = 0.25f * s;
		q.z = (m.m[1][2] + m.m[2][1]) / s;
	} else {
		float s = sqrtf(1.0f + m.m[2][2] - m.m[0][0] - m.m[1][1]) * 2.0f;
		q.w = (m.m[1][0] - m.m[0][1]) / s;
		q.x = (m.m[0][2] + m.m[2][0]) / s;
		q.y = (m.m[1][2] + m.m[2][1]) / s;
		q.z = 0.25f * s;
	}

	return Normalize(q);
}

void Course::CreateTubeCourse() {
	walls_.clear();
	wallModel_.clear();

	// 弧長テーブル
	lengthTable_.resize(sampleCount_);
	tTable_.resize(sampleCount_);

	Vector3 prev = GetPoint(0.0f);
	lengthTable_[0] = 0.0f;
	tTable_[0] = 0.0f;

	for (int i = 1; i < sampleCount_; ++i) {
		float t = (float)i / (sampleCount_ - 1);
		Vector3 p = GetPoint(t);

		totalLength_ += Length(p - prev);

		lengthTable_[i] = totalLength_;
		tTable_[i] = t;

		prev = p;
	}

	float t0 = 0.0f;
	Vector3 T0 = Normalize(GetTangent(t0));
	Vector3 upRef = fabs(T0.y) < 0.9f ? Vector3{ 0,1,0 } : Vector3{ 1,0,0 };
	Vector3 N = Normalize(Cross(upRef, T0));
	Vector3 B = Normalize(Cross(T0, N));

	Vector3 prevT = T0;

	// 円の位置
	for (float dist = 0.0f; dist < totalLength_; dist += wallSpace_) {
		// dist → t を探す（線形探索）
		float t = 1.0f;

		for (int i = 1; i < sampleCount_; ++i) {
			if (lengthTable_[i] >= dist) {
				float ratio =
					(dist - lengthTable_[i - 1]) /
					(lengthTable_[i] - lengthTable_[i - 1]);

				t = lerp(tTable_[i - 1], tTable_[i], ratio);
				break;
			}
		}

		Vector3 center = GetPoint(t);
		Vector3 T = Normalize(GetTangent(t));

		// Parallel Transport
		Vector3 axis = Cross(prevT, T);
		float len = Length(axis);

		if (len > 0.0001f) {
			axis = Normalize(axis);

			float dot = std::clamp(Dot(prevT, T), -1.0f, 1.0f);
			float angle = acos(dot);

			Quaternion q =
				MakeRotateAxisAngleQuaternion(axis, angle);

			N = RotateVector(N, q);
			B = RotateVector(B, q);
		}

		prevT = T;

		// 円状に配置
		for (int r = 0; r < wallCount_; ++r) {
			float angle =
				2.0f * 3.1415926535f * r / wallCount_;

			Vector3 radial =
				Normalize(cosf(angle) * N +
					sinf(angle) * B);

			Vector3 wallPos = center + radial * radius_;

			// 回転生成
			Vector3 forward = -radial;
			Vector3 up = Normalize(T);
			Vector3 right = Normalize(Cross(up, forward));
			up = Normalize(Cross(forward, right));

			Matrix3x3 rot;
			rot.m[0][0] = right.x;   rot.m[0][1] = right.y;   rot.m[0][2] = right.z;
			rot.m[1][0] = up.x;      rot.m[1][1] = up.y;      rot.m[1][2] = up.z;
			rot.m[2][0] = forward.x; rot.m[2][1] = forward.y; rot.m[2][2] = forward.z;

			Quaternion q = FromRotationMatrix(rot);

			// OBB
			OBB obb;
			obb.center = wallPos;
			obb.size = wallSize_;
			obb.orientations[0] = right;
			obb.orientations[1] = up;
			obb.orientations[2] = forward;

			walls_.push_back(obb);

			// モデル
			SRT transform;
			transform.scale = wallSize_ * 2.0f;
			transform.rotate = q;
			transform.translate = wallPos;

			std::unique_ptr<Object> model =
				std::make_unique<Object>();

			model->Initialize(
				ModelManager::GetInstance()
				->GetModel("resources/Course/Wall", "Wall.obj"));

			model->SetShininess(40.0f);
			model->SetColor({ 1,1,1,0.75f });
			model->SetTransform(transform);

			wallModel_.push_back(std::move(model));
		}
	}
}

void Course::ReadCSV() {
	// ---------
	// 0=無
	// 1,2,3=リング(小/中/大)
	// 4,5,6=障害物(小/中/大)
	// ---------

	for (int i = 0; i < kLayerCount_; ++i) {
		std::vector<std::vector<int>> layer;

		// ファイルをロード
		std::string path = "Resources/Course/LayoutCSV/Layout" + std::to_string(i) + ".csv";
		std::ifstream file(path);
		std::string line;
		assert(file.is_open());

		// ファイル読む
		while (std::getline(file, line)) {
			std::vector<int> row;
			std::stringstream ss(line);
			std::string cell;

			while (std::getline(ss, cell, ',')) {
				int value = std::stoi(cell);
				row.push_back(value);
			}

			layer.push_back(row);
		}
		std::reverse(layer.begin(), layer.end());

		// 層ごとの配置
		for (int x = 0; x < kCSVWidth_; ++x) {
			for (int z = 0; z < kCSVHeight_; ++z) {
				Vector2 blockSize = { (radius_ * 2) / kCSVWidth_, (radius_ * 2) / kCSVHeight_ };

				Vector3 pos = Vector3{
					(x + 0.5f)* blockSize.x,
					0,
					(z + 0.5f) * blockSize.y
				};
				pos -= { (kCSVWidth_ * blockSize.x) / 2.0f, 0, (kCSVHeight_ * blockSize.y) / 2.0f};

				float targetDist = totalLength_ * (float(i) / kLayerCount_);
				float t = DistanceToT(targetDist);
				pos += GetPoint(t); // コース上に移動

				// 設置
				switch (layer[z][x]) {
				case 1:
					AddRing(pos, 1.0f);
					break;
				case 2:
					AddRing(pos, 2.0f);
					break;
				case 3:
					AddRing(pos, 3.0f);
					break;

				case 4:
					AddSpike(pos, 1.0f);
					break;
				case 5:
					AddSpike(pos, 2.0f);
					break;
				case 6:
					AddSpike(pos, 3.0f);
					break;

				default:
					break;
				}
			}
		}
	}
}

void Course::AddRing(const Vector3& spawnPos, const float radius) {
	std::unique_ptr ring = std::make_unique<Ring>();
	ring->Initialize(spawnPos, radius);
	rings_.push_back(std::move(ring));

	//ソート
	std::sort(rings_.begin(), rings_.end(),
		[](const auto& a, const auto& b) {
			return *a < *b;
		});
}

void Course::AddSpike(const Vector3& spawnPos, const float radius) {
	std::unique_ptr spike = std::make_unique<Spike>();
	spike->Initialize(spawnPos, radius);
	spikes_.push_back(std::move(spike));
}

float Course::DistanceToT(float dist) const {
	dist = std::clamp(dist, 0.0f, totalLength_);

	for (int i = 1; i < lengthTable_.size(); ++i) {
		if (lengthTable_[i] >= dist) {
			float segmentLength =
				lengthTable_[i] - lengthTable_[i - 1];

			float ratio =
				(dist - lengthTable_[i - 1]) / segmentLength;

			return std::lerp(
				tTable_[i - 1],
				tTable_[i],
				ratio
			);
		}
	}

	return 1.0f;
}