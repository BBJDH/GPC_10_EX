//hlsl 작성 방식 첫번째
//void 이지만 out으로 반환된다
#include "Layout.hlsli"
struct Input
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

Output Shader(const Input input)
{
    
    Output output =
    {
        input.Position,
        input.TexCoord
    };

    return output;

}
//작성한후 컴파일되고 이진코드 상태로 실제로 엘레먼트 디스크립터와 내용을 비교해야한다