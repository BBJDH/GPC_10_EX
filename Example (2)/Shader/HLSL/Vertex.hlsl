//hlsl 작성 방식 첫번째
//void 이지만 out으로 반환된다
void Shader
(
    //좌표와 색상을 넘겨줄 예정
    in float4 iPosition : POSITION0,
    in float4 iColor : COLOR0,
    out float4 oPosition : SV_POSITION0,
    out float4 oColor : COLOR0 //0안붙여도 자동으로 0
// SV(SysValue)는 시스템 값으로 시스템에서 처리요청을 나타냄
// 컴파일후 확인 가능
)
{
    /*TODO : 변환 적용*/
    //float2 = float4(iPosition.zw, 0, 1); //성분 추출 원하는대로도 가능
    //iposition.xrgb 같이 혼용 사용은 불가능
    
    oPosition = iPosition;
    oColor = iColor;
}
//작성한후 컴파일되고 이진코드 상태로 실제로 엘레먼트 디스크립터와 내용을 비교해야한다