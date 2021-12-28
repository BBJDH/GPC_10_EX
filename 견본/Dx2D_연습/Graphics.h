#pragma once

class Graphics final
{
public:
	Graphics();
	~Graphics();

	void Initialize();
	void CreateBackBuffer(uint const & width, uint const & height);

	auto GetDevice() -> ID3D11Device* {return device;} //��ȯ���� �ڿ� ���� ��� ��� ����� �ٲ��Ͼ�� �������� ������ ����
	auto GetDeviceContext() -> ID3D11DeviceContext* {return device_context;}

	void Begin();
	void End();
private:
	//I -> com(Component Object Model) �������̽� Description�� �ʿ��ϴ�(��� ������ ����) 
	ID3D11Device* device			           = nullptr;
	ID3D11DeviceContext* device_context        = nullptr;		//��ġ�� ����
	IDXGISwapChain* swap_chain		           = nullptr;		//����۸� �����ϴ� DXGI
	//DXGI�� �����縶�� �ٸ� �׷���ī�带 ���α׷��Ӱ� ���ϰ� ����� ���ֵ��� �����ش�
	ID3D11RenderTargetView* render_target_view = nullptr; //��ȭ��
	D3D11_VIEWPORT viewport					   = { 0 };//�׸� ����(����������)
	D3DXCOLOR clear_color					   = 0xff555566;//�׵��� ���� 0~255 ������ 1�� ����ȭ ���ѵ�
	//ȭ�� �ʱ�ȭ ���� ���

};
//Rendering Pipeline
//IA -VS-RS-PS-OM