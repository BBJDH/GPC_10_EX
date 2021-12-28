struct VertexInput
{
	float4 position : POSITION0; //�÷��� 4�� �ִ� ����
	float2 uv : TEXCOORD0;
};
//�ڿ� �ʷϺκ��� Semantic
//�������� ��ó�� ���ҿ� ���� �и��� �ǹ̸� �ο��ϱ� ���� Ű����
//Semantic����+�ε���
struct PixelInput
{
	float4 position : SV_POSITION0;
    float2 uv : TEXCOORD0;
};
//SV : System Value
//�ݵ�� �ʿ��� �����Ϳ� ���δ�

//������� ���� ���̴� �ۼ�
cbuffer Transformbuffer : register(b0) // -> 0 ~ 13 ���Կ� ���(4096*16����Ʈ���� 16���)
{
	/*row_major */matrix world;   //float4x4 �� ����
	/*row_major */matrix view;
	/*row_major */matrix proj;
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
	output.uv = input.uv;

	return output;
}

Texture2D sourc_texture1 : register(t0); //�ؽ��� �ڿ��̶�� �ǹ�
Texture2D sourc_texture2 : register(t1); //�ؽ��� �ڿ��̶�� �ǹ� //0~127 �� 128��

SamplerState samp : register(s0); //���ø��� �� ���� ����//0~15����
//SamplerState�� �������� �ʾƵ� �⺻���� ���õǾ� �ִ�
float4 PS(PixelInput input) : SV_Target //���� ���õ� ���η��� Ÿ���� ����϶�� �˷��־�� �Ѵ�
{
    //float4 color = 0.0f;
    //if (input.uv.x < 1.0f)
    //    color = sourc_texture1.Sample(samp, input.uv);
    //else
    //    color = sourc_texture2.Sample(samp, float2(input.uv.x - 1.0f, input.uv.y));
	
    //clip(color.a -0.1f); //���İ����� 0.9���� �۴ٸ� 0�̳� ������ �ش��÷��� ������
	//if(color.a<0.9f)
    //    discard;//�ΰ��� ����� �ִ� ��� �̷� ����� ������ ����
	
	
    float4 color = sourc_texture1.Sample(samp, input.uv);
	
    return color; //������ �����͸� ����
}
//ȭ�鿡 �ȼ������� ������ ����Ѵ�
//pixel shader�� rasterizer�� ������ �ȼ� ������ŭ ȣ��ȴ