#include"stdafx.h"
#include"Execute.h"
#include"Graphics.h"

Execute::Execute()
{
	graphics = new Graphics();
	graphics->Initialize();
	graphics->CreateBackBuffer
	(
		static_cast<uint>(Settings::Get().GetWidth()),
		static_cast<uint>(Settings::Get().GetHeight())
	);

#pragma region Vertex Data 생성
	//Vertex Data
	{
		//각 꼭지점으로부터 GPU는 병렬 처리를 한다
		//각 꼭지점으로부터 동시에 색상 작업이 이루어지므로 그라데이션으로 빛의 색상 조합이 형성된다
		//1 3
		//0 2
		vertices = new VertexTexture[4];
		vertices[0].position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);		//0
		vertices[0].uv = D3DXVECTOR2(0.0f, 1.0f);

		vertices[1].position = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);		//1
		vertices[1].uv = D3DXVECTOR2(0.0f, 0.0f);

		vertices[2].position = D3DXVECTOR3(0.5f, -0.5f, 0.0f);		//2
		vertices[2].uv = D3DXVECTOR2(1.0f, 1.0f);

		vertices[3].position = D3DXVECTOR3(0.5f, 0.5f, 0.0f);		//3
		vertices[3].uv = D3DXVECTOR2(1.0f, 0.0f);
	}
#pragma endregion


#pragma region Vertex Buffer 생성
	//Vertex Bufffer
	{
		D3D11_BUFFER_DESC desc; //리소스중 버퍼의 서술자 생성
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_IMMUTABLE;//어떻게 사용할 것인지
		//GPU와 CPU중 누가 데이터를 읽거나 쓸것인지 결정하는 플래그
		//Default : GPU만 데이터를 읽고 쓸 수 있다
		//Immutable : GPU만 데이터를 읽을 수 있다.
		//Dynamic   : CPU에는 쓰기권한, GPU는 읽기 권한
		//Staging	: 각자에게 모든 권한
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		//buffer형 데이터를 어느 스테이지에 바인딩할지 
		desc.ByteWidth = sizeof(VertexTexture) * 4; //버퍼의 크기

		D3D11_SUBRESOURCE_DATA sub_data; //추가적으로 사용할 데이터는 무엇이냐
		//CPU 데이터를 GPU에 연결할 수 있게 해준다(버퍼로 넘길때)
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = vertices;

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, &sub_data, &vertex_buffer);
		assert(SUCCEEDED(hr));
	}
#pragma endregion


#pragma region Index data,Index Buffer 생성
	//Index Data
	{
		indices = new uint[6]{ 0, 1, 2, 2, 1, 3 };

	}

	//Index Buffer (위의 정점 그리는 순서를 id3d버퍼 형태로 만든다)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = sizeof(uint) * 6;

		D3D11_SUBRESOURCE_DATA sub_data = D3D11_SUBRESOURCE_DATA();
		sub_data.pSysMem = indices;

		auto hr = graphics->GetDevice()->CreateBuffer
		(
			&desc,
			&sub_data,
			&index_buffer
		);
		assert(SUCCEEDED(hr));
	}
#pragma endregion



#pragma region Vertex Shader 생성
	//Vertex Shader
//정점의 공간변환을 해준다
//정점의 개수만큼 호출
	{
		//hlsl을 blob 데이터로 만든다
		auto hr = D3DX11CompileFromFileA //컴파일 함수
		(
			"Texture.hlsl",	//소스파일,원본파일, hlsl 작성한 파일
			nullptr,		//셰이더 매크로작성
			nullptr,		//셰이더 인클루드 사용 가능, 미사용시 널
			"VS",			//함수이름(진입점)
			"vs_5_0",		//셰이더의 모델
			0,				//플래그
			0,				//플래그(엄격한 컴파일을 위한)
			nullptr,		//
			&vs_blob,		//블롭안에 바이너리 데이터가 들어간다(바이트크기의 배열)
			nullptr,		//컴파일중 오류 발생시 에러메세지 받을곳
			nullptr			//HResult 받을곳 이미 hr로 하니까 널
		);//A는 ASCII
		//D3DX11로 시작하는 함수들은 최신 DirectX SDK에는 지원하지 않는다

		assert(SUCCEEDED(hr));
		//블롭으로 셰이더 이진코드가 들어왔으니 버텍스 셰이더를 만든다
		//생성은 device 파이프라인은 컨텍스트

		hr = graphics->GetDevice()->CreateVertexShader
		(
			vs_blob->GetBufferPointer(),	//블롭 정보(바이트 데이터의 시작주소)
			vs_blob->GetBufferSize(),		//블롭 크기(바이트 코드의 길이)
			nullptr,						//
			&vertex_shader					//만들 버텍스 셰이더 포인터
		);
		assert(SUCCEEDED(hr));
	}
