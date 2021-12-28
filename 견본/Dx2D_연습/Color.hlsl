struct VertexInput
{
	float4 position : POSITION0; //�÷��� 4�� �ִ� ����
	float4 color : COLOR0;
};
//�ڿ� �ʷϺκ��� Semantic
//�������� ��ó�� ���ҿ� ���� �и��� �ǹ̸� �ο��ϱ� ���� Ű����
//Semantic����+�ε���
struct PixelInput
{
	float4 position : SV_POSITION0;
	float4 color : COLOR0;
};
//SV : System Value
//�ݵ�� �ʿ��� �����Ϳ� ���δ�

//������� ���� ���̴� �ۼ�
cbuffer Transformbuffer : register(b0) // -> 0 ~ 13 ���Կ� ���(4096*16����Ʈ���� 16���)
{
	matrix world;   //float4x4 �� ����
	matrix view;
	matrix proj;
};

PixelInput VS(VertexInput input)
{
	// 1X4         4X4    ----> 1X4
	//1, 2, 3, 1 * 0000
	//             0000
	//             0000
	//             0000

	PixelInput output;
	output.position = mul(input.position, world); //���� ��ȯ
	output.position = mul(output.position, view); //��   ��ȯ
	output.position = mul(output.position, proj); //�������� ��ȯ	
	output.color = input.color;

	return output;
}
float4 PS(PixelInput input) : SV_Target //���� ���õ� ���η��� Ÿ���� ����϶�� �˷��־�� �Ѵ�
{
    return input.color;
}
//ȭ�鿡 �ȼ������� ������ ����Ѵ�
//pixel shader�� rasterizer�� ������ �ȼ� ������ŭ ȣ��ȴ