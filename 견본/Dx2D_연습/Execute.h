#pragma once

struct VertexColor
{
	D3DXVECTOR3 position;
	D3DXCOLOR color;
};

struct VertexTexture
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 uv; //texcoord :Texture Coordinate (�ؽ��� ��ǥ��)
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

	void Update(); //������ ������Ʈ
	void Render();	//ȭ�鿡 ������ �ǽ�
private:
	class Graphics* graphics =nullptr; //class Graphics ���漱��� ����

	VertexTexture*  vertices = nullptr;
	ID3D11Buffer* vertex_buffer = nullptr;

	uint * indices = nullptr;
	ID3D11Buffer* index_buffer = nullptr;

	ID3D11InputLayout* input_layout = nullptr; 
	//���ؽ� ���̴� ���������� ������ ���踦 ������

	ID3D11VertexShader* vertex_shader = nullptr;
	ID3DBlob* vs_blob =nullptr;
	//Blob : Binary Large Object :���� ������ ����
	//HLSL�� �ڵ带 2�� ���Ϸ� �����ϴ� ��ü(���� HLSL �ۼ��� ����Ǿ���Ѵ�)

	ID3D11PixelShader* pixel_shader = nullptr;
	ID3DBlob* ps_blob = nullptr;

	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;

	TRANSFORM_DATA cpu_buffer;
	ID3D11Buffer* gpu_buffer = nullptr; //constant buffer(��� ����)

	ID3D11RasterizerState * rasterizer_state = nullptr;

	//ID3D11ShaderResourceView * shader_resource[2]; //�̹��� �迭�� �����ϴ¹��
	ID3D11ShaderResourceView * shader_resource = nullptr;
	//�迭���� ������ �׸��� ���
	ID3D11SamplerState * sampler_state = nullptr;
	ID3D11BlendState * blend_state = nullptr;

};
#pragma region ����
//���
//�������(����� �׻� ���� ��ķ� �ʱ�ȭ �ؾ��Ѵ�)
/*  x y z w
1 0 0 0  =>x�� ���ϴº���
0 1 0 0  =>y�� ���ϴº���
0 0 1 0  =>z�� ���ϴº���
0 0 0 1  =>0��(��ġ)
=>xyz������ 1�� ���ϴ� �߽���ǥ�� 0,0,0 ���
������ Ŀ���� ���� �����͵鵵 ���� Ŀ����
������ �̵��ϸ� ���� �����͵鵵 �̵��Ѵ�
*/

//������ ���� 
// -> IA - VS - RS - PS - OM 
// 
// IA : Input Assembler Stage
// -Vertex ����ü ������ �ʿ��ϴ� 
// vertex ����ü�� ���̷�ƮX���� ���������ʴ´�. ����ڸ��� �䱸������ �پ��ϱ� ������
// -��ǥ�� �÷� Ȥ�� ��ǥ ���� �÷� ���� �ƴϸ��÷��� �ΰ� ��� ����ڰ� ���� ���谡���ϴ�
// 
// Shader : GPU�� ���۽�Ű�� �Լ����� ����
// VS : Vertex Shader -> shader : HLSL(High Level Shader Languege)
// ������ �������� �̷���� �־����� ���꼺�� �������� HLSL�� ����
// (OpenGL�� GLSL ���)
// IA, RS, OM �ܰ�� �ڵ� ������ �Ұ�,
// VS, PS�� �ڵ��� ����
// VS : Vertex Shader -> shader : HLSL(High Level Shader Languege)
// -Transform�� ���ش�(clip space)
// 
// RS : Rasterrizer
// Clipping : ��������
// NDC		: ����ȭ ��Ű��
// Back-face Culling : ������ �ʴ� ���� ����
// Viewport Transform : �ٽ� �÷��ְ�(����Ʈ �������)
// Scan Transform : �ȼ����̴��� ĥ�� ���� ����
// 
// (NDC ��ǥ��)(�������� ������ȯ 3�������� 2���� �׸����� 0~1 �Ǵ� -1~1�� ����ȭ)
// NDC ��ǥ���?(0~ 1024 �̷��� ��ǥ���� 0~1 �Ǵ� -1~1�� ����ȭ�����Ѵ� rgba ó��)
// -ó�� �簢���� ����Ҷ� �־��� ���ؽ� ������ 1.0f -0.5f...�ε� 
// ����� ������ ȭ���� 100���� 50���� ����(ȭ���� �������κ���)�� �������� ������ 
// 3d������(����)�� 2d ������(�ȼ�)�� ��ȯ
// ���ؽ� ���̴����� �Ѿ�� ������ ȭ�鿡�°�(����Ʈ) ũ�� �÷��ش�
// (����ȭ�� ���� ������ �߶󳻰� �յ� Ȯ�� �ϰ� ����Ʈ���� ����ȭ�Ȱ��� �÷��ش�)
// �ȼ� ���̴� �ܰ�� �Ѿ �ȼ����� �������ش�
// 
// 
// 
//
// PS : �� �ȼ��� �����ϴ� �� �̹��� ������ ���� 
//


//����
//
// Local      : ������ǥ��(�ڱ������ȿ� �߽��� �������ִ� ��ǥ��)- ������ ���� �� �������κ����� ��ǥ�� ����
// World      : ������ǥ�踦 ����� �����ͼ� ���� ��ǥ�� �÷����� ����
// View	      :	(ī�޶�)�������� ���� ��ü ������ ���� �����ִ� 3���� ����
// Projection : �����ִ� ������ ����ó�� 2d�� �翵�� ����
//				(DX���� Projection�� Clip Space�� �����Ͽ� �ִ�)
// Clip       : Ŭ������ �ϱ����� ����ȭ�� ����(Clip Space)
//				 - �׳� �߶󳻴� �۾����ϱ⿡�� �������� ������ �ʹ� ũ�⿡ ����ȭ�� �����Ѵ�
//				 - Homogeneous space - ��������
//               ���������̶�? (1,0,0) - ��ǥ���� ���⺤������ �˼��� ���� 
//							->(1,0,0,0)����(xyzw)�� �ϳ� �߰��ؼ� �ĺ������ϰ��Ѵ�
//							->w���� 0�̶�� ����
//							->w���� 1�̻��̶�� ��ġ(1�� ����Ѵ�)
// 
// (x��ǥ,������ǥ)  1������ �ΰ����� �������� ����
// |5y
// |--------------------0--------- 4y (21,4)
// |3y
// |------0----------------------- 2y (7,2)
// |1y
// -> ���� ��� ������ �ұ�? 
// : ������������ �ܸ��� �߶� ������ �� ������ǥ�� ���߸� ���� �������� �����Եȴ�
// 
// 
// =======================������� ������ VS���� ���=========================
// =======================���ķ� RS���� ���=================================
// 
// NDC        : - Euclidean space
// Viewport   : RS���ø�ŭ �ٽ� ũ���������ִ� ����
// 
//  
#pragma endregion
