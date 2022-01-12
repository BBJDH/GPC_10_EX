#pragma once

#include "Engine/Quadrangle.h"
#include "Engine/Physics.h"
#include "Engine/Rendering.h"

#include "Actor.h"

class Dummy final : public Actor
{
public:
    void  Start() final override;
    void Update() final override;
    void    End() final override;

private:
    Engine::Rendering::Camera Camera;

    Engine::Rendering::Animation::Component Skin;
    Engine::Rendering::Image::Component     Box;

    Engine::Physics::Component<Quadrangle>  Body;

    float Speed;
};