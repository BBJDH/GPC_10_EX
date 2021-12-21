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
            }

            return 0;
        }
        case WM_APP:
        {
            static float element = 0.0f;
            static float delta = 0.0001f;


            float const Color[4] = { element, element, element, 1.0f };

            DeviceContext->ClearRenderTargetView(RenderTargetView,Color);

            if(element < 0.0f or 1.0f<element)
                delta *= -1;
            element += delta;

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