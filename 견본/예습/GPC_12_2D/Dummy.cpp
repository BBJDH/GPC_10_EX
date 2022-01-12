#include "Engine/Input.h"
#include "Engine/Time.h"

#include "Dummy.h"

void Dummy::Start()
{
    {
        Camera.Sight = Vector<2>(1280, 720);
    }
    {
        Skin.Name       = "Animation/Sonic/Idle";
        Skin.Duration   = 1.0f;
        Skin.Repeatable = true;

        Skin.Length = Vector<2>(156, 160);
    }
    {
        Body.Length = Point(100, 150);

        Box.Name   = "Image/GBB";
        Box.Length = Vector<2>(100, 150);
    }
    {
        Speed = 300;
    }
}

void Dummy::Update()
{
    {
        using namespace Engine;

        Vector<2> direction = Vector<2>();

        if
        (
            Input::Get::Key::Down('A') or
            Input::Get::Key::Down('D') or
            Input::Get::Key::Down('W') or
            Input::Get::Key::Down('S')
        )
        {
            if(Input::Get::Key::Press('E'))
            {
                Skin.Name     = "Animation/Sonic/Run";
                Skin.Duration = 0.25f;

                Speed = 600;
            }
            else
            {
                Skin.Name     = "Animation/Sonic/Walk";
                Skin.Duration = 1.0f;

                Speed = 300;
            }
        }

        if
        (
            not Input::Get::Key::Press('A') and
            not Input::Get::Key::Press('D') and
            not Input::Get::Key::Press('W') and
            not Input::Get::Key::Press('S')
        )
        Skin.Name = "Animation/Sonic/Idle";

        if(Input::Get::Key::Press('A')) direction += { -1,  0 }, Skin.Flipped = true;
        if(Input::Get::Key::Press('D')) direction += { +1,  0 }, Skin.Flipped = false;
        if(Input::Get::Key::Press('W')) direction += {  0, +1 };
        if(Input::Get::Key::Press('S')) direction += {  0, -1 };

        if(direction[0] != 0 or direction[1] != 0)
            Skin.Location += Normalize(direction) * Speed * Time::Get::Delta();
    }
    {
        Camera.Location = Skin.Location;

        Camera.Set();
    }
    {
        Body.Center.x = Skin.Location[0];
        Body.Center.y = Skin.Location[1];

        Box.Location = Skin.Location;
    }
    {
        Skin.Render();

        Box.Render();
    }
}

void Dummy::End()
{
}