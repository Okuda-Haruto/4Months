#include "Combine.h"
#include <numbers>

static float EaseInCubic(float t) {
	return t * t * t;
}

static float EaseOutCubic(float t) {
	return 1.0f - powf(1.0f - t, 3.0f);
}

Quaternion QuaternionFromMatrix(const Matrix4x4& m) {
	float trace = m.m[0][0] + m.m[1][1] + m.m[2][2];

	Quaternion q;

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

Quaternion MakeLookRotation(const Vector3& forward, const Vector3& up) {
	Vector3 f = Normalize(forward);
	Vector3 r = Normalize(Cross(up, f));
	Vector3 u = Cross(f, r);

	Matrix4x4 m = {
		r.x, r.y, r.z, 0,
		u.x, u.y, u.z, 0,
		f.x, f.y, f.z, 0,
		0,   0,   0,   1
	};

	return QuaternionFromMatrix(m);
}

void Combine::InitializeTitle(std::shared_ptr<DirectionalLight> directionalLight, shared_ptr<Camera> camera) {
	camera_ = camera;

	isTitle_ = true;
	phase_ = Phase::Black;

	black_ = std::make_unique<Sprite>();
	black_->Initialize("resources/Effect/Combine/hole.png");
	black_->SetSize({});
	black_->SetAnchorPoint({ 0.5f, 0.5f });
	black_->SetPosition({ 640,360 });
}

void Combine::InitializeGame(std::shared_ptr<DirectionalLight> directionalLight, std::shared_ptr<Camera> camera) {
	camera_ = camera;

	// カメラのワールド行列を取得（ビュー行列の逆）
	Matrix4x4 view = camera_->GetViewMatrix();
	Matrix4x4 cameraWorld = Inverse(view);
	// カメラ位置と前方ベクトル（ワールド空間）
	Vector3 cameraPos = { cameraWorld.m[3][0], cameraWorld.m[3][1], cameraWorld.m[3][2] };
	Vector3 cameraForward = { cameraWorld.m[2][0], cameraWorld.m[2][1], cameraWorld.m[2][2] };
	cameraForward = Normalize(cameraForward);
	// カメラの相対位置
	Vector3 worldPos = cameraPos + cameraForward * gameDefaultZ_;
	// キャラをカメラ方向に向ける
	Quaternion lookRot = MakeLookRotation(cameraPos - worldPos, Vector3{ 0,1,0 });

	// 人モデル
	human_ = std::make_unique<Object>();
	human_->Initialize(ModelManager::GetInstance()->GetModel("resources/Player", "Player.gltf"));
	human_->SetDirectionalLight(directionalLight);
	human_->SetShininess(0);
	SRT h;
	h.scale = { 1.0f / 0.13f,1.0f / 0.13f ,1.0f / 0.13f };
	h.rotate = lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_);
	h.translate = RotateVector({ 0,defaultHY_,gameDefaultZ_ }, lookRot);
	human_->SetTransform(h);
	human_->SetIsUseAnimation(true);
	human_->SetAnimationIndex(19);
	human_->Update();

	// コマモデル
	beyblade_ = std::make_unique<Object>();
	beyblade_->Initialize(ModelManager::GetInstance()->GetModel("resources/Title/startAnim", "beyblade.obj"));
	beyblade_->SetDirectionalLight(directionalLight);
	beyblade_->SetShininess(0);
	beyblade_->SetTransform({
		{1,1,1},
		 MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * (lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_)),
		worldPos
		}
	);

	// 初期配置
	auto parts = beyblade_->GetParts();
	for (int i = 0; i < 8; i++) {
		parts[i].transform->translate = partsTranslate[i];
		beyblade_->SetParts(parts[i], i);
	}

	isTitle_ = false;

	phase_ = Phase::Fall;

	black_ = std::make_unique<Sprite>();
	black_->Initialize("resources/Effect/Combine/hole.png");
	black_->SetAnchorPoint({ 0.5f, 0.5f });

	SRT uv = black_->GetUVTransform();
	uv.scale = { 0.1f, 0.1f, 1.0f };
	black_->SetUVTransform(uv);

	ParticleManager::GetInstance()->CreateParticleGroup("particle", "resources/DebugResources/circle.png");
	particleEmitter_ = make_unique<ParticleEmitter>("particle");
	emitter_.transform.scale = { 1.0f,1.0f,1.0f };
	emitter_.transform.translate = { 0.0f,0.0f,0.0f };
	emitter_.count = 2;
	emitter_.beforeColor = { 1.0f,1.0f,1.0f,1.0f };
	emitter_.afterColor = { 1.0f,1.0f,1.0f,0.0f };
	emitter_.lifeTime = 1.0f;
	emitter_.frequency = 0.5f;
	emitter_.frequencyTime = 0.0f;
	ParticleManager::GetInstance()->SetEmitter("particle", emitter_);
	accelerationField_.area.min = { -0.5f,-0.5f,-0.5f };
	accelerationField_.area.max = { 0.5f,0.5f,0.5f };
	accelerationField_.acceleration = { 0.0f,0.0f,0.0f };
	ParticleManager::GetInstance()->SetField("particle", accelerationField_);
	particleEditor_.SetEmitter(emitter_);
	particleEditor_.SetField(accelerationField_);
}

