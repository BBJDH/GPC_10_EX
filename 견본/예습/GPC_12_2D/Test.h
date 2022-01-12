#pragma once

#include "Engine/Rendering.h"
#include "Engine/Physics.h"
#include "Engine/Circle.h"

#include "Scene.h"

class Test final : public Scene
{
public:
    void     Start() final override;
    Scene * Update() final override;
    void       End() final override;

private:
    Engine::Rendering::Image::Component Background;

    struct
    {
        Engine::Rendering::Image::Test           Skin;
        Engine::Physics::Component<class Circle> Body;
    }
    Free;

    class Actor * Player = nullptr;
};