/* 고수준 쉐이더 언어(High Level Shader Language, HLSL) */

#include "IL.hlsli"

cbuffer Transform : register(B0) { matrix World; }
cbuffer Transform : register(B1) { matrix View; }
cbuffer Transform : register(B2) { matrix Projection; }

Pixel VS(const Vertex Input)
{
    Pixel Output =
    {
        Input.Position,
        Input.TexCoord
    };
    
	Output.Position = mul(Output.Position, World);
	Output.Position = mul(Output.Position, View);
	Output.Position = mul(Output.Position, Projection);

    return Output;
}