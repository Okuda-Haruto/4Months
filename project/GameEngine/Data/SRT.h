#pragma once
#include "Vector3.h"
#include "Quaternion.h"

struct SRT {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};