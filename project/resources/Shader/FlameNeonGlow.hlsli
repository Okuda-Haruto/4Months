#pragma once

struct GlowVertexShaderOutput
{
    float4 position : SV_Position;
    float3 localPosition : TEXCOORD0;
};

struct GlowMaterial
{
    float4 baseColor;
    float4 tipColor;
    
    float glowIntensity;
    float flameHeight;

    float2 padding2;
};