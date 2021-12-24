//#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")
#pragma comment(lib,"d3d11.lib")

#include<cassert>
#include<d3d11.h>

using namespace std;

#if not defined _DEBUG
#define MUST(Expression) (      (         (Expression)))
#endif
#define MUST(Expression) (assert(SUCCEEDED(Expression)))
//Time, Input, Collision,Rendering, Sound
//������ ���ν����� �߰踦 ���ش�
//�ɼ� - �ؽ�Ʈ ������ ctrl ���� ��ŷ Ű ���� ����

namespace Example
{
    namespace
    {
        ID3D11Device        * Device;         //Create, ...
        ID3D11DeviceContext * DeviceContext;  //Set, Draw, ...������ ������ �߻�ȭ
        IDXGISwapChain      * SwapChain;   //present, ... ȭ����ȯ�� �Ѱ�, ����۰���

        ID3D11InputLayout   * InputLayout;
        ID3D11VertexShader  * VertexShader;
        ID3D11PixelShader   * PixelShader;

        namespace Buffer
        {
            ID3D11Buffer* Vertex;
            ID3D11Buffer* Index;
        }

        ID3D11RenderTargetView * RenderTargetView;

    }


    LRESULT CALLBACK Procedure
    (HWND   const hWindow,UINT   const uMessage,WPARAM const wParameter,  LPARAM const lParameter )
    {

        switch (uMessage)
        {
        case WM_CREATE:
        {
            {
                DXGI_SWAP_CHAIN_DESC Descriptor = DXGI_SWAP_CHAIN_DESC();

                Descriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;//�ϳ��� ä�δ� ��ŭ�� �޸𸮸� �Ҵ�����
                //�� ä�κ� 8��Ʈ �Ҵ�(0~255),UNORM ũ�� 1�� ����ȭ
                //BPP : Bits Per Pixel(32), BPC : Bits Per Channel(8)
                //Signed NORM : -1 ~ +1 , Unsigned NORM : 0 ~ 1

                Descriptor.SampleDesc.Count = 1;
                Descriptor.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                Descriptor.BufferCount      = 1;
                Descriptor.OutputWindow     = hWindow;
                Descriptor.Windowed         = true;
                Descriptor.Flags            = 0;

                //����Ϳ� ����ü���� ���� ���丮���� �����Ǿ�� �Ѵ�
                MUST(D3D11CreateDeviceAndSwapChain
                (
                    nullptr,                            //���������� ���丮�� ����͸� �����
                    D3D_DRIVER_TYPE_HARDWARE,           //����� ��, �ϵ���� Ÿ������ �����ϸ� �� ��� ����͸� �����´�
                    nullptr,
                    D3D11_CREATE_DEVICE_SINGLETHREADED, //�ణ�� �����ս� ���
                    nullptr, //nullptr
                    0, //0 ���� ��ü
                    D3D11_SDK_VERSION,
                    &Descriptor,
                    &SwapChain,
                    &Device,
                    nullptr,
                    &DeviceContext
                ));
            }
            {
                //TODO : IL ���� �� ����
                //IL ������ ���ؽ� ���ۿ� ���ϱ����� ���̴� �ۼ��� �ʿ�
                //hlsl���� �ۼ��� ���ؽ� �� ���к��� ������ ����� �ʿ�(������(xyzw)16����Ʈ �÷�16����Ʈ)
               
                
                //Vertex Shader

                #include "../Shader/Bytecode/Vertex.h"
                {
                    D3D11_INPUT_ELEMENT_DESC Descriptor[2]
                    {
                        D3D11_INPUT_ELEMENT_DESC()
                    };
                    Descriptor[0].SemanticName          = "POSITION";
                    Descriptor[0].SemanticIndex         = 0;
                    Descriptor[0].Format                = DXGI_FORMAT_R32G32B32A32_FLOAT;    //�ش� ���п� ���� ��������
                    Descriptor[0].InputSlot             = 0;        //���� ��ȣ       
                    Descriptor[0].AlignedByteOffset     = 0;        //���� ����Ʈ ��  (D3D11_APPEND_ALIGNED_ELEMENT �־ ����) 
                    Descriptor[0].InputSlotClass        = D3D11_INPUT_PER_VERTEX_DATA;
                    //D3D11_INPUT_CLASSIFICATION; //���Կ� ���� �����Ͱ� ���ؽ� ���������� �ν��Ͻ� ����������
                    Descriptor[0].InstanceDataStepRate  = 0;    //������ �ν��Ͻ��� �Ѵٸ� �ۼ��ؾ���
                                                        
                    Descriptor[1].SemanticName          = "COLOR";
                    Descriptor[1].SemanticIndex         = 0;
                    Descriptor[1].Format                = DXGI_FORMAT_R32G32B32A32_FLOAT;    
                    Descriptor[1].InputSlot             = 0;        //���� ��ȣ       
                    Descriptor[1].AlignedByteOffset     = D3D11_APPEND_ALIGNED_ELEMENT;  //�� ���п� �پ��ִٶ�� ��ũ��(16)   
                    Descriptor[1].InputSlotClass        = D3D11_INPUT_PER_VERTEX_DATA;
                    Descriptor[1].InstanceDataStepRate  = 0;   

                    MUST(Device->CreateInputLayout
                    (
                        Descriptor,
                        2, //sizeof(Descriptor)/sizeof(*Descriptor) //_ARRAYSIZE(Descriptor) -> winapi���� �����ϴ� ��ũ��
                        Bytecode,
                        sizeof(Bytecode),
                        &InputLayout
                    ));

                    DeviceContext->IASetInputLayout(InputLayout);
                }
                {
                    MUST(Device->CreateVertexShader
                    (
                        Bytecode,
                        sizeof(Bytecode),
                        nullptr,
                        &VertexShader
                    ));
                    DeviceContext->VSSetShader(VertexShader,nullptr,0);

                }
            }
            {
                //Pixel Shader
                #include "../Shader/Bytecode/Pixel.h"
                {
                    MUST(Device->CreatePixelShader
                    (
                        Bytecode,
                        sizeof(Bytecode),
                        nullptr,
                        &PixelShader
                    ));

                    DeviceContext->PSSetShader(PixelShader, nullptr, 0);

                }
            }
            {
                DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            }
            {
                //Vertex buffer


                struct 
                {
                    float Position[4];
                    float Color[4];
                }
                const Vertices[4]
                {
                    {{ -0.5f, +0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f}},
                    {{ +0.5f, +0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 0.0f}},
                    {{ -0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 0.0f}},
                    {{ +0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
                };

                D3D11_BUFFER_DESC Descritor = D3D11_BUFFER_DESC();

                Descritor.ByteWidth           = sizeof(Vertices);
                Descritor.Usage               = D3D11_USAGE_IMMUTABLE;
                Descritor.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
                Descritor.CPUAccessFlags      = 0;  //�������� �аų� �� ������ �ִٸ� ����
                Descritor.MiscFlags           = 0;
                Descritor.StructureByteStride = 0;

                D3D11_SUBRESOURCE_DATA Subresource = D3D11_SUBRESOURCE_DATA();

                Subresource.pSysMem           = Vertices;
                Subresource.SysMemPitch       = 0; //2���� �̻��� �����Ϳ��� ���
                                                   //�ڿ��� �࿡���� ����Ʈwidth�� ���ؼ� �ִ´�
                Subresource.SysMemSlicePitch  ;     //3������ ������� �ϴ� ������
                


                MUST(Device->CreateBuffer
                (
                    &Descritor,
                    &Subresource,
                    &Buffer::Vertex
                ));
                UINT const Stride = sizeof(*Vertices);//���� �ϳ��� ũ��(����) 
                UINT const Offset = 0;//� �ǳ� ����
                DeviceContext->IASetVertexBuffers
                (
                    0,
                    1,
                    &Buffer::Vertex,
                    &Stride,
                    &Offset
                );
            }
            {
                //Index buffer
                //���������ο� �Ѱ��� ���� ����
                struct
                {
                    USHORT Indices[3];
                }
                const Triangles[2]
                {
                    {0, 1, 2},
                    {3, 2, 1}
                };

                D3D11_BUFFER_DESC Descritor = D3D11_BUFFER_DESC();

                Descritor.ByteWidth = sizeof(Triangles);
                Descritor.Usage = D3D11_USAGE_IMMUTABLE;
                Descritor.BindFlags = D3D11_BIND_INDEX_BUFFER;
                Descritor.CPUAccessFlags = 0;  //�������� �аų� �� ������ �ִٸ� ����
                Descritor.MiscFlags = 0;
                Descritor.StructureByteStride = 0;

                D3D11_SUBRESOURCE_DATA Subresource = D3D11_SUBRESOURCE_DATA();

                Subresource.pSysMem = Triangles;
                Subresource.SysMemPitch = 0; //2���� �̻��� �����Ϳ��� ���
                                                   //�ڿ��� �࿡���� ����Ʈwidth�� ���ؼ� �ִ´�
                Subresource.SysMemSlicePitch;     //3������ ������� �ϴ� ������




                MUST(Device->CreateBuffer
                (
                    &Descritor,
                    &Subresource,
                    &Buffer::Index
                ));

                DeviceContext->IASetIndexBuffer
                (
                    Buffer::Index,
                    DXGI_FORMAT_R16_UINT,
                    0
                );
            }

            return 0;
        }
        case WM_APP:
        {
            float const Color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

            DeviceContext->ClearRenderTargetView(RenderTargetView,Color);

            //������ �׸��� ���
            //DrawIndexed -> index������ �׸���
            //�ε��� ����, ���� �ε���, 0���� �� �ε���
            DeviceContext->DrawIndexed(6,0,0);



            MUST(SwapChain->Present(0,0));
            return 0;
        }

        case WM_DESTROY:
        {
            DeviceContext->ClearState();
            RenderTargetView->Release();

            Buffer::Index->Release();
            Buffer::Vertex->Release();

            PixelShader->Release();
            VertexShader->Release();
            InputLayout->Release();
            SwapChain->Release();
            DeviceContext->Release();
            Device->Release();
            PostQuitMessage(0); 

            return 0;
        }
        case WM_SIZE:
        {
            {
                D3D11_VIEWPORT Viewport = D3D11_VIEWPORT();

                Viewport.TopLeftX = 0;
                Viewport.TopLeftY = 0;
                Viewport.Width    = LOWORD(lParameter);
                Viewport.Height   = HIWORD(lParameter);
                Viewport.MinDepth = 0; //�⺻�� (������� ���� ����)
                Viewport.MaxDepth = 0; //�⺻�� (������� ���� ����)

                DeviceContext->RSSetViewports(1,&Viewport);
            }
            {
                if(RenderTargetView!=nullptr)
                {
                    RenderTargetView->Release();
                    MUST(SwapChain->ResizeBuffers
                    (
                        1,
                        LOWORD(lParameter),
                        HIWORD(lParameter),
                        DXGI_FORMAT_B8G8R8A8_UNORM,
                        0
                    ));
                }
                {
                    ID3D11Texture2D * Texture2D = nullptr;
                    //����ü���� ���۴� IDXGISurface1 ���·� �����Ѵ�
                    //DXGISurface1* ���� ��ũ�������� �ڷ���
                    MUST(SwapChain->GetBuffer
                    (
                        0,
                        IID_PPV_ARGS(&Texture2D) //PPV : ���̵� ���� ������, IID�� ���̵� ���������͸� ������ ��ũ��
                    ));
                    {
                        MUST(Device->CreateRenderTargetView(Texture2D,nullptr,&RenderTargetView));
                    }
                    Texture2D->Release();
                }
                DeviceContext->OMSetRenderTargets(1,&RenderTargetView,nullptr); 

            }
            return 0;
        }
        
        default:
        {
            return DefWindowProc(hWindow, uMessage, wParameter, lParameter);
        }
        }
    }

}