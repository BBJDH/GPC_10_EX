#include <Windows.h>

#include "Test.h"
#include "Test2.h"

#include "Engine/Input.h"
#include "Dummy.h"

void Test::Start()
{
    {
        Free.Skin.Name     = "Image/Free";
        Free.Skin.Length   = Vector<2>(500, 500) / 2;
        Free.Skin.Location = Vector<2>(320, 180);

        Free.Body.Radius = Free.Skin.Length[0] / 2;
        Free.Body.Center = Point(Free.Skin.Location[0], Free.Skin.Location[1]);
    }
    {
        Background.Name = "Image/Celadon";

        Background.Length = Vector<2>(960, 703) * 4;
    }
    {
        Player = new Dummy;

        Player->Start();
    }
}

Scene * Test::Update()
{
    using namespace Engine;

    Background.Render();

    Physics::Component<Point> const cursor(Point((float)Input::Get::Cursor::X(), (float)Input::Get::Cursor::Y()));

//  if(Free.Body.Collide(cursor) and Input::Get::Key::Press(VK_LBUTTON))
    
    Free.Skin.Render();

    Player->Update();

    if(Input::Get::Key::Press(VK_RBUTTON)) return new Test2;
    else                                   return nullptr;
}

void Test::End()
{
    Player->End();

    delete Player;
}