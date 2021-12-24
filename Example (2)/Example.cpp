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
                    D3D11_INPUT_ELEMENT_DESC Descriptor[2]
                    {
                        D3D11_INPUT_ELEMENT_DESC()
                    };
                    Descriptor[0].SemanticName          = "POSITION";
                    Descriptor[0].SemanticIndex         = 0;
                    Descriptor[0].Format                = DXGI_FORMAT_R32G32B32A32_FLOAT;    //해당 성분에 대한 형식정보
                    Descriptor[0].InputSlot             = 0;        //슬롯 번호       
                    Descriptor[0].AlignedByteOffset     = 0;        //시작 바이트 점  (D3D11_APPEND_ALIGNED_ELEMENT 넣어도 무방) 
                    Descriptor[0].InputSlotClass        = D3D11_INPUT_PER_VERTEX_DATA;
                    //D3D11_INPUT_CLASSIFICATION; //슬롯에 들어가는 데이터가 버텍스 데이터인지 인스턴스 데이터인지
                    Descriptor[0].InstanceDataStepRate  = 0;    //위에서 인스턴싱을 한다면 작성해야함
                                                        
                    Descriptor[1].SemanticName          = "COLOR";
                    Descriptor[1].SemanticIndex         = 0;
                    Descriptor[1].Format                = DXGI_FORMAT_R32G32B32A32_FLOAT;    
                    Descriptor[1].InputSlot             = 0;        //슬롯 번호       
                    Descriptor[1].AlignedByteOffset     = D3D11_APPEND_ALIGNED_ELEMENT;  //앞 성분에 붙어있다라는 매크로(16)   
                    Descriptor[1].InputSlotClass        = D3D11_INPUT_PER_VERTEX_DATA;
                    Descriptor[1].InstanceDataStepRate  = 0;   

                    MUST(Device->CreateInputLayout
                    (
                        Descriptor,
                        2, //sizeof(Descriptor)/sizeof(*Descriptor) //_ARRAYSIZE(Descriptor) -> winapi에서 지원하는 매크로
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
                Descritor.CPUAccessFlags      = 0;  //시피유가 읽거나 쓸 권한이 있다면 설정
                Descritor.MiscFlags           = 0;
                Descritor.StructureByteStride = 0;

                D3D11_SUBRESOURCE_DATA Subresource = D3D11_SUBRESOURCE_DATA();

                Subresource.pSysMem           = Vertices;
                Subresource.SysMemPitch       = 0; //2차원 이상의 데이터에서 사용
                                                   //자원의 행에대한 바이트width를 정해서 넣는다
                Subresource.SysMemSlicePitch  ;     //3차원을 대상으로 하는 데이터
                


                MUST(Device->CreateBuffer
                (
                    &Descritor,
                    &Subresource,
                    &Buffer::Vertex
                ));
                UINT const Stride = sizeof(*Vertices);//정점 하나의 크기(보폭) 
                UINT const Offset = 0;//몇개 건너 뛸지
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
                //파이프라인에 한개만 결합 가능
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
                Descritor.CPUAccessFlags = 0;  //시피유가 읽거나 쓸 권한이 있다면 설정
                Descritor.MiscFlags = 0;
                Descritor.StructureByteStride = 0;

                D3D11_SUBRESOURCE_DATA Subresource = D3D11_SUBRESOURCE_DATA();

                Subresource.pSysMem = Triangles;
                Subresource.SysMemPitch = 0; //2차원 이상의 데이터에서 사용
                                                   //자원의 행에대한 바이트width를 정해서 넣는다
                Subresource.SysMemSlicePitch;     //3차원을 대상으로 하는 데이터




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

            //마지막 그리는 명령
            //DrawIndexed -> index순서로 그린다
            //인덱스 개수, 시작 인덱스, 0으로 할 인덱스
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
                Viewport.MinDepth = 0; //기본값 (사용하지 않을 예정)
                Viewport.MaxDepth = 0; //기본값 (사용하지 않을 예정)

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