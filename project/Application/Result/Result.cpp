#include "Result.h"
#include "Human/Player/Player.h"
#include <numbers>
#include "GameCamera/GameCamera.h"
#include "Course/Course.h"
#include "HUD/HUD.h"

void Result::Initialize(std::shared_ptr<DirectionalLight> directionalLight, int stageNum) {
	studio_ = make_unique<Object>();
	studio_->Initialize(ModelManager::GetInstance()->GetModel("resources/Studio", "Studio.obj"));
	studio_->SetShininess(30.0f);
	studio_->SetDirectionalLight(directionalLight);
	studio_->SetTransform(studioTransform_);


	scores_.resize(kScoreItemCount);
	for (int i = 0; i < kScoreItemCount; ++i) {
		scores_[i] = make_unique<Object>();
	}
	scores_[0]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Stage_flame.obj"));
	scores_[1]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Stage_flame.obj"));
	scores_[2]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Stage_flame.obj"));

	scores_[3]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Stage_number.obj"));
	scores_[4]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Stage_number.obj"));
	scores_[5]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Stage_number.obj"));

	scores_[6]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_A.obj"));
	scores_[7]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_A.obj"));
	scores_[8]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_A.obj"));

	scores_[9]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "UI1.obj"));
	scores_[10]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "UI2.obj"));
	scores_[11]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "UI3.obj"));

	scores_[12]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "rank_flame.obj"));
	scores_[13]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_A.obj"));
	scores_[14]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Long_flame.obj"));
	for (int i = 0; i < kScoreItemCount; ++i) {
		scores_[i]->SetShininess(30.0f);
		scores_[i]->SetDirectionalLight(directionalLight);

		float rot = 2.89f;
		Vector3 translate = scoreTransform_[i].translate;
		if (i >= 3 && i <= 5) {
			rot = 1.38f;
			translate = stageNumPos[stageNum][i - 3];
			for (auto& part : scores_[i]->GetParts()) {
				part.material->color.w = 0;
			}
			scores_[i]->GetParts()[stageNum * 3 + (i - 3)].material->color.w = 1;


		}
		scores_[i]->SetTransform({
			scoreTransform_[i].scale,
			MatrixToQuaternion(MakeAffineMatrix({ 1,1,1 }, {0,rot,0}, {0,0,0})),
			translate
			});

		scoreTimer_[i] = kScoreTime[i];
	}


	breakRate_.object.resize(breakRate_.digitCount);
	for (int i = 0; i < breakRate_.digitCount; ++i) {
		breakRate_.object[i] = std::make_unique<Object>();

		breakRate_.object[i]->Initialize(
			ModelManager::GetInstance()->GetModel(
				"./resources/HUD/Result",
				"Number.obj"
			)
		);

		breakRate_.object[i]->SetColor({ 1,1,1,1 });
		breakRate_.object[i]->SetShininess(30.0f);
		breakRate_.object[i]->SetDirectionalLight(directionalLight);
	}

	breakCount_.object.resize(breakCount_.digitCount);
	for (int i = 0; i < breakCount_.digitCount; ++i) {
		breakCount_.object[i] = std::make_unique<Object>();

		breakCount_.object[i]->Initialize(
			ModelManager::GetInstance()->GetModel(
				"./resources/HUD/Result",
				"Number.obj"
			)
		);

		breakCount_.object[i]->SetColor({ 1,1,1,1 });
		breakCount_.object[i]->SetShininess(30.0f);
		breakCount_.object[i]->SetDirectionalLight(directionalLight);
	}

	sectionTime_.object.resize(sectionTime_.digitCount);
	for (int i = 0; i < sectionTime_.digitCount; ++i) {
		sectionTime_.object[i] = std::make_unique<Object>();

		sectionTime_.object[i]->Initialize(
			ModelManager::GetInstance()->GetModel(
				"./resources/HUD/Result",
				"Number.obj"
			)
		);

		sectionTime_.object[i]->SetColor({ 1,1,1,1 });
		sectionTime_.object[i]->SetShininess(30.0f);
		sectionTime_.object[i]->SetDirectionalLight(directionalLight);
	}
	Quaternion q = MatrixToQuaternion(MakeAffineMatrix({ 1,1,1 }, { 0,2.89f,0 }, { 0,0,0 }));
	breakCount_.object[0]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.105f,155.6f,2.884f} });
	breakCount_.object[1]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.147f,155.6f,3.049f} });
	breakCount_.object[2]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.19f,155.6f,3.213f} });
	breakCount_.object[3]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.229f,155.6f,3.368f} });
	breakCount_.object[4]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.276f,155.6f,3.552f} });
	breakCount_.object[5]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.321f,155.6f,3.727f} });

	breakRate_.object[0]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.137f,155,3.01f} });
	breakRate_.object[1]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.179f,155,3.175f} });
	breakRate_.object[2]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.222f,155,3.339f} });
	breakRate_.object[3]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.264f,155,3.504f} });

	sectionTime_.object[0]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.090f,154.4f,2.826f} });
	sectionTime_.object[1]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.129f,154.4f,2.981f} });
	sectionTime_.object[2]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.172f,154.4f,3.145f} });
	sectionTime_.object[3]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.214f,154.4f,3.310f} });
	sectionTime_.object[4]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.264f,154.4f,3.504f} });
	sectionTime_.object[5]->SetTransform({ {0.4f,0.25f,0.25f},q,{-9.304f,154.4f,3.659f} });
}

