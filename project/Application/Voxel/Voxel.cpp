#include "Voxel.h"
#include "Scene/BaseScene/GameScene/GameScene.h"
#include <Collision.h>
#include "Course/Course.h"

#include <numbers>
#include <cassert>

void Voxel::Initialize(Course* course, std::shared_ptr<Model> face, CSVData data, GameCamera* camera, std::shared_ptr<DirectionalLight> directionalLigth) {

	course_ = course;
	face_ = face;
	camera_ = camera;
	directionalLight_ = directionalLigth;

	for (index_ = 0; index_ < objects_.size(); index_++) {
		objects_[index_] = std::make_unique<Object>();
		objects_[index_]->Initialize(face_);
		objects_[index_]->SetShininess(0);
		std::vector<Parts> parts = objects_[index_]->GetParts();
		parts[0].UVtransform.scale.x = 0.5f;
		objects_[index_]->SetParts(parts[0], 0);
	}

	index_ = 0;

	std::string str;
	Chunk chunk;
	//サイズ変更
 	chunks_.resize(int(data.size.y));
	for (int y = 0; y < data.size.y; y++) {
		//サイズ変更
		chunks_[y].resize(int(data.size.z));
		for (int z = 0; z < data.size.z; z++) {
			//サイズ変更
			chunks_[y][z].resize(int(data.size.x));
			for (int x = 0; x < data.size.x; x++) {
				str = data.directoryPath + "/chunk_" + std::to_string(y) + "_" + std::to_string(z) + "_" + std::to_string(x) + ".csv";
				chunk = LoadChunk(str);
				chunks_[y][z][x] = chunk;
			}
		}
	}

	data_ = data;

}

void Voxel::Update() {


#ifdef USE_IMGUI
	ImGui::Begin("マップチップCSV書き込みツール");

	static int chunkIndexY = 0;
	static int chunkIndexZ = 0;
	static int chunkIndexX = 0;
	ImGui::SliderInt("チャンク番号_Y", &chunkIndexY, 0, int(data_.size.y - 1));
	ImGui::SliderInt("チャンク番号_Z", &chunkIndexZ, 0, int(data_.size.z - 1));
	ImGui::SliderInt("チャンク番号_X", &chunkIndexX, 0, int(data_.size.x - 1));
	static int yIndex = 0;
	ImGui::SliderInt("Y軸", &yIndex, 0, 15);
	ImGui::Text("マップエディター");
	// テクスチャID (DxLibやDirectXから取得したID)
	ImTextureID textureID = (ImTextureID)GameEngine::GetSRVManager()->GetGPUDescriptorHandle(face_->GetTextureIndex(0)).ptr;
	ImVec2 uv00 = ImVec2(0.0f, 0.0f); // タイルのUV開始位置
	ImVec2 uv11 = ImVec2(0.0f, 1.0f); // タイルのUV終了位置 (例: 4x4のタイルセットの左上)

	for (int z = 0; z < 16; z++) {
		for (int x = 0; x < 16; x++) {
			std::string id = "tile##" + std::to_string(z) + "_" + std::to_string(x);
			//TILEの見た目変更
			if (chunks_[chunkIndexY][chunkIndexZ][chunkIndexX].mapChip[yIndex][z][x] != TILE_None) {
				uv00.x = float(chunks_[chunkIndexY][chunkIndexZ][chunkIndexX].mapChip[yIndex][z][x] - 1) / int(VOXEL_TILE_END - 1);
			} else {
				uv00.x = 0.0f;
			}
			uv11.x = float(chunks_[chunkIndexY][chunkIndexZ][chunkIndexX].mapChip[yIndex][z][x]) / int(VOXEL_TILE_END - 1);

				// クリック可能なマップチップボタン
				if (ImGui::ImageButton(id.c_str(), textureID, ImVec2(16, 16), uv00, uv11)) {
					// タイルが選択された時の処理
					chunks_[chunkIndexY][chunkIndexZ][chunkIndexX].mapChip[yIndex][z][x]++;
					if (chunks_[chunkIndexY][chunkIndexZ][chunkIndexX].mapChip[yIndex][z][x] >= VOXEL_TILE_END) {
						chunks_[chunkIndexY][chunkIndexZ][chunkIndexX].mapChip[yIndex][z][x] = TILE_None;
					}
				}

			// グリッドを並べる
			if (x < 15) ImGui::SameLine();
		}
	}

	std::string str;
	if (ImGui::Button("SAVE")) {
		for (int y = 0; y < chunks_.size(); y++) {
			for (int z = 0; z < chunks_[y].size(); z++) {
				for (int x = 0; x < chunks_[y][z].size(); x++) {
					str = data_.directoryPath + "/chunk_" + std::to_string(y) + "_" + std::to_string(z) + "_" + std::to_string(x) + ".csv";
					WriteChunk(chunks_[y][z][x], str);
				}
			}
		}
	}

	SRT cameraTransform = camera_->GetTransform();
	Vector3 cameraPosition;
	cameraPosition.x = float(int((cameraTransform.translate.x / (scale * 2)) + 8 * data_.size.x) % 16);
	cameraPosition.y = -float(int(cameraTransform.translate.y / (scale * 2)) % 16);
	cameraPosition.z = float(int((cameraTransform.translate.z / (scale * 2)) + 8 * data_.size.z) % 16);
	Vector3 cameraChunkNumber;
	cameraChunkNumber.x = float(int((cameraTransform.translate.x / (scale * 2)) + 8 * data_.size.x) / 16);
	cameraChunkNumber.y = -float(int(cameraTransform.translate.y / (scale * 2)) / 16);
	cameraChunkNumber.z = float(int((cameraTransform.translate.z / (scale * 2)) + 8 * data_.size.z) / 16);

	ImGui::DragFloat3("cameraPosition", &cameraPosition.x);
	ImGui::DragFloat3("cameraChunkNumber", &cameraChunkNumber.x);

	ImGui::End();

#endif // USE_IMGUI

}

