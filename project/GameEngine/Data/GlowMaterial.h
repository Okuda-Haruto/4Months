#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

struct GlowMaterial {
    Vector4 baseColor;
    Vector4 tipColor;

    float glowIntensity;
    float flameHeight;

    Vector2 padding2;
};