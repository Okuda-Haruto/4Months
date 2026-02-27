#include "Course.h"

Course::Course() {
	// 配置物の設置
	for (int i = 0; i < ringCount_; ++i) {
		rings_.push_back(std::make_unique<Ring>());
	}
	for (int i = 0; i < spikeCount_; ++i) {
		spikes_.push_back(std::make_unique<Spike>());
	}

	// 制御点
	controlPoints_.push_back({});
	controlPoints_.push_back({});
	controlPoints_.push_back({ 0,-50,0 });
	controlPoints_.push_back({ 0,-80,20 });
	controlPoints_.push_back({ 0,-100,50 });
	controlPoints_.push_back({ 0,-120,90 });

	// 壁配置
	CreateTubeCourse();
}

void Course::Initialize() {
	for (auto& ring : rings_) {
		ring->Initialize();
	}

	for (auto& spike : spikes_) {
		spike->Initialize();
	}
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

	std::list<Object*> objects;
	for (std::unique_ptr<Object>& model : wallModel_) {
		objects.push_back(model.get());
	}
	Object::InstancingDraw3D(objects, directionalLight, nullptr, nullptr);
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
	const int sampleCount = 1000;        // 長さ計算用サンプル

	walls_.clear();
	wallModel_.clear();

	// ==============================
	// ① 弧長テーブル作成
	// ==============================
	std::vector<float> lengthTable(sampleCount);
	std::vector<float> tTable(sampleCount);

	float totalLength = 0.0f;

	Vector3 prev = GetPoint(0.0f);
	lengthTable[0] = 0.0f;
	tTable[0] = 0.0f;

	for (int i = 1; i < sampleCount; ++i) {
		float t = (float)i / (sampleCount - 1);
		Vector3 p = GetPoint(t);

		totalLength += Length(p - prev);

		lengthTable[i] = totalLength;
		tTable[i] = t;

		prev = p;
	}

	// ==============================
	// ② 初期フレーム作成
	// ==============================
	float t0 = 0.0f;

	Vector3 T0 = Normalize(GetTangent(t0));

	Vector3 upRef =
		fabs(T0.y) < 0.9f ? Vector3{ 0,1,0 }
	: Vector3{ 1,0,0 };

	Vector3 N = Normalize(Cross(upRef, T0));
	Vector3 B = Normalize(Cross(T0, N));

	Vector3 prevT = T0;

	// ==============================
	// ③ 等距離で配置
	// ==============================
	for (float dist = 0.0f; dist < totalLength; dist += wallSpace_) {
		// ---- dist → t を探す（線形探索）
		float t = 1.0f;

		for (int i = 1; i < sampleCount; ++i) {
			if (lengthTable[i] >= dist) {
				float ratio =
					(dist - lengthTable[i - 1]) /
					(lengthTable[i] - lengthTable[i - 1]);

				t = lerp(tTable[i - 1], tTable[i], ratio);
				break;
			}
		}

		Vector3 center = GetPoint(t);
		Vector3 T = Normalize(GetTangent(t));

		// ==============================
		// Parallel Transport
		// ==============================
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

		// ==============================
		// 円周方向
		// ==============================
		for (int r = 0; r < wallCount_; ++r) {
			float angle =
				2.0f * 3.1415926535f * r / wallCount_;

			Vector3 radial =
				Normalize(cosf(angle) * N +
					sinf(angle) * B);

			Vector3 wallPos = center + radial * radius_;

			// ---- 回転生成
			Vector3 forward = -radial;  // Z-前方
			Vector3 up = T;
			Vector3 right = Normalize(Cross(up, forward));
			up = Normalize(Cross(forward, right));

			Matrix3x3 rot;
			rot.m[0][0] = right.x;   rot.m[0][1] = right.y;   rot.m[0][2] = right.z;
			rot.m[1][0] = up.x;      rot.m[1][1] = up.y;      rot.m[1][2] = up.z;
			rot.m[2][0] = forward.x; rot.m[2][1] = forward.y; rot.m[2][2] = forward.z;

			Quaternion q = FromRotationMatrix(rot);

			// ---- OBB
			OBB obb;
			obb.center = wallPos;
			obb.size = wallSize_;
			obb.orientations[0] = right;
			obb.orientations[1] = up;
			obb.orientations[2] = forward;

			walls_.push_back(obb);

			// ---- モデル
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