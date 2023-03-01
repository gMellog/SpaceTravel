#include "Actor.h"

void Actor::die()
{
    died = true;
}
    
bool Actor::isDied() const noexcept
{
    return died;
}