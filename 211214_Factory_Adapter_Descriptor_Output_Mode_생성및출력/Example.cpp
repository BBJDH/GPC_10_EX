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
//각각의 프로시져로 중계를 해준다
//옵션 - 텍스트 편집기 ctrl 정의 피킹 키 편집 가능

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
                //ref카운트를 증가시키는 함수, 다른곳에서 추가로 역참조 시 꼭 사용해주도록 하자
                //사용하지않으면 댕글링 포인터가 된다

                IDXGIAdapter1* Adapter = nullptr;

                for (int i = 0; Factory->EnumAdapters1(i, &Adapter) == S_OK; ++i)
                {
                    {
                        DXGI_ADAPTER_DESC1 Descriptor = DXGI_ADAPTER_DESC1();
                        MUST(Adapter->GetDesc1(&Descriptor));
                        wcout << "어댑터 이름\t\t: "      << Descriptor.Description << endl;
                        wcout << "전용 비디오 메모리\t: " << Descriptor.DedicatedVideoMemory << endl;
                        wcout << "전용 시스템 메모리\t: " << Descriptor.DedicatedSystemMemory << endl;
                        wcout << "공유 시스템 메모리\t: " << Descriptor.SharedSystemMemory << endl;
                        //wcout << L"어댑터 이름\t\t: " << Descriptor.Description << endl;
                        //wchar로 출력시 L 앞에 사용 한글로 사용시 깨진다


                    }
                    
                    {
                        IDXGIOutput* Output = nullptr;
                        for (int i = 0; SUCCEEDED(Adapter->EnumOutputs(i, &Output)); ++i)
                        {
                            DXGI_OUTPUT_DESC Descriptor = DXGI_OUTPUT_DESC();

                            MUST(Output->GetDesc(&Descriptor));

                            wcout << "장치 이름\t: " << Descriptor.DeviceName << endl;

                            UINT Count = 0;

                            MUST(Output->GetDisplayModeList          //모니터 출력모드 리스트 받아온다 개수 맞춰서 count 증가
                            (
                                DXGI_FORMAT_B8G8R8A8_UNORM,
                                DXGI_ENUM_MODES_INTERLACED|DXGI_ENUM_MODES_SCALING,
                                &Count,
                                nullptr
                            ));

                            DXGI_MODE_DESC* Modes = new DXGI_MODE_DESC[Count];
                            {
                                MUST(Output->GetDisplayModeList            //카운트 만큼 할당받아서 넣는다
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