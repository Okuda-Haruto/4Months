#pragma once
#include "Vector3.h"
#include "Operation/Operation.h"

inline float Rand01() {
	return rand() / (float)RAND_MAX;
}

inline float Clamp01(float x) {
	if (x < 0.0f) return 0.0f;
	if (x > 1.0f) return 1.0f;
	return x;
}

inline Vector3 Mix(
	Vector3& pos,
	const Vector3& center,
	const Vector3& axis,
	const float swirlSpeed,
	float maxRadius, bool reverse) {

	float baseRadiusStrength = 10.0f;
	float speed = swirlSpeed;
	float noiseStrength = 1.5f;
	float damping = 0.9f;
	float shrinkSpeed = 2.5f;

	if (reverse) speed *= -1.0f;

	Vector3 toCenter = pos - center;
	float dist = Length(toCenter);

	if (dist < 0.0001f) {
		return Vector3{};
	}

	Vector3 vel = {};
	Vector3 dir = toCenter / dist;
	Vector3 tangent = Cross(axis, dir);

	// 距離による強さ
	float t = Clamp01(dist / maxRadius);
	float strength = 1.0f - t * t;

	// 回転（中心ほど強い）
	vel += tangent * speed * strength;

	// 半径を縮める（吸引）
	float targetRadius = dist - shrinkSpeed;

	// ノイズ
	float noise = (Rand01() - 0.5f) * noiseStrength;
	targetRadius += noise;

	float radiusError = targetRadius - dist;

	vel += dir * radiusError * baseRadiusStrength * strength * GameEngine::GetDeltaTime();

	// 減衰
	vel *= damping;

	return vel;
}

inline Vector3 Mix2(
	Vector3& pos,
	const Vector3& center,
	const Vector3& axis,
	const float swirlSpeed,
	float maxRadius, bool reverse) {

	float baseRadiusStrength = 5.0f;
	float speed = swirlSpeed;
	float noiseStrength = 1.5f;
	float damping = 0.9f;
	float shrinkSpeed = 1.0f;

	if (reverse) speed *= -1.0f;

	Vector3 toCenter = pos - center;
	float dist = Length(toCenter);

	if (dist < 0.0001f) {
		return Vector3{};
	}

	Vector3 vel = {};
	Vector3 dir = toCenter / dist;
	Vector3 tangent = Cross(axis, dir);

	// 距離による強さ
	float t = Clamp01(dist / maxRadius);
	float strength = 1.0f - t * t;

	// 回転（中心ほど強い）
	vel += tangent * speed * strength;

	// 半径を縮める（吸引）
	float targetRadius = dist - shrinkSpeed;

	// ノイズ
	float noise = (Rand01() - 0.5f) * noiseStrength;
	targetRadius += noise;

	float radiusError = targetRadius - dist;

	vel += dir * radiusError * baseRadiusStrength * strength * GameEngine::GetDeltaTime();

	// 減衰
	vel *= damping;

	return vel;
}