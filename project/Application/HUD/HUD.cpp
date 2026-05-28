#include "HUD.h"
#include "Human/Player/Player.h"
#include "Course/Course.h"

void HUD::Initialize(Input* input, std::shared_ptr<Camera> camera, std::shared_ptr<DirectionalLight> directionalLight) {
	input_ = input;
	stars_ = std::make_unique<Stars>();
	stars_->Initialize(camera);
	camera_ = camera;

	// チャージ背景
	chargeBGSprite_ = std::make_unique<Sprite>();
	chargeBGSprite_->Initialize("./resources/HUD/Charge.png");
	chargeBGSprite_->SetColor({ 1,1,1,1 });
	chargeBGSprite_->SetSize({ kEnergyBarWidth, 256 + 64});
	chargeBGSprite_->SetPosition(chargeLTPos_);

	// 現在チャージ量
	currentChargeSprite_ = std::make_unique<Sprite>();
	currentChargeSprite_->Initialize("./resources/DebugResources/white2x2.png");
	currentChargeSprite_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
	currentChargeSprite_->SetSize({ kEnergyBarWidth, 32.0f });
	currentChargeSprite_->SetPosition(chargeLTPos_);

	// 破壊量背景
	breakBGSprite_ = std::make_unique<Sprite>();
	breakBGSprite_->Initialize("./resources/DebugResources/white2x2.png");
	breakBGSprite_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	breakBGSprite_->SetSize({ kBreakBarWidth, 32.0f });
	breakBGSprite_->SetPosition(breakLTPos_);

	// 現在破壊量
	currentBreakSprite_ = std::make_unique<Sprite>();
	currentBreakSprite_->Initialize("./resources/DebugResources/white2x2.png");
	currentBreakSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	currentBreakSprite_->SetSize({ kBreakBarWidth, 32.0f });
	currentBreakSprite_->SetPosition(breakLTPos_);

	bonusBreakSprite_ = std::make_unique<Sprite>();
	bonusBreakSprite_->Initialize("./resources/DebugResources/white2x2.png");
	bonusBreakSprite_->SetColor({ 1.0f, 0.6f, 0.6f, 1.0f });
	bonusBreakSprite_->SetSize({ kBreakBarWidth, 32.0f });
	bonusBreakSprite_->SetPosition(breakLTPos_);

	// 時間
	timer_.sprite.resize(timer_.digitCount);
	for (int i = 0; i < timer_.digitCount; ++i) {
		timer_.sprite[i] = std::make_unique<Sprite>();
		timer_.sprite[i]->Initialize("./resources/HUD/Numbers/Number.png");
		timer_.sprite[i]->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		timer_.sprite[i]->SetSize(timer_.size);
		timer_.sprite[i]->SetAnchorPoint({ 0.5f,0.5f });
		timer_.sprite[i]->SetPosition({ timer_.pos.x + timer_.spacing * i, timer_.pos.y });
		timer_.sprite[i]->SetTextureSize(kNumberSize);
	}

	// 区間
	sectionSprite_ = std::make_unique<Sprite>();
	sectionSprite_->Initialize("./resources/DebugResources/white2x2.png");
	sectionSprite_->SetColor({ 0.2f,0.2f,0.2f, 1.0f });
	sectionSprite_->SetSize(sectionBarSize_);
	sectionSprite_->SetPosition(sectionLTPos_);
	sectionSprite_->Update();
	progressSprite_ = std::make_unique<Sprite>();
	progressSprite_->Initialize("./resources/DebugResources/white2x2.png");
	progressSprite_->SetColor({ 1.0f,1.0f,1.0f, 1.0f });
	progressSprite_->SetSize({ sectionBarSize_.x,0 });
	progressSprite_->SetPosition(sectionLTPos_);
	progressSprite_->Update();

	// 現時点の目的
	objective_[0] = std::make_unique<Sprite>();
	objective_[0]->Initialize("./resources/HUD/norma_Dontclear.png");
	objective_[1] = std::make_unique<Sprite>();
	objective_[1]->Initialize("./resources/HUD/norma_clear.png");
	for (int i = 0; i < 2; ++i) {
		objective_[i]->SetAnchorPoint({ 0.5f,0.5f });
		objective_[i]->SetPosition(objectivePos_);
		objective_[i]->SetSize(objectiveSize_);
		objective_[i]->Update();
	}


	//情報
	infoSprite_ = std::make_unique<Sprite>();
	infoSprite_->Initialize("./resources/HUD/Info.png");
	infoSprite_->SetSize(Vector2{ 471.0f,62.0f });
	infoSprite_->SetTextureSize(Vector2{ 471.0f,62.0f });
	infoSprite_->SetPosition(infoLTPos_);

	startNumSprite_ = std::make_unique<Sprite>();
	startNumSprite_->Initialize("./resources/HUD/Start_Nums.png");
	startNumSprite_->SetSize(Vector2{ 50,61.0f });
	startNumSprite_->SetTextureSize(Vector2{ 50.0f,61.0f });
	startNumSprite_->SetPosition(startNumPos_);
	startNumSprite_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
	startNumIsDraw_ = true;

	canShoot_ = std::make_unique<Sprite>();
	canShoot_->Initialize("./resources/HUD/CanShoot.png");
	canShoot_->SetSize(Vector2{ 180.0f,40.0f });
	canShoot_->SetTextureSize(Vector2{ 180.0f,40.0f });
	canShoot_->SetPosition({ 640,300 });
	canShoot_->SetAnchorPoint(Vector2{ 0.5f,0.5f });

	// リザルト項目
	for (int i = 0; i < 3; ++i) {
		sectionResult_[i] = std::make_unique<Object>();

		if (i == 0) {
			sectionResult_[i]->Initialize(ModelManager::GetInstance()->GetModel("./resources/HUD/Result", "UI1.obj"));
		} else if (i == 1) {
			sectionResult_[i]->Initialize(ModelManager::GetInstance()->GetModel("./resources/HUD/Result", "UI2.obj"));
		} else {
			sectionResult_[i]->Initialize(ModelManager::GetInstance()->GetModel("./resources/HUD/Result", "UI3.obj"));
		}

		sectionResult_[i]->SetShininess(30.0f);
		sectionResult_[i]->SetDirectionalLight(directionalLight);
		sectionResult_[i]->SetCamera(camera);
	}

	// リザルト:破壊率
	breakRate_.object.resize(breakRate_.digitCount);
	for (int i = 0; i < breakRate_.digitCount; ++i) {
		breakRate_.object[i] = std::make_unique<Object>();
		breakRate_.object[i]->Initialize(ModelManager::GetInstance()->GetModel("./resources/HUD/Result", "Number.obj"));
		breakRate_.object[i]->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		breakRate_.object[i]->SetTransform({ breakRate_.scale, {}, {} });
		breakRate_.object[i]->SetShininess(30.0f);
		breakRate_.object[i]->SetDirectionalLight(directionalLight);
		breakRate_.object[i]->SetCamera(camera);
	}

	// リザルト:破壊量
	breakCount_.object.resize(breakCount_.digitCount);
	for (int i = 0; i < breakCount_.digitCount; ++i) {
		breakCount_.object[i] = std::make_unique<Object>();
		breakCount_.object[i]->Initialize(ModelManager::GetInstance()->GetModel("./resources/HUD/Result", "Number.obj"));
		breakCount_.object[i]->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		breakCount_.object[i]->SetShininess(30.0f);
		breakCount_.object[i]->SetDirectionalLight(directionalLight);
		breakCount_.object[i]->SetCamera(camera);
	}

	// リザルト:時間
	sectionTime_.object.resize(sectionTime_.digitCount);
	for (int i = 0; i < sectionTime_.digitCount; ++i) {
		sectionTime_.object[i] = std::make_unique<Object>();
		sectionTime_.object[i]->Initialize(ModelManager::GetInstance()->GetModel("./resources/HUD/Result", "Number.obj"));
		sectionTime_.object[i]->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		sectionTime_.object[i]->SetShininess(30.0f);
		sectionTime_.object[i]->SetDirectionalLight(directionalLight);
		sectionTime_.object[i]->SetCamera(camera);
	}

	// ランク
	for (int i = 0; i < 3; ++i) {
		sectionRank_.object[i] = std::make_unique<Object>();
	}
	sectionRank_.object[A-1]->Initialize(ModelManager::GetInstance()->GetModel("./resources/HUD/Result", "Rank_A.obj"));
	sectionRank_.object[B-1]->Initialize(ModelManager::GetInstance()->GetModel("./resources/HUD/Result", "Rank_B.obj"));
	sectionRank_.object[C-1]->Initialize(ModelManager::GetInstance()->GetModel("./resources/HUD/Result", "Rank_C.obj"));
	for (int i = 0; i < 3; ++i) {
		sectionRank_.object[i]->SetShininess(30.0f);
		sectionRank_.object[i]->SetDirectionalLight(directionalLight);
		sectionRank_.object[i]->SetCamera(camera);
	}

	objectRot_ = MakeRotateAxisAngleQuaternion({ 0,1,0 }, float(std::numbers::pi) / 2.0f);
}

