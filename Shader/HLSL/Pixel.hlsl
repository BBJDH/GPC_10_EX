//hlsl �ۼ� ��� ù��°
//void ������ out���� ��ȯ�ȴ�
#include "Layout.hlsli"

Texture2D Texture;

float4 Shader(Output output) : SV_Target //��ȯ���� �� SV_Target�� �ǹ��Ѵٴ� ����
{
    return Texture.Load(int3(output.TexCoord, 0));

}