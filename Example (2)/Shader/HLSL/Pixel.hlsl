//hlsl �ۼ� ��� ù��°
//void ������ out���� ��ȯ�ȴ�
void Shader
(
    //��ǥ�� ������ �Ѱ��� ����
    in float4 iPosition : SV_Position0,
    in float4 iColor    : COLOR0,
    out float4 oColor   : SV_TARGET0    //RTV�� ���� ��
)
{
    oColor = iColor;
    //�ȼ� ���̴��� �÷��� ��ȯ���ָ� �ȴ�
}