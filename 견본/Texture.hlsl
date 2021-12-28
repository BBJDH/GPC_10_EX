struct VertexInput
{
	float4 position : POSITION0; //플롯이 4개 있는 벡터
	float2 uv : TEXCOORD0;
};
//뒤에 초록부분은 Semantic
//데이터의 출처와 역할에 대한 분명한 의미를 부여하기 위한 키워드
//Semantic네임+인덱스
struct PixelInput
{
	float4 position : SV_POSITION0;
    float2 uv : TEXCOORD0;
};
//SV : System Value
//반드시 필요한 데이터에 붙인다

//상수버퍼 받을 셰이더 작성
cbuffer Transformbuffer : register(b0) // -> 0 ~ 13 슬롯에 등록(4096*16바이트이하 16배수)
{
	/*row_major */matrix world;   //float4x4 와 동일
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
	output.position = mul(input.position, world); //월드 변환
	output.position = mul(output.position, view); //뷰   변환
	output.position = mul(output.position, proj); //프로젝션 변환	
	output.uv = input.uv;

	return output;
}

Texture2D sourc_texture1 : register(t0); //텍스쳐 자원이라는 의미
Texture2D sourc_texture2 : register(t1); //텍스쳐 자원이라는 의미 //0~127 총 128개

SamplerState samp : register(s0); //샘플링을 할 상태 정보//0~15슬롯
//SamplerState는 세팅하지 않아도 기본값이 세팅되어 있다
float4 PS(PixelInput input) : SV_Target //현재 세팅된 메인렌더 타깃을 사용하라고 알려주어야 한다
{
    //float4 color = 0.0f;
    //if (input.uv.x < 1.0f)
    //    color = sourc_texture1.Sample(samp, input.uv);
    //else
    //    color = sourc_texture2.Sample(samp, float2(input.uv.x - 1.0f, input.uv.y));
	
    //clip(color.a -0.1f); //알파값에서 0.9보다 작다면 0이나 음수로 해당컬러를 오려냄
	//if(color.a<0.9f)
    //    discard;//두가지 방법이 있다 통상 이런 방법을 쓰지는 않음
	
	
    float4 color = sourc_texture1.Sample(samp, input.uv);
	
    return color; //임의의 데이터를 추출
}
//화면에 픽셀단위로 색상을 출력한다
//pixel shader는 rasterizer가 지정한 픽셀 개수만큼 호출된다