#pragma endregion



#pragma region Input Layout 생성
	//Input Layout
//-0.5,-0.5, 0.0, 1.0f, 1.0f, 1.0f, 1.0f,
//어디서 어디까지 좌표고 어디까지가 색상이야? (레이아웃을 잡아야한다)
	{
		//Vertex Buffer와 Vertex Shader의 입력 데이터는 항상 일치 하여야 한다
		// 레이아웃과 버텍스 셰이더의 정보를 가지고 서로 일치하는지 검사를 실시
		// Semantic은 데이터의 출처와 역할에 대한 분명한 의미를 부여하기위한 키워드이다
		//시맨틱 네임 : 작성한 hlsl의 element시맨틱네임과 일치해야함
		//시맨틱 인덱스
		//포멧 :집어 넣는 데이터의 포멧이 무엇인지
		//인풋 슬롯 : 슬롯번호
		//얼라인바이트오프셋 : 정렬된 바이트의 시작점(처음은 0번 바이트시작~11까지)
		//인풋슬롯클래스	: 슬롯에 들어가는 데이터가 버텍스 형(VERTEX_DATA)인지 인스턴싱인지
		//인스턴스데이터스텝레이트 : 인스턴싱 데이터를 얼만큼의 비율로 할지
		//D3D11_INPUT_ELEMENT_DESC layout_desc[]		
		//{
		//	{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		//	{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		//};
		D3D11_INPUT_ELEMENT_DESC layout_desc[]
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		};
		//float4 3차원+1(차후 행렬 변환을 위해) 정석적으로는 4개를 모두 채우는게 맞다
		//Instancing : 하나의 오브젝트에 대한 여러 개의 인스턴스를 한 번의 그리기 호출로 렌더링하는 방식
		auto hr = graphics->GetDevice()->CreateInputLayout
		(
			layout_desc,		//레이아웃 서술자의 배열 시작주소
			2,					//input_element 갯수
			vs_blob->GetBufferPointer(),	//바이트코드
			vs_blob->GetBufferSize(),		//바이트코드 길이
			&input_layout					//반환받을 레이아웃
		);
		assert(SUCCEEDED(hr));
	}
#pragma endregion


#pragma region Pixel Shader 생성
	//Pixel Shader
	{
		auto hr = D3DX11CompileFromFileA
		(
			"Texture.hlsl",
			nullptr,
			nullptr,
			"PS",
			"ps_5_0",
			0,
			0,
			nullptr,
			&ps_blob,
			nullptr,
			nullptr
		);
		assert(SUCCEEDED(hr));

		hr = graphics->GetDevice()->CreatePixelShader
		(
			ps_blob->GetBufferPointer(),
			ps_blob->GetBufferSize(),
			nullptr,
			&pixel_shader
		);
		assert(SUCCEEDED(hr));
	}
#pragma endregion


