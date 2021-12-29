//hlsl 작성 방식 첫번째
//void 이지만 out으로 반환된다
#include "Layout.hlsli"

Texture2D Texture;

float4 Shader(Output output) : SV_Target //함수 옆의 시멘틱은 반환값이 곧 SV_Target을 의미한다는 문구
{
    float4 color = Texture.Load(int3(output.TexCoord, 0));
    if(color.a !=1)
        discard;
    return color;
}