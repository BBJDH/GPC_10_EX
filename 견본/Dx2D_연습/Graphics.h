#pragma once

class Graphics final
{
public:
	Graphics();
	~Graphics();

	void Initialize();
	void CreateBackBuffer(uint const & width, uint const & height);

	auto GetDevice() -> ID3D11Device* {return device;} //반환형을 뒤에 적는 방법 사실 안적어도 바뀔일없어서 괜찮지만 나중을 위해
	auto GetDeviceContext() -> ID3D11DeviceContext* {return device_context;}

	void Begin();
	void End();
private:
	//I -> com(Component Object Model) 인터페이스 Description이 필요하다(어떻게 만들지 설명서) 
	ID3D11Device* device			           = nullptr;
	ID3D11DeviceContext* device_context        = nullptr;		//장치의 내용
	IDXGISwapChain* swap_chain		           = nullptr;		//백버퍼를 관리하는 DXGI
	//DXGI는 제조사마다 다른 그래픽카드를 프로그래머가 편하게 사용할 수있도록 도와준다
	ID3D11RenderTargetView* render_target_view = nullptr; //도화지
	D3D11_VIEWPORT viewport					   = { 0 };//그릴 영역(보여질영역)
	D3DXCOLOR clear_color					   = 0xff555566;//그동안 쓰던 0~255 범위를 1로 정규화 시켜둠
	//화면 초기화 색상 담당

};
//Rendering Pipeline
//IA -VS-RS-PS-OM