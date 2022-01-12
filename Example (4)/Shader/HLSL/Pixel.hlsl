//hlsl �ۼ� ��� ù��°
//void ������ out���� ��ȯ�ȴ�
#include "Layout.hlsli"

Texture2D Texture;

float4 Shader(Output output) : SV_Target //�Լ� ���� �ø�ƽ�� ��ȯ���� �� SV_Target�� �ǹ��Ѵٴ� ����
{
    float4 color = Texture.Load(int3(output.TexCoord, 0));
    if(color.a !=1)
        discard;
    return color;
}