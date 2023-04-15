#pragma once

#include "Utils.h"

struct Intersections
{
    static bool checkQuadrilateralsIntersection(
                const Area& area1, const Area& area2);
    
    static bool checkDiscRectangleIntersection(
                const Vector& SW, const Vector& NE, 
                const Vector& asteroidLoc, float asteroidR);
};