//hlsl 작성 방식 첫번째
//void 이지만 out으로 반환된다
#include "Layout.hlsli"

Texture2D Texture;

float4 Shader(Output output) : SV_Target //반환값이 곧 SV_Target을 의미한다는 문구
{
    return Texture.Load(int3(output.TexCoord, 0));

}