#pragma region World View Projection 행렬 생성
	//Create World View Projection
	{
		D3DXMatrixIdentity(&world);
		D3DXMatrixIdentity(&view);
		D3DXMatrixIdentity(&projection);
		D3DXVECTOR3 zero = D3DXVECTOR3(0, 0, 0);
		D3DXVECTOR3 zvector = D3DXVECTOR3(0, 0, 1);
		D3DXVECTOR3 yvector = D3DXVECTOR3(0, 1, 0);
		D3DXMatrixLookAtLH			//view행렬 만들기
		(
			&view,					//출력할 행렬
			&zero,	//눈(카메라)의 위치
			&zvector,	//어디를 바라볼지(대상의 위치)
			&yvector		//카메라 축
		);//실제 카메라는 없지만 존재하는것처럼 보이게 카메라 이동시 오브젝트를 반대로 이동

		D3DXMatrixOrthoLH				//직교투영
		(
			&projection,
			Settings::Get().GetWidth(),  //Orthographic평면의 width
			Settings::Get().GetHeight(), //Orthographic평면의 height
			0,							//nearPlane
			1							//farPlane 
		);
		//D3DXMatrixOrthoOffCenterLH(&view, 0, Settings::Get().GetWidth(),Settings::Get().GetHeight(),0,0,1);
		//D3DXMatrixOrthoOffCenterLH(&view, 0, Settings::Get().GetWidth(),0,Settings::Get().GetHeight(),0,1);
		//위의 OrthoLH는 센터가 0,0이었고
		//OffCenterLH는 기준점을 마음대로 이동 가능하다(인자 좌우상하)
		//위의 함수는 윈도우 좌표계처럼 왼쪽 상단이0,0
		//아래 함수는 왼쪽 아래가 0,0
		//좌표계가 뒤집히는경우 back-face culling도 신경써야 한다


		//투영행렬 방식
		//원근 투영과 직교투영이 있다
		//Perspective(원근투영) :앞뒤 물체에 원근감을 부여(시야 공간은 절두체로 표현된다)
		//Orthographic : 원근감의 차이가 없음 일반적인 2d, UI 구성에 사용(시야 공간은 직육면체로 표현된다)
		//				(가까운 평면은 nearPlane, 제일 먼 평면은 farPlane)

		std::cout << "world matrix" << std::endl;
		std::cout << world._11 << " " << world._12 << " " << world._13 << " " << world._14 << std::endl;
		std::cout << world._21 << " " << world._22 << " " << world._23 << " " << world._24 << std::endl;
		std::cout << world._31 << " " << world._32 << " " << world._33 << " " << world._34 << std::endl;
		std::cout << world._41 << " " << world._42 << " " << world._43 << " " << world._44 << std::endl;

		std::cout << std::endl;

		std::cout << "view matrix" << std::endl;
		std::cout << view._11 << " " << view._12 << " " << view._13 << " " << view._14 << std::endl;
		std::cout << view._21 << " " << view._22 << " " << view._23 << " " << view._24 << std::endl;
		std::cout << view._31 << " " << view._32 << " " << view._33 << " " << view._34 << std::endl;
		std::cout << view._41 << " " << view._42 << " " << view._43 << " " << view._44 << std::endl;

		std::cout << std::endl;

		std::cout << "Projection matrix" << std::endl;
		std::cout << projection._11 << " " << projection._12 << " " << projection._13 << " " << projection._14 << std::endl;
		std::cout << projection._21 << " " << projection._22 << " " << projection._23 << " " << projection._24 << std::endl;
		std::cout << projection._31 << " " << projection._32 << " " << projection._33 << " " << projection._34 << std::endl;
		std::cout << projection._41 << " " << projection._42 << " " << projection._43 << " " << projection._44 << std::endl;


		//world 크기 세팅
		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 500, 500, 1);//공간의 크기를 늘려주는 함수 
		//스케일 세팅시 1이상


		std::cout << std::endl;

		std::cout << "Scale matrix" << std::endl;
		std::cout << S._11 << " " << S._12 << " " << S._13 << " " << S._14 << std::endl;
		std::cout << S._21 << " " << S._22 << " " << S._23 << " " << S._24 << std::endl;
		std::cout << S._31 << " " << S._32 << " " << S._33 << " " << S._34 << std::endl;
		std::cout << S._41 << " " << S._42 << " " << S._43 << " " << S._44 << std::endl;


		//world 이동
		D3DXMATRIX T;

		D3DXMatrixTranslation(&T, 0, 0, 0); //공간을 이동시켜주는 함수
		//위치는 0 가능
		//자기할일만 함
		std::cout << std::endl;

		std::cout << "Translation matrix" << std::endl;
		std::cout << T._11 << " " << T._12 << " " << T._13 << " " << T._14 << std::endl;
		std::cout << T._21 << " " << T._22 << " " << T._23 << " " << T._24 << std::endl;
		std::cout << T._31 << " " << T._32 << " " << T._33 << " " << T._34 << std::endl;
		std::cout << T._41 << " " << T._42 << " " << T._43 << " " << T._44 << std::endl;


		D3DXMATRIX R;
		//회전축 X - 끄덕끄덕(위아래) Y - 도리도리(<>좌우) Z - 갸우뚱갸우뚱<오뚜기>
		D3DXMatrixRotationZ(&R, static_cast<float>(D3DXToRadian(45)));  //z축 고정 회전
		//angle은 radian 각
		std::cout << std::endl;

		std::cout << "Rotation matrix" << std::endl;
		std::cout << R._11 << " " << R._12 << " " << R._13 << " " << R._14 << std::endl;
		std::cout << R._21 << " " << R._22 << " " << R._23 << " " << R._24 << std::endl;
		std::cout << R._31 << " " << R._32 << " " << R._33 << " " << R._34 << std::endl;
		std::cout << R._41 << " " << R._42 << " " << R._43 << " " << R._44 << std::endl;

		world = S */* R **/ T;
		//행렬은 교환법칙이 성립되지 않는다
		//스*자(자이로)*이(이동)*공*부
		// S * R * T * R * P
		//스케일행렬 - 회전행렬 - 이동행렬 - 공전행렬 - 부모행렬
		//순으로 계산해야 한다
	}
