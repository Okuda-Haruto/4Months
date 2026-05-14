#include "Voxel.h"
#include "Scene/BaseScene/GameScene/GameScene.h"
#include <Collision.h>
#include "Course/Course.h"

#include <numbers>
#include <cassert>

void Voxel::Initialize(Course* course, std::shared_ptr<Model> face, CSVData csvData, GameCamera* camera, std::shared_ptr<DirectionalLight> directionalLigth) {

	course_ = course;
	face_ = face;
	camera_ = camera;
	directionalLight_ = directionalLigth;

	for (index_ = 0; index_ < objects_.size(); index_++) {
		objects_[index_] = std::make_unique<Object>();
		objects_[index_]->Initialize(face_);
		objects_[index_]->SetShininess(0);
		std::vector<Parts> parts = objects_[index_]->GetParts();
		parts[0].UVtransform.scale.x = 1.0f / (VOXEL_TILE_END - 1);
		objects_[index_]->SetParts(parts[0], 0);
	}

	index_ = 0;

	std::string str;
	Chunk chunk;
	//サイズ変更
	chunks_.resize(int(csvData.size.y));
	for (int y = 0; y < csvData.size.y; y++) {
		//サイズ変更
		chunks_[y].resize(int(csvData.size.z));
		for (int z = 0; z < csvData.size.z; z++) {
			//サイズ変更
			chunks_[y][z].resize(int(csvData.size.x));
			for (int x = 0; x < csvData.size.x; x++) {
				str = csvData.chunkDataDirectoryPath + "/chunk_" + std::to_string(y) + "_" + std::to_string(z) + "_" + std::to_string(x) + ".csv";
				chunk = LoadChunk(str);
				chunks_[y][z][x] = chunk;
			}
		}
	}

	csvData.voxelStatus = LoadVoxel(csvData.voxelDataFilePath);

	csvData_ = csvData;

}

void Voxel::Update() {

}

void Voxel::Draw() {

	index_ = 0;

	SRT cameraTransform = camera_->GetTransform();
	Vector3 cameraPosition;
	cameraPosition.x = float(int((cameraTransform.translate.x / (scale * 2)) + 8 * csvData_.size.x) % 16);
	cameraPosition.y = -float(int(cameraTransform.translate.y / (scale * 2)) % 16);
	cameraPosition.z = float(int((cameraTransform.translate.z / (scale * 2)) + 8 * csvData_.size.z) % 16);
	Vector3 cameraChunkNumber;
	cameraChunkNumber.x = float(int((cameraTransform.translate.x / (scale * 2)) + 8 * csvData_.size.x) / 16);
	cameraChunkNumber.y = -float(int(cameraTransform.translate.y / (scale * 2)) / 16);
	cameraChunkNumber.z = float(int((cameraTransform.translate.z / (scale * 2)) + 8 * csvData_.size.z) / 16);

	for (int y = 0; y < chunks_.size(); y++) {
		for (int z = 0; z < chunks_[y].size(); z++) {
			for (int x = 0; x < chunks_[y][z].size(); x++) {
				DrawChunk(y, z, x, cameraPosition, cameraChunkNumber);
			}
		}
	}

	if (!drawOdjects_.empty()) {
		GameEngine::DrawInstancingVoxel_3D(drawOdjects_, TextureManager::GetInstance()->GetSrvIndex("BackGround"), directionalLight_, nullptr, nullptr);
		drawOdjects_.clear();
	}
}

void Voxel::Draw(AABB drawRange) {
	index_ = 0;

	SRT cameraTransform = camera_->GetTransform();
	Vector3 cameraChunkNumber;
	cameraChunkNumber.x = float(int((cameraTransform.translate.x / (scale * 2)) + 8 * csvData_.size.x) / 16);
	cameraChunkNumber.y = -float(int(cameraTransform.translate.y / (scale * 2)) / 16);
	cameraChunkNumber.z = float(int((cameraTransform.translate.z / (scale * 2)) + 8 * csvData_.size.z) / 16);

	for (int y = int(drawRange.min.y); y < int(drawRange.max.y); y++) {
		for (int z = int(drawRange.min.z); z < int(drawRange.max.z); z++) {
			for (int x = int(drawRange.min.x); x < int(drawRange.max.x); x++) {
				DrawChunkAll(y, z, x, cameraChunkNumber);
			}
		}
	}


	if (!drawOdjects_.empty()) {
		GameEngine::DrawInstancingVoxel_3D(drawOdjects_, TextureManager::GetInstance()->GetSrvIndex("BackGround"), directionalLight_, nullptr, nullptr);
		drawOdjects_.clear();
	}
}

