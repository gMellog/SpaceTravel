#include "Actor.h"

void Actor::tick(float deltaTime)
{
    tickCalled = true;
}

void Actor::die()
{
    died = true;
}
    
bool Actor::isDied() const noexcept
{
    return died;
}