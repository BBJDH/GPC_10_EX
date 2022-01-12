#include "Demo.h"

#include "Test.h"

void Demo::Start()
{
    Now = new Test;

    Now->Start();
}

void Demo::Update()
{
    Scene * const Next = Now->Update();

    if(Next != nullptr)
    {
        Now->End();

        delete Now;

        Now = Next;

        Now->Start();
    }
}

void Demo::End()
{
    Now->End();

    delete Now;
}