void Voxel::DrawAll() {

	index_ = 0;

	SRT cameraTransform = camera_->GetTransform();
	Vector3 cameraChunkNumber;
	cameraChunkNumber.x = float(int((cameraTransform.translate.x / (scale * 2)) + 8 * csvData_.size.x) / 16);
	cameraChunkNumber.y = -float(int(cameraTransform.translate.y / (scale * 2)) / 16);
	cameraChunkNumber.z = float(int((cameraTransform.translate.z / (scale * 2)) + 8 * csvData_.size.z) / 16);

	for (int y = 0; y < chunks_.size(); y++) {
		//範囲を絞る
		if ((cameraChunkNumber.y + 1 >= y && cameraChunkNumber.y - 1 <= y)) {
			for (int z = 0; z < chunks_[y].size(); z++) {
				for (int x = 0; x < chunks_[y][z].size(); x++) {
					DrawChunkAll(y, z, x, cameraChunkNumber);
				}
			}
		}
	}


	if (!drawOdjects_.empty()) {
		GameEngine::DrawInstancingVoxel_3D(drawOdjects_, TextureManager::GetInstance()->GetSrvIndex("BackGround"), directionalLight_, nullptr, nullptr);
		drawOdjects_.clear();
	}
}

void Voxel::DrawUp() {
	// カメラより上だけ描画
	index_ = 0;

	SRT cameraTransform = camera_->GetTransform();

	Vector3 cameraChunkNumber;
	cameraChunkNumber.x = float(int((cameraTransform.translate.x / (scale * 2)) + 8 * csvData_.size.x) / 16);
	cameraChunkNumber.y = -float(int(cameraTransform.translate.y / (scale * 2)) / 16);
	cameraChunkNumber.z = float(int((cameraTransform.translate.z / (scale * 2)) + 8 * csvData_.size.z) / 16);

	int maxUpRange = 5; // カメラから上に何チャンクまで描画するか
	for (int y = 0; y < chunks_.size(); y++) {
		if (y > cameraChunkNumber.y ||
			y < cameraChunkNumber.y - maxUpRange) {
			continue;
		}

		for (int z = 0; z < chunks_[y].size(); z++) {
			for (int x = 0; x < chunks_[y][z].size(); x++) {
				DrawChunkAllUp(y, z, x, cameraChunkNumber,maxUpRange);
			}
		}
	}

	if (!drawOdjects_.empty()) {
		GameEngine::DrawInstancingVoxel_3D(drawOdjects_, TextureManager::GetInstance()->GetSrvIndex("BackGround"), directionalLight_, nullptr, nullptr);
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
			(chunkX * chunkSize - chunkSize / 2 * csvData_.size.x) + x * voxelSize + voxelSize * 0.5f,
			-(chunkY * chunkSize + y * voxelSize + voxelSize * 0.5f),
			(chunkZ * chunkSize - chunkSize / 2 * csvData_.size.z) + z * voxelSize + voxelSize * 0.5f
		};

		objects_[index_]->SetTransform(transform);
		if (number > 0) {
			std::vector<Parts> parts = objects_[index_]->GetParts();
			parts[0].UVtransform.translate.x = (1.0f / (VOXEL_TILE_END - 1)) * (number - 1);
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
					chunkX * chunkSize - chunkSize / 2 * csvData_.size.x,
					-((chunkY + 1) * chunkSize),
					chunkZ * chunkSize - chunkSize / 2 * csvData_.size.z
				};

				chunkAABB.min = chunkOrigin;
				chunkAABB.max = chunkOrigin + Vector3{ chunkSize, chunkSize, chunkSize };

				if (IsCollision(chunkAABB, sphere)) {
					for (int y = 0; y < 16; y++) {
						if (sphere.center.y + sphere.radius >= chunkOrigin.y + voxelSize * (16 - y) &&
							sphere.center.y - sphere.radius <= chunkOrigin.y + voxelSize * (16 - y + 1)) {
							for (int z = 0; z < 16; z++) {
								if (sphere.center.z + sphere.radius >= chunkOrigin.z + voxelSize * z &&
									sphere.center.z - sphere.radius <= chunkOrigin.z + voxelSize * (z + 1)){
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

											//Boxにする
											course_->AddBox(
												transform,
												{},
												chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
												csvData_.voxelStatus[chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x]].vacuumSensitivity,
												GameEngine::randomFloat(
													csvData_.voxelStatus[chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x]].MaxHP - csvData_.voxelStatus[chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x]].randomRate,
													csvData_.voxelStatus[chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x]].MaxHP + csvData_.voxelStatus[chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x]].randomRate
												)
											);

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

CollisionVoxel Voxel::GetCollisionVoxel(Ray ray, AABB chunkRate) {
	CollisionVoxel collisionVoxel{};

	for (int chunkY = int(chunkRate.min.y); chunkY < int(chunkRate.max.y); chunkY++) {
		for (int chunkZ = int(chunkRate.min.z); chunkZ < int(chunkRate.max.z); chunkZ++) {
			for (int chunkX = int(chunkRate.min.x); chunkX < int(chunkRate.max.x); chunkX++) {
				AABB chunkAABB;
				float voxelSize = scale * 2.0f;
				float chunkSize = voxelSize * 16.0f;

				Vector3 chunkOrigin = {
					chunkX * chunkSize - chunkSize / 2 * csvData_.size.x,
					-((chunkY + 1) * chunkSize),
					chunkZ * chunkSize - chunkSize / 2 * csvData_.size.z
				};

				chunkAABB.min = chunkOrigin;
				chunkAABB.max = chunkOrigin + Vector3{ chunkSize, chunkSize, chunkSize };

				if (IsCollision(chunkAABB, ray)) {
					for (int y = 0; y < 16; y++) {
						for (int z = 0; z < 16; z++) {
							for (int x = 0; x < 16; x++) {
								if (chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x] == 0)continue;

								AABB voxelAABB;
								voxelAABB.min = {
									chunkOrigin.x + voxelSize * x,
									chunkOrigin.y + voxelSize * (15 - y),
									chunkOrigin.z + voxelSize * z
								};
								voxelAABB.max = voxelAABB.min + Vector3{ voxelSize, voxelSize, voxelSize };

								if (IsCollision(voxelAABB, ray)) {

									Vector3 translate = {
										voxelAABB.min.x + voxelSize * 0.5f,
										voxelAABB.min.y - voxelSize * 0.5f,
										voxelAABB.min.z + voxelSize * 0.5f
									};

									if (Length(collisionVoxel.translate - ray.origin) > Length(translate - ray.origin) || Length(collisionVoxel.normal) < 0.5f) {
										collisionVoxel.normal = GetHitNormal(voxelAABB, ray);
										collisionVoxel.chunkPos = {
											float(chunkX),
											float(chunkY),
											float(chunkZ)
										};
										collisionVoxel.mapChipPos = {
											float(x),
											float(y),
											float(z)
										};
										collisionVoxel.translate = translate;
									}
								}
							}
						}
					}
				}

			}
		}
	}

	return collisionVoxel;
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
					if ((cameraTranslate.z <= z && int(cameraChunkNumber.z) == chunkZ) || int(cameraChunkNumber.z) < chunkZ) {
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
					if ((cameraTranslate.x <= x && int(cameraChunkNumber.x) == chunkX) || int(cameraChunkNumber.x) < chunkX) {
						if (x <= 0) {
							//前が空白
							if (chunkX <= 0) {
								if (index_ < objects_.size()) {
									drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x, chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x], MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, -std::numbers::pi_v<float> / 2)));
									index_++;
								}
							} else {
								if (!chunks_[chunkY][chunkZ][chunkX - 1].mapChip[y][z][15]) {
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
				//下面
				if (y >= 15) {
					//下が空白
					if (chunkY >= chunks_.size() - 1) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, std::numbers::pi_v<float> / 2)));
							index_++;
						}
					}
					else if (!chunks_[chunkY + 1][chunkZ][chunkX].mapChip[0][z][x]) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, std::numbers::pi_v<float> / 2)));
							index_++;
						}
					}
				}
				else {
					if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y + 1][z][x]) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, std::numbers::pi_v<float> / 2)));
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
						if (!chunks_[chunkY][chunkZ][chunkX - 1].mapChip[y][z][15]) {
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

void Voxel::DrawChunkAllUp(
	int chunkY, int chunkZ, int chunkX,
	Vector3 cameraChunkNumber,
	int maxUpRange
) {
	for (int y = 0; y < 16; y++) {
		for (int z = 0; z < 16; z++) {
			for (int x = 0; x < 16; x++) {

				if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x]) continue;

				// 上面
				if (y <= 0) {
					if (chunkY <= 0) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
							index_++;
						}
					} else if (!chunks_[chunkY - 1][chunkZ][chunkX].mapChip[15][z][x]) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
							index_++;
						}
					}
				} else if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y - 1][z][x]) {
					if (index_ < objects_.size()) {
						drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
							chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
							MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
						index_++;
					}
				}
				//下面
				if (y >= 15) {
					//下が空白
					if (chunkY >= chunks_.size() - 1) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, std::numbers::pi_v<float> / 2)));
							index_++;
						}
					}
					else if (!chunks_[chunkY + 1][chunkZ][chunkX].mapChip[0][z][x]) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, std::numbers::pi_v<float> / 2)));
							index_++;
						}
					}
				}
				else {
					if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y + 1][z][x]) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, std::numbers::pi_v<float> / 2)));
							index_++;
						}
					}
				}
				// 前面
				if (z >= 15) {
					if (chunkZ >= chunks_[chunkY].size() - 1) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
							index_++;
						}
					} else if (!chunks_[chunkY][chunkZ + 1][chunkX].mapChip[y][0][x]) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
							index_++;
						}
					}
				} else if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z + 1][x]) {
					if (index_ < objects_.size()) {
						drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
							chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
							MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
						index_++;
					}
				}

				// 後面
				if (z <= 0) {
					if (chunkZ <= 0) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								IdentityQuaternion()));
							index_++;
						}
					} else if (!chunks_[chunkY][chunkZ - 1][chunkX].mapChip[y][15][x]) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								IdentityQuaternion()));
							index_++;
						}
					}
				} else if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z - 1][x]) {
					if (index_ < objects_.size()) {
						drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
							chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
							IdentityQuaternion()));
						index_++;
					}
				}

				// 右面
				if (x >= 15) {
					if (chunkX >= chunks_[chunkY][chunkZ].size() - 1) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
							index_++;
						}
					} else if (!chunks_[chunkY][chunkZ][chunkX + 1].mapChip[y][z][0]) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
							index_++;
						}
					}
				} else if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x + 1]) {
					if (index_ < objects_.size()) {
						drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
							chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
							MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
						index_++;
					}
				}

				// 左面
				if (x <= 0) {
					if (chunkX <= 0) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, -std::numbers::pi_v<float> / 2)));
							index_++;
						}
					} else if (!chunks_[chunkY][chunkZ][chunkX - 1].mapChip[y][15][x]) {
						if (index_ < objects_.size()) {
							drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
								chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
								MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, -std::numbers::pi_v<float> / 2)));
							index_++;
						}
					}
				} else if (!chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x - 1]) {
					if (index_ < objects_.size()) {
						drawOdjects_.push_back(AddFace(chunkY, chunkZ, chunkX, y, z, x,
							chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x],
							MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, -std::numbers::pi_v<float> / 2)));
						index_++;
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

