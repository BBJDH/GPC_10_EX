#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")
#pragma comment(lib,"d3dll.lib")

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
    
        IDXGISwapChain      * SwapChain;   //present, ... 화전면환을 총괄
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
                    }
                    {
                        IDXGIOutput* Output = nullptr;

                        MUST(Adapter->EnumOutputs(0, &Output));
                        {

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

            return 0;
        }

        case WM_DESTROY:
        {

            PostQuitMessage(0); 

            return 0;
        }
        
        default:
        {
            return DefWindowProc(hWindow, uMessage, wParameter, lParameter);
        }
        }
    }

}