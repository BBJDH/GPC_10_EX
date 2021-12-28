#pragma once
#include"stdafx.h"

namespace Window
{
	static HINSTANCE global_instance;
	static HWND global_handle;

	inline LRESULT CALLBACK WndProc  //CALLBACK 내부 시스템에서 알아서 호출한다
	(
		HWND handle,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	)
	{
		switch (message)
		{
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(handle,message,wParam,lParam);
		}
		
		return 0;
	}
	inline void Create(HINSTANCE hinstance, UINT const& width, UINT const& height)
	{
		WNDCLASSEX wnd_class;
		wnd_class.cbClsExtra = 0;
		wnd_class.cbWndExtra = 0;
		wnd_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
		wnd_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wnd_class.hIcon = LoadIcon(nullptr, IDI_ERROR);
		wnd_class.hIconSm = LoadIcon(nullptr, IDI_ERROR);
		wnd_class.hInstance = hinstance;
		wnd_class.lpfnWndProc = WndProc;
		wnd_class.lpszClassName = L"D2D11Game";
		wnd_class.lpszMenuName = nullptr;
		wnd_class.style = CS_HREDRAW | CS_VREDRAW;
		wnd_class.cbSize = sizeof(WNDCLASSEX);

		auto check = RegisterClassEx(&wnd_class);
		assert(check != 0);

		global_handle = CreateWindowEx
		(
			WS_EX_APPWINDOW,
			L"D2D11Game",
			L"D2D11Game",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<int>(width),
			static_cast<int>(height),
			nullptr,
			nullptr,
			hinstance,
			nullptr
		);
		assert(global_handle != nullptr);
	}

	inline void Show()
	{
		SetForegroundWindow(global_handle); //후면이면 전면으로 빼라
		SetFocus(global_handle); //포커스를 줘라
		ShowCursor(TRUE);			//커서 보여라
		ShowWindow(global_handle, SW_NORMAL);	//윈도우 normal로 보여라
		UpdateWindow(global_handle);
	}

	inline const bool Update()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return msg.message != WM_QUIT;
	}
	inline void Destroy()
	{
		DestroyWindow(global_handle);
		UnregisterClass(L"D2D11Game",global_instance);
	}

	inline const UINT GetWidth()
	{
		RECT rect;
		GetClientRect(global_handle, &rect);
		return static_cast<UINT>(rect.right - rect.left);
	}
	inline const UINT GetHeight()
	{
		RECT rect;
		GetClientRect(global_handle, &rect);
		return static_cast<UINT>(rect.bottom - rect.top);
	}
}

