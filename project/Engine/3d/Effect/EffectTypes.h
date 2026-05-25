#pragma once
#include "Math/Type/Vector3.h"
#include "Math/Type/Vector4.h"
#include "Utility/BlendMode/BlendMode.h"
#include <string>

/// <summary>
/// エフェクトを構成する要素の種類
/// </summary>
enum class EffectElementType : uint32_t {
	Particle = 0,  // パーティクルシステムを使用
	Plane    = 1,  // 平面プリミティブを使用
};

/// <summary>
/// 単一Planeエレメントのパラメータ
/// </summary>
struct PlaneElementParams {
	// --- スケール ---
	float scaleX        = 0.05f;  // X方向の固定スケール（縦長ライン等に）
	float scaleYMin     = 0.3f;   // Y方向スケールのランダム最小値
	float scaleYMax     = 0.8f;   // Y方向スケールのランダム最大値
	float scaleZ        = 1.0f;   // Z方向スケール（通常1.0）

	// --- 回転 ---
	float rotateZMin    = -3.14159265f;  // Z軸回転ランダム最小値 (-π)
	float rotateZMax    =  3.14159265f;  // Z軸回転ランダム最大値 (+π)

	// --- 速度 ---
	Vector3 velocityMin = { 0.0f, 0.0f, 0.0f };
	Vector3 velocityMax = { 0.0f, 0.0f, 0.0f };

	// --- ライフタイム ---
	float lifeTime      = 1.0f;

	// --- 色・テクスチャ ---
	Vector4 startColor  = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 endColor    = { 1.0f, 1.0f, 1.0f, 0.0f };  // フェードアウト
	std::string texturePath = "resources/image/particle/circle2.png";
	BlendMode blendMode = kBlendModeAdd;

	// --- 生成数 ---
	uint32_t spawnCount = 8;  // 一度に生成するPlane数（星型→8個など）
};

/// <summary>
/// エフェクト全体のパラメータ（複数種類の要素を束ねる）
/// </summary>
struct EffectParams {
	std::string effectName  = "Effect";
	bool usePlane           = true;   // Planeエレメントを使用するか
	bool useParticle        = true;   // Particleエレメントを使用するか

	PlaneElementParams plane;

	// Particle側のJsonファイル名（resources/json/particle/ 以下）
	std::string particleJsonPath = "hitEffect";

	// Particleエミッタ位置オフセット
	Vector3 particleOffset = { 0.0f, 0.0f, 0.0f };
};

/// <summary>
/// 実行中のPlane1枚のランタイムデータ
/// </summary>
struct PlaneInstance {
	Vector3 position    = { 0.0f, 0.0f, 0.0f };
	Vector3 scale       = { 1.0f, 1.0f, 1.0f };
	Vector3 rotate      = { 0.0f, 0.0f, 0.0f };
	Vector3 velocity    = { 0.0f, 0.0f, 0.0f };
	Vector4 color       = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 startColor  = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 endColor    = { 1.0f, 1.0f, 1.0f, 0.0f };
	float lifeTime      = 1.0f;
	float currentTime   = 0.0f;
	bool  alive         = false;
};