#pragma endregion


#pragma region Constant Buffer 생성
	//Create Constant Buffer
	{
	D3D11_BUFFER_DESC desc = D3D11_BUFFER_DESC();
	desc.Usage = D3D11_USAGE_DYNAMIC; //CPU -write// GPU - Read
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(TRANSFORM_DATA);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	auto hr = graphics->GetDevice()->CreateBuffer(&desc, nullptr, &gpu_buffer);
	assert(SUCCEEDED(hr));
	}
#pragma endregion


#pragma region Rasterizer State 생성
	//Create Rasterizer State
	{
		D3D11_RASTERIZER_DESC desc = D3D11_RASTERIZER_DESC();

		desc.FillMode = D3D11_FILL_SOLID;   //내부를 칠하거나 다 색칠하거나
		desc.CullMode = D3D11_CULL_BACK;	//뒷면을 잘라내거나 앞면을 잘라내거나
		desc.FrontCounterClockwise = false; //반시계방향으로 그릴지 여부

		auto hr = graphics->GetDevice()->CreateRasterizerState
		(
			&desc,
			&rasterizer_state
		);
		assert(SUCCEEDED(hr));
	}
#pragma endregion

#pragma region  Shader Resource View 생성
	//Create Shader Resource View
	{
		//배열로 이미지 여러개 출력하는 방법
		//auto hr =D3DX11CreateShaderResourceViewFromFileA 
		//(
		//	graphics->GetDevice(),
		//	"./Dx2D_연습/Tree.png",
		//	nullptr,
		//	nullptr,
		//	&shader_resource[0],
		//	nullptr
		//);
		//assert(SUCCEEDED(hr));
		//hr =D3DX11CreateShaderResourceViewFromFileA
		//(
		//	graphics->GetDevice(),
		//	"./Dx2D_연습/forest1.png",
		//	nullptr,
		//	nullptr,
		//	&shader_resource[1],
		//	nullptr
		//);

		auto hr = D3DX11CreateShaderResourceViewFromFileA //
		(
			graphics->GetDevice(),
			"./Dx2D_연습/Tree.png",
			nullptr,
			nullptr,
			&shader_resource,
			nullptr
		);
		assert(SUCCEEDED(hr));
	}
#pragma endregion


#pragma region sampler state 생성
	//Create sampler state
	{
		D3D11_SAMPLER_DESC	desc = D3D11_SAMPLER_DESC();
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //정규화된 좌표를 넘었을때 어떻게 할지
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;//WRAP : 원본픽셀(이밈지)로 나머지 부분을 채운다
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;//CLAMP : 맨 마지막 픽셀로 나머지 부분을 채운다
		//MIRROR: 인접한 픽셀을 반전시켜 나머지 부분을 채운다
		desc.BorderColor[0] = 1;//외곽선
		desc.BorderColor[1] = 0;
		desc.BorderColor[2] = 0;
		desc.BorderColor[3] = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		//이전과 현재 데이터를 비교하는 방법을 정하는 플래그
		//기존의 샘플과 4값을 비교하여 샘플4을 절대통과시키지 않는다~항상통과시킨다 플래그로 통과 
		desc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;//이미지가 축소 확대, 밉맵이 일어날때 어떻게 보정할지
		//Min:이미지가 축소, MAG: 이미지 확대, MIP : 밉맵
		//이미지를 축소하거나 확대할때 여러개의 픽셀을 계속 비교해서 축소(겹치기), 확대(복제)
		//작업을 수행해야 하므로 미리 1/4 크기의 이미지를 구성하는 작업
		//point : 두개의 픽셀이 있다면 한 픽셀으르 폐기
		//linear : 두개의 픽셀의 평균값(조금더 부드러운 이미지가 출력됨)

		desc.MaxAnisotropy = 16;//비등방성 필터링
		desc.MaxLOD = std::numeric_limits<float>::max();//Level of Detail
		desc.MinLOD = std::numeric_limits<float>::min();//거리에따라 보이는 이미지 선명도
		desc.MipLODBias = 0.0f; //밉맵의 인덱스번들에 추가하는 오프셋

		auto hr = graphics->GetDevice()->CreateSamplerState
		(&desc, &sampler_state);
		assert(SUCCEEDED(hr));
	}
#pragma endregion


