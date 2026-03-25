#pragma once
#include <GameEngine.h>
#include <Shape/Sphere.h>
#include "Box/Box.h"
#include "GameCamera/GameCamera.h"

class Course;

struct Chunk {
	std::array<std::array<std::array<uint8_t, 16>, 16>, 16> mapChip;
};

class Voxel {
private:

	const float scale = 3.0f;

	std::shared_ptr<DirectionalLight> directionalLigth_;

	std::vector<Chunk> chunks_;

	std::shared_ptr<Model> face_;

	std::array<std::unique_ptr<Object>, 4096> objects_;
	uint32_t index_ = 0;

	Course* course_;

	GameCamera* camera_;

public:
	void Initialize(Course* course, std::shared_ptr<Model> face, GameCamera* camera, std::shared_ptr<DirectionalLight> directionalLigth);

	void Update();

	void Draw();

	void Collision(Sphere sphere);

private:
	Object* AddFace(int i,int y, int z, int x, int8_t number, Quaternion rotate);

	Chunk LoadChunk(std::string loadFile);
	void WriteChunk(const Chunk& chunk, const std::string& loadFile);
};