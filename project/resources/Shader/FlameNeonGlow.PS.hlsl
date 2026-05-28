#include "Object3d.hlsli"
#include "FlameNeonGlow.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

ConstantBuffer<TransformationMatrix>
    gTransformationMatrix : register(b0);

ConstantBuffer<FlameMaterial>
    gMaterial : register(b1);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(GlowVertexShaderOutput input)
{
    PixelShaderOutput output;

    float safeHeight = max(gMaterial.flameHeight, 1e-4f); // 0除算回避
    float gradient = saturate(input.localPosition.y / safeHeight);
    float3 flameColor = lerp(
            gMaterial.baseColor.rgb,
            gMaterial.tipColor.rgb,
            gradient
        );

    flameColor *= gMaterial.glowIntensity;
    output.color = float4(flameColor, 1.0f);

    return output;
}