std::vector<VoxelStatus> Voxel::LoadVoxel(std::string loadFile) {
	std::ifstream file(loadFile);

	std::vector<VoxelStatus> data{};

	//開けないなら空データを返す
	if (!file.is_open()) {
		//空気用ダミー
		VoxelStatus status{};

		data.push_back(status);
		return data;
	}

	std::string line;

	while (std::getline(file, line)) {

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// コメント
		if (word.find("//") == 0) {
			continue;
		}

		// カンマ区切りで読む
		if (word.find("VoxelData") == 0) {
			VoxelStatus status;

			getline(line_stream, word, ',');
			status.MaxHP = stof(word);
			getline(line_stream, word, ',');
			status.randomRate = stof(word);
			getline(line_stream, word, ',');
			status.vacuumSensitivity = stof(word);

			data.push_back(status);
		}

	}

	file.close();
	return data;
}

void Voxel::WriteVoxel(const std::string& loadFile) {
	std::ofstream file(loadFile);
	assert(file.is_open());
	
	for (int i = 0; i < csvData_.voxelStatus.size(); i++) {
		file << "VoxelData" << "," << csvData_.voxelStatus[i].MaxHP << "," << csvData_.voxelStatus[i].randomRate << "," << csvData_.voxelStatus[i].vacuumSensitivity << "\n\n";
	}

	file.close();
}

