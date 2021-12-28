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

#pragma region Vertex Data ����
	//Vertex Data
	{
		//�� ���������κ��� GPU�� ���� ó���� �Ѵ�
		//�� ���������κ��� ���ÿ� ���� �۾��� �̷�����Ƿ� �׶��̼����� ���� ���� ������ �����ȴ�
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


#pragma region Vertex Buffer ����
	//Vertex Bufffer
	{
		D3D11_BUFFER_DESC desc; //���ҽ��� ������ ������ ����
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_IMMUTABLE;//��� ����� ������
		//GPU�� CPU�� ���� �����͸� �аų� �������� �����ϴ� �÷���
		//Default : GPU�� �����͸� �а� �� �� �ִ�
		//Immutable : GPU�� �����͸� ���� �� �ִ�.
		//Dynamic   : CPU���� �������, GPU�� �б� ����
		//Staging	: ���ڿ��� ��� ����
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		//buffer�� �����͸� ��� ���������� ���ε����� 
		desc.ByteWidth = sizeof(VertexTexture) * 4; //������ ũ��

		D3D11_SUBRESOURCE_DATA sub_data; //�߰������� ����� �����ʹ� �����̳�
		//CPU �����͸� GPU�� ������ �� �ְ� ���ش�(���۷� �ѱ涧)
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = vertices;

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, &sub_data, &vertex_buffer);
		assert(SUCCEEDED(hr));
	}
#pragma endregion


#pragma region Index data,Index Buffer ����
	//Index Data
	{
		indices = new uint[6]{ 0, 1, 2, 2, 1, 3 };

	}

	//Index Buffer (���� ���� �׸��� ������ id3d���� ���·� �����)
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



#pragma region Vertex Shader ����
	//Vertex Shader
//������ ������ȯ�� ���ش�
//������ ������ŭ ȣ��
	{
		//hlsl�� blob �����ͷ� �����
		auto hr = D3DX11CompileFromFileA //������ �Լ�
		(
			"Texture.hlsl",	//�ҽ�����,��������, hlsl �ۼ��� ����
			nullptr,		//���̴� ��ũ���ۼ�
			nullptr,		//���̴� ��Ŭ��� ��� ����, �̻��� ��
			"VS",			//�Լ��̸�(������)
			"vs_5_0",		//���̴��� ��
			0,				//�÷���
			0,				//�÷���(������ �������� ����)
			nullptr,		//
			&vs_blob,		//��Ӿȿ� ���̳ʸ� �����Ͱ� ����(����Ʈũ���� �迭)
			nullptr,		//�������� ���� �߻��� �����޼��� ������
			nullptr			//HResult ������ �̹� hr�� �ϴϱ� ��
		);//A�� ASCII
		//D3DX11�� �����ϴ� �Լ����� �ֽ� DirectX SDK���� �������� �ʴ´�

		assert(SUCCEEDED(hr));
		//������� ���̴� �����ڵ尡 �������� ���ؽ� ���̴��� �����
		//������ device ������������ ���ؽ�Ʈ

		hr = graphics->GetDevice()->CreateVertexShader
		(
			vs_blob->GetBufferPointer(),	//��� ����(����Ʈ �������� �����ּ�)
			vs_blob->GetBufferSize(),		//��� ũ��(����Ʈ �ڵ��� ����)
			nullptr,						//
			&vertex_shader					//���� ���ؽ� ���̴� ������
		);
		assert(SUCCEEDED(hr));
	}
#pragma endregion



#pragma region Input Layout ����
	//Input Layout
