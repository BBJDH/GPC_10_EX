#pragma once

class Scene abstract
{
public:
    virtual void     Start() abstract;
    virtual Scene * Update() abstract;
    virtual void       End() abstract;
};