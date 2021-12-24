//hlsl 작성 방식 첫번째
//void 이지만 out으로 반환된다
void Shader
(
    //좌표와 색상을 넘겨줄 예정
    in float4 iPosition : SV_Position0,
    in float4 iColor    : COLOR0,
    out float4 oColor   : SV_TARGET0    //RTV에 찍힐 값
)
{
    oColor = iColor;
    //픽셀 셰이더는 컬러만 반환해주면 된다
}