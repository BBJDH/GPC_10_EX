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
//������ ���ν����� �߰踦 ���ش�
//�ɼ� - �ؽ�Ʈ ������ ctrl ���� ��ŷ Ű ���� ����

namespace Example
{
    namespace
    {
        ID3D11Device        * Device;         //Create, ...
        ID3D11DeviceContext * DeviceContext;  //Set, Draw, ...������ ������ �߻�ȭ
    
        IDXGISwapChain      * SwapChain;   //present, ... ȭ����ȯ�� �Ѱ�
    }

    LRESULT CALLBACK Procedure
    (HWND   const hWindow,UINT   const uMessage,WPARAM const wParameter,  LPARAM const lParameter )
    {

        switch (uMessage)
        {
        case WM_CREATE:
        {
            IDXGIFactory1* Factory = nullptr;  
            //IDXGIFactory�� ��ӹ޾� Ȯ��Ǿ��־� 1�� �پ��ִ�

            MUST(CreateDXGIFactory1(__uuidof(*Factory),reinterpret_cast<void **>(&Factory)));
            //MUST(CreateDXGIFactory1(IID_IDXGIFactory1,reinterpret_cast<void **>(&Factory)));
            {
                IDXGIAdapter1* Adapter = nullptr;
                MUST(Factory->EnumAdapters1(0, &Adapter));
                {
                    {
                        //�ش� ����Ϳ� ���ؼ� �� dx ������ ������ �ǽ��Ѵ�
                        //���н� �������� �Ѿ�� ����
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
                            D3D11_CREATE_DEVICE_SINGLETHREADED, //�ణ�� �����ս� ���
                            Levels, //nullptr
                            sizeof(Levels)/sizeof(*Levels), //0 ���� ��ü
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
            //����Ƚ���� 0�� �� ��ü�� ��ü������ �����Ѵ�(�ƹ��͵� �������� �ʴ� ��ü�� ������ �����Ƿ� ����!)
            Factory->Release();//release(); ��ȯ�� uns long refī��Ʈ�� ��ȯ, �ʰ����� ����÷ο�



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