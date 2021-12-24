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
//각각의 프로시져로 중계를 해준다
//옵션 - 텍스트 편집기 ctrl 정의 피킹 키 편집 가능

namespace Example
{
    namespace
    {
        ID3D11Device        * Device;         //Create, ...
        ID3D11DeviceContext * DeviceContext;  //Set, Draw, ...파이프 라인을 추상화
        IDXGISwapChain      * SwapChain;   //present, ... 화전면환을 총괄, 백버퍼관리

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

                Descriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;//하나의 채널당 얼만큼의 메모리를 할당할지
                //각 채널별 8비트 할당(0~255),UNORM 크기 1로 정규화
                //BPP : Bits Per Pixel(32), BPC : Bits Per Channel(8)
                //Signed NORM : -1 ~ +1 , Unsigned NORM : 0 ~ 1

                Descriptor.SampleDesc.Count = 1;
                Descriptor.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                Descriptor.BufferCount      = 1;
                Descriptor.OutputWindow     = hWindow;
                Descriptor.Windowed         = true;
                Descriptor.Flags            = 0;

                //어댑터와 스왑체인은 같은 팩토리에서 생성되어야 한다
                MUST(D3D11CreateDeviceAndSwapChain
                (
                    nullptr,                            //내부적으로 팩토리와 어댑터를 만든다
                    D3D_DRIVER_TYPE_HARDWARE,           //어댑터 널, 하드웨어 타입으로 지정하면 주 사용 어댑터를 가져온다
                    nullptr,
                    D3D11_CREATE_DEVICE_SINGLETHREADED, //약간의 퍼포먼스 상승
                    nullptr, //nullptr
                    0, //0 으로 대체
                    D3D11_SDK_VERSION,
                    &Descriptor,
                    &SwapChain,
                    &Device,
                    nullptr,
                    &DeviceContext
                ));
            }
            {
                //TODO : IL 생성 및 결합
                //IL 정보와 버텍스 버퍼와 비교하기위해 셰이더 작성이 필요
                //hlsl에서 작성한 버텍스 각 성분별로 서술자 기술이 필요(포지션(xyzw)16바이트 컬러16바이트)
               
                
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
                        2, //sizeof(Descriptor)/sizeof(*Descriptor) //_ARRAYSIZE(Descriptor) -> winapi에서 지원하는 매크로
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


                float const Vertices[4][4]
                {
                    { -0.5f, +0.5f, 0.0f, 1.0f},
                    { +0.5f, +0.5f, 0.0f, 1.0f},
                    { -0.5f, -0.5f, 0.0f, 1.0f},
                    { +0.5f, -0.5f, 0.0f, 1.0f}
                };

                D3D11_BUFFER_DESC const Descritor 
                {
                    sizeof(Vertices),
                    D3D11_USAGE_IMMUTABLE,  //불변
                    D3D11_BIND_VERTEX_BUFFER,
                    0,  //시피유가 읽거나 쓸 권한이 있다면 설정
                };


                D3D11_SUBRESOURCE_DATA Subresource = D3D11_SUBRESOURCE_DATA();

                Subresource.pSysMem           = Vertices;
                Subresource.SysMemPitch       = 0; //2차원 이상의 데이터에서 사용
                                                   //자원의 행에대한 바이트width를 정해서 넣는다
                Subresource.SysMemSlicePitch  = 0;     //3차원을 대상으로 하는 데이터
                
                ID3D11Buffer* buffer = nullptr;


                MUST(Device->CreateBuffer
                (
                    &Descritor,
                    &Subresource,
                    &buffer
                ));


                UINT const Stride = sizeof(*Vertices);//정점 하나의 크기(보폭) 
                UINT const Offset = 0;//몇개 건너 뛸지
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
                //-> 0 1 2, 1(중간점) 2 3(추가점) 순으로 그린다
            }
            {
                //Vertex buffer


                D3D11_BUFFER_DESC Descritor
                {
                    sizeof(float[4][4]),
                    D3D11_USAGE_DYNAMIC,  //불변
                    D3D11_BIND_VERTEX_BUFFER,
                    D3D10_CPU_ACCESS_WRITE,
                };

                Device->CreateBuffer
                (
                    &Descritor,
                    nullptr,
                    &Buffer::Vertex
                );


                UINT const Stride = sizeof(float[4]);//정점 하나의 크기(보폭) 
                UINT const Offset = 0;//몇개 건너 뛸지
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
            float const Color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

            DeviceContext->ClearRenderTargetView(RenderTargetView,Color);

            //마지막 그리는 명령
            //DrawIndexed -> index순서로 그린다
            //인덱스 개수, 시작 인덱스, 0으로 할 인덱스
            DeviceContext->Draw(4,0);



            MUST(SwapChain->Present(0,0));
            return 0;
        }

        case WM_DESTROY:
        {
            DeviceContext->ClearState();
            //현재 파이프라인 결합상태를 모두 초기화
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
                    //스왑체인의 버퍼는 IDXGISurface1 형태로 존재한다
                    //DXGISurface1* 원래 스크린버퍼의 자료형
                    MUST(SwapChain->GetBuffer
                    (
                        0,
                        IID_PPV_ARGS(&Texture2D) //PPV : 보이드 더블 포인터, IID와 보이드 더블포인터를 보내는 매크로
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