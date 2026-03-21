#pragma once
#include "Vector3.h"
#include "Operation/Operation.h"

Vector3 CalcAttraction(
	const Vector3& pos,
	const Vector3& center,
	float strength) {
	Vector3 toCenter = center - pos;
	float dist = Length(toCenter);

	if (dist < 0.0001f) return Vector3(0, 0, 0);

	Vector3 dir = toCenter / dist;

	// 距離が近いほど強くする
	float force = strength / (dist + 1.0f);

	return dir * force;
}

Vector3 CalcSwirl(
	const Vector3& pos,
	const Vector3& center,
	const Vector3& axis,
	float strength) {
	Vector3 toCenter = center - pos;
	float dist = Length(toCenter);

	if (dist < 0.0001f) return Vector3(0, 0, 0);

	Vector3 dir = toCenter / dist;

	// 回転方向 = 軸 × 中心方向
	Vector3 swirlDir = Cross(axis, dir);

	// 中心に近いほど速くするとそれっぽい
	float force = strength / (dist + 0.5f);

	return swirlDir * force;
}

float Rand01() {
	return rand() / (float)RAND_MAX;
}

Vector3 CalcNoise(float strength) {
	return Vector3(
		(Rand01() - 0.5f) * strength,
		(Rand01() - 0.5f) * strength,
		(Rand01() - 0.5f) * strength
	);
}
void ApplyBlackHole(
	Vector3& pos,
	Vector3& vel,
	const Vector3& center,
	const Vector3& axis,
	float dt) {
	// パラメータ
	float attractionStrength = 50.0f;
	float swirlStrength = 80.0f;
	float noiseStrength = 10.0f;
	float damping = 0.98f;

	// 各力を計算
	Vector3 attraction = CalcAttraction(pos, center, attractionStrength);
	Vector3 swirl = CalcSwirl(pos, center, axis, swirlStrength);
	Vector3 noise = CalcNoise(noiseStrength);

	// 合成
	Vector3 accel = attraction + swirl + noise;

	// 速度更新
	vel += accel * dt;

	// 減衰（暴れすぎ防止）
	vel *= damping;

	// 位置更新
	pos += vel * dt;
}

Vector3 Mix(
	Vector3& pos,
	Vector3& vel,
	const Vector3& center,
	const Vector3& axis) {
	float baseRadiusStrength = 10.0f;
	float swirlSpeed = 1.0f;
	float noiseStrength = 1.5f;
	float damping = 0.9f;
	float shrinkSpeed = 1.0f;

	Vector3 toCenter = pos - center;
	float dist = Length(toCenter);

	if (dist < 0.0001f) {
		return Vector3{};
	}

	Vector3 dir = toCenter / dist;

	// 接線方向（円周方向）
	Vector3 tangent = Cross(axis, dir);

	// 回転
	vel += tangent * swirlSpeed;

	// 半径を少しずつ縮める
	float targetRadius = dist - shrinkSpeed;

	// ノイズ
	float noise = (Rand01() - 0.5f) * noiseStrength;
	targetRadius += noise;

	float radiusError = targetRadius - dist;

	// 半径補正
	vel += dir * radiusError * baseRadiusStrength * (1.0f / 60.0f);

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
	float swirlSpeed = 1.0f;
	float noiseStrength = 1.5f;
	float damping = 0.9f;
	float shrinkSpeed = -1.0f;
	float expand = 0.5f; // 拡散強さ

	Vector3 toCenter = pos - center;
	float dist = Length(toCenter);

	if (dist < 0.0001f) {
		return Vector3{};
	}

	Vector3 dir = toCenter / dist;

	// 接線方向（円周方向）
	Vector3 tangent = Cross(axis, dir);

	// 回転
	vel += tangent * swirlSpeed;

	// 半径を少しずつ縮める
	float targetRadius = dist - shrinkSpeed;

	// ノイズ
	float noise = (Rand01() - 0.5f) * noiseStrength;
	targetRadius += noise;

	float radiusError = targetRadius - dist;

	// 半径補正
	vel += dir * (radiusError * baseRadiusStrength + expand) * (1.0f / 60.0f);

	// 減衰
	vel *= damping;

	return vel;
}