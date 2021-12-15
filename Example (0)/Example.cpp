#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

#include<Windows.h>
#include<cassert>
#include<dxgi.h>
#include<iostream>

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
    LRESULT CALLBACK Procedure
    (HWND   const hWindow,UINT   const uMessage,WPARAM const wParameter,  LPARAM const lParameter )
    {

        switch (uMessage)
        {
        case WM_CREATE:
        {
            IDXGIFactory1* Factory = nullptr;

            MUST(CreateDXGIFactory1(__uuidof(*Factory),reinterpret_cast<void **>(&Factory)));
            //MUST(CreateDXGIFactory1(IID_IDXGIFactory1,reinterpret_cast<void **>(&Factory)));
            {
                //IDXGIFactory1* f = nullptr;
                //f->AddRef();
                //refī��Ʈ�� ������Ű�� �Լ�, �ٸ������� �߰��� ������ �� �� ������ֵ��� ����
                //������������� ��۸� �����Ͱ� �ȴ�

                IDXGIAdapter1* Adapter = nullptr;

                for (int i = 0; Factory->EnumAdapters1(i, &Adapter) == S_OK; ++i)
                {
                    {
                        DXGI_ADAPTER_DESC1 Descriptor = DXGI_ADAPTER_DESC1();
                        MUST(Adapter->GetDesc1(&Descriptor));
                        wcout << "����� �̸�\t\t: "      << Descriptor.Description << endl;
                        wcout << "���� ���� �޸�\t: " << Descriptor.DedicatedVideoMemory << endl;
                        wcout << "���� �ý��� �޸�\t: " << Descriptor.DedicatedSystemMemory << endl;
                        wcout << "���� �ý��� �޸�\t: " << Descriptor.SharedSystemMemory << endl;
                        //wcout << L"����� �̸�\t\t: " << Descriptor.Description << endl;
                        //wchar�� ��½� L �տ� ��� �ѱ۷� ���� ������


                    }
                    
                    {
                        IDXGIOutput* Output = nullptr;
                        for (int i = 0; SUCCEEDED(Adapter->EnumOutputs(i, &Output)); ++i)
                        {
                            DXGI_OUTPUT_DESC Descriptor = DXGI_OUTPUT_DESC();

                            MUST(Output->GetDesc(&Descriptor));

                            wcout << "��ġ �̸�\t: " << Descriptor.DeviceName << endl;

                            UINT Count = 0;

                            MUST(Output->GetDisplayModeList          //����� ��¸�� ����Ʈ �޾ƿ´� ���� ���缭 count ����
                            (
                                DXGI_FORMAT_B8G8R8A8_UNORM,
                                DXGI_ENUM_MODES_INTERLACED|DXGI_ENUM_MODES_SCALING,
                                &Count,
                                nullptr
                            ));

                            DXGI_MODE_DESC* Modes = new DXGI_MODE_DESC[Count];
                            {
                                MUST(Output->GetDisplayModeList            //ī��Ʈ ��ŭ �Ҵ�޾Ƽ� �ִ´�
                                (
                                    DXGI_FORMAT_B8G8R8A8_UNORM,
                                    DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
                                    &Count,
                                    Modes
                                ));

                                for (UINT i = 0; i < Count; ++i)
                                {
                                    cout << '[' << i << ']' << endl;
                                    cout << endl;
                                    cout << "Resolution\t: " << Modes[i].Width << "x" << Modes[i].Height << endl;
                                    cout << "Refresh Rate\t: " 
                                        << static_cast<float>
                                        (Modes[i].RefreshRate.Numerator / Modes[i].RefreshRate.Denominator)
                                                                                                         << endl;
                                    cout << "Scanline Ordering\t: " << Modes[i].ScanlineOrdering         << endl;
                                    cout << "Scaling\t: " << Modes[i].Scaling                            << endl;

                                }
                            }
                            delete[] Modes;
                            Output->Release();
                            
                        }
                    }
                    Adapter->Release();

                }
                //MUST(Factory->EnumAdapters1(0,&Adapter));
                //{
                //    DXGI_ADAPTER_DESC1 Descriptor = DXGI_ADAPTER_DESC1();
                //    MUST(Adapter->GetDesc1(&Descriptor));
                //    int i = 0;

                //}
                //Adapter->Release();
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