void Result::Update(Player* player, Input* input, Course* course) {
	switch (phase_) {
	case ResultPhase::Set:
		timer_[int(ResultPhase::Set)] += GameEngine::GetDeltaTime();

		if (timer_[int(ResultPhase::Set)] >= kTime[int(ResultPhase::Set)]) {
			startY_ = player->GetTransform().translate.y;
			phase_ = ResultPhase::Rise;

			int total = A * 3;
			int rank = course->GetRank(0);
			if (rank == B) {
				scores_[6]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_B.obj"));
				total -= 1;
			} else if (rank == C) {
				scores_[6]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_C.obj"));
				total -= 2;
			}
			rank = course->GetRank(2);
			if (rank == B) {
				scores_[7]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_B.obj"));
				total -= 1;
			} else if (rank == C) {
				scores_[7]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_C.obj"));
				total -= 2;
			}
			rank = course->GetRank(4);
			if (rank == B) {
				scores_[8]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_B.obj"));
				total -= 1;
			} else if (rank == C) {
				scores_[8]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_C.obj"));
				total -= 2;
			}
			int ave = int(std::roundf(float(total) / 3.0f));
			if (ave == B) {
				scores_[13]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_B.obj"));
			} else if (ave <= C) {
				scores_[13]->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Result", "Rank_C.obj"));
			}

			for (int i = 6; i <= 8; ++i) {
				float rot = 2.89f;
				Vector3 translate = scoreTransform_[i].translate;

				scores_[i]->SetTransform({
				scoreTransform_[i].scale,
				MatrixToQuaternion(MakeAffineMatrix({ 1,1,1 }, {0,rot,0}, {0,0,0})),
				translate
					});
			}
			float rot = 2.89f;
			Vector3 translate = scoreTransform_[13].translate;

			scores_[13]->SetTransform({
			scoreTransform_[13].scale,
			MatrixToQuaternion(MakeAffineMatrix({ 1,1,1 }, {0,rot,0}, {0,0,0})),
			translate
				});

			UpdateTimer(course);
			UpdateBreakRate(course);
			UpdateBreakAmount(course);
		}
		break;

	case ResultPhase::Rise:
	{
		timer_[int(ResultPhase::Rise)] += GameEngine::GetDeltaTime();

		float x = min(CurrentTimeRate(), 1.0f);
		float t = powf(x, 3.0f);
		player->SetTranslate(Lerp(Vector3{ 0,startY_,0 }, Vector3{ 0,endY_,0 }, t));


		if (timer_[int(ResultPhase::Rise)] >= kTime[int(ResultPhase::Rise)] + 0.5f) {
			phase_ = ResultPhase::ZoomOut;
		}

#ifdef USE_IMGUI
		ImGui::Begin("FinalResult");
		ImGui::DragFloat3("T", &studioTransform_.translate.x, 0.01f);
		studio_->SetTransform(studioTransform_);

		for (int i = 0; i < 13; ++i) {
			Vector3 translate = studio_->GetParts()[i].transform->translate;
			std::string label = "studioTranslate" + std::to_string(i);
			ImGui::DragFloat3(label.c_str(), &translate.x, 0.01f);
			studio_->GetParts()[i].transform->translate = translate;
		}
		ImGui::End();
#endif
	}
	break;
	case ResultPhase::ZoomOut:
		timer_[int(ResultPhase::ZoomOut)] += GameEngine::GetDeltaTime();
		timer_[int(ResultPhase::ZoomOut)] = min(timer_[int(ResultPhase::ZoomOut)], kTime[int(ResultPhase::ZoomOut)] + 0.5f);

		if (timer_[int(ResultPhase::ZoomOut)] >= kTime[int(ResultPhase::ZoomOut)] + 0.5f) {
			phase_ = ResultPhase::ResultSet;
		}
		break;

	case ResultPhase::ResultSet:
		timer_[int(ResultPhase::ResultSet)] += GameEngine::GetDeltaTime();
		timer_[int(ResultPhase::ResultSet)] = min(timer_[int(ResultPhase::ResultSet)], kTime[int(ResultPhase::ResultSet)]);

		if (timer_[int(ResultPhase::ResultSet)] >= kTime[int(ResultPhase::ResultSet)]) {
			phase_ = ResultPhase::DisplayResult;
		}
		break;

	case ResultPhase::DisplayResult:
		timer_[int(ResultPhase::DisplayResult)] += GameEngine::GetDeltaTime();
		timer_[int(ResultPhase::DisplayResult)] = min(timer_[int(ResultPhase::DisplayResult)], kTime[int(ResultPhase::DisplayResult)]);

		for (int i = 0; i < kScoreItemCount; ++i) {
			scoreTimer_[i] -= GameEngine::GetDeltaTime();

			// 大きさ変更
		/*	if (scoreTimer_[i] <= 0.2f) {
				SRT transform = scoreTransform_[i];
				float t = 1.0f - clamp(scoreTimer_[i] / 0.2f, 0.0f, 1.0f);
				transform.scale = t * scoreTransform_->scale;
				scores_[i]->SetTransform(transform);
			}

			if (scoreTimer_[9] <= 0.2f) {
				for (auto& obj : breakCount_.object) {
					SRT transform = obj->GetTransform();
					float t = 1.0f - clamp(scoreTimer_[9] / 0.2f, 0.0f, 1.0f);
					transform.scale = t * Vector3{ 0.4f,0.25f,0.25f };
					obj->SetTransform(transform);
				}
			}
			if (scoreTimer_[10] <= 0.2f) {
				for (auto& obj : breakRate_.object) {
					SRT transform = obj->GetTransform();
					float t = 1.0f - clamp(scoreTimer_[10] / 0.2f, 0.0f, 1.0f);
					transform.scale = t * Vector3{ 0.4f,0.25f,0.25f };
					obj->SetTransform(transform);
				}
			}
			if (scoreTimer_[11] <= 0.2f) {
				for (auto& obj : sectionTime_.object) {
					SRT transform = obj->GetTransform();
					float t = 1.0f - clamp(scoreTimer_[11] / 0.2f, 0.0f, 1.0f);
					transform.scale = t * Vector3{ 0.4f,0.25f,0.25f };
					obj->SetTransform(transform);
				}
			}*/

		}

#ifdef USE_IMGUI
		{
			static Vector2 wallOffset[15]{};
			constexpr float kWallYaw = 2.890f;

			Vector3 forward = {
				std::sin(kWallYaw),
				0.0f,
				std::cos(kWallYaw)
			};

			Vector3 right = {
				forward.z,
				0.0f,
				-forward.x
			};

			ImGui::Begin("EndCamera");

			for (int i = 0; i < 15; ++i) {
				std::string s = "Score.pos" + std::to_string(i);
				std::string s2 = "Score.rot" + std::to_string(i);
				std::string s3 = "Score.sca" + std::to_string(i);
				std::string s4 = "WallOffset" + std::to_string(i);

				SRT transform = scores_[i]->GetTransform();

				// 従来の位置編集
				ImGui::DragFloat3(
					s.c_str(),
					&transform.translate.x,
					0.1f
				);

				// 壁方向移動補助
				Vector2 oldOffset = wallOffset[i];

				if (ImGui::DragFloat2(
					s4.c_str(),
					&wallOffset[i].x,
					0.1f
				)) {
					Vector2 delta = {
						wallOffset[i].x - oldOffset.x,
						wallOffset[i].y - oldOffset.y
					};

					transform.translate +=
						right * delta.x +
						forward * delta.y;
				}

				// 回転
				if (ImGui::DragFloat3(
					s2.c_str(),
					&euler[i].x,
					0.01f
				)) {
					transform.rotate =
						MatrixToQuaternion(
							MakeAffineMatrix(
								{ 1,1,1 },
								euler[i],
								{ 0,0,0 }
							)
						);
				}

				// スケール
				ImGui::DragFloat3(
					s3.c_str(),
					&transform.scale.x,
					0.1f
				);

				scores_[i]->SetTransform(transform);
			}

			ImGui::End();
		}
#endif

#ifdef USE_IMGUI
		{
			static Vector2 wallOffsetRate[4]{};
			static Vector2 wallOffsetCount[6]{};
			static Vector2 wallOffsetTime[6]{};

			constexpr float kWallYaw = 2.890f;

			Vector3 forward = {
				std::sin(kWallYaw),
				0.0f,
				std::cos(kWallYaw)
			};

			Vector3 right = {
				forward.z,
				0.0f,
				-forward.x
			};

			ImGui::Begin("ResultNumbers");

			//----------------------------------
			// BreakRate
			//----------------------------------
			if (ImGui::TreeNode("BreakRate")) {

				for (int i = 0; i < breakRate_.digitCount; i++) {

					std::string pos = "Rate.pos" + std::to_string(i);
					std::string rot = "Rate.rot" + std::to_string(i);
					std::string sca = "Rate.sca" + std::to_string(i);
					std::string off = "Rate.offset" + std::to_string(i);

					SRT transform = breakRate_.object[i]->GetTransform();

					ImGui::DragFloat3(
						pos.c_str(),
						&transform.translate.x,
						0.1f
					);

					Vector2 oldOffset = wallOffsetRate[i];

					if (ImGui::DragFloat2(
						off.c_str(),
						&wallOffsetRate[i].x,
						0.1f
					)) {

						Vector2 delta = {
							wallOffsetRate[i].x - oldOffset.x,
							wallOffsetRate[i].y - oldOffset.y
						};

						transform.translate +=
							right * delta.x +
							forward * delta.y;
					}

					ImGui::DragFloat3(
						sca.c_str(),
						&transform.scale.x,
						0.1f
					);

					breakRate_.object[i]->SetTransform(transform);
				}

				ImGui::TreePop();
			}

			//----------------------------------
			// BreakCount
			//----------------------------------
			if (ImGui::TreeNode("BreakCount")) {

				for (int i = 0; i < breakCount_.digitCount; i++) {

					std::string pos = "Count.pos" + std::to_string(i);
					std::string sca = "Count.sca" + std::to_string(i);
					std::string off = "Count.offset" + std::to_string(i);

					SRT transform = breakCount_.object[i]->GetTransform();

					ImGui::DragFloat3(
						pos.c_str(),
						&transform.translate.x,
						0.1f
					);

					Vector2 oldOffset = wallOffsetCount[i];

					if (ImGui::DragFloat2(
						off.c_str(),
						&wallOffsetCount[i].x,
						0.1f
					)) {

						Vector2 delta = {
							wallOffsetCount[i].x - oldOffset.x,
							wallOffsetCount[i].y - oldOffset.y
						};

						transform.translate +=
							right * delta.x +
							forward * delta.y;
					}

					ImGui::DragFloat3(
						sca.c_str(),
						&transform.scale.x,
						0.1f
					);

					breakCount_.object[i]->SetTransform(transform);
				}

				ImGui::TreePop();
			}

			//----------------------------------
			// SectionTime
			//----------------------------------
			if (ImGui::TreeNode("SectionTime")) {

				for (int i = 0; i < sectionTime_.digitCount; i++) {

					std::string pos = "Time.pos" + std::to_string(i);
					std::string sca = "Time.sca" + std::to_string(i);
					std::string off = "Time.offset" + std::to_string(i);

					SRT transform = sectionTime_.object[i]->GetTransform();

					ImGui::DragFloat3(
						pos.c_str(),
						&transform.translate.x,
						0.1f
					);

					Vector2 oldOffset = wallOffsetTime[i];

					if (ImGui::DragFloat2(
						off.c_str(),
						&wallOffsetTime[i].x,
						0.1f
					)) {

						Vector2 delta = {
							wallOffsetTime[i].x - oldOffset.x,
							wallOffsetTime[i].y - oldOffset.y
						};

						transform.translate +=
							right * delta.x +
							forward * delta.y;
					}

					ImGui::DragFloat3(
						sca.c_str(),
						&transform.scale.x,
						0.1f
					);

					sectionTime_.object[i]->SetTransform(transform);
				}

				ImGui::TreePop();
			}

			ImGui::End();
		}
#endif


		if (timer_[int(ResultPhase::DisplayResult)] >= kTime[int(ResultPhase::DisplayResult)]) {
			Keyboard keyboard = input->GetKeyBoard();
			Pad pad = input->GetPad(0);
			if ((keyboard.release[DIK_SPACE] || pad.Button[PAD_BUTTON_B].release || pad.Button[PAD_BUTTON_A].release)) {
				phase_ = ResultPhase::ResultOut;
			}
		}
		break;

	case ResultPhase::ResultOut:
		timer_[int(ResultPhase::ResultOut)] += GameEngine::GetDeltaTime();
		timer_[int(ResultPhase::ResultOut)] = min(timer_[int(ResultPhase::ResultOut)], kTime[int(ResultPhase::ResultOut)]);

		endTimer_ -= GameEngine::GetDeltaTime();
		if (endTimer_ <= 0) {
			isEnd_ = true;
		}

		break;
	}
}