std::optional<Vector3> Voxel::CollisionCheck(Sphere sphere) {

	float minDistSq = FLT_MAX;
	Vector3 bestPoint;
	bool found = false;

	for (int chunkY = 0; chunkY < chunks_.size(); chunkY++) {
		for (int chunkZ = 0; chunkZ < chunks_[chunkY].size(); chunkZ++) {
			for (int chunkX = 0; chunkX < chunks_[chunkY][chunkZ].size(); chunkX++) {

				float voxelSize = scale * 2.0f;
				float chunkSize = voxelSize * 16.0f;

				Vector3 chunkOrigin = {
					chunkX * chunkSize - chunkSize / 2 * csvData_.size.x,
					-((chunkY + 1) * chunkSize),
					chunkZ * chunkSize - chunkSize / 2 * csvData_.size.z
				};

				AABB chunkAABB;
				chunkAABB.min = chunkOrigin;
				chunkAABB.max = chunkOrigin + Vector3{ chunkSize, chunkSize, chunkSize };

				if (!IsCollision(chunkAABB, sphere)) continue;

				for (int y = 0; y < 16; y++) {
					for (int z = 0; z < 16; z++) {
						for (int x = 0; x < 16; x++) {

							if (chunks_[chunkY][chunkZ][chunkX].mapChip[y][z][x] == 0) continue;

							AABB voxelAABB;
							voxelAABB.min = {
								chunkOrigin.x + voxelSize * x,
								chunkOrigin.y + voxelSize * (16 - y),
								chunkOrigin.z + voxelSize * z
							};
							voxelAABB.max = voxelAABB.min + Vector3{ voxelSize, voxelSize, voxelSize };

							if (!IsCollision(voxelAABB, sphere)) continue;

							// 最近接点
							Vector3 closest;
							closest.x = std::clamp(sphere.center.x, voxelAABB.min.x, voxelAABB.max.x);
							closest.y = std::clamp(sphere.center.y, voxelAABB.min.y, voxelAABB.max.y);
							closest.z = std::clamp(sphere.center.z, voxelAABB.min.z, voxelAABB.max.z);

							// 距離²
							float dx = sphere.center.x - closest.x;
							float dy = sphere.center.y - closest.y;
							float dz = sphere.center.z - closest.z;
							float distSq = dx * dx + dy * dy + dz * dz;

							if (distSq < minDistSq) {
								minDistSq = distSq;
								bestPoint = closest;
								found = true;
							}
						}
					}
				}
			}
		}
	}

	if (found) return bestPoint;
	return std::nullopt;
}