//-0.5,-0.5, 0.0, 1.0f, 1.0f, 1.0f, 1.0f,
//��� ������ ��ǥ�� �������� �����̾�? (���̾ƿ��� ��ƾ��Ѵ�)
	{
		//Vertex Buffer�� Vertex Shader�� �Է� �����ʹ� �׻� ��ġ �Ͽ��� �Ѵ�
		// ���̾ƿ��� ���ؽ� ���̴��� ������ ������ ���� ��ġ�ϴ��� �˻縦 �ǽ�
		// Semantic�� �������� ��ó�� ���ҿ� ���� �и��� �ǹ̸� �ο��ϱ����� Ű�����̴�
		//�ø�ƽ ���� : �ۼ��� hlsl�� element�ø�ƽ���Ӱ� ��ġ�ؾ���
		//�ø�ƽ �ε���
		//���� :���� �ִ� �������� ������ ��������
		//��ǲ ���� : ���Թ�ȣ
		//����ι���Ʈ������ : ���ĵ� ����Ʈ�� ������(ó���� 0�� ����Ʈ����~11����)
		//��ǲ����Ŭ����	: ���Կ� ���� �����Ͱ� ���ؽ� ��(VERTEX_DATA)���� �ν��Ͻ�����
		//�ν��Ͻ������ͽ��ܷ���Ʈ : �ν��Ͻ� �����͸� ��ŭ�� ������ ����
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
		//float4 3����+1(���� ��� ��ȯ�� ����) ���������δ� 4���� ��� ä��°� �´�
		//Instancing : �ϳ��� ������Ʈ�� ���� ���� ���� �ν��Ͻ��� �� ���� �׸��� ȣ��� �������ϴ� ���
		auto hr = graphics->GetDevice()->CreateInputLayout
		(
			layout_desc,		//���̾ƿ� �������� �迭 �����ּ�
			2,					//input_element ����
			vs_blob->GetBufferPointer(),	//����Ʈ�ڵ�
			vs_blob->GetBufferSize(),		//����Ʈ�ڵ� ����
			&input_layout					//��ȯ���� ���̾ƿ�
		);
		assert(SUCCEEDED(hr));
	}
#pragma endregion


#pragma region Pixel Shader ����
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


#pragma region World View Projection ��� ����
	//Create World View Projection
	{
		D3DXMatrixIdentity(&world);
		D3DXMatrixIdentity(&view);
		D3DXMatrixIdentity(&projection);
		D3DXVECTOR3 zero = D3DXVECTOR3(0, 0, 0);
		D3DXVECTOR3 zvector = D3DXVECTOR3(0, 0, 1);
		D3DXVECTOR3 yvector = D3DXVECTOR3(0, 1, 0);
		D3DXMatrixLookAtLH			//view��� �����
		(
			&view,					//����� ���
			&zero,	//��(ī�޶�)�� ��ġ
			&zvector,	//��� �ٶ���(����� ��ġ)
			&yvector		//ī�޶� ��
		);//���� ī�޶�� ������ �����ϴ°�ó�� ���̰� ī�޶� �̵��� ������Ʈ�� �ݴ�� �̵�

		D3DXMatrixOrthoLH				//��������
		(
			&projection,
			Settings::Get().GetWidth(),  //Orthographic����� width
			Settings::Get().GetHeight(), //Orthographic����� height
			0,							//nearPlane
			1							//farPlane 
		);
		//D3DXMatrixOrthoOffCenterLH(&view, 0, Settings::Get().GetWidth(),Settings::Get().GetHeight(),0,0,1);
		//D3DXMatrixOrthoOffCenterLH(&view, 0, Settings::Get().GetWidth(),0,Settings::Get().GetHeight(),0,1);
		//���� OrthoLH�� ���Ͱ� 0,0�̾���
		//OffCenterLH�� �������� ������� �̵� �����ϴ�(���� �¿����)
		//���� �Լ��� ������ ��ǥ��ó�� ���� �����0,0
		//�Ʒ� �Լ��� ���� �Ʒ��� 0,0
		//��ǥ�谡 �������°�� back-face culling�� �Ű��� �Ѵ�


		//������� ���
		//���� ������ ���������� �ִ�
		//Perspective(��������) :�յ� ��ü�� ���ٰ��� �ο�(�þ� ������ ����ü�� ǥ���ȴ�)
		//Orthographic : ���ٰ��� ���̰� ���� �Ϲ����� 2d, UI ������ ���(�þ� ������ ������ü�� ǥ���ȴ�)
		//				(����� ����� nearPlane, ���� �� ����� farPlane)

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


		//world ũ�� ����
		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 500, 500, 1);//������ ũ�⸦ �÷��ִ� �Լ� 
		//������ ���ý� 1�̻�


		std::cout << std::endl;

		std::cout << "Scale matrix" << std::endl;
		std::cout << S._11 << " " << S._12 << " " << S._13 << " " << S._14 << std::endl;
		std::cout << S._21 << " " << S._22 << " " << S._23 << " " << S._24 << std::endl;
		std::cout << S._31 << " " << S._32 << " " << S._33 << " " << S._34 << std::endl;
		std::cout << S._41 << " " << S._42 << " " << S._43 << " " << S._44 << std::endl;


		//world �̵�
		D3DXMATRIX T;

		D3DXMatrixTranslation(&T, 0, 0, 0); //������ �̵������ִ� �Լ�
		//��ġ�� 0 ����
		//�ڱ����ϸ� ��
		std::cout << std::endl;

		std::cout << "Translation matrix" << std::endl;
		std::cout << T._11 << " " << T._12 << " " << T._13 << " " << T._14 << std::endl;
		std::cout << T._21 << " " << T._22 << " " << T._23 << " " << T._24 << std::endl;
		std::cout << T._31 << " " << T._32 << " " << T._33 << " " << T._34 << std::endl;
		std::cout << T._41 << " " << T._42 << " " << T._43 << " " << T._44 << std::endl;


		D3DXMATRIX R;
		//ȸ���� X - ��������(���Ʒ�) Y - ��������(<>�¿�) Z - ����װ����<���ѱ�>
		D3DXMatrixRotationZ(&R, static_cast<float>(D3DXToRadian(45)));  //z�� ���� ȸ��
		//angle�� radian ��
		std::cout << std::endl;

		std::cout << "Rotation matrix" << std::endl;
		std::cout << R._11 << " " << R._12 << " " << R._13 << " " << R._14 << std::endl;
		std::cout << R._21 << " " << R._22 << " " << R._23 << " " << R._24 << std::endl;
		std::cout << R._31 << " " << R._32 << " " << R._33 << " " << R._34 << std::endl;
		std::cout << R._41 << " " << R._42 << " " << R._43 << " " << R._44 << std::endl;

		world = S */* R **/ T;
		//����� ��ȯ��Ģ�� �������� �ʴ´�
		//��*��(���̷�)*��(�̵�)*��*��
		// S * R * T * R * P
		//��������� - ȸ����� - �̵���� - ������� - �θ����
		//������ ����ؾ� �Ѵ�
	}