void Voxel::Draw() {

	index_ = 0;

	SRT cameraTransform = camera_->GetTransform();
	Vector3 cameraPosition;
	cameraPosition.x = float(int((cameraTransform.translate.x / (scale * 2)) + 8 * data_.size.x) % 16);
	cameraPosition.y = -float(int(cameraTransform.translate.y / (scale * 2)) % 16);
	cameraPosition.z = float(int((cameraTransform.translate.z / (scale * 2)) + 8 * data_.size.z) % 16);
	Vector3 cameraChunkNumber;
	cameraChunkNumber.x = float(int((cameraTransform.translate.x / (scale * 2)) + 8 * data_.size.x) / 16);
	cameraChunkNumber.y = -float(int(cameraTransform.translate.y / (scale * 2)) / 16);
	cameraChunkNumber.z = float(int((cameraTransform.translate.z / (scale * 2)) + 8 * data_.size.z) / 16);

	for (int y = 0; y < chunks_.size(); y++) {
		for (int z = 0; z < chunks_[y].size(); z++) {
			for (int x = 0; x < chunks_[y][z].size(); x++) {
				DrawChunk(y, z, x, cameraPosition, cameraChunkNumber);
			}
		}
	}

	if (!drawOdjects_.empty()) {
		GameEngine::DrawInstancingObject_3D(drawOdjects_, directionalLight_, nullptr, nullptr);
		drawOdjects_.clear();
	}
}

void Voxel::DrawAll() {

	index_ = 0;

	SRT cameraTransform = camera_->GetTransform();
	Vector3 cameraChunkNumber;
	cameraChunkNumber.x = float(int((cameraTransform.translate.x / (scale * 2)) + 8 * data_.size.x) / 16);
	cameraChunkNumber.y = -float(int(cameraTransform.translate.y / (scale * 2)) / 16);
	cameraChunkNumber.z = float(int((cameraTransform.translate.z / (scale * 2)) + 8 * data_.size.z) / 16);

	for (int y = 0; y < chunks_.size(); y++) {
		for (int z = 0; z < chunks_[y].size(); z++) {
			for (int x = 0; x < chunks_[y][z].size(); x++) {
				DrawChunkAll(y, z, x, cameraChunkNumber);
			}
		}
	}
	

	if (!drawOdjects_.empty()) {
		GameEngine::DrawInstancingObject_3D(drawOdjects_, directionalLight_, nullptr, nullptr);
		drawOdjects_.clear();
	}
}

