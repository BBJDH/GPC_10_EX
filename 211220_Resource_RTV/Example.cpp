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
//������ ���ν����� �߰踦 ���ش�
//�ɼ� - �ؽ�Ʈ ������ ctrl ���� ��ŷ Ű ���� ����

namespace Example
{
    namespace
    {
        ID3D11Device        * Device;         //Create, ...
        ID3D11DeviceContext * DeviceContext;  //Set, Draw, ...������ ������ �߻�ȭ
        IDXGISwapChain      * SwapChain;   //present, ... ȭ����ȯ�� �Ѱ�, ����۰���
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

                        //Device->CheckMultisampleQualityLevels ��Ƽ ���ø� ���� �ϴ��� Ȯ���ϴ� �ڵ�
                    }
                    {
                        IDXGIOutput* Output = nullptr;

                        MUST(Adapter->EnumOutputs(0, &Output));
                        {
                            DXGI_SWAP_CHAIN_DESC Descriptor = DXGI_SWAP_CHAIN_DESC();


                            Descriptor.BufferDesc.Width                   = 500;      //output���� �޾ƿ´�
                            Descriptor.BufferDesc.Height                  = 500;     
                            Descriptor.BufferDesc.RefreshRate.Numerator   = 60;     //����
                            Descriptor.BufferDesc.RefreshRate.Denominator = 1;      //�и�
                            Descriptor.BufferDesc.Format                  = DXGI_FORMAT_B8G8R8A8_UNORM;//�ϳ��� ä�δ� ��ŭ�� �޸𸮸� �Ҵ�����
                            //�� ä�κ� 8��Ʈ �Ҵ�(0~255),UNORM ũ�� 1�� ����ȭ
                            //BPP : Bits Per Pixel(32), BPC : Bits Per Channel(8)
                            //Signed NORM : -1 ~ +1 , Unsigned NORM : 0 ~ 1
                            
                            Descriptor.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;     
                            //���� �ֻ� : �¿�� ������ �Ʒ���
                            //��� �ֻ� : ���پ� �ǳʶٰ� ��� ��� (¦���� Ȧ���� �̷���) ordering 2,3���� ��� �ֻ�
                            Descriptor.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;  //�»�ܿ� ���, ��� ���, ũ���������   
                            //dx�� MSAA�� ����Ѵ�
                            Descriptor.SampleDesc.Count                   = 1 ;
                            Descriptor.SampleDesc.Quality                 = 0 ;
                            Descriptor.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                            Descriptor.BufferCount                        = 1 ;
                            Descriptor.OutputWindow                       = hWindow ;
                            Descriptor.Windowed                           = true ;
                            Descriptor.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
                            Descriptor.Flags                              = 0 ;
                            
                            //����ü���� ���� ���ۿ� �ĸ� ���۴� �����ͷ� ��ȯ�� �ǽ�
                            //���� ���� ���۴� DWM(������ ���)���� ������
                            //bitblt : Bit Block Transfer, ������
                            //Flip : �ּ� ���� (�����ϸ� �̱���� ����)

                            

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
            //����Ƚ���� 0�� �� ��ü�� ��ü������ �����Ѵ�(�ƹ��͵� �������� �ʴ� ��ü�� ������ �����Ƿ� ����!)
            Factory->Release();//release(); ��ȯ�� uns long refī��Ʈ�� ��ȯ, �ʰ����� ����÷ο�



            return 0;
        }
        case WM_APP:
        {
            //TODO: ������ ȭ�鿡�� ���� ��� ȭ������ ���ϰ� ����� �Ǹ� �ٽ� �������� �ǵ��� �ݺ�
            //������ ����� Time ���ν����� Ȱ���Ͽ����ϴ�

            Time::Procedure(hWindow,uMessage,wParameter,lParameter); 
            if(Time::isinterval())                      //0.03�� �̻� ��ŸŸ�� ������ ����
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
                    //����ü���� ���۴� IDXGISurface1 ���·� �����Ѵ�
                    //DXGISurface1* ���� ��ũ�������� �ڷ���
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