#pragma endregion


#pragma region Constant Buffer ����
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


#pragma region Rasterizer State ����
	//Create Rasterizer State
	{
		D3D11_RASTERIZER_DESC desc = D3D11_RASTERIZER_DESC();

		desc.FillMode = D3D11_FILL_SOLID;   //���θ� ĥ�ϰų� �� ��ĥ�ϰų�
		desc.CullMode = D3D11_CULL_BACK;	//�޸��� �߶󳻰ų� �ո��� �߶󳻰ų�
		desc.FrontCounterClockwise = false; //�ݽð�������� �׸��� ����

		auto hr = graphics->GetDevice()->CreateRasterizerState
		(
			&desc,
			&rasterizer_state
		);
		assert(SUCCEEDED(hr));
	}
#pragma endregion

#pragma region  Shader Resource View ����
	//Create Shader Resource View
	{
		//�迭�� �̹��� ������ ����ϴ� ���
		//auto hr =D3DX11CreateShaderResourceViewFromFileA 
		//(
		//	graphics->GetDevice(),
		//	"./Dx2D_����/Tree.png",
		//	nullptr,
		//	nullptr,
		//	&shader_resource[0],
		//	nullptr
		//);
		//assert(SUCCEEDED(hr));
		//hr =D3DX11CreateShaderResourceViewFromFileA
		//(
		//	graphics->GetDevice(),
		//	"./Dx2D_����/forest1.png",
		//	nullptr,
		//	nullptr,
		//	&shader_resource[1],
		//	nullptr
		//);

		auto hr = D3DX11CreateShaderResourceViewFromFileA //
		(
			graphics->GetDevice(),
			"./Dx2D_����/Tree.png",
			nullptr,
			nullptr,
			&shader_resource,
			nullptr
		);
		assert(SUCCEEDED(hr));
	}
#pragma endregion


