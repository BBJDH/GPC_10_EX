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
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC)); //������ �̷��� �ʱ�ȭ����
	desc.BufferDesc.Width = 0;
	desc.BufferDesc.Height =0;
	desc.BufferDesc.RefreshRate.Numerator =60;   //����
	desc.BufferDesc.RefreshRate.Denominator =1;		//�и�
	desc.BufferDesc.Format =DXGI_FORMAT_R8G8B8A8_UNORM; //�ϳ��� ä�δ� ��ŭ�� �޸𸮸� �Ҵ�����
	//�� ä�κ� 8��Ʈ �Ҵ�(0~255),UNORM ũ�� 1�� ����ȭ
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferCount = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//���� ���۸� ��� �������()
	desc.SampleDesc.Count = 1;//������ �ȼ��� ��ŭ ���ø� ���� 1�̸� �⺻��
	desc.SampleDesc.Quality = 0;//��ŭ�� �������� ���ø� 0�̸� �⺻��
	desc.OutputWindow = Settings::Get().GetWindowHandle();  //����� ������
	desc.Windowed = TRUE; //â���
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //���� �������� ����ض�

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
		D3D_DRIVER_TYPE_HARDWARE, //�ϵ���� ������ ���
		nullptr,
		0,
		feature_levels.data(),  //�������ͷ� ������ 11~ 9������ �������� ��
		feature_levels.size(),	//��ó������ � �־�����
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
	auto hr = swap_chain->ResizeBuffers //���۸� �����Ѵ� ���� bufferdesc�� �״�� �ᵵ ����
	(
		0,			//�ٲ��� �����Ŷ� 0
		width,		//���δ� 0���� width�ιٲ�� 
		height,		//���δ� 0���� height�ιٲ��
		DXGI_FORMAT_UNKNOWN, //������ ������ ��Ƴ��� ������ �⺻��(UNKNOWN)
		0			//�÷��� ���ٰŶ�� 0
	);
	//���� desc�� �����ѳ��뿡�� ���� ���ΰ� �����ǰ� ������ �״�� ���� ũ�Ⱑ ��������
	assert(SUCCEEDED(hr));

	ID3D11Texture2D* back_buffer = nullptr; //
	hr = swap_chain->GetBuffer   //����ü�ξȿ��� ���۸� ������
	(
		0,						  //���° ����ü���� ���۸� ������
		__uuidof(ID3D11Texture2D),//IID_ID3D11Texture2D, 
								  //UUID Universally Unique Identifier
								  //GUID Globally Unique Identifier
		reinterpret_cast<void**>(&back_buffer)
	);
	assert(SUCCEEDED(hr));
	hr = device->CreateRenderTargetView	//���ҿ��� ���� ���۸� 2D�ؽ��ķ� �޾����� ���� ��ȭ���� �ű�� 
	(
		back_buffer,		//���� ����
		nullptr,			//����ü�ο��� �̹� desc�� �����Ǿ������Ƿ� �������� nullptr
		&render_target_view
	);
	assert(SUCCEEDED(hr));

	//����Ʈ(������ ����) �ʱ�ȭ
	viewport.TopLeftX =0.0f;		//������
	viewport.TopLeftY =0.0f;
	viewport.Width  = static_cast<float>(width);
	viewport.Height = static_cast<float>(height); //������
	viewport.MinDepth = 0.0f;		//����
	viewport.MaxDepth = 1.0f;		//2d�ϱ� 1������ ����

	SAFE_RELEASE(back_buffer);
}
void Graphics::Begin()	//�ĸ� �׸��� ����
{
	//��ȭ�� ����,��ȭ�� Ŭ����
	//������ ���������� ���������� �ڿ��� �����ϱ����ؼ��� device context�� ����Ѵ�
	//ID3D11RenderTargetView* rtvs[] = {render_target_view}; //�������� ���
	device_context->OMSetRenderTargets(1, /*rtvs*/&render_target_view, nullptr);//OM��������
	//���� ��� ��� �׸����ΰ� ����
	//depth ���� ������ ���� ���̿� ���õǾ��ִ� 2D�� �׸��Ƿ� �ϴ� ��������
	//OM�� �� �� �ִ� ���� Ÿ���� ������ ���ÿ� 8���̸� �ݵ�� viewport�� ���� �������־���Ѵ�

	device_context->RSSetViewports(1,&viewport); //RS ��������
	device_context->ClearRenderTargetView(render_target_view,clear_color);
}
void Graphics::End()
{
	//�׷��� �ĸ� �ٱ׷����� �������� ������

	auto hr = swap_chain->Present(1,0);//��������ȭ 1 , �÷��� �������� 0
	//�ĸ� ���۸� ���� ���۷� ���
	assert(SUCCEEDED(hr));
}
//Aliasing
//�ȼ� ǥ������ ����� ��� ����
//Anti Aliasing
//��� ������ �����ϴ� ���

//SSAA(Super Samling Anti Aliasing) : �׸� ��ü�� ��ҽ�Ű�ų� Ȯ���Ű�ų� �Ͽ� �����ϸ� �ε巴�� ���̰���
//MSAA(Multi Sampling Anti Aliasing) : Ư��(�ܰ���) �κи� �����Ͽ� �Ų����� �Ѵ�



//DirectX���� �����ϴ� �ڿ����� ID3D11Resource�� ��ӹް� �ִ�
//							ID3D11Resource
//ID3D11Buffer								ID3D11Texture1D, ID3D11Texture2D, ID3D11Texture3D
//����ü�� ���� �ѱ���ڿ�					�̹���ȭ �Ǿ��ִ� �ڿ���
//											�����͸� �׸�����, �̹� ������� �׸��� ������ ���������ִ�
//											������ Resource View��� �뵵�� �������ִ� ��
//											����Ÿ�� ����� Resource View�� ����
//											Resource View
//											-ID3D11RenderTargetView(���� �׸������� ��),
//											 ID3D11ShaderResourceView(png,jpg�� ������� �̹����� �����ϱ����� ��),
//											 ID3D11DepthStencilView, ID3D11UnorderedAccessView(���� �ΰ��� �����, �б�� �̶�� �̰��� �а� �����)
//											
// 
//