void HUD::Update(Player* player, Course* course, GameTimer* timer, int startNum, std::shared_ptr<Camera> camera) {
	UpdateCharge(player);
	UpdateScore(course);
	UpdateTimer(course);
	UpdateBreakRate(course);
	UpdateBreakAmount(course);
	UpdateSection(player, course);
	UpdateInfo();
	UpdateStartNum(startNum);
	UpdateReload(player, camera);
	UpdateResult(course);

	auto inverseView = Inverse(camera->GetViewMatrix());
	cameraTransform_ = { {1,1,1},MatrixToQuaternion(inverseView), {inverseView.m[3][0], inverseView.m[3][1], inverseView.m[3][2]} };
	Vector3 right = Normalize(Vector3{ inverseView.m[0][0], inverseView.m[0][1], inverseView.m[0][2] });
	Vector3 up = Normalize(Vector3{inverseView.m[1][0], inverseView.m[1][1], inverseView.m[1][2]});
	Vector3 forward = Normalize(Vector3{ inverseView.m[2][0], inverseView.m[2][1], inverseView.m[2][2] });

	// エフェクト
	auto pos = course->GetBreakPos();
	for (int i = 0; i < int(pos.size()); i += 800) {
		stars_->AddStar(pos[i]);
	}
	Vector2 center = currentBreakSprite_->GetSize();
	center.x += bonusBreakSprite_->GetSize().x;
	center.y /= 2.0f;
	stars_->Update(breakLTPos_ + center);

#ifdef USE_IMGUI
	ImGui::Begin("Result HUD");
	if (ImGui::BeginTabBar("HUD")) {
		if (ImGui::BeginTabItem("Rate")) {
			ImGui::DragFloat3("rate.pos", &breakRate_.pos.x, 0.5f);
			ImGui::DragFloat3("rate.scale", &breakRate_.scale.x, 0.1f);
			ImGui::DragFloat("rate.spacing", &breakRate_.spacing, 0.1f);

			Vector3 s = breakRate_.object[2]->GetTransform().scale;
			ImGui::Text("rateS %f, %f, %f", s.x, s.y, s.z);
			Quaternion r = breakRate_.object[2]->GetTransform().rotate;
			ImGui::Text("rateR %f, %f, %f, %f", r.x, r.y, r.z, r.w);
			Vector3 t = breakRate_.object[2]->GetTransform().translate;
			ImGui::Text("rateT %f, %f, %f", t.x, t.y, t.z);

			ImGui::Text("cameraS %f, %f, %f", cameraTransform_.scale.x, cameraTransform_.scale.y, cameraTransform_.scale.z);
			ImGui::Text("cameraR %f, %f, %f", cameraTransform_.rotate.x, cameraTransform_.rotate.y, cameraTransform_.rotate.z);
			ImGui::Text("cameraT %f, %f, %f", cameraTransform_.translate.x, cameraTransform_.translate.y, cameraTransform_.translate.z);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Count")) {
			ImGui::DragFloat3("count.pos", &breakCount_.pos.x, 0.5f);
			ImGui::DragFloat3("count.scale", &breakCount_.scale.x, 0.1f);
			ImGui::DragFloat("count.spacing", &breakCount_.spacing, 0.1f);

			Vector3 text = RotateVector(breakCount_.pos, cameraTransform_.rotate);
			ImGui::Text("count %f, %f, %f", text.x, text.y, text.z);
			Vector3 a = RotateVector(breakCount_.pos, cameraTransform_.rotate);
			Vector3 b = right * breakCount_.pos.x + up * breakCount_.pos.y + forward * breakCount_.pos.z;
			ImGui::Text("a %f, %f, %f", a.x, a.y, a.z);
			ImGui::Text("b %f, %f, %f", b.x, b.y, b.z);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Time")) {
			ImGui::DragFloat3("time.pos", &sectionTime_.pos.x, 0.5f);
			ImGui::DragFloat3("time.scale", &sectionTime_.scale.x, 0.1f);
			ImGui::DragFloat("time.spacing", &sectionTime_.spacing, 0.1f);

			Vector3 text = RotateVector(sectionTime_.pos, cameraTransform_.rotate);
			ImGui::Text("time %f, %f, %f", text.x, text.y, text.z);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Rank")) {
			ImGui::DragFloat3("rank.pos", &sectionRank_.pos.x, 0.5f);
			ImGui::DragFloat3("rank.scale", &sectionRank_.scale.x, 0.1f);

			Vector3 text = RotateVector(sectionRank_.pos, cameraTransform_.rotate);
			ImGui::Text("rank %f, %f, %f", text.x, text.y, text.z);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Items")) {
			ImGui::DragFloat3("resultItem.pos0", &resultPos_[0].x, 0.5f);
			ImGui::DragFloat3("resultItem.pos1", &resultPos_[1].x, 0.5f);
			ImGui::DragFloat3("resultItem.pos2", &resultPos_[2].x, 0.5f);
			ImGui::DragFloat3("resultItem.size", &resultSize_.x, 0.1f);

			Vector3 text = RotateVector(resultPos_[0], cameraTransform_.rotate);
			ImGui::Text("resultItem %f, %f, %f", text.x, text.y, text.z);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	float t = 0;

	if (course->GetResultState() == ResultState::SetResults) {
		float setTime = 0.12f;
		resultTimer_ += GameEngine::GetDeltaTime();
		t = std::clamp(1.0f - powf(1 - resultTimer_ / setTime, 3), 0.0f, 1.0f);

	} else if (course->GetResultState() == ResultState::RotateOut) {
		float outTime = 0.12f;
		resultTimer_ += GameEngine::GetDeltaTime();
		t = std::clamp(powf(1 - resultTimer_ / outTime, 3), 0.0f, 1.0f);

	} else  if (course->GetResultState() == ResultState::Wait) {
		t = 1;
		resultTimer_ = 0;
	}

	for (int i = 0; i < 3; ++i) {
		sectionResult_[i]->SetTransform({ {resultSize_ * t},
			objectRot_ * cameraTransform_.rotate,
			CameraLocalToWorld(resultPos_[i], cameraTransform_.translate, right, up, forward) });
	}
	rank_ = course->GetRank();

	// 破壊率
	unuseDigitCountBreakRate_ = 0;
	if (course->GetPrevBreakRate() < 100) unuseDigitCountBreakRate_++;
	if (course->GetPrevBreakRate() < 10) unuseDigitCountBreakRate_++;

	float width = breakRate_.scale.x + breakRate_.spacing * (breakRate_.digitCount - unuseDigitCountBreakRate_ - 1);
	float startX = breakRate_.pos.x - width * 0.5f + breakRate_.scale.x * 0.5f;
	int index = 0;
	for (int i = unuseDigitCountBreakRate_; i < breakRate_.digitCount; ++i) {
		breakRate_.object[i]->SetTransform({ {breakRate_.scale * t},
			objectRot_ * cameraTransform_.rotate,
			CameraLocalToWorld(Vector3{ startX + breakRate_.spacing * index, breakRate_.pos.y, breakRate_.pos.z },
				cameraTransform_.translate, right, up, forward)
			});

		index++;
	}

	// 破壊個数
	unuseDigitCountBreakAmount_ = 0;
	if (course->GetBreakCount() < 100000) unuseDigitCountBreakAmount_++;
	if (course->GetBreakCount() < 10000) unuseDigitCountBreakAmount_++;
	if (course->GetBreakCount() < 1000) unuseDigitCountBreakAmount_++;
	if (course->GetBreakCount() < 100) unuseDigitCountBreakAmount_++;
	if (course->GetBreakCount() < 10) unuseDigitCountBreakAmount_++;

	width = breakCount_.scale.x + breakCount_.spacing * (breakCount_.digitCount - unuseDigitCountBreakAmount_ - 1);
	startX = breakCount_.pos.x - width * 0.5f + breakCount_.scale.x * 0.5f;
	index = 0;
	for (int i = unuseDigitCountBreakAmount_; i < breakCount_.digitCount; ++i) {
		breakCount_.object[i]->SetTransform({ { breakCount_.scale * t },
			objectRot_ * cameraTransform_.rotate,
			CameraLocalToWorld(Vector3{ startX + breakCount_.spacing * index, breakCount_.pos.y, breakCount_.pos.z },
			cameraTransform_.translate, right, up, forward)
			});

		index++;
	}

	// 時間
	useMinusSectionTime_ = lastTime_ < 0;
	if (useMinusSectionTime_) {
		float width = sectionTime_.scale.x + sectionTime_.spacing * (sectionTime_.digitCount - 1);
		float startX = sectionTime_.pos.x - width * 0.5f + sectionTime_.scale.x * 0.5f;
		index = 0;
		for (int i = 0; i < sectionTime_.digitCount; ++i) {
			sectionTime_.object[i]->SetTransform({ {sectionTime_.scale * t},
			objectRot_ * cameraTransform_.rotate,
			CameraLocalToWorld(Vector3{ startX + sectionTime_.spacing * index, sectionTime_.pos.y, sectionTime_.pos.z },
				cameraTransform_.translate, right, up, forward)
				});

			index++;
		}
	} else {
		// 一番左(マイナス)を非表示
		float width = sectionTime_.scale.x + sectionTime_.spacing * (sectionTime_.digitCount - 2);
		float startX = sectionTime_.pos.x - width * 0.5f + sectionTime_.scale.x * 0.5f;
		index = 0;
		for (int i = 1; i < sectionTime_.digitCount; ++i) {
			sectionTime_.object[i]->SetTransform({ {sectionTime_.scale * t},
			objectRot_ * cameraTransform_.rotate,
			CameraLocalToWorld(Vector3{ startX + sectionTime_.spacing * index, sectionTime_.pos.y, sectionTime_.pos.z },
				cameraTransform_.translate, right, up, forward)
				});

			index++;
		}
	}

	// ランク
	for (auto& obj : sectionRank_.object) {
		obj->SetTransform({ {sectionRank_.scale * t},
			objectRot_ * cameraTransform_.rotate,
			CameraLocalToWorld(sectionRank_.pos, cameraTransform_.translate, right, up, forward)
			});
	}

	ImGui::End();
#endif
}

void HUD::Draw() {
	chargeBGSprite_->Draw2D();
	currentChargeSprite_->Draw2D();

	if (canDrawPlayingInfo_) {
		// 区間
		breakBGSprite_->Draw2D();
		bonusBreakSprite_->Draw2D();
		currentBreakSprite_->Draw2D();

		sectionSprite_->Draw2D();
		progressSprite_->Draw2D();

		// 目的
		currentObjective_->Draw2D();

		int start = 0;
		if (!useMinusSectionTime_) start++;
		for (int i = start; i < timer_.digitCount; ++i) {
			timer_.sprite[i]->Draw2D();
		}

		// エフェクト
		stars_->Draw();

		canShoot_->Draw2D();

	} else if (!isSectionFailed_) {
		// リザルト中
		for (int i = 0; i < 3; ++i) {
			sectionResult_[i]->Draw3D();
		}

		// 破壊率
		for (int i = unuseDigitCountBreakRate_; i < breakRate_.digitCount; ++i) {
			breakRate_.object[i]->Draw3D();
		}

		// 破壊量
		for (int i = unuseDigitCountBreakAmount_; i < breakCount_.digitCount; ++i) {
			breakCount_.object[i]->Draw3D();
		}

		// 時間
		int start = 0;
		if (!useMinusSectionTime_) start++;
		for (int i = start; i < sectionTime_.digitCount; ++i) {
			sectionTime_.object[i]->Draw3D();
		}

		sectionRank_.object[rank_-1]->Draw3D();
	}

	infoSprite_->Draw2D();

	if (startNumIsDraw_) {
		startNumSprite_->Draw2D();
	}
}

void HUD::SetPauseDisplay(bool isOn) {
	if (isOn) {
		Vector4 color;
		color = canShoot_->GetColor();
		color.w = 0.0f;
		canShoot_->SetColor(color);
	} else {
		Vector4 color;
		color = canShoot_->GetColor();
		color.w = 1.0f;
		canShoot_->SetColor(color);
	}
}

void HUD::UpdateCharge(Player* player) {
	float current = player->GetCharge();
	float max = player->GetMaxCharge();
	if (current < 0) return;

	// 割合を求める
	float rate = current / max;
	// 溜め量に応じてスプライトのサイズ変更
	float length = kEnergyBarWidth * rate;
	currentChargeSprite_->SetSize({ length, currentChargeSprite_->GetSize().y });
	chargeBGSprite_->Update();
	currentChargeSprite_->Update();
}

void HUD::UpdateScore(Course* course) {
	Section* currentSection = course->GetCurrentSection();
	int current = currentSection->GetCurrentScore();
	int clear = currentSection->GetClearScore();
	int max = currentSection->GetMaxScore();
	if (current < 0) return;


	// 区間などの情報を描画するかどうか
	canDrawScore_ = !currentSection->IsSubSection();
	isSectionFailed_ = course->GetIsSectionFailed();

	// プレイ中の情報を描画するかどうか
	canDrawPlayingInfo_ = !currentSection->IsSubSection();


	if (canDrawPlayingInfo_) {
		// 割合を求める
		float rate = float(current) / float(clear);
		float clearRate = float(clear) / float(max);
		rate = clamp(rate, 0.0f, 1.0f);
		// 必要スコアに応じてスプライトのサイズ変更
		float length = kBreakBarWidth * rate * clearRate;
		currentBreakSprite_->SetSize({ length, currentBreakSprite_->GetSize().y });
		currentBreakSprite_->SetPosition({ breakLTPos_.x, breakLTPos_.y });
		breakBGSprite_->Update();
		currentBreakSprite_->Update();

		// 割合を求める
		rate = 0;
		if (currentSection->IsCleared()) {
			rate = float(current - clear) / float(max - clear);
			rate = clamp(rate, 0.0f, 1.0f);
		}
		// 最大スコアに応じてスプライトのサイズ変更
		length = kBreakBarWidth * rate * (1.0f - clearRate);
		bonusBreakSprite_->SetSize({ length, bonusBreakSprite_->GetSize().y });
		bonusBreakSprite_->SetPosition({ breakLTPos_.x + kBreakBarWidth * clearRate, breakLTPos_.y });
		bonusBreakSprite_->Update();

		// ノルマ達成/未達成
		if (currentSection->IsCleared()) {
			currentObjective_ = objective_[1].get();
		} else {
			currentObjective_ = objective_[0].get();
		}
	}
}

void HUD::UpdateTimer(Course* course) {
	Section* currentSection = course->GetCurrentSection();
	if (!currentSection->IsSubSection()) {
		lastTime_ = int(currentSection->GetTimer()->GetCurrent());
		int min = abs(lastTime_) / 60;
		int sec = abs(lastTime_) % 60;

		int num[4] = { min/10, min % 10, sec / 10, sec % 10 };
		int spriteNum[3]{};
		for (int i = 0; i < 3; ++i) {
			if (num[i+1] == 0) {
				spriteNum[i] = 9;
			} else {
				spriteNum[i] = num[i+1] - 1;
			}
		}

		if (lastTime_ < 0) {
			// マイナス
			timer_.sprite[0]->SetTextureLeftTop({ 11.0f * kNumberSize.x, 0 });


			for (int i = 0; i < timer_.digitCount; ++i) {
				timer_.sprite[i]->SetColor({ 1,0,0,1 });
				sectionTime_.object[i]->SetColor({ 1,0,0,1 });
			}
			sectionTime_.object[5]->SetColor({ 1,0,0,1 });
		} else {
			for (int i = 0; i < timer_.digitCount; ++i) {
				timer_.sprite[i]->SetColor({ 1,1,1,1 });
				sectionTime_.object[i]->SetColor({ 1,1,1,1 });
			}
			sectionTime_.object[5]->SetColor({ 1,1,1,1 });
		}

		timer_.sprite[1]->SetTextureLeftTop({ spriteNum[0] * kNumberSize.x, 0 });
		timer_.sprite[2]->SetTextureLeftTop({ 10.0f * kNumberSize.x, 0 });
		timer_.sprite[3]->SetTextureLeftTop({ spriteNum[1] * kNumberSize.x, 0 });
		timer_.sprite[4]->SetTextureLeftTop({ spriteNum[2] * kNumberSize.x, 0 });
		for (int i = 0; i < timer_.digitCount; ++i) {
			timer_.sprite[i]->Update();
		}

		// まず見えなくする
		for (int i = 0; i < sectionTime_.digitCount; ++i) {
			for (auto& part : sectionTime_.object[i]->GetParts()) {
				part.material->color.w = 0;
			}
		}
		// 該当パーツだけ表示状態にする
		sectionTime_.object[0]->GetParts()[ConvertPartNumber(10)].material->color.w = 1; // -
		sectionTime_.object[1]->GetParts()[ConvertPartNumber(num[0])].material->color.w = 1;
		sectionTime_.object[2]->GetParts()[ConvertPartNumber(num[1])].material->color.w = 1;
		sectionTime_.object[3]->GetParts()[ConvertPartNumber(11)].material->color.w = 1; // :
		sectionTime_.object[4]->GetParts()[ConvertPartNumber(num[2])].material->color.w = 1;
		sectionTime_.object[5]->GetParts()[ConvertPartNumber(num[3])].material->color.w = 1;
	}
}

void HUD::UpdateBreakRate(Course* course) {
	if (!course->InSubSection()) return;

	// リザルト中なら破壊率表示準備
	int rate = int(course->GetPrevBreakRate());
	int num[3] = { rate / 100, rate % 100 / 10, rate % 10 };

	// まず見えなくする
	for (int i = 0; i < breakRate_.digitCount; ++i) {
		for (auto& part : breakRate_.object[i]->GetParts()) {
			part.material->color.w = 0;
		}
	}
	// 該当パーツだけ表示状態にする
	breakRate_.object[0]->GetParts()[ConvertPartNumber(num[0])].material->color.w = 1;
	breakRate_.object[1]->GetParts()[ConvertPartNumber(num[1])].material->color.w = 1;
	breakRate_.object[2]->GetParts()[ConvertPartNumber(num[2])].material->color.w = 1;
	breakRate_.object[3]->GetParts()[ConvertPartNumber(12)].material->color.w = 1; // %
}

void HUD::UpdateBreakAmount(Course* course) {
	if (!course->InSubSection()) return;

	int breakCount = course->GetBreakCount();
	int num[6]{};
	for (int i = 5; i >= 0; --i) {
		num[i] = breakCount % 10;
		breakCount /= 10;
	}

	// まず見えなくする
	for (int i = 0; i < breakCount_.digitCount; ++i) {
		for (auto& part : breakCount_.object[i]->GetParts()) {
			part.material->color.w = 0;
		}

		// 該当パーツだけ表示状態にする
		breakCount_.object[i]->GetParts()[ConvertPartNumber(num[i])].material->color.w = 1;
	}
}

void HUD::UpdateSection(Player* player, Course* course) {
	Section* currentSection = course->GetCurrentSection();
	float rate = currentSection->GetPositionRate();

	// 進度に応じてスプライトのサイズ変更
	float length = sectionBarSize_.y * rate;
	progressSprite_->SetSize({ sectionBarSize_.x, length });
	sectionSprite_->Update();
	progressSprite_->Update();
}

void HUD::UpdateInfo() {
	if (input_->GetPad(0).isConnected) {
		infoSprite_->SetTextureLeftTop(Vector2{ 0.0f,62.0f });
	} else {
		infoSprite_->SetTextureLeftTop(Vector2{ 0.0f,0.0f });
	}
	infoSprite_->Update();
}

void HUD::UpdateStartNum(int num) {

	if (num == 0) {
		startNumIsDraw_ = false;
		return;
	}

	startNumSprite_->SetTextureLeftTop(Vector2{ 150.0f - 50.0f * num,0.0f });
	startNumSprite_->Update();
}

void HUD::UpdateReload(Player* player, std::shared_ptr<Camera> camera) {
	Matrix4x4 viewProjection = camera->GetViewMatrix() * camera->GetProjectionMatrix();

	Vector2 screen = ToScreen(camera, player->GetTransform().translate);

	drawCanShoot_ = player->CanShoot();
	if (drawCanShoot_) {
		canShoot_->SetTextureLeftTop({ 0,0 });
	} else {
		canShoot_->SetTextureLeftTop({ 0,40 });
	}
	canShoot_->SetPosition(screen + Vector2{ 0,-50 });
	canShoot_->Update();
}

void HUD::UpdateResult(Course* course) {
	if (course->GetResultState() == ResultState::SetResults ||
		course->GetResultState() == ResultState::RotateOut) {
		float t = 0;

		if (course->GetResultState() == ResultState::SetResults) {
			float setTime = 0.12f;
			resultTimer_ += GameEngine::GetDeltaTime();
			t = std::clamp(1.0f - powf(1 - resultTimer_ / setTime, 3), 0.0f, 1.0f);

		} else if (course->GetResultState() == ResultState::RotateOut) {
			float outTime = 0.12f;
			resultTimer_ += GameEngine::GetDeltaTime();
			t = std::clamp(powf(1 - resultTimer_ / outTime, 3), 0.0f, 1.0f);

		}

		// 破壊率
		unuseDigitCountBreakRate_ = 0;
		if (course->GetPrevBreakRate() < 100) unuseDigitCountBreakRate_++;
		if (course->GetPrevBreakRate() < 10) unuseDigitCountBreakRate_++;



	}
}

inline Vector4 Transform(const Vector4& vector, const Matrix4x4& matrix) {
	// 座標変換した結果
	Vector4 transform;

	transform.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + vector.w * matrix.m[3][0];
	transform.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + vector.w * matrix.m[3][1];
	transform.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + vector.w * matrix.m[3][2];
	transform.w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + vector.w * matrix.m[3][3];

	assert(transform.w != 0.0f);
	transform.x /= transform.w;
	transform.y /= transform.w;
	transform.z /= transform.w;
	transform.w /= transform.w;

	return transform;
}

inline Vector2 ToScreen(std::shared_ptr<Camera> camera, Vector3 worldPos) {
	Matrix4x4 viewProjection = camera->GetViewMatrix() * camera->GetProjectionMatrix();
	Vector3 pos = worldPos;
	Vector4 world = { pos.x, pos.y, pos.z, 1.0f };
	Vector4 clipPos = Transform(world, viewProjection);

	// w除算
	Vector3 ndc;
	ndc.x = clipPos.x / clipPos.w;
	ndc.y = clipPos.y / clipPos.w;
	ndc.z = clipPos.z / clipPos.w;

	// スクリーン変換
	Vector2 screen;
	screen.x = (ndc.x + 1.0f) * 0.5f * 1280;
	screen.y = (1.0f - ndc.y) * 0.5f * 720;

	return screen;
}

Vector3 CameraLocalToWorld(Vector3 local, Vector3 camPos, Vector3 right, Vector3 up, Vector3 forward) {
	return camPos + right * local.x + up * local.y + forward * local.z;
}

int ConvertPartNumber(int num) {
	switch (num) {
	case 0:
		return 7;
		break;

	case 1:
		return 6;
		break;

	case 2:
		return 10;
		break;

	case 3:
		return 11;
		break;

	case 4:
		return 5;
		break;

	case 5:
		return 4;
		break;

	case 6:
		return 3;
		break;

	case 7:
		return 12;
		break;

	case 8:
		return 2;
		break;

	case 9:
		return 0;
		break;

	case 10: // -
		return 1;
		break;

	case 11: // :
		return 8;
		break;
	}

	// %
	return 9;
}