Object* Voxel::AddFace(int chunkY, int chunkZ, int chunkX, int y, int z, int x, int8_t number, Quaternion rotate) {

	if (index_ < objects_.size()) {
		SRT transform;
		transform.scale = { scale,scale,scale };
		transform.rotate = rotate;
		float voxelSize = scale * 2.0f;
		float chunkSize = voxelSize * 16.0f;

		transform.translate = {
			(chunkX * chunkSize - chunkSize / 2 * data_.size.x) + x * voxelSize + voxelSize * 0.5f,
			-(chunkY * chunkSize + y * voxelSize + voxelSize * 0.5f),
			(chunkZ * chunkSize - chunkSize / 2 * data_.size.z) + z * voxelSize + voxelSize * 0.5f
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
	for (int chunkY = 0; chunkY < chunks_.size(); chunkY++) {
		for (int chunkZ = 0; chunkZ < chunks_[chunkY].size(); chunkZ++) {
			for (int chunkX = 0; chunkX < chunks_[chunkY][chunkZ].size(); chunkX++) {
				AABB chunkAABB;
				float voxelSize = scale * 2.0f;
				float chunkSize = voxelSize * 16.0f;

				Vector3 chunkOrigin = {
					chunkX * chunkSize - chunkSize / 2 * data_.size.x,
					-((chunkY + 1) * chunkSize),
					chunkZ * chunkSize - chunkSize / 2 * data_.size.z
				};

				chunkAABB.min = chunkOrigin;
				chunkAABB.max = chunkOrigin + Vector3{ chunkSize, chunkSize, chunkSize };

				if (IsCollision(chunkAABB, sphere)) {
					for (int y = 0; y < 16; y++) {
						if (sphere.center.y + sphere.radius >= chunkOrigin.y + voxelSize * (16 - y) &&
							sphere.center.y - sphere.radius <= chunkOrigin.y + (voxelSize + 1) * (16 - y)) {
							for (int z = 0; z < 16; z++) {
								if (sphere.center.z + sphere.radius >= chunkOrigin.z + voxelSize * z &&
									sphere.center.z - sphere.radius <= chunkOrigin.z + (voxelSize + 1) * z) {
									for (int x = 0; x < 16; x++) {
										if (chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x] == 0)continue;

										AABB voxelAABB;
										voxelAABB.min = {
											chunkOrigin.x + voxelSize * x,
											chunkOrigin.y + voxelSize * (16 - y),
											chunkOrigin.z + voxelSize * z
										};
										voxelAABB.max = voxelAABB.min + Vector3{ voxelSize, voxelSize, voxelSize };

										if (IsCollision(voxelAABB, sphere)) {

											//PrimitiveManager::GetInstance()->AddAABB(voxelAABB);

											SRT transform;
											transform.scale = { scale,scale,scale };
											transform.rotate = IdentityQuaternion();
											transform.translate = {
												voxelAABB.min.x + voxelSize * 0.5f,
												voxelAABB.min.y - voxelSize * 0.5f,
												voxelAABB.min.z + voxelSize * 0.5f
											};

											switch (chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x])
											{
											case 1:
												//Boxにする
												course_->AddBox(transform, {}, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], 0.5f, GameEngine::randomInt(6, 8));
												break;
											case 2:
												//Boxにする
												course_->AddBox(transform, {}, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], 0.05f, 13);
												break;
											default:
												break;
											}

											chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x] = 0;
										}
									}
								}
							}
						}
					}
				}

			}
		}
	}


}

