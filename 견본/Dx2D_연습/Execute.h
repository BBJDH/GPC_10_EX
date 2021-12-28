#pragma once

struct VertexColor
{
	D3DXVECTOR3 position;
	D3DXCOLOR color;
};

struct VertexTexture
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 uv; //texcoord :Texture Coordinate (텍스쳐 좌표계)
};

struct TRANSFORM_DATA
{
	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;
};

class Execute final
{
public:
	Execute();
	~Execute();

	void Update(); //데이터 업데이트
	void Render();	//화면에 렌더링 실시
private:
	class Graphics* graphics =nullptr; //class Graphics 전방선언과 동일

	VertexTexture*  vertices = nullptr;
	ID3D11Buffer* vertex_buffer = nullptr;

	uint * indices = nullptr;
	ID3D11Buffer* index_buffer = nullptr;

	ID3D11InputLayout* input_layout = nullptr; 
	//버텍스 셰이더 스테이지와 밀접한 관계를 가진다

	ID3D11VertexShader* vertex_shader = nullptr;
	ID3DBlob* vs_blob =nullptr;
	//Blob : Binary Large Object :이진 데이터 파일
	//HLSL의 코드를 2진 파일로 저장하는 객체(따라서 HLSL 작성이 선행되어야한다)

	ID3D11PixelShader* pixel_shader = nullptr;
	ID3DBlob* ps_blob = nullptr;

	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;

	TRANSFORM_DATA cpu_buffer;
	ID3D11Buffer* gpu_buffer = nullptr; //constant buffer(상수 버퍼)

	ID3D11RasterizerState * rasterizer_state = nullptr;

	//ID3D11ShaderResourceView * shader_resource[2]; //이미지 배열로 전달하는방법
	ID3D11ShaderResourceView * shader_resource = nullptr;
	//배열없이 여러개 그리는 방법
	ID3D11SamplerState * sampler_state = nullptr;
	ID3D11BlendState * blend_state = nullptr;

};
#pragma region 정리
//행렬
//단위행렬(행렬은 항상 단위 행렬로 초기화 해야한다)
/*  x y z w
1 0 0 0  =>x로 향하는벡터
0 1 0 0  =>y로 향하는벡터
0 0 1 0  =>z로 향하는벡터
0 0 0 1  =>0점(위치)
=>xyz축으로 1씩 향하는 중심좌표는 0,0,0 행렬
공간이 커지면 안의 데이터들도 같이 커진다
공간이 이동하면 안의 데이터들도 이동한다
*/

//파이프 라인 
// -> IA - VS - RS - PS - OM 
// 
// IA : Input Assembler Stage
// -Vertex 구조체 정보가 필요하다 
// vertex 구조체는 다이렉트X에서 제공되지않는다. 사용자마다 요구사항이 다양하기 때문에
// -좌표와 컬러 혹은 좌표 따로 컬러 따로 아니면컬러만 두개 등등 사용자가 직접 설계가능하다
// 
// Shader : GPU를 동작시키는 함수들의 집합
// VS : Vertex Shader -> shader : HLSL(High Level Shader Languege)
// 원래는 어셈블리어로 이루어져 있었으나 생산성이 떨어져서 HLSL로 보완
// (OpenGL은 GLSL 등등)
// IA, RS, OM 단계는 코드 수정이 불가,
// VS, PS는 코딩이 가능
// VS : Vertex Shader -> shader : HLSL(High Level Shader Languege)
// -Transform을 해준다(clip space)
// 
// RS : Rasterrizer
// Clipping : 오려내고
// NDC		: 정규화 시키고
// Back-face Culling : 보이지 않는 면을 제거
// Viewport Transform : 다시 늘려주고(뷰포트 사이즈로)
// Scan Transform : 픽셀셰이더가 칠할 색을 정함
// 
// (NDC 좌표계)(정점들의 공간변환 3차원에서 2차원 그림으로 0~1 또는 -1~1로 정규화)
// NDC 좌표계란?(0~ 1024 이러한 좌표들을 0~1 또는 -1~1로 정규화진행한다 rgba 처럼)
// -처음 사각형을 출력할때 넣어준 버텍스 값들은 1.0f -0.5f...인데 
// 출력은 도형이 화면의 100프로 50프로 비율(화면의 중점으로부터)의 지점으로 찍힌다 
// 3d데이터(벡터)를 2d 데이터(픽셀)로 변환
// 버텍스 셰이더에서 넘어온 정보를 화면에맞게(뷰포트) 크게 늘려준다
// (정규화된 값을 가지고 잘라내고 앞뒤 확인 하고 뷰포트까지 정규화된값을 늘려준다)
// 픽셀 셰이더 단계로 넘어갈 픽셀들을 지정해준다
// 
// 
// 
//
// PS : 각 픽셀에 대응하는 각 이미지 색상을 추출 
//


//공간
//
// Local      : 지역좌표계(자기지역안에 중심을 가지고있는 좌표계)- 중점을 놓고 그 중점으로부터의 좌표로 설계
// World      : 지역좌표계를 월드로 가져와서 월드 좌표에 올려놓은 공간
// View	      :	(카메라)보여지는 영역 전체 월드중 내가 보고있는 3차원 공간
// Projection : 보고있는 공간을 사진처럼 2d로 사영한 공간
//				(DX에서 Projection과 Clip Space는 결합하여 있다)
// Clip       : 클리핑을 하기위해 정규화된 공간(Clip Space)
//				 - 그냥 잘라내는 작업을하기에는 프로젝션 공간이 너무 크기에 정규화를 진행한다
//				 - Homogeneous space - 동차공간
//               동차공간이란? (1,0,0) - 좌표인지 방향벡터인지 알수가 없다 
//							->(1,0,0,0)차원(xyzw)을 하나 추가해서 식별가능하게한다
//							->w값이 0이라면 방향
//							->w값이 1이상이라면 위치(1로 계산한다)
// 
// (x좌표,동차좌표)  1차원의 두공간을 동차공간 묘사
// |5y
// |--------------------0--------- 4y (21,4)
// |3y
// |------0----------------------- 2y (7,2)
// |1y
// -> 둘을 어떻게 만나게 할까? 
// : 동차공간으로 단면을 잘라 묘사한 후 동차좌표를 맞추면 같은 차원에서 만나게된다
// 
// 
// =======================여기까지 공간을 VS에서 담당=========================
// =======================이후로 RS에서 담당=================================
// 
// NDC        : - Euclidean space
// Viewport   : RS세팅만큼 다시 크기조정해주는 공간
// 
//  
#pragma endregion