void Result::Draw() {
	studio_->Draw3D();

	for (int i = 0; i < kScoreItemCount; ++i) {
		scores_[i]->Draw3D();
	}

	for (int i = 0; i < breakCount_.digitCount; ++i) {
		breakCount_.object[i]->Draw3D();
	}
	for (int i = 0; i < breakRate_.digitCount; ++i) {
		breakRate_.object[i]->Draw3D();
	}
	for (int i = 0; i < sectionTime_.digitCount; ++i) {
		sectionTime_.object[i]->Draw3D();
	}
}

void Result::UpdateTimer(Course* course) {
	int lastTime_ = int(course->GetAllTime());

	int min = abs(lastTime_) / 60;
	int sec = abs(lastTime_) % 60;

	int num[4] = { min / 10, min % 10, sec / 10, sec % 10 };

	if (lastTime_ < 0) {

		for (int i = 0; i < sectionTime_.digitCount; ++i) {
			sectionTime_.object[i]->SetColor({ 1,0,0,1 });
		}
		sectionTime_.object[5]->SetColor({ 1,0,0,1 });
	} else {
		for (int i = 0; i < sectionTime_.digitCount; ++i) {
			sectionTime_.object[i]->SetColor({ 1,1,1,1 });
		}
		sectionTime_.object[5]->SetColor({ 1,1,1,1 });
	}

	// まず見えなくする
	for (int i = 0; i < sectionTime_.digitCount; ++i) {
		for (auto& part : sectionTime_.object[i]->GetParts()) {
			part.material->color.w = 0;
		}
	}
	// 該当パーツだけ表示状態にする
	if (lastTime_ < 0) {
		sectionTime_.object[0]->GetParts()[ConvertPartNumber(10)].material->color.w = 1;
	} // -
	sectionTime_.object[1]->GetParts()[ConvertPartNumber(num[0])].material->color.w = 1;
	sectionTime_.object[2]->GetParts()[ConvertPartNumber(num[1])].material->color.w = 1;
	sectionTime_.object[3]->GetParts()[ConvertPartNumber(11)].material->color.w = 1; // :
	sectionTime_.object[4]->GetParts()[ConvertPartNumber(num[2])].material->color.w = 1;
	sectionTime_.object[5]->GetParts()[ConvertPartNumber(num[3])].material->color.w = 1;
}

void Result::UpdateBreakRate(Course* course) {
	// リザルト中なら破壊率表示準備
	int rate = int(course->GetAllRate());
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

void Result::UpdateBreakAmount(Course* course) {
	int breakCount = course->GetAllBreakCount();
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
