#include "Voxel.h"
#include "Scene/BaseScene/GameScene/GameScene.h"
#include <Collision.h>
#include "Course/Course.h"

#include <numbers>
#include <cassert>

void Voxel::Initialize(Course* course, std::shared_ptr<Model> face, std::shared_ptr<DirectionalLight> directionalLigth) {

	course_ = course;
	face_ = face;
	directionalLigth_ = directionalLigth;

	for (index_ = 0; index_ < objects_.size(); index_++) {
		objects_[index_] = std::make_unique<Object>();
		objects_[index_]->Initialize(face_);
		objects_[index_]->SetShininess(0);
		std::vector<Parts> parts = objects_[index_]->GetParts();
		parts[0].UVtransform.scale.x = 0.5f;
		objects_[index_]->SetParts(parts[0], 0);
	}

	index_ = 0;

	Chunk chunk;
	chunk = LoadChunk("resources/CSV/chunk_01.csv");
	chunks_.push_back(chunk);
	chunk = LoadChunk("resources/CSV/chunk_02.csv");
	chunks_.push_back(chunk);
	chunk = LoadChunk("resources/CSV/chunk_03.csv");
	chunks_.push_back(chunk);
	chunk = LoadChunk("resources/CSV/chunk_04.csv");
	chunks_.push_back(chunk);
}

void Voxel::Update() {


#ifdef USE_IMGUI
	ImGui::Begin("マップチップCSV書き込みツール");

	static int chunkIndex = 0;
	ImGui::SliderInt("チャンク番号", &chunkIndex, 0, 3);
	static int yIndex = 0;
	ImGui::SliderInt("Y軸", &yIndex, 0, 15);
	ImGui::Text("マップエディター");
	// テクスチャID (DxLibやDirectXから取得したID)
	ImTextureID textureID = (ImTextureID)GameEngine::GetSRVManager()->GetGPUDescriptorHandle(face_->GetTextureIndex(0)).ptr;
	ImVec2 uv00 = ImVec2(0.0f, 0.0f); // タイルのUV開始位置
	ImVec2 uv11 = ImVec2(0.5f, 1.0f); // タイルのUV終了位置 (例: 4x4のタイルセットの左上)
	ImVec2 uv10 = ImVec2(0.5f, 0.0f); // タイルのUV開始位置
	ImVec2 uv21 = ImVec2(1.0f, 1.0f); // タイルのUV終了位置 (例: 4x4のタイルセットの左上)

	for (int z = 0; z < 16; z++) {
		for (int x = 0; x < 16; x++) {
			std::string id = "tile##" + std::to_string(z) + "_" + std::to_string(x);

			if (chunks_[chunkIndex].mapChip[yIndex][z][x] == 1) {
				// クリック可能なマップチップボタン
				if (ImGui::ImageButton(id.c_str(), textureID, ImVec2(16, 16), uv00, uv11)) {
					// タイルが選択された時の処理
					chunks_[chunkIndex].mapChip[yIndex][z][x] = 2;
				}
			} else if(chunks_[chunkIndex].mapChip[yIndex][z][x] == 2) {
				// クリック可能なマップチップボタン
				if (ImGui::ImageButton(id.c_str(), textureID, ImVec2(16, 16), uv10, uv21)) {
					// タイルが選択された時の処理
					chunks_[chunkIndex].mapChip[yIndex][z][x] = 0;
				}
			} else {
				// クリック可能なマップチップボタン
				if (ImGui::ImageButton(id.c_str(), textureID, ImVec2(16, 16), uv00, uv00)) {
					// タイルが選択された時の処理
					chunks_[chunkIndex].mapChip[yIndex][z][x] = 1;
				}
			}

			// グリッドを並べる
			if (x < 15) ImGui::SameLine();
		}
	}

	if (ImGui::Button("SAVE")) {
		WriteChunk(chunks_[0], "resources/CSV/chunk_01.csv");
		WriteChunk(chunks_[1], "resources/CSV/chunk_02.csv");
		WriteChunk(chunks_[2], "resources/CSV/chunk_03.csv");
		WriteChunk(chunks_[3], "resources/CSV/chunk_04.csv");
	}

	ImGui::End();

#endif // USE_IMGUI

}

void Voxel::Draw() {
	std::list<Object*> objects;
	for (int i = 0; i < 4; i++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				for (int x = 0; x < 16; x++) {
					if (!chunks_[i].mapChip[y][z][x])continue;
					//上面
					if (y == 0) {
						//上が空白
						if (i == 0) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, chunks_[i].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
							}
						} else {
							if (!chunks_[i - 1].mapChip[15][z][x]) {
								if (index_ < objects_.size()) {
									objects.push_back(AddFace(i, y, z, x, chunks_[i].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
									index_++;
								}
							}
						}
					} else {
						if (!chunks_[i].mapChip[y - 1][z][x]) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, chunks_[i].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
							}
						}
					}
					//前面
					if (z != 0) {
						if (!chunks_[i].mapChip[y][z - 1][x]) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, chunks_[i].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
								index_++;
							}
						}
					}
					//後面
					if (z != 15) {
						if (!chunks_[i].mapChip[y][z + 1][x]) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, chunks_[i].mapChip[y][z][x], IdentityQuaternion()));
								index_++;
							}
						}
					}
					//右面
					if (x != 15) {
						if (!chunks_[i].mapChip[y][z][x + 1]) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, chunks_[i].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
								index_++;
							}
						}
					}
					//左面
					if (x != 0) {
						if (!chunks_[i].mapChip[y][z][x - 1]) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, chunks_[i].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> + std::numbers::pi_v<float> / 2)));
								index_++;
							}
						}
					}
				}
			}
		}
	}

	GameEngine::DrawInstancingObject_3D(objects, directionalLigth_, nullptr, nullptr);

	index_ = 0;
}

