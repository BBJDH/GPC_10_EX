//hlsl �ۼ� ��� ù��°
//void ������ out���� ��ȯ�ȴ�
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
//�ۼ����� �����ϵǰ� �����ڵ� ���·� ������ ������Ʈ ��ũ���Ϳ� ������ ���ؾ��Ѵ