#include"stdafx.h"
#include"Graphics.h"

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
	SAFE_RELEASE(render_target_view);
	SAFE_RELEASE(device);
	SAFE_RELEASE(device_context);
	SAFE_RELEASE(swap_chain);
}

void Graphics::Initialize()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC)); //가급적 이렇게 초기화하자
	desc.BufferDesc.Width = 0;
	desc.BufferDesc.Height =0;
	desc.BufferDesc.RefreshRate.Numerator =60;   //분자
	desc.BufferDesc.RefreshRate.Denominator =1;		//분모
	desc.BufferDesc.Format =DXGI_FORMAT_R8G8B8A8_UNORM; //하나의 채널당 얼만큼의 메모리를 할당할지
	//각 채널별 8비트 할당(0~255),UNORM 크기 1로 정규화
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferCount = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//위의 버퍼를 어떻게 사용할지()
	desc.SampleDesc.Count = 1;//각각의 픽셀을 얼만큼 샘플링 할지 1이면 기본값
	desc.SampleDesc.Quality = 0;//얼만큼의 수준으로 샘플링 0이면 기본값
	desc.OutputWindow = Settings::Get().GetWindowHandle();  //출력할 윈도우
	desc.Windowed = TRUE; //창모드
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //버퍼 스왑이후 폐기해라

	std::vector<D3D_FEATURE_LEVEL>feature_levels
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	auto hr = D3D11CreateDeviceAndSwapChain
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE, //하드웨어 가속을 사용
		nullptr,
		0,
		feature_levels.data(),  //널포인터로 넣으면 11~ 9까지의 레벨들이 들어감
		feature_levels.size(),	//피처레벨에 몇개 넣었는지
		D3D11_SDK_VERSION,
		&desc,
		&swap_chain,
		&device,
		nullptr,
		&device_context
	);
	assert(SUCCEEDED(hr));
}
void Graphics::CreateBackBuffer(uint const& width, uint const& height)
{
	auto hr = swap_chain->ResizeBuffers //버퍼를 수정한다 기존 bufferdesc를 그대로 써도 무방
	(
		0,			//바꾸지 않을거라서 0
		width,		//가로는 0에서 width로바뀐다 
		height,		//세로는 0에서 height로바뀐다
		DXGI_FORMAT_UNKNOWN, //포멧은 위에서 잡아놨기 때문에 기본값(UNKNOWN)
		0			//플래그 안줄거라면 0
	);
	//위에 desc로 정의한내용에서 가로 세로가 변동되고 나머진 그대로 버퍼 크기가 정해진다
	assert(SUCCEEDED(hr));

	ID3D11Texture2D* back_buffer = nullptr; //
	hr = swap_chain->GetBuffer   //스왑체인안에서 버퍼를 꺼낸다
	(
		0,						  //몇번째 스왑체인의 버퍼를 꺼낼지
		__uuidof(ID3D11Texture2D),//IID_ID3D11Texture2D, 
								  //UUID Universally Unique Identifier
								  //GUID Globally Unique Identifier
		reinterpret_cast<void**>(&back_buffer)
	);
	assert(SUCCEEDED(hr));
	hr = device->CreateRenderTargetView	//스왑에서 꺼낸 버퍼를 2D텍스쳐로 받았으니 이제 도화지에 옮긴다 
	(
		back_buffer,		//꺼낸 버퍼
		nullptr,			//스왑체인에서 이미 desc가 서술되어있으므로 변동없이 nullptr
		&render_target_view
	);
	assert(SUCCEEDED(hr));

	//뷰포트(보여질 영역) 초기화
	viewport.TopLeftX =0.0f;		//시작점
	viewport.TopLeftY =0.0f;
	viewport.Width  = static_cast<float>(width);
	viewport.Height = static_cast<float>(height); //종단점
	viewport.MinDepth = 0.0f;		//깊이
	viewport.MaxDepth = 1.0f;		//2d니까 1정도만 주자

	SAFE_RELEASE(back_buffer);
}
void Graphics::Begin()	//후면 그리기 시작
{
	//도화지 세팅,도화지 클리어
	//렌더링 파이프라인 스테이지에 자원을 세팅하기위해서는 device context를 사용한다
	//ID3D11RenderTargetView* rtvs[] = {render_target_view}; //정석적인 방법
	device_context->OMSetRenderTargets(1, /*rtvs*/&render_target_view, nullptr);//OM스테이지
	//내가 어디에 어떻게 그릴것인가 세팅
	//depth 관련 변수는 뷰의 깊이와 관련되어있다 2D를 그리므로 일단 널포인터
	//OM에 들어갈 수 있는 렌더 타겟의 개수는 동시에 8장이며 반드시 viewport도 같이 세팅해주어야한다

	device_context->RSSetViewports(1,&viewport); //RS 스테이지
	device_context->ClearRenderTargetView(render_target_view,clear_color);
}
void Graphics::End()
{
	//그려진 후면 다그렸으니 전면으로 보내자

	auto hr = swap_chain->Present(1,0);//수직동기화 1 , 플레그 쓰지않음 0
	//후면 버퍼를 전면 버퍼로 출력
	assert(SUCCEEDED(hr));
}
//Aliasing
//픽셀 표현으로 생기는 계단 현상
//Anti Aliasing
//계단 현상을 제거하는 방식

//SSAA(Super Samling Anti Aliasing) : 그림 전체를 축소시키거나 확대시키거나 하여 보정하며 부드럽게 보이게함
//MSAA(Multi Sampling Anti Aliasing) : 특정(외곽선) 부분만 보정하여 매끄럽게 한다



//DirectX에서 제공하는 자원들은 ID3D11Resource를 상속받고 있다
//							ID3D11Resource
//ID3D11Buffer								ID3D11Texture1D, ID3D11Texture2D, ID3D11Texture3D
//구조체로 만들어서 넘기는자원					이미지화 되어있는 자원들
//											데이터를 그릴수도, 이미 만들어진 그림을 가지고 있을수도있다
//											때문에 Resource View라는 용도를 설명해주는 것
//											렌더타겟 뷰또한 Resource View의 일종
//											Resource View
//											-ID3D11RenderTargetView(직접 그리기위한 뷰),
//											 ID3D11ShaderResourceView(png,jpg등 만들어진 이미지를 전달하기위한 뷰),
//											 ID3D11DepthStencilView, ID3D11UnorderedAccessView(위의 두개가 쓰기용, 읽기용 이라면 이것은 읽고 쓰기용)
//											
// 
//