void Combine::Update() {
	// カメラのワールド行列を取得（ビュー行列の逆）
	Matrix4x4 view = camera_->GetViewMatrix();
	Matrix4x4 cameraWorld = Inverse(view);

	// カメラ位置と前方ベクトル（ワールド空間）
	Vector3 cameraPos = { cameraWorld.m[3][0], cameraWorld.m[3][1], cameraWorld.m[3][2] };
	Vector3 cameraForward = { cameraWorld.m[2][0], cameraWorld.m[2][1], cameraWorld.m[2][2] };
	cameraForward = Normalize(cameraForward);

	Vector3 startPos = cameraPos + cameraForward * gameDefaultZ_;
	Quaternion lookRot = MakeLookRotation(cameraPos - startPos, Vector3{ 0,1,0 });

	switch (phase_) {
	case Phase::Black:
	{
		float t = min(timer_, kDarkTime) / kDarkTime;
		if (t <= kDarkTime) {
			black_->SetSize(Vector2{ 1280,720 } *max((1 - t), 0.03f) * 60);
			Vector2 screenSize = { float(GameEngine::GetWindowWidth()), float(GameEngine::GetWindowHeight()) };
			black_->SetPosition(screenSize * 0.5f);

			SRT uv = black_->GetUVTransform();
			float uvScale = 1.0f - 0.85f * (1 - t);
			if (1 - t <= 0) {
				black_->SetAnchorPoint({});
				black_->SetSize(Vector2{ 1280,720 } *4);
				black_->SetPosition({});
			}
			uv.scale = { uvScale, uvScale, 1.0f };
			uv.translate = { 0.5f - 0.5f * uvScale, 0.5f - 0.5f * uvScale, 0.0f };
			black_->SetUVTransform(uv);
		}

		if (timer_ / (kDarkTime + kDarkWaitTime) >= 1.0f) {
			timer_ = 0;
			isEnd_ = true;
		}
		break;
	}
	case Phase::Fall:
	{
		rotate_ += 0.07f;

		// バラバラのパーツが降下
		float t = min(timer_ / kFallTime, 1.0f);
		auto b = beyblade_->GetTransform();
		b.translate = Lerp(RotateVector(Vector3{ 0,defaultBY_,0 }, lookRot) + startPos, startPos, t);
		b.rotate = Normalize(MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * (lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_)));
		beyblade_->SetTransform(b);

		if (t == 1.0f) {
			timer_ = 0;
			phase_ = Phase::Set;
		}
	}

	break;
	case Phase::Set:
	{
		rotate_ += 0.15f;

		// パーツごとに合体
		float deltaTime = GameEngine::GetDeltaTime();
		auto parts = beyblade_->GetParts();
		for (int i = 0; i < 8; i++) {
			// パーツごとのタイマー
			setCountdown_[i] -= deltaTime;

			if (setCountdown_[i] <= 0.3f) {
				float t = 1.0f - max(setCountdown_[i] / 0.3f, 0.0f);
				t = EaseOutCubic(t);
				parts[i].transform->translate = Lerp(partsTranslate[i], Vector3{}, t);
				beyblade_->SetParts(parts[i], i);
			}
		}

		auto b = beyblade_->GetTransform();
		b.rotate = Normalize(MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * (lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_)));
		beyblade_->SetTransform(b);

		if (timer_ / kSetTime >= 1.0f) {
			timer_ = 0;
			//phase_ = Phase::Ride;

			float tmp[8] = {
			0.7f,0.7f,0.9f,0.9f,
			5.0f,0.4f,0.4f,0.4f
			};
			std::copy(std::begin(tmp), std::end(tmp), setCountdown_);
		}

		particleEmitter_->Emit();
		particleEmitter_->Update();
		emitter_.transform.translate = beyblade_->GetTransform().translate;

		ImGui::DragFloat("パーティクル Scale", &emitter_.transform.scale.x, 0.1f);
		emitter_.transform.scale = { emitter_.transform.scale.x ,emitter_.transform.scale.x ,emitter_.transform.scale.x };
		ImGui::SliderAngle("パーティクル RotateX", &emitter_.transform.rotate.x);
		ImGui::SliderAngle("パーティクル RotateY", &emitter_.transform.rotate.y);
		ImGui::SliderAngle("パーティクル Rotatez", &emitter_.transform.rotate.z);
		ImGui::DragFloat3("パーティクル Translate", &emitter_.transform.translate.x, 0.1f);
		ImGui::ColorPicker4("beforecolor", &emitter_.beforeColor.x);
		ImGui::ColorPicker4("aftercolor", &emitter_.afterColor.x);
		int count = int(emitter_.count);
		ImGui::DragInt("パーティクル count", &count);
		emitter_.count = count;
		ImGui::DragFloat("パーティクル LifeTime", &emitter_.lifeTime, 0.01f);
		ImGui::DragFloat("パーティクル frequency", &emitter_.frequency, 0.01f);
		particleEmitter_->SetEmitter(emitter_);
		ImGui::DragFloat3("エリアmin", &accelerationField_.area.min.x, 0.1f);
		ImGui::DragFloat3("エリアmax", &accelerationField_.area.max.x, 0.1f);
		ImGui::DragFloat3("エリアAcceleration", &accelerationField_.acceleration.translate.x, 0.01f);
		particleEmitter_->SetField(accelerationField_);
	}
	break;
	case Phase::Ride:
	{
		rotate_ += 0.1f;

		// 人と最終パーツ落下
		float deltaTime = GameEngine::GetDeltaTime();
		float t = min(timer_, kRideTime) / kRideTime;
		auto parts = beyblade_->GetParts();
		parts[4].transform->translate = Lerp(RotateVector(partsTranslate[4], lookRot), {}, t);
		beyblade_->SetParts(parts[4], 4);

		auto h = human_->GetTransform();
		h.rotate = Normalize(lookRot * MakeRotateAxisAngleQuaternion({ 0,1,0 }, std::numbers::pi_v<float>) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_));
		h.translate = parts[4].transform->translate + Vector3{ 0, endY_,0 };
		human_->SetTransform(h);
		human_->Update();

		auto b = beyblade_->GetTransform();
		b.rotate = Normalize(MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * (lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_)));
		beyblade_->SetTransform(b);

		if (timer_ / kRideTime >= 1.0f) {
			timer_ = 0;
			phase_ = Phase::Back;
		}
	}
	break;
	case Phase::Back:
	{
		rotate_ += 0.15f;

		float t = min(timer_, kBackTime) / kBackTime;
		SRT h = human_->GetTransform();
		h.rotate = Normalize(lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, kExtraXRot * t));
		Vector3 translate = startPos + cameraForward * (t * backAmount_) + RotateVector(Vector3{ 0,endY_,0 }, lookRot);
		h.translate = translate + RotateVector(Vector3{ 0,0,kFixOffset }, h.rotate);
		human_->SetTransform(h);
		human_->Update();

		auto b = beyblade_->GetTransform();
		b.rotate = Normalize(MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * (lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_)) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, kExtraXRot * t));
		b.translate = translate - RotateVector(Vector3{ 0,endY_ ,0 }, lookRot);
		beyblade_->SetTransform(b);

		if (timer_ / kBackTime >= 1.0f) {
			phase_ = Phase::Wait;
		}
	}
	break;
	case Phase::Wait:
	{
		Vector3 pos = startPos + cameraForward * backAmount_;

		// キャラをカメラ方向に向ける
		Quaternion lookRot = MakeLookRotation(cameraPos - startPos, Vector3{ 0,1,0 });

		SRT h = human_->GetTransform();
		h.rotate = Normalize(lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, kExtraXRot));
		h.translate = pos + RotateVector(Vector3{ 0,endY_,0 }, lookRot) + RotateVector(Vector3{ 0,0,kFixOffset }, h.rotate);
		human_->SetTransform(h);
		human_->Update();

		auto b = beyblade_->GetTransform();
		b.rotate = Normalize(MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * (lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_)) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, kExtraXRot));
		b.translate = pos;
		beyblade_->SetTransform(b);
	}

	if (timer_ / kBackTime >= 1.0f) {
		timer_ = 0;
		phase_ = Phase::GameStart;
	}
	break;
	case Phase::GameStart:
	{
		rotate_ += 0.15f;

		float t = min(timer_, kStartTime) / kStartTime;
		if (timer_ > kStartIrisIn + 0.05f) {
			SRT uv = black_->GetUVTransform();
			uv.scale = { 1, 1, 1 };
			black_->SetUVTransform(uv);

			float inT = min(timer_ - kStartIrisIn, kStartTime - kStartIrisIn) / (kStartTime - kStartIrisIn);
			black_->SetSize(Vector2{ 1280,720 } *inT * 100);
			Vector2 screenSize = { float(GameEngine::GetWindowWidth()), float(GameEngine::GetWindowHeight()) };
			black_->SetPosition(screenSize * 0.5f);
		}

		// カメラの相対位置
		float ease = EaseInCubic(t);
		Vector3 start = startPos + cameraForward * backAmount_;
		Vector3 end = cameraPos + cameraForward * -30.0f;
		Vector3 worldPos = Lerp(start, end, ease);

		// キャラをカメラ方向に向ける
		Quaternion lookRot = MakeLookRotation(cameraPos - worldPos, Vector3{ 0,1,0 });

		SRT h = human_->GetTransform();
		h.rotate = Normalize(lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, kExtraXRot));
		h.translate = worldPos + RotateVector(Vector3{ 0,endY_,0 }, lookRot) + RotateVector(Vector3{ 0,0,kFixOffset }, h.rotate);
		human_->SetTransform(h);
		human_->Update();

		auto b = beyblade_->GetTransform();
		b.rotate = Normalize(MakeRotateAxisAngleQuaternion({ 0,1,0 }, rotate_) * (lookRot * MakeRotateAxisAngleQuaternion({ 1,0,0 }, angle_)) * MakeRotateAxisAngleQuaternion({ 1,0,0 }, kExtraXRot));
		b.translate = worldPos;
		beyblade_->SetTransform(b);

		if (timer_ / kStartTime >= 1.0f) {
			isEnd_ = true;
		}
		break;
	}
	}

	timer_ += GameEngine::GetDeltaTime();
}

void Combine::Draw() {
	black_->Update();
	black_->Draw2D();

	if (phase_ > Phase::Black) {
		human_->Draw3D();
		beyblade_->Draw3D();

		particleEmitter_->Draw();
	}
}