Object* Voxel::AddFace(int i, int y, int z, int x, int8_t number, Quaternion rotate) {

	if (index_ < objects_.size()) {
		SRT transform;
		transform.scale = { scale,scale,scale };
		transform.rotate = rotate;
		transform.translate = {
			(x * scale * 2 - 16 * scale - scale / 2),
			-(y * scale * 2) - scale / 2 - i * 32 * scale ,
			32 * scale - (z * scale * 2 + 16 * scale) - scale / 2,
		};

		objects_[index_]->SetTransform(transform);
		if (number > 0) {
			std::vector<Parts> parts = objects_[index_]->GetParts();
			parts[0].UVtransform.translate.x = 0.5f * (number - 1);
			objects_[index_]->SetParts(parts[0], 0);
		}

		return objects_[index_].get();
	}

	assert(false);

	return nullptr;
}

void Voxel::Collision(Sphere sphere) {
	for (int i = 0; i < 4; i++) {
		AABB chunkAABB;
		chunkAABB.min = {
			-16 * scale - scale - scale / 2 ,
			-(i + 1) * 32 * scale + scale / 2,
			-16 * scale + scale / 2,
		};
		chunkAABB.max = chunkAABB.min + Vector3{32 * scale,32 * scale ,32 * scale };

		if (IsCollision(chunkAABB, sphere)) {
			for (int y = 0; y < 16; y++) {
				if (sphere.center.y + sphere.radius >= -(y * scale * 2) - scale - i * 32 * scale - scale &&
					sphere.center.y - sphere.radius <= -(y * scale * 2) - scale - i * 32 * scale) {
					for (int z = 0; z < 16; z++) {
						if (sphere.center.z + sphere.radius >= 32 * scale - (z * scale * 2 + 16 * scale) - scale &&
							sphere.center.z - sphere.radius <= 32 * scale - (z * scale * 2 + 16 * scale)) {
							for (int x = 0; x < 16; x++) {
								if (chunks_[i].mapChip[y][z][x] == 0)continue;

								AABB voxelAABB;
								voxelAABB.min = {
									(x * scale * 2 - 16 * scale - scale - scale / 2) - -scale / 2,
									-(y * scale * 2) - scale - i * 32 * scale - scale ,
									32 * scale - (z * scale * 2 + 16 * scale) - scale,
								};
								voxelAABB.max = voxelAABB.min + Vector3{ scale * 2,scale * 2 ,scale * 2 };

								if (IsCollision(voxelAABB, sphere)) {
									SRT transform;
									transform.scale = { scale,scale,scale };
									transform.rotate = IdentityQuaternion();
									transform.translate = {
										(x * scale * 2 - 16 * scale - scale / 2),
										-(y * scale * 2) - scale / 2 - i * 32 * scale ,
										32 * scale - (z * scale * 2 + 16 * scale) - scale / 2,
									};

									switch (chunks_[i].mapChip[y][z][x])
									{
									case 1:
										//Boxにする
										course_->AddBox(transform, {}, chunks_[i].mapChip[y][z][x], 0.5f, scale / 2, 6);
										break;
									case 2:
										//Boxにする
										course_->AddBox(transform, {}, chunks_[i].mapChip[y][z][x], 0.001f, scale / 2, 600);
										break;
									default:
										break;
									}

									chunks_[i].mapChip[y][z][x] = 0;
								}
							}
						}
					}
				}
			}
		}
	}


}

Chunk Voxel::LoadChunk(std::string loadFile) {
	std::ifstream file(loadFile);
	assert(file.is_open());

	Chunk chunk{};

	std::string line;

	int y = 0; // 高さ（層）
	int z = 0; // 奥行き

	while (std::getline(file, line)) {

		// 空行 → 次の層へ
		if (line.empty()) {
			y++;
			z = 0;
			continue;
		}

		// コメント
		if (line.rfind("//", 0) == 0) {
			continue;
		}

		std::stringstream ss(line);
		std::string cell;

		int x = 0; // 横

		// カンマ区切りで読む
		while (std::getline(ss, cell, ',')) {
			chunk.mapChip[y][z][x] = std::stoi(cell);
			x++;
		}

		z++;
	}

	file.close();
	return chunk;
}

void Voxel::WriteChunk(const Chunk& chunk, const std::string& loadFile) {
	std::ofstream file(loadFile);
	assert(file.is_open());

	for (int y = 0; y < 16; y++) {
		for (int z = 0; z < 16; z++) {
			for (int x = 0; x < 16; x++) {
				file << int(chunk.mapChip[y][z][x]) << ",";
			}
			file << "\n";
		}
		file << "\n";
	}

	file.close();
}