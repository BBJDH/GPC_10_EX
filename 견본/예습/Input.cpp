#include <Windows.h>

#include "Input.h"

namespace Engine::Input
{
    namespace
    {
        class
        {
        public:
            void Update()
            {
                ZeroMemory(State.Changed, sizeof(State.Changed));
            }

            void Down(WORD const code)
            {
                if(not Pressed(code))
                {
                    State.Changed[code >> 0x4] ^= (0x8000 >> (code & 0xF));
                    State.Pressed[code >> 0x4] ^= (0x8000 >> (code & 0xF));
                }
            }

            void Up(WORD const code)
            {
                State.Changed[code >> 0x4] ^= (0x8000 >> (code & 0xF));
                State.Pressed[code >> 0x4] ^= (0x8000 >> (code & 0xF));
            }

            bool Changed(WORD const code)
            {
                return State.Changed[code >> 0x4] & (0x8000 >> (code & 0xF));
            }

            bool Pressed(WORD const code)
            {
                return State.Pressed[code >> 0x4] & (0x8000 >> (code & 0xF));
            }

        private:
            struct
            {
                WORD Changed[16];
                WORD Pressed[16];
            }
            State;
        }
        Key;

        POINT Cursor;
        POINT Wheel;
    }

    namespace Get
    {
        namespace Key
        {
            bool Press(WORD const code) { return                                  Input::Key.Pressed(code); }
            bool Down (WORD const code) { return Input::Key.Changed(code) and     Input::Key.Pressed(code); }
            bool Up   (WORD const code) { return Input::Key.Changed(code) and not Input::Key.Pressed(code); }
        }

        namespace Cursor
        {
            long X() { return Input::Cursor.x; }
            long Y() { return Input::Cursor.y; }
        }

        namespace Wheel
        {
            long H() { return Input::Wheel.x; }
            long V() { return Input::Wheel.y; }
        }
    }

    void Procedure(HWND const hWindow, UINT const uMessage, WPARAM const wParameter, LPARAM const lParameter)
    {
        switch(uMessage)
        {
            case WM_APP:
            {
                Key.Update();

                Wheel = POINT();

                return;
            }
            case WM_MOUSEHWHEEL: { Wheel.x += GET_WHEEL_DELTA_WPARAM(wParameter) / WHEEL_DELTA; return; }
            case WM_MOUSEWHEEL:  { Wheel.y += GET_WHEEL_DELTA_WPARAM(wParameter) / WHEEL_DELTA; return; }
            case WM_MOUSEMOVE:
            {
                Cursor.x = static_cast<SHORT>(LOWORD(lParameter));
                Cursor.y = static_cast<SHORT>(HIWORD(lParameter));

                return;
            }
            case WM_LBUTTONDOWN: { Key.Down(VK_LBUTTON); return; }
            case WM_RBUTTONDOWN: { Key.Down(VK_RBUTTON); return; }
            case WM_MBUTTONDOWN: { Key.Down(VK_MBUTTON); return; }
            case WM_XBUTTONDOWN:
            {
                switch(GET_XBUTTON_WPARAM(wParameter))
                {
                    case XBUTTON1: { Key.Down(VK_XBUTTON1); return; }
                    case XBUTTON2: { Key.Down(VK_XBUTTON2); return; }
                }
            }
            case WM_LBUTTONUP: { Key.Up(VK_LBUTTON); return; }
            case WM_RBUTTONUP: { Key.Up(VK_RBUTTON); return; }
            case WM_MBUTTONUP: { Key.Up(VK_MBUTTON); return; }
            case WM_XBUTTONUP:
            {
                switch(GET_XBUTTON_WPARAM(wParameter))
                {
                    case XBUTTON1: { Key.Up(VK_XBUTTON1); return; }
                    case XBUTTON2: { Key.Up(VK_XBUTTON2); return; }
                }
            }
            case WM_KEYDOWN: case WM_SYSKEYDOWN: { Key.Down(static_cast<WORD>(wParameter)); return; }
            case WM_KEYUP:   case WM_SYSKEYUP:   { Key.Up  (static_cast<WORD>(wParameter)); return; }
        }
    }
}