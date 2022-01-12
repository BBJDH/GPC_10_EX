#include <Windows.h>

namespace Example
{ LRESULT CALLBACK Procedure(HWND   const , UINT   const , WPARAM const , LPARAM const );}
//#define GPC_UPDATE WM_APP
//#define GPC_END WM_APP +1
//이런 용도로 사용자 정의 메세지를 여러개 사용

int APIENTRY WinMain
(
    _In_     HINSTANCE  const hInstance, //nullptr 불허함 _in_ 포인터 형식에 대해서만!
    _In_opt_ HINSTANCE  const hPrevInstance, //_In_ 옵셔널 널포인터 허용
    _In_     LPSTR      const lpCmdLine, //추가되는 커맨드 라인을 받는다
    //LPSTR ->longptr string 
    _In_     int        const nShowCmd //실행탭에 어떤설정을 한상태로 실행 되었는지 전달
)
{


    HWND hWindow = HWND(); //윈도우 창 생성
    {//형식에 대한 정의
        WNDCLASSEX Class = WNDCLASSEX();

        Class.cbSize = sizeof(WNDCLASSEX);  //윈도우 사이즈 지정
        Class.lpfnWndProc = Example::Procedure;
        Class.hInstance = hInstance; //인스턴스 핸들 (어느 인스턴스 소속인가)
        Class.hIcon = LoadIcon(hInstance, IDI_APPLICATION);  //아이콘 모양 뭐쓸래
        //일반적인 어플리케이션 아이콘 사용
        Class.hCursor = LoadCursor(hInstance, IDC_ARROW); //마우스 모양 뭐쓸래
        Class.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
        //배경 브러쉬 핸들
        Class.lpszClassName = "Window";
        /*클래스 네임 지정 like (class name)*/
        Class.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
        //창 좌상단에 뜨는 작은 아이콘 

        RegisterClassEx(&Class);
    }
    { 
        CREATESTRUCT Window = CREATESTRUCT();
        //CREATESTUCT 특수목적으로 생성
        //함수 인자전달시 순서가 뒤바뀌므로 역순으로 구성되어있다
        Window.lpszClass = "Window";  //String terminated by Zero
        Window.lpszName = "Game";    //-> null로 끝나는 문자열
        Window.style = WS_OVERLAPPEDWINDOW; //뒤에 윈도우 붙어야함
        Window.cx = 1200;
        Window.cy = 720;
        Window.hInstance = hInstance;

        {//윈도우 작업공간 맞추기
            RECT Rectangle = RECT();
            Rectangle.right = Window.cx;
            Rectangle.bottom = Window.cy;

            AdjustWindowRectEx(&Rectangle, Window.style, static_cast<bool>(Window.hMenu), Window.dwExStyle);
            //cx cy에 대해서 윈도우 스타일에 맞게 재작성해준다
            Window.cx = Rectangle.right - Rectangle.left;
            Window.cy = Rectangle.bottom - Rectangle.top;

            //화면 가운데로 세팅
            Window.x = (GetSystemMetrics(SM_CXSCREEN) - Window.cx) / 2; //SM_CXSCREEN 화면해상도 크기를 받아온다 
            Window.y = (GetSystemMetrics(SM_CYSCREEN) - Window.cy) / 2;
        }

        hWindow = CreateWindowEx  //WndProc 포인터 지정 없으면 여기서 오류!
        (//CreateWindowEx 논큐 메세지(메세지는 큐와 논큐 메세지로 나뉜다)
            //create은 큐를 거치지 않고 넘어감
            Window.dwExStyle,
            Window.lpszClass,
            Window.lpszName,
            Window.style,
            Window.x,
            Window.y,
            Window.cx,
            Window.cy,
            Window.hwndParent,
            Window.hMenu,
            Window.hInstance,
            Window.lpCreateParams
        ); //마지막 세미콜론을 다시 붙이면 기본문자 세팅으로..

        ShowWindow(hWindow, SW_RESTORE); //showcmd -> SW_ ...
    }   //SW_RESTORE 창 옵션 지정 못하게 막기

    {
        MSG Message = MSG();


        while (true)
        {
          
            //bkcolor = SetBkColor(hDC, bkcolor);//기존에 설정 되어있던 색을 돌려받고 새 색으로 바꿈 (기존값을 기억하기위해)
            if (PeekMessage(&Message, HWND(), WM_NULL, WM_NULL, PM_REMOVE))
            {
                if (Message.message == WM_QUIT)
                    return static_cast<int>(Message.wParam);   //WM_QUIT 의 wParam 0 전달

                DispatchMessage(&Message);
            }
            else
            {
                //메세지 전송 함수 메세지큐에 메세지 추가
                //PostMessage(hWindow,WM_APP,0,0);  
                //WM_APP 내가 만들고 싶은 특수 메세지
                SendMessage(hWindow, WM_APP, 0, 0);  //논큐 메세지로 보낸다
                
            }

        }

    }

}