//hlsl �ۼ� ��� ù��°
//void ������ out���� ��ȯ�ȴ�
void Shader
(
    //��ǥ�� ������ �Ѱ��� ����
    in float4 iPosition : POSITION0,
    in float4 iColor : COLOR0,
    out float4 oPosition : SV_POSITION0,
    out float4 oColor : COLOR0 //0�Ⱥٿ��� �ڵ����� 0
// SV(SysValue)�� �ý��� ������ �ý��ۿ��� ó����û�� ��Ÿ��
// �������� Ȯ�� ����
)
{
    /*TODO : ��ȯ ����*/
    //float2 = float4(iPosition.zw, 0, 1); //���� ���� ���ϴ´�ε� ����
    //iposition.xrgb ���� ȥ�� ����� �Ұ���
    
    oPosition = iPosition;
    oColor = iColor;
}
//�ۼ����� �����ϵǰ� �����ڵ� ���·� ������ ������Ʈ ��ũ���Ϳ� ������ ���ؾ��Ѵ