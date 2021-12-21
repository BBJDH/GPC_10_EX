//#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")
#pragma comment(lib,"d3d11.lib")

#include<cassert>
#include<d3d11.h>

using namespace std;
namespace Time
{
    void Procedure
    (HWND   const hWindow, UINT   const uMessage, WPARAM const wParameter, LPARAM const lParameter);
    bool isinterval();
}
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
        ID3D11RenderTargetView * RenderTargetView;
        float light_value =0.0f;
        bool light_up = true;
    }
    void lightcontrol()
    {
        if (light_up)
            light_value += 0.01f;
        else
            light_value -= 0.01f;
        //if ((1.0f - light_value) < 0.01f)
        //    light_up = false;
        //if ((light_value) < 0.01f)
        //    light_up = true;
        if (light_value > 1.0f)
        {
            light_up = false;
            light_value = 1.0f;
        }
        if (light_value < 0.01f)
        {
            light_up = true;
            light_value = 0.0f;
        }
    }

    LRESULT CALLBACK Procedure
    (HWND   const hWindow,UINT   const uMessage,WPARAM const wParameter,  LPARAM const lParameter )
    {

        switch (uMessage)
        {
        case WM_CREATE:
        {
            IDXGIFactory1* Factory = nullptr;  
            //IDXGIFactory를 상속받아 확장되어있어 1이 붙어있다
          
            MUST(CreateDXGIFactory1(__uuidof(*Factory),reinterpret_cast<void **>(&Factory)));
            //MUST(CreateDXGIFactory1(IID_IDXGIFactory1,reinterpret_cast<void **>(&Factory)));
            {
                IDXGIAdapter1* Adapter = nullptr;
                MUST(Factory->EnumAdapters1(0, &Adapter));
                {
                    {
                        //해당 어댑터에 대해서 각 dx 레벨별 검증을 실시한다
                        //실패시 다음으로 넘어가서 검증
                        D3D_FEATURE_LEVEL Levels[]
                        {
                            D3D_FEATURE_LEVEL_11_0,
                            D3D_FEATURE_LEVEL_10_1,
                            D3D_FEATURE_LEVEL_10_0
                        };

                        D3D_FEATURE_LEVEL Level = D3D_FEATURE_LEVEL();

                        MUST(D3D11CreateDevice
                        (
                            Adapter,
                            D3D_DRIVER_TYPE_UNKNOWN,
                            nullptr,
                            D3D11_CREATE_DEVICE_SINGLETHREADED, //약간의 퍼포먼스 상승
                            Levels, //nullptr
                            sizeof(Levels)/sizeof(*Levels), //0 으로 대체
                            D3D11_SDK_VERSION,
                            &Device,
                            &Level,
                            &DeviceContext
                         ));

                        //Device->CheckMultisampleQualityLevels 멀티 샘플링 지원 하는지 확인하는 코드
                    }
                    {
                        IDXGIOutput* Output = nullptr;

                        MUST(Adapter->EnumOutputs(0, &Output));
                        {
                            DXGI_SWAP_CHAIN_DESC Descriptor = DXGI_SWAP_CHAIN_DESC();


                            Descriptor.BufferDesc.Width                   = 500;      //output에서 받아온다
                            Descriptor.BufferDesc.Height                  = 500;     
                            Descriptor.BufferDesc.RefreshRate.Numerator   = 60;     //분자
                            Descriptor.BufferDesc.RefreshRate.Denominator = 1;      //분모
                            Descriptor.BufferDesc.Format                  = DXGI_FORMAT_B8G8R8A8_UNORM;//하나의 채널당 얼만큼의 메모리를 할당할지
                            //각 채널별 8비트 할당(0~255),UNORM 크기 1로 정규화
                            //BPP : Bits Per Pixel(32), BPC : Bits Per Channel(8)
                            //Signed NORM : -1 ~ +1 , Unsigned NORM : 0 ~ 1
                            
                            Descriptor.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;     
                            //순차 주사 : 좌우로 위에서 아래로
                            //비월 주사 : 한줄씩 건너뛰고 찍는 방식 (짝수줄 홀수줄 이런식) ordering 2,3번이 비월 주사
                            Descriptor.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;  //좌상단에 찍기, 가운데 찍기, 크기조정모드   
                            //dx는 MSAA를 사용한다
                            Descriptor.SampleDesc.Count                   = 1 ;
                            Descriptor.SampleDesc.Quality                 = 0 ;
                            Descriptor.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                            Descriptor.BufferCount                        = 1 ;
                            Descriptor.OutputWindow                       = hWindow ;
                            Descriptor.Windowed                           = true ;
                            Descriptor.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
                            Descriptor.Flags                              = 0 ;
                            
                            //스왑체인의 전면 버퍼와 후면 버퍼는 포인터로 교환을 실시
                            //이후 전면 버퍼는 DWM(윈도우 출력)으로 보낸다
                            //bitblt : Bit Block Transfer, 값전달
                            //Flip : 주소 전달 (가능하면 이기능이 좋다)

                            

                            MUST(Output->FindClosestMatchingMode
                            (
                                &Descriptor.BufferDesc,
                                &Descriptor.BufferDesc,
                                Device
                            ));
                            MUST(Factory->CreateSwapChain
                            (
                                Device,
                                &Descriptor,
                                &SwapChain
                            ));
                        }
                        Output->Release();
                    }
                }
                Adapter->Release();
            }
            //참조횟수가 0이 된 개체를 자체적으로 해제한다(아무것도 참조하지 않는 개체는 사용되지 않으므로 해제!)
            Factory->Release();//release(); 반환값 uns long ref카운트를 반환, 초과사용시 언더플로우



            return 0;
        }
        case WM_APP:
        {
            //TODO: 검은색 화면에서 점점 흰색 화면으로 변하고 흰색이 되면 다시 검은색이 되도록 반복
            //이전에 배웠던 Time 프로시져를 활용하였습니다

            Time::Procedure(hWindow,uMessage,wParameter,lParameter); 
            if(Time::isinterval())                      //0.03초 이상 델타타임 누적시 시행
                lightcontrol();

            float const Color[4] = { light_value, light_value, light_value, 1.0f };

            DeviceContext->ClearRenderTargetView(RenderTargetView,Color);

            MUST(SwapChain->Present(0,0));
            return 0;
        }

        case WM_DESTROY:
        {
            DeviceContext->ClearState();
            RenderTargetView->Release();
            SwapChain->Release();
            DeviceContext->Release();
            Device->Release();
            PostQuitMessage(0); 

            return 0;
        }
        case WM_SIZE:
        {
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
                        __uuidof(*Texture2D),
                        reinterpret_cast<void**>(&Texture2D)
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