//上のマップチップをコピー
void Voxel::CopyUpperMapChip(Vector3 chunkPos, int y) {
	if (y <= 0) {
		if (int(chunkPos.y) <= 0) {
			//空白にする
			for (int z = 0; z < 16; z++) {
				for (int x = 0; x < 16; x++) {
					chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)].mapChip[y][z][x] = 0;
				}
			}
		}
		else {
			//上のチャンクの一番下
			chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)].mapChip[y] = 
				chunks_[int(chunkPos.y - 1)][int(chunkPos.z)][int(chunkPos.x)].mapChip[15];
		}
	}
	else {
		//上のマップチップをコピー
		chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)].mapChip[y] =
			chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)].mapChip[y - 1];
	}
}

//下のマップチップをコピー
void Voxel::CopyUnderMapChip(Vector3 chunkPos, int y) {
	if (y >= 15) {
		if (int(chunkPos.y) >= csvData_.size.y) {
			//空白にする
			for (int z = 0; z < 16; z++) {
				for (int x = 0; x < 16; x++) {
					chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)].mapChip[y][z][x] = 0;
				}
			}
		}
		else {
			//下のチャンクの一番上
			chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)].mapChip[y] =
				chunks_[int(chunkPos.y + 1)][int(chunkPos.z)][int(chunkPos.x)].mapChip[0];
		}
	}
	else {
		//下のマップchipをコピー
		chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)].mapChip[y] =
			chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)].mapChip[y + 1];
	}
}

