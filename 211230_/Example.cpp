//#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")
//#pragma comment(lib,"d3d11.lib")

#include<cassert>
#include<d3d11.h> //windows 헤더파일을 포함하고 있다. 라이브러리 파일로 선언 부분만 정의되어있다 내부 구성은 공개되어 있지 않음
#include"../FreeImage.h"
//dll : 다이나믹 링크 라이브러리 : 동적으로 필요한 내용에대해서 라이브러리 로드가 시행

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
        ID3D11Device        * Device;         //Create, ... 컴인터페이스 생성 (컴인터페이스 : 컴퓨터가 생성을 도와주는 인터페이스)
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
#pragma region Swap Chain 생성
            //Swap Chain 생성
            {
                // IDXGISwapChain      * SwapChain; 위 네임스페이스에 생성되어있음
                DXGI_SWAP_CHAIN_DESC Descriptor = DXGI_SWAP_CHAIN_DESC();

                Descriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;//하나의 채널당 얼만큼의 메모리를 할당할지
                //형식 : DXGI_MODE_DESC(너비높이,주사율,데이터 포멧형식 등) 
                //각 채널별 8비트 할당(0~255),UNORM 크기 1로 정규화
                //BPP : Bits Per Pixel(32), BPC : Bits Per Channel(8)
                //Signed NORM : -1 ~ +1 , Unsigned NORM : 0 ~ 1

                Descriptor.SampleDesc.Count = 1;        //최대 32 개까지 가능
                Descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                Descriptor.BufferCount = 1;
                Descriptor.OutputWindow = hWindow;
                Descriptor.Windowed = true;
                Descriptor.Flags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
                //스왑체인이 gdi와 호환가능하게 할지 결정 여부
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
#pragma endregion

            {
                //IL 정보와 버텍스 버퍼와 비교하기위해 셰이더 작성이 필요
                //hlsl에서 작성한 버텍스 각 성분별로 서술자 기술이 필요(포지션(xyzw)16바이트 컬러16바이트)
#pragma region Input LayOut 생성
#include "../Shader/Bytecode/Vertex.h"
                {
                    ID3D11InputLayout* InputLayout = nullptr;

                    D3D11_INPUT_ELEMENT_DESC const Descriptor[2]
                    {
                        {"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0},
                        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,1}
                    };

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
#pragma endregion

                DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

                
#pragma region Vertex Shader 생성
                //Vertex Shader
                {
                    ID3D11VertexShader* VertexShader = nullptr;

                    MUST(Device->CreateVertexShader
                    (
                        Bytecode,
                        sizeof(Bytecode),
                        nullptr,
                        &VertexShader
                    ));
                    DeviceContext->VSSetShader(VertexShader, nullptr, 0);

                    VertexShader->Release();

                }
#pragma endregion

            }
            {
#pragma region Pixel Shader 생성
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
#pragma endregion

            }
#pragma region Vertex buffer 생성
            //Vertex buffer
            {
                ID3D11Buffer* buffer = nullptr;
                float const Coordinates[4][2]
                {
                    { -0.5f, +0.5f}, //0     0 1
                    { +0.5f, +0.5f}, //1     2 3
                    { -0.5f, -0.5f}, //2
                    { +0.5f, -0.5f}  //3
                };//각 정점의 좌표
                D3D11_BUFFER_DESC const Descritor
                {
                    sizeof(Coordinates),
                    D3D11_USAGE_IMMUTABLE,  //불변
                    D3D11_BIND_VERTEX_BUFFER,
                    0,  //시피유가 읽거나 쓸 권한이 있다면 설정
                };


                D3D11_SUBRESOURCE_DATA Subresource = D3D11_SUBRESOURCE_DATA();  //버퍼에 들어갈 리소스 데이터

                Subresource.pSysMem = Coordinates;
                Subresource.SysMemPitch = 0; //2차원 이상의 데이터에서 사용
                                                   //자원의 행에대한 바이트width를 정해서 넣는다
                Subresource.SysMemSlicePitch = 0;     //3차원을 대상으로 하는 데이터


                MUST(Device->CreateBuffer
                (
                    &Descritor,
                    &Subresource,
                    &buffer
                ));
                UINT const Stride = sizeof(*Coordinates);//정점 하나의 크기(보폭) 
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

                //D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP 
                //-> 0 1 2, 1(중간점) 2 3(추가점) 순으로 그린다
            }
#pragma endregion

#pragma region Pixel buffer 생성

            //Pixel buffer
            {
                //ID3D11Buffer* Vertex; 위 네임스페이스에 선언됨
                D3D11_BUFFER_DESC Descritor
                {
                    sizeof(float[4][2]),
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
                UINT const Stride = sizeof(float[2]);//정점 하나의 크기(보폭) 
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
#pragma endregion

#pragma region 이미지텍스쳐생성(Texture2d, ShaderResourceView)
            {
                ID3D11Texture2D* Texture2d = nullptr;
                char const* const File = "./Asset/Walk[9].png";

                FreeImage_Initialise();
                {
                    FIBITMAP* Bitmap = FreeImage_Load(FreeImage_GetFileType(File), File);
                    {
                        FreeImage_FlipVertical(Bitmap);

                        //Adjust 32BPP
                        if(FreeImage_GetBPP(Bitmap)!= 32)
                        {
                            FIBITMAP * const Previous = Bitmap;

                            Bitmap = FreeImage_ConvertTo32Bits(Bitmap);
                            FreeImage_Unload(Previous);
                        }
                    }
                    {
                        //DIB : Device Independant Bitmap
                        D3D11_TEXTURE2D_DESC Descriptor = D3D11_TEXTURE2D_DESC();
                        Descriptor.Width = FreeImage_GetWidth(Bitmap);
                        Descriptor.Height = FreeImage_GetHeight(Bitmap);
                        Descriptor.MipLevels = 1;//0 -> 가능한 모든 밉맵을 생성
                        Descriptor.ArraySize = 1;
                        Descriptor.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                        Descriptor.SampleDesc.Count = 1;
                        Descriptor.SampleDesc.Quality = 0;
                        Descriptor.Usage = D3D11_USAGE_DEFAULT; 
                        Descriptor.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                        Descriptor.CPUAccessFlags = 0;
                        Descriptor.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
                        //알트 오른쪽 자동완성!
                        //gdi 호환


                        D3D11_SUBRESOURCE_DATA Subresource = D3D11_SUBRESOURCE_DATA();

                        Subresource.pSysMem = FreeImage_GetBits(Bitmap);
                        //헤더를 제외한 순수 bits 만 가져온다
                        Subresource.SysMemPitch = FreeImage_GetPitch(Bitmap); //2차원 이상의 데이터에서 사용
                                                           //자원의 행에대한 바이트width를 정해서 넣는다
                        Subresource.SysMemSlicePitch = 0;     //3차원을 대상으로 하는 데이터

                        MUST(Device->CreateTexture2D
                        (
                            &Descriptor,
                            &Subresource,
                            &Texture2d
                        ));
                        {
                            IDXGISurface1 * temp = nullptr;
                            Texture2d->QueryInterface(IID_PPV_ARGS(&temp));
                                //어떤 인터페이스를 가지고있는지 묻고 가져오는 것
                            //
                            //temp->GetDC();

                            /*
                            컴퓨트 셰이더, 스테이징 픽셀값 반환 
                            

                            */

                            //ShaderResourceView 생성
                            ID3D11ShaderResourceView* SRV = nullptr;
                            Device->CreateShaderResourceView(Texture2d, nullptr, &SRV);

                            DeviceContext->PSSetShaderResources(0, 1, &SRV);
                            SRV->Release();
                        }
                        Texture2d->Release();
                    }
                    FreeImage_Unload(Bitmap);
                }
                FreeImage_DeInitialise();
            }
#pragma endregion

            return 0;
        }
        case WM_APP:
        {
            {
                //상호 배제(Mutual Exclusion, Mutex)
                // 한 개체가 자원에 접근중일 때 다른 개체가 접근 불가하게 하도록 함
                // ->race condition을 방지
                //특정자원에 대한 gpu의 접근을 차단하고 cpu로부터 자원을 갱신(lock)

                D3D11_MAPPED_SUBRESOURCE subresource = D3D11_MAPPED_SUBRESOURCE();

                MUST(DeviceContext->Map
                (
                    Buffer::Vertex,
                    0,
                    D3D11_MAP_WRITE_DISCARD,        //기존 데이터를 버리고 데이터를 쓴다
                    0,
                    &subresource
                ));
                {
                    unsigned static Frame = 0;
                    unsigned const FPM    = 10;
                    unsigned const Motion = 9;


                    //128너비 144높이(156너비,160높이)
                    float const Coordinates[4][2]
                    {
                        { 156.0f * (Frame / FPM + 0.0f ), 160.0f * 0.0f }, //LT
                        { 156.0f * (Frame / FPM + 1.0f ), 160.0f * 0.0f }, //RT
                        { 156.0f * (Frame / FPM + 0.0f ), 160.0f * 1.0f }, //LB
                        { 156.0f * (Frame / FPM + 1.0f ), 160.0f * 1.0f }  //RB
                    };
                    //if( FPM * Motion -1 >Frame ) 
                    //    Frame++;
                    //else       Frame=0;
                    //++Frame;
                    //if(FPM *Motion <=Frame)
                    //    Frame =0;
                    (++Frame)%=FPM*Motion;

                    memcpy_s(subresource.pData, subresource.RowPitch, Coordinates, sizeof(Coordinates));
                }
                DeviceContext->Unmap(Buffer::Vertex, 0);

            }
            {
                //마지막 그리는 명령
                //DrawIndexed -> index순서로 그린다
                //인덱스 개수, 시작 인덱스, 0으로 할 인덱스
                DeviceContext->Draw(4,0);

                MUST(SwapChain->Present(true,0));

                float const Color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

                DeviceContext->ClearRenderTargetView(RenderTargetView,Color);
            }

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
                        DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE
                    ));
                }
                {
                    ID3D11Texture2D * Texture2D = nullptr;
                    //스왑체인의 버퍼는 IDXGISurface1 형태로 존재한다
                    //DXGISurface1* 원래 스크린버퍼의 자료형
                    IDXGISurface1* Surface=nullptr;
                    //스왑체인에서 백버퍼를 서페이스 형식으로 받아올 수 있다
                    HDC hdc;
                    Surface->GetDC(false, &hdc);//현재 그려진 내용을 버릴지 결정
                    Surface->ReleaseDC(nullptr);//건드린 dc영역에 대해서 처리, 전체 영역에 대해서 처리
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
                //hdc를 릴리즈하면 파이프라인에 렌더타겟이 결합 해제 되므로 재결합이 필요하다.


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
//IA -> VS -> HS -> TS -> DS -> GS -> SO -> RS-> PS -> OM 의 각 세부 단계가 있다
             //------코드 수정 불가-------//
//해당 구간에서 인덱스 버퍼 설정되고 설정을 읽어오거나 하는것들이 시행된다

// IA
// VertexBuffer
// IndexBuffer
// Primitive Topology
// Input Layout
// 
// 
// VS
// SV(System Value)
// 함수를 만들어서 
// 정점에 대한 연산
// 
// 
// RS
// Viewport
// 
// PS
// SV_Target
// 
// OM
// RendertagetView
//


//Texture2d, DXGISurface1?
//RTT Render to Texture로 Texture2d에 이미지를 가져와서 마젠타로 지형을 파내고