#pragma region sampler state ����
	//Create sampler state
	{
		D3D11_SAMPLER_DESC	desc = D3D11_SAMPLER_DESC();
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //����ȭ�� ��ǥ�� �Ѿ����� ��� ����
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;//WRAP : �����ȼ�(�̹���)�� ������ �κ��� ä���
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;//CLAMP : �� ������ �ȼ��� ������ �κ��� ä���
		//MIRROR: ������ �ȼ��� �������� ������ �κ��� ä���
		desc.BorderColor[0] = 1;//�ܰ���
		desc.BorderColor[1] = 0;
		desc.BorderColor[2] = 0;
		desc.BorderColor[3] = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		//������ ���� �����͸� ���ϴ� ����� ���ϴ� �÷���
		//������ ���ð� 4���� ���Ͽ� ����4�� ���������Ű�� �ʴ´�~�׻������Ų�� �÷��׷� ��� 
		desc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;//�̹����� ��� Ȯ��, �Ӹ��� �Ͼ�� ��� ��������
		//Min:�̹����� ���, MAG: �̹��� Ȯ��, MIP : �Ӹ�
		//�̹����� ����ϰų� Ȯ���Ҷ� �������� �ȼ��� ��� ���ؼ� ���(��ġ��), Ȯ��(����)
		//�۾��� �����ؾ� �ϹǷ� �̸� 1/4 ũ���� �̹����� �����ϴ� �۾�
		//point : �ΰ��� �ȼ��� �ִٸ� �� �ȼ����� ���
		//linear : �ΰ��� �ȼ��� ��հ�(���ݴ� �ε巯�� �̹����� ��µ�)

		desc.MaxAnisotropy = 16;//���漺 ���͸�
		desc.MaxLOD = std::numeric_limits<float>::max();//Level of Detail
		desc.MinLOD = std::numeric_limits<float>::min();//�Ÿ������� ���̴� �̹��� ����
		desc.MipLODBias = 0.0f; //�Ӹ��� �ε������鿡 �߰��ϴ� ������

		auto hr = graphics->GetDevice()->CreateSamplerState
		(&desc, &sampler_state);
		assert(SUCCEEDED(hr));
	}
#pragma endregion


#pragma region Blend State ����
	//Create Blend State
	{
		D3D11_BLEND_DESC  desc = D3D11_BLEND_DESC();
		desc.AlphaToCoverageEnable = false;	//Alpha���� ����
		//Alpha������ ����� ���� �ܰ����� ������ �ش�
		//�ܵ�, ö���� � ����ϴµ� �׳� ����ϸ� jagggy ������ �ߤ���
		//jaggy : ��Ϲ��� ���ó�� �̹����� ���۰Ÿ��� ����

		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;//���� ȥ��
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		//om stage rtv�� 8����� ���� ����

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
	//world._22 = 100;  //������
	//
	//world._41 = 100;
	//world._42 = 100; 

	//d3dxmatrix - ��켱 ���
	//gpu - shader - matrix - �� �켱 ���

	//1, 0, 0, 0  ->�� �켱
	//0
	//0
	//0 ->�� �켱

	static float radian = 0.0f;
	radian += 0.01f;
	D3DXMATRIX P;
	//����+�θ� ���
	D3DXMatrixRotationZ(&P, radian);

	//world = world * P;
	//�θ������ �����ָ� �θ���Ŀ� ���ӵȴ�(ĳ������ ���� �����̸� �չ� �� ���� �̵��ϴ°�ó��)
	// ĳ���� ���ο� born�� �ɴµ� �̰��� �θ� ���
	// ��� ���ռ����� �ſ� �߿��ϴ�
	
	
	
	//shader matrix�� �� ����̱� ������ ��켱 ����� world�� ��ġ��İ�����ش�

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
	uint stride = sizeof(VertexTexture); //���� �ϳ��� ũ�⿡ ���� ����
	uint offset = 0; //���� ���� �߿� � �������� ����� �������� ���� ����
	graphics->Begin();
	{
		//�̺κп��� ������ �ǽ�

		//IA
		graphics->GetDeviceContext()->IASetVertexBuffers(0,1,&vertex_buffer, &stride, &offset);
		graphics->GetDeviceContext()->IASetIndexBuffer(index_buffer,DXGI_FORMAT_R32_UINT,0);
		graphics->GetDeviceContext()->IASetInputLayout(input_layout);//input layout
		graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//�ﰢ���� �׸��� ���θ� ä����


		//VS(IA���� ������ 10�����´ٸ� VS�� 10�� ȣ��ȴ�. ���� ������ŭ VSȣ��)
		graphics->GetDeviceContext()->VSSetShader(vertex_shader,nullptr,0);
		graphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, &gpu_buffer);

		//RS
		graphics->GetDeviceContext()->RSSetState(rasterizer_state);
		//�ѹ� ���������ο� ������ ��� �ٸ� ������ ���ٸ� ���� ������ �ʴ´�

		//PS(RS�� ������ �ȼ� ������ŭ ȣ��)
		graphics->GetDeviceContext()->PSSetShader(pixel_shader,nullptr,0);
		graphics->GetDeviceContext()->PSSetShaderResources(0,1,&shader_resource);
		graphics->GetDeviceContext()->PSSetSamplers(0,1,&sampler_state);
		
		//OM
		graphics->GetDeviceContext()->OMSetBlendState(blend_state,nullptr,0xffffffff);
		
		//���������� �۵� 
		graphics->GetDeviceContext()->DrawIndexed(6,0,0);
		
	}
	graphics->End();
}

