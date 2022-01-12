#include "Engine/Initialize.h"

#include "Demo.h"

namespace Engine
{
    Game * Initialize()
    {
        return new Demo;
    }
}