void Voxel::DrawChunk(int chunkY, int chunkZ, int chunkX, Vector3 cameraTranslate, Vector3 cameraChunkNumber) {
	for (int y = 0; y < 16; y++) {
		if ((cameraTranslate.y < y && cameraChunkNumber.y == chunkY) || (cameraChunkNumber.y + 4 >= chunkY && cameraChunkNumber.y < chunkY)) {
			for (int z = 0; z < 16; z++) {
				for (int x = 0; x < 16; x++) {
					if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x])continue;
					//上面
					if (y <= 0) {
						//上が空白
						if (chunkY <= 0) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
							}
						} else {
							if (!chunks_[chunkY - 1][chunkZ][chunkX].mapChip[15][z][x]) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
									index_++;
								}
							}
						}
					} else {
						if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y - 1][z][x]) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
							}
						}
					}
					//前面
					if ((cameraTranslate.z > z && int(cameraChunkNumber.z) == chunkZ) || int(cameraChunkNumber.z) > chunkZ) {
						if (z >= 15) {
							//前が空白
							if (chunkZ >= chunks_[chunkY].size() - 1) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
									index_++;
								}
							} else {
								if (!chunks_[chunkY][chunkZ + 1][chunkX].mapChip[y][0][x]) {
									if (index_ < objects_.size()) {
										drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
										index_++;
									}
								}
							}
						} else {
							if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z + 1][x]) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
									index_++;
								}
							}
						}
					}
					if ((cameraTranslate.z < z && int(cameraChunkNumber.z) == chunkZ) || int(cameraChunkNumber.z) < chunkZ) {
						//後面
						if (z <= 0) {
							//前が空白
							if (chunkZ <= 0) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], IdentityQuaternion()));
									index_++;
								}
							} else {
								if (!chunks_[chunkY][chunkZ - 1][chunkX].mapChip[y][15][x]) {
									if (index_ < objects_.size()) {
										drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], IdentityQuaternion()));
										index_++;
									}
								}
							}
						} else {
							if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z - 1][x]) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], IdentityQuaternion()));
									index_++;
								}
							}
						}
					}
					//右面
					if ((cameraTranslate.x > x && int(cameraChunkNumber.x) == chunkX) || int(cameraChunkNumber.x) > chunkX) {
						if (x >= 15) {
							//前が空白
							if (chunkX >= chunks_[chunkY][chunkZ].size() - 1) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
									index_++;
								}
							} else {
								if (!chunks_[chunkY][chunkZ][chunkX + 1].mapChip[y][z][0]) {
									if (index_ < objects_.size()) {
										drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
										index_++;
									}
								}
							}
						} else {
							if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x + 1]) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
									index_++;
								}
							}
						}
					}
					//左面
					if ((cameraTranslate.x < x && int(cameraChunkNumber.x) == chunkX) || int(cameraChunkNumber.x) < chunkX) {
						if (x <= 0) {
							//前が空白
							if (chunkX <= 0) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, -std::numbers::pi_v<float> / 2)));
									index_++;
								}
							} else {
								if (!chunks_[chunkY][chunkZ][chunkX - 1].mapChip[y][15][x]) {
									if (index_ < objects_.size()) {
										drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, -std::numbers::pi_v<float> / 2)));
										index_++;
									}
								}
							}
						} else {
							if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x - 1]) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, -std::numbers::pi_v<float> / 2)));
									index_++;
								}
							}
						}
					}
				}
			}
		}
	}
}

void Voxel::DrawChunkAll(int chunkY, int chunkZ, int chunkX, Vector3 cameraChunkNumber) {
	for (int y = 0; y < 16; y++) {
		if ((cameraChunkNumber.y + 1 >= chunkY && cameraChunkNumber.y - 1 <= chunkY)) {
			for (int z = 0; z < 16; z++) {
				for (int x = 0; x < 16; x++) {
					if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x])continue;
					//上面
					if (y <= 0) {
						//上が空白
						if (chunkY <= 0) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
							}
						} else {
							if (!chunks_[chunkY - 1][chunkZ][chunkX].mapChip[15][z][x]) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
									index_++;
								}
							}
						}
					} else {
						if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y - 1][z][x]) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
							}
						}
					}
					//前面
					if (z >= 15) {
						//前が空白
						if (chunkZ >= chunks_[chunkY].size() - 1) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
								index_++;
							}
						} else {
							if (!chunks_[chunkY][chunkZ + 1][chunkX].mapChip[y][0][x]) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
									index_++;
								}
							}
						}
					} else {
						if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z + 1][x]) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
								index_++;
							}
						}
					}
					//後面
					if (z <= 0) {
						//前が空白
						if (chunkZ <= 0) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], IdentityQuaternion()));
								index_++;
							}
						} else {
							if (!chunks_[chunkY][chunkZ - 1][chunkX].mapChip[y][15][x]) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], IdentityQuaternion()));
									index_++;
								}
							}
						}
					} else {
						if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z - 1][x]) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], IdentityQuaternion()));
								index_++;
							}
						}
					}
					//右面
					if (x >= 15) {
						//前が空白
						if (chunkX >= chunks_[chunkY][chunkZ].size() - 1) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
								index_++;
							}
						} else {
							if (!chunks_[chunkY][chunkZ][chunkX + 1].mapChip[y][z][0]) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
									index_++;
								}
							}
						}
					} else {
						if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x + 1]) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
								index_++;
							}
						}
					}
					//左面
					if (x <= 0) {
						//前が空白
						if (chunkX <= 0) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
							}
						} else {
							if (!chunks_[chunkY][chunkZ][chunkX - 1].mapChip[y][15][x]) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, -std::numbers::pi_v<float> / 2)));
									index_++;
								}
							}
						}
					} else {
						if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x - 1]) {
							if (index_ < objects_.size()) {
								drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
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

	Chunk chunk{};

	//開けないなら空チャンクを返す
	if (!file.is_open()) {
		return chunk;
	}

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