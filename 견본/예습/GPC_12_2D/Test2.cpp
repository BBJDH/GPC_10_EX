#include "Test2.h"

#include "Engine/Input.h"
#include "Dummy.h"

void Test2::Start()
{    
    {
        Background.Name = "Image/Map";

        Background.Length = Vector<2>(2400, 1500);
    }
    {
        Player = new Dummy;

        Player->Start();
    }
}

Scene * Test2::Update()
{
    Background.Render();
    
    Player->Update();

    return nullptr;
}

void Test2::End()
{
    Player->End();

    delete Player;
}