#pragma region Blend State 생성
	//Create Blend State
	{
		D3D11_BLEND_DESC  desc = D3D11_BLEND_DESC();
		desc.AlphaToCoverageEnable = false;	//Alpha적용 범위
		//Alpha에의해 생기는 검은 외곽선을 제거해 준다
		//잔디, 철조망 등에 사용하는데 그냥 사용하면 jagggy 현상이 발ㄹ생
		//jaggy : 톱니바퀴 모양처럼 이미지가 지글거리는 현상

		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;//알파 혼합
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		//om stage rtv는 8장까지 세팅 가능

		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		auto hr = graphics->GetDevice()->CreateBlendState(&desc, &blend_state);
		assert(SUCCEEDED(hr));
	}
#pragma endregion

}

Execute::~Execute()
{
	SAFE_RELEASE(blend_state);
	SAFE_RELEASE(sampler_state);
	//SAFE_RELEASE(shader_resource[1]);
	//SAFE_RELEASE(shader_resource[0]);
	SAFE_RELEASE(shader_resource);
	SAFE_RELEASE(gpu_buffer);

	SAFE_RELEASE(pixel_shader);
	SAFE_RELEASE(ps_blob);

	SAFE_RELEASE(input_layout);
	
	SAFE_RELEASE(vertex_shader);
	SAFE_RELEASE(vs_blob);

	SAFE_RELEASE(index_buffer);
	SAFE_DELETE_ARRAY(indices);

	SAFE_RELEASE(vertex_buffer);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(graphics);
}

void Execute::Update()
{
	//world._11 = 100;
	//world._22 = 100;  //스케일
	//
	//world._41 = 100;
	//world._42 = 100; 

	//d3dxmatrix - 행우선 행렬
	//gpu - shader - matrix - 열 우선 행렬

	//1, 0, 0, 0  ->행 우선
	//0
	//0
	//0 ->열 우선

	static float radian = 0.0f;
	radian += 0.01f;
	D3DXMATRIX P;
	//공전+부모 행렬
	D3DXMatrixRotationZ(&P, radian);

	//world = world * P;
	//부모행렬을 곱해주면 부모행렬에 종속된다(캐릭터의 뼈가 움직이면 손발 다 같이 이동하는것처럼)
	// 캐릭터 내부에 born을 심는데 이것이 부모 행렬
	// 행렬 결합순서는 매우 중요하다
	
	
	
	//shader matrix가 열 행렬이기 때문에 행우선 행렬인 world를 전치행렬계산해준다

	D3DXMatrixTranspose(&cpu_buffer.world, &world);
	D3DXMatrixTranspose(&cpu_buffer.view ,&view);
	D3DXMatrixTranspose(&cpu_buffer.projection, &projection);

	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	graphics->GetDeviceContext()->Map
	(
		gpu_buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped_subresource
	);

	memcpy_s(mapped_subresource.pData, mapped_subresource.RowPitch, &cpu_buffer, sizeof(cpu_buffer));
	//memcpy(mapped_subresource.pData, &cpu_buffer, sizeof(TRANSFORM_DATA));

	graphics->GetDeviceContext()->Unmap(gpu_buffer, 0);

}

void Execute::Render()
{
	uint stride = sizeof(VertexTexture); //정점 하나의 크기에 대한 정보
	uint offset = 0; //정점 정보 중에 어떤 정점부터 사용할 것인지에 대한 정보
	graphics->Begin();
	{
		//이부분에서 렌더링 실시

		//IA
		graphics->GetDeviceContext()->IASetVertexBuffers(0,1,&vertex_buffer, &stride, &offset);
		graphics->GetDeviceContext()->IASetIndexBuffer(index_buffer,DXGI_FORMAT_R32_UINT,0);
		graphics->GetDeviceContext()->IASetInputLayout(input_layout);//input layout
		graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//삼각형을 그리고 내부를 채워라


		//VS(IA에서 정점이 10개들어온다면 VS도 10번 호출된다. 정점 개수만큼 VS호출)
		graphics->GetDeviceContext()->VSSetShader(vertex_shader,nullptr,0);
		graphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, &gpu_buffer);

		//RS
		graphics->GetDeviceContext()->RSSetState(rasterizer_state);
		//한번 파이프라인에 세팅할 경우 다른 세팅이 없다면 값은 변하지 않는다

		//PS(RS가 지정한 픽셀 개수만큼 호출)
		graphics->GetDeviceContext()->PSSetShader(pixel_shader,nullptr,0);
		graphics->GetDeviceContext()->PSSetShaderResources(0,1,&shader_resource);
		graphics->GetDeviceContext()->PSSetSamplers(0,1,&sampler_state);
		
		//OM
		graphics->GetDeviceContext()->OMSetBlendState(blend_state,nullptr,0xffffffff);
		
		//파이프라인 작동 
		graphics->GetDeviceContext()->DrawIndexed(6,0,0);
		
	}
	graphics->End();
}

