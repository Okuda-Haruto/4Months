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

struct VertexShaderInput
{
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

GlowVertexShaderOutput main(VertexShaderInput input)
{
	GlowVertexShaderOutput output;

	output.position =
        mul(input.position, gTransformationMatrix.WVP);

    output.localPosition = input.position.xyz;


	return output;
}