//チャンク縦回転
void Voxel::ChunkVerticalRotation(Vector3 chunkPos) {
	Chunk newChunk{};
	for (int y = 0; y < 16; y++) {
		for (int z = 0; z < 16; z++) {
			newChunk.mapChip[y][z] = chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)].mapChip[15 - z][y];
		}
	}
	
	chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)] = newChunk;
}

//チャンク横回転
void Voxel::ChunkHorizontalRotation(Vector3 chunkPos) {
	Chunk newChunk{};
	for (int y = 0; y < 16; y++) {
		for (int z = 0; z < 16; z++) {
			for (int x = 0; x < 16; x++) {
				newChunk.mapChip[y][z][x] = chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)].mapChip[y][15 - x][z];
			}
		}
	}

	chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)] = newChunk;
}

//チャンクコピー
void Voxel::ChunkCopy(Vector3 fromChunkPos, Vector3 toChunkPos) {
	chunks_[int(toChunkPos.y)][int(toChunkPos.z)][int(toChunkPos.x)] = chunks_[int(fromChunkPos.y)][int(fromChunkPos.z)][int(fromChunkPos.x)];
}

//チャンク交換
void Voxel::ChunkSwap(Vector3 fromChunkPos, Vector3 toChunkPos) {
	Chunk keepChunk = chunks_[int(toChunkPos.y)][int(toChunkPos.z)][int(toChunkPos.x)];
	chunks_[int(toChunkPos.y)][int(toChunkPos.z)][int(toChunkPos.x)] = chunks_[int(fromChunkPos.y)][int(fromChunkPos.z)][int(fromChunkPos.x)];
	chunks_[int(fromChunkPos.y)][int(fromChunkPos.z)][int(fromChunkPos.x)] = keepChunk;
}

//チャンク数変更
void Voxel::Resize(Vector3 size) {
	csvData_.size = size;

	//サイズ変更
	chunks_.resize(int(csvData_.size.y));
	for (int y = 0; y < csvData_.size.y; y++) {
		//サイズ変更
		chunks_[y].resize(int(csvData_.size.z));
		for (int z = 0; z < csvData_.size.z; z++) {
			//サイズ変更
			chunks_[y][z].resize(int(csvData_.size.x));
		}
	}
}

void Voxel::Save(const std::string& directoryPath) {
	std::string str;
	for (int y = 0; y < chunks_.size(); y++) {
		for (int z = 0; z < chunks_[y].size(); z++) {
			for (int x = 0; x < chunks_[y][z].size(); x++) {
				str = csvData_.chunkDataDirectoryPath + "/chunk_" + std::to_string(y) + "_" + std::to_string(z) + "_" + std::to_string(x) + ".csv";
				WriteChunk(chunks_[y][z][x], str);
			}
		}
	}

	WriteVoxel(csvData_.voxelDataFilePath);
}