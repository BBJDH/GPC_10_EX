#include "Layout.hlsli"

namespace Shader
{
    namespace Resource
    {
        cbuffer Transform : register(B0) { matrix World; };
        cbuffer Transform : register(B1) { matrix View; };
        cbuffer Transform : register(B2) { matrix Projection; };
    }

    Layout::Pixel Vertex(const Layout::Vertex Input)
    {
        Layout::Pixel Output =
        {
            Input.Position,
            Input.TexCoord
        };
        
        Output.Position = mul(Output.Position, Resource::World);
        Output.Position = mul(Output.Position, Resource::View);
        Output.Position = mul(Output.Position, Resource::Projection);

        return Output;
    }
}