struct VertexInput
{
	float4 position : POSITION0; //플롯이 4개 있는 벡터
	float4 color : COLOR0;
};
//뒤에 초록부분은 Semantic
//데이터의 출처와 역할에 대한 분명한 의미를 부여하기 위한 키워드
//Semantic네임+인덱스
struct PixelInput
{
	float4 position : SV_POSITION0;
	float4 color : COLOR0;
};
//SV : System Value
//반드시 필요한 데이터에 붙인다

//상수버퍼 받을 셰이더 작성
cbuffer Transformbuffer : register(b0) // -> 0 ~ 13 슬롯에 등록(4096*16바이트이하 16배수)
{
	matrix world;   //float4x4 와 동일
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
	output.position = mul(input.position, world); //월드 변환
	output.position = mul(output.position, view); //뷰   변환
	output.position = mul(output.position, proj); //프로젝션 변환	
	output.color = input.color;

	return output;
}
float4 PS(PixelInput input) : SV_Target //현재 세팅된 메인렌더 타깃을 사용하라고 알려주어야 한다
{
    return input.color;
}
//화면에 픽셀단위로 색상을 출력한다
//pixel shader는 rasterizer가 지정한 픽셀 개수만큼 호출된다