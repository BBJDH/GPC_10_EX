//#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")
//#pragma comment(lib,"d3d11.lib")

#include<cassert>
#include<d3d11.h>
#include"../FreeImage.h"
//dll : ���̳��� ��ũ ���̺귯�� : �������� �ʿ��� ���뿡���ؼ� ���̺귯�� �ε尡 ����

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

        namespace Buffer
        {
            ID3D11Buffer* Vertex;
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
                    D3D11_INPUT_ELEMENT_DESC const Descriptor[2]
                    {
                        {"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0},
                        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1}
                    };


                    ID3D11InputLayout* InputLayout = nullptr;


                    MUST(Device->CreateInputLayout
                    (
                        Descriptor,
                        2, //sizeof(Descriptor)/sizeof(*Descriptor) //_ARRAYSIZE(Descriptor) -> winapi���� �����ϴ� ��ũ��
                        Bytecode,
                        sizeof(Bytecode),
                        &InputLayout
                    ));

                    DeviceContext->IASetInputLayout(InputLayout);
                    InputLayout->Release();
                }
                {
                    ID3D11VertexShader* VertexShader = nullptr;

                    MUST(Device->CreateVertexShader
                    (
                        Bytecode,
                        sizeof(Bytecode),
                        nullptr,
                        &VertexShader
                    ));
                    DeviceContext->VSSetShader(VertexShader,nullptr,0);

                    VertexShader->Release();

                }
            }
            {
                //Pixel Shader
                #include "../Shader/Bytecode/Pixel.h"
                {
                    ID3D11PixelShader* PixelShader = nullptr;

                    MUST(Device->CreatePixelShader
                    (
                        Bytecode,
                        sizeof(Bytecode),
                        nullptr,
                        &PixelShader
                    ));

                    DeviceContext->PSSetShader(PixelShader, nullptr, 0);

                    PixelShader->Release();
                }
            }
            {
            }
            {
                //Vertex buffer


                float const Coordinates[4][4]
                {
                    { -0.5f, +0.5f, 0.0f, 1.0f},
                    { +0.5f, +0.5f, 0.0f, 1.0f},
                    { -0.5f, -0.5f, 0.0f, 1.0f},
                    { +0.5f, -0.5f, 0.0f, 1.0f}
                };

                D3D11_BUFFER_DESC const Descritor 
                {
                    sizeof(Coordinates),
                    D3D11_USAGE_IMMUTABLE,  //�Һ�
                    D3D11_BIND_VERTEX_BUFFER,
                    0,  //�������� �аų� �� ������ �ִٸ� ����
                };


                D3D11_SUBRESOURCE_DATA Subresource = D3D11_SUBRESOURCE_DATA();

                Subresource.pSysMem           = Coordinates;
                Subresource.SysMemPitch       = 0; //2���� �̻��� �����Ϳ��� ���
                                                   //�ڿ��� �࿡���� ����Ʈwidth�� ���ؼ� �ִ´�
                Subresource.SysMemSlicePitch  = 0;     //3������ ������� �ϴ� ������
                
                ID3D11Buffer* buffer = nullptr;


                MUST(Device->CreateBuffer
                (
                    &Descritor,
                    &Subresource,
                    &buffer
                ));


                UINT const Stride = sizeof(*Coordinates);//���� �ϳ��� ũ��(����) 
                UINT const Offset = 0;//� �ǳ� ����
                DeviceContext->IASetVertexBuffers
                (
                    0,
                    1,
                    &buffer,
                    &Stride,
                    &Offset
                );

                buffer->Release();

                DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
                //D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP 
                //-> 0 1 2, 1(�߰���) 2 3(�߰���) ������ �׸���
            }
            {
                //Vertex buffer


                D3D11_BUFFER_DESC Descritor
                {
                    sizeof(float[4][4]),
                    D3D11_USAGE_DYNAMIC,  
                    D3D11_BIND_VERTEX_BUFFER,
                    D3D10_CPU_ACCESS_WRITE,
                };

                Device->CreateBuffer
                (
                    &Descritor,
                    nullptr,
                    &Buffer::Vertex
                );


                UINT const Stride = sizeof(float[4]);//���� �ϳ��� ũ��(����) 
                UINT const Offset = 0;//� �ǳ� ����
                DeviceContext->IASetVertexBuffers
                (
                    1,
                    1,
                    &Buffer::Vertex,
                    &Stride,
                    &Offset
                );


            }
            return 0;
        }
        case WM_APP:
        {
            {
                //��ȣ ����(Mutual Exclusion, Mutex)
                // �� ��ü�� �ڿ��� �������� �� �ٸ� ��ü�� ���� �Ұ��ϰ� �ϵ��� ��
                // ->race condition�� ����
                //

                //Ư���ڿ��� ���� gpu�� ������ �����ϰ� cpu�κ��� �ڿ��� ����(lock)


                D3D11_MAPPED_SUBRESOURCE subresource = D3D11_MAPPED_SUBRESOURCE();

                MUST(DeviceContext->Map
                (
                    Buffer::Vertex,
                    0,
                    D3D11_MAP_WRITE_DISCARD,        //���� �����͸� ������ �����͸� ����
                    0,
                    &subresource
                ));
                {
                    //�Է��� �ڿ� ����
                    float static Intensity = 0.0f;
                    float static Delta = 5.0f/60.0f;
                    float const Elements[4][4]
                    {
                        {Intensity,      0.0f,      0.0f, 1.0f},
                        {     0.0f, Intensity,      0.0f, 1.0f},
                        {     0.0f,      0.0f, Intensity, 1.0f},
                        {Intensity, Intensity, Intensity, 1.0f},
                    };
                    Intensity += Delta;

                    if (Intensity < 0.0f or 5.0f < Intensity)
                        Delta *= -1;

                    memcpy_s(subresource.pData,subresource.RowPitch,Elements,sizeof(Elements));

                }
                DeviceContext->Unmap(Buffer::Vertex, 0);

            }
            {
                //������ �׸��� ����
                //DrawIndexed -> index������ �׸���
                //�ε��� ����, ���� �ε���, 0���� �� �ε���
                DeviceContext->Draw(4,0);

                MUST(SwapChain->Present(1,0));

                float const Color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

                DeviceContext->ClearRenderTargetView(RenderTargetView,Color);
            }

            return 0;
        }

        case WM_DESTROY:
        {
            DeviceContext->ClearState();
            //���� ���������� ���ջ��¸� ��� �ʱ�ȭ
            RenderTargetView->Release();

            Buffer::Vertex->Release();


            SwapChain->Release();
            DeviceContext->Release();
            Device->Release();
            PostQuitMessage(0); 

            return 0;
        }
        case WM_SIZE:
        {
            {
                D3D11_VIEWPORT Viewport
                {
                     0.0f,
                     0.0f,
                     static_cast<float>(LOWORD(lParameter)),
                     static_cast<float>(HIWORD(lParameter)),

                };

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