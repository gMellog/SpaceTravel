#include "Intersections.h"

float det2(float a11, float a12, float a21, float a22)
{
    return a11 * a22 - a12 * a21;
}

float det3(float a11, float a12, float a13, 
           float a21, float a22, float a23, 
           float a31, float a32, float a33)
{
    return a11*a22*a33 + a13*a21*a32 + a12*a23*a31 - a13*a22*a31 - a12*a21*a33 - a11*a23*a32;
}

int checkPointWRTSegment(const Vector& p1, const Vector& p2, const Vector& p3)
{
    if(p1.X < p2.X)
    {
        if(p3.X < p1.X) return -1;
        else if(p3.X > p2.X) return 1;
        else return 0;
    }
    else if(p1.X > p2.X)
    {
        if(p3.X < p2.X) return -1;
        else if(p3.X > p1.X) return 1;
        else return 0;
    }
    else
    {
        if(p1.Z < p2.Z)
        {
            if(p3.Z < p1.Z) return -1;
            else if(p3.Z > p2.Z) return 1;
            else return 0;
        }
        else
        {
            if(p3.Z < p2.Z) return -1;
            else if(p3.Z > p1.Z) return 1;
            else return 0;
        }
    }
}

bool onSameSideFromP1P2(const Vector& p1, const Vector& p2, const Vector& p3, const Vector& p4)
{
    return ((checkPointWRTSegment(p1, p2, p3) == -1) && (checkPointWRTSegment(p1, p2, p4) == -1)) ||
           ((checkPointWRTSegment(p1, p2, p3) == 1) && (checkPointWRTSegment(p1, p2, p4) == 1));
}

bool checkSegmentIntersection(const Vector& p1, const Vector& p2, const Vector& p3, const Vector& p4)
{
    const int interDenom = det2(p2.X - p1.X, p3.X - p4.X, p2.Z - p1.Z, p3.Z - p4.Z);

    if(interDenom != 0)
    {
        const float p = det2(p3.X - p1.X, p3.X - p4.X, p3.Z - p1.Z, p3.Z - p4.Z) / interDenom;
        const float q = det2(p2.X - p1.X, p3.X - p1.X, p2.Z - p1.Z, p3.Z - p1.Z) / interDenom;

        return (p >= 0) && (p <= 1) && (q >= 0) && (q <= 1);
    }
    else if(det2(p3.X - p2.X, p3.X - p1.X, p3.Z - p2.Z, p3.Z - p1.Z) != 0)
    {
       return false; 
    }
    else
    {
        return !onSameSideFromP1P2(p1, p2, p3, p4);
    }
}

bool checkPointInQuadrilateral(const Vector& p1, const Vector& p2, const Vector& p3, const Vector& p4, const Vector& p5)
{
    const bool allRhs = 
            (det3(p1.X, p1.Z, 1.f, p2.X, p2.Z, 1.f, p5.X, p5.Z, 1.f) >= 0) && 
            (det3(p2.X, p2.Z, 1.f, p3.X, p3.Z, 1.f, p5.X, p5.Z, 1.f) >= 0) && 
            (det3(p3.X, p3.Z, 1.f, p4.X, p4.Z, 1.f, p5.X, p5.Z, 1.f) >= 0) && 
            (det3(p4.X, p4.Z, 1.f, p1.X, p1.Z, 1.f, p5.X, p5.Z, 1.f) >= 0);

    const bool allLhs =
            (det3(p1.X, p1.Z, 1.f, p2.X, p2.Z, 1.f, p5.X, p5.Z, 1.f) <= 0) && 
            (det3(p2.X, p2.Z, 1.f, p3.X, p3.Z, 1.f, p5.X, p5.Z, 1.f) <= 0) && 
            (det3(p3.X, p3.Z, 1.f, p4.X, p4.Z, 1.f, p5.X, p5.Z, 1.f) <= 0) && 
            (det3(p4.X, p4.Z, 1.f, p1.X, p1.Z, 1.f, p5.X, p5.Z, 1.f) <= 0);

    return allRhs || allLhs;
}

bool checkSegmentsIntersection(const Vector& p1, const Vector& p2, const Area& area)
{
    return  checkSegmentIntersection(p1, p2, area.SW, area.NW) ||
            checkSegmentIntersection(p1, p2, area.NW, area.NE) ||
            checkSegmentIntersection(p1, p2, area.NE, area.SE) ||
            checkSegmentIntersection(p1, p2, area.SE, area.SW);
}

bool Intersections::checkQuadrilateralsIntersection(
                const Area& area1, const Area& area2)
{
    return
        checkSegmentsIntersection(area1.SW, area1.NW, area2) ||
        checkSegmentsIntersection(area1.NW, area1.NE, area2) ||
        checkSegmentsIntersection(area1.NE, area1.SE, area2) ||
        checkSegmentsIntersection(area1.SE, area1.SW, area2) ||
        checkPointInQuadrilateral(area1.SW, area1.NW, area1.NE, area1.SE, area2.SW) || 
        checkPointInQuadrilateral(area2.SW, area2.NW, area2.NE, area2.SE, area1.SW);
}

bool onRadius(const Vector& asterLoc, const Vector& edge, float r)
{
    return ((asterLoc.X - edge.X) * (asterLoc.X - edge.X) + (asterLoc.Z - edge.Z) * (asterLoc.Z - edge.Z)) <= (r * r);
}

bool Intersections::checkDiscRectangleIntersection(
                const Vector& SW, const Vector& NE, 
                const Vector& asteroidLoc, float asteroidR)
{
    const auto minX = std::min(SW.X, NE.X);
    const auto maxX = std::max(SW.X, NE.X);   
    const auto minZ = std::min(SW.Z, NE.Z);   
    const auto maxZ = std::max(SW.Z, NE.Z);   

    const auto asterX = asteroidLoc.X;
    const auto asterZ = asteroidLoc.Z;
    const auto r = asteroidR;

    const Vector NW{
        SW.X, 0.f, NE.Z
    };

    const Vector SE{
        NE.X, 0.f, SW.Z
    };

    if((asterX >= (minX - r) && (asterX <= (maxX + r))) && ((asterZ >= minZ) && (asterZ <= maxZ))) return true;
    else if(((asterX >= minX) && (asterX <= maxX)) && ((asterZ >= (minZ - r)) && (asterZ <= (maxZ + r)))) return true;
    else if( onRadius(asteroidLoc, SW, r) || 
             onRadius(asteroidLoc, NW, r) || 
             onRadius(asteroidLoc, NE, r) || 
             onRadius(asteroidLoc, SE, r))
        return true;
    
    return false;
}


