#pragma once

class Actor abstract
{
public:
    virtual void  Start() abstract;
    virtual void Update() abstract;
    virtual void    End() abstract;
};