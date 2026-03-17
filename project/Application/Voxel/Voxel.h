#pragma once
#include <GameEngine.h>

struct Chunk {
	std::array<std::array<std::array<uint8_t, 16>, 16>, 16> mapChip;
};

class Voxel {
private:

	std::shared_ptr<DirectionalLight> directionalLigth_;

	std::vector<Chunk> chunks_;

	std::array<std::unique_ptr<Object>, 4096> objects_;
	uint32_t index_ = 0;

public:
	void Initialize(std::shared_ptr<Model> face_, std::shared_ptr<DirectionalLight> directionalLigth);

	void Update();

	void Draw();

private:
	Object* AddFace(int i,int y, int z, int x, Quaternion rotate);

};