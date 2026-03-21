#pragma once
#include "Vector3.h"
#include "Operation/Operation.h"

float Rand01() {
	return rand() / (float)RAND_MAX;
}

float Clamp01(float x) {
	if (x < 0.0f) return 0.0f;
	if (x > 1.0f) return 1.0f;
	return x;
}

Vector3 Mix(
	Vector3& pos,
	Vector3& vel,
	const Vector3& center,
	const Vector3& axis) {

	float baseRadiusStrength = 10.0f;
	float swirlSpeed = 2.0f;
	float noiseStrength = 1.5f;
	float damping = 0.9f;
	float shrinkSpeed = 2.5f;

	float maxRadius = 40.0f;

	Vector3 toCenter = pos - center;
	float dist = Length(toCenter);

	if (dist < 0.0001f) {
		return Vector3{};
	}

	Vector3 dir = toCenter / dist;
	Vector3 tangent = Cross(axis, dir);

	// 距離による強さ
	float t = Clamp01(dist / maxRadius);
	float strength = 1.0f - t * t;

	// 回転（中心ほど強い）
	vel += tangent * swirlSpeed * strength;

	// 半径を縮める（吸引）
	float targetRadius = dist - shrinkSpeed;

	// ノイズ
	float noise = (Rand01() - 0.5f) * noiseStrength;
	targetRadius += noise;

	float radiusError = targetRadius - dist;

	vel += dir * radiusError * baseRadiusStrength * strength * (1.0f / 60.0f);

	// 減衰
	vel *= damping;

	return vel;
}

Vector3 Mix2(
	Vector3& pos,
	Vector3& vel,
	const Vector3& center,
	const Vector3& axis) {

	float baseRadiusStrength = 10.0f;
	float swirlSpeed = 2.0f;
	float noiseStrength = 1.5f;
	float damping = 0.9f;
	float expandSpeed = 0.4f;

	float maxRadius = 40.0f;

	Vector3 toCenter = pos - center;
	float dist = Length(toCenter);

	if (dist < 0.0001f) {
		return Vector3{};
	}

	Vector3 dir = toCenter / dist;
	Vector3 tangent = Cross(axis, dir);

	// 距離による強さ
	float t = Clamp01(dist / maxRadius);
	float strength = 1.0f - t * t;

	// 回転（中心ほど強い）
	vel += tangent * swirlSpeed * strength;

	// 半径を広げる（拡散）
	float targetRadius = dist + expandSpeed;

	// ノイズ
	float noise = (Rand01() - 0.5f) * noiseStrength;
	targetRadius += noise;

	float radiusError = targetRadius - dist;

	// 外方向へ補正
	vel += dir * radiusError * baseRadiusStrength * strength * (1.0f / 60.0f);

	// 中心付近でより強く広げる
	float coreRadius = 5.0f;
	float corePushStrength = 2.0f;
	if (dist < coreRadius) {
		float coreT = 1.0f - (dist / coreRadius);
		vel += dir * corePushStrength * coreT;
	}

	// 減衰
	vel *= damping;

	return vel;
}