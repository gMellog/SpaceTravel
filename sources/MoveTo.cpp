#include "MoveTo.h"
#include "SpaceTravel.h"
#include "Spacecraft.h"
#include "Utils.h"
#include "GoldenAsteroid.h"
#include "SimpleAsteroid.h"
#include "Tags.h"

MoveTo::MoveTo(Spacecraft* pSpacecraft)
		:
		spacecraft{ pSpacecraft },
		moveToLoc{},
		moveX{ std::abs(SpaceTravel::getXOffset() / 2.f), 0.f , 0.f },
		moveZ{ 0.f, 0.f, std::abs(SpaceTravel::getZOffset() / 2.f) },
		moving{},
        rotating{},
        rotateZAngle{},
        leftRotate{},
        t{},
        currTask{},
        minusX{},
        minusZ{},
        didntEvenTry{}
	{
        if(spacecraft == nullptr)
        {
            std::cerr << "spacecraft nullptr in MoveTo\n";
            std::exit(1);
        }
    }

void MoveTo::init()
{

}

Vector MoveTo::getForwardVector(const Transform& spacecraftTransform) const noexcept
{
    const auto& shipLoc = spacecraftTransform.translation;
    const auto& shipRot = spacecraftTransform.rotation;

    return normalize({ -sinf(shipRot.angle * M_PI / 180.0), 0.f, -cosf(shipRot.angle * M_PI / 180.0) });
}

void MoveTo::moveTo(const Vector& newMoveToLoc, bool startAtGrid)
 {
    moveToLoc = newMoveToLoc;

    if(startAtGrid)
    {
        const auto startGridPoint = getCloseObstacle();
        auto newTransform = spacecraft->getTransform();

        movePoints.push_back({ startGridPoint.X + moveX.X, 0.f, newTransform.translation.Z });
    }

    movePoints = calcPoints();

    auto currTransform = spacecraft->getTransform();
    bool xMovement = true;
    for(const auto& nextLoc : movePoints)
    {
        const auto diff = nextLoc - currTransform.translation;
        if(diff.length() == 0)
        {
            xMovement = !xMovement;
            continue;
        }
        const auto moveDir = normalize(diff);

        const auto forwardVector = getForwardVector(currTransform);

        rotateZAngle = getAngleBetweenVectors(forwardVector, diff);
        if(rotateZAngle != 0.f)
        {
            bool leftRotate = rotateZAngle == 180.f ? true : (moveDir == getLeftDir(currTransform));
            
            auto rotateTask = std::make_unique<RotateTask>(
                    spacecraft, rotateZAngle, leftRotate, currTransform.rotation.angle
                );

            currTransform.rotation.angle = rotateTask->getFutureAngle();
            transformTasks.push_back(std::move(rotateTask));
        }
        
        transformTasks.push_back(std::make_unique<MoveTask>(
                spacecraft, nextLoc, xMovement
        ));

        currTransform.translation = nextLoc;
        xMovement = !xMovement;
    }
    
    if(!transformTasks.empty())
    {
        currTask = transformTasks[0].get();
        currTask->init();
    }
    
    movePoints.clear();
}

void MoveTo::toggleMovement()
{
    moving = !moving;
}

void MoveTo::tick()
{
    if(currTask != nullptr)
    {   
        currTask->tick();
        if(currTask->isPendingToKill())
        {  
           transformTasks.erase(std::begin(transformTasks));
           if(transformTasks.empty())
           {
                currTask = nullptr;
                
                auto goldenAster = spacecraft->asteroidCollision<GoldenAsteroid>(spacecraft->getTransform());

                if (goldenAster != nullptr)
                {
                    spacecraft->notifyCollide(Tags::GoldenAsteroidTag);
                    goldenAster->die();
                }

                if(onMoveToEndDelegate)
                    onMoveToEndDelegate();
           }
           else 
           {
                currTask = transformTasks[0].get();
                if(currTask)
                    currTask->init();
           }
        }
    }
}

std::vector<Vector> MoveTo::calcPoints()
{   
    std::vector<Vector> r;
    auto shipTransform = getSpacecraftTransform();
    const auto& start = shipTransform.translation;
    fullPath = moveToLoc - start;

    minusX = fullPath.X < 0.f;
    minusZ = fullPath.Z < 0.f;

    auto pathCond = calcPath(start, true);
    if(pathCond.second)
        r = std::move(pathCond.first);
    else
    {
        if(isNearlyEqual(fullPath.X, 0.f, 0.1f))
        {
            r.push_back(shipTransform.translation + moveX);
        }
        else if(isNearlyEqual(fullPath.Z, 0.f, 0.1f))
        {
            r.push_back(shipTransform.translation + moveZ);
        }
    }

    return r;
}

Transform MoveTo::getSpacecraftTransform() const noexcept
{
    Transform r;
    
    r = spacecraft->getTransform();
    
    if(!movePoints.empty())
    {
        r.translation = movePoints[0];
    }

    return r;
}

std::pair<std::vector<Vector>, bool> MoveTo::calcPath(const Vector& from, bool x)
{
    const auto fullPathCopy = fullPath;
    auto movePointCond = getMovePoint(from, x);
    if(!movePointCond.second)
    {
        if(didntEvenTry)
        {
            didntEvenTry = false;
            if(!x ? std::abs(fullPath.X) > 0.f : std::abs(fullPath.Z) > 0.f)
            {
                auto pathCond = calcPath(movePointCond.first, !x);
                auto r = std::move(pathCond.first);

                r.insert(std::begin(r), movePointCond.first);
                return { r, true };
            }
            else
            {
                fullPath = fullPathCopy;
                return { {}, false };
            }
        }
        else
        {
            fullPath = fullPathCopy;
            return { {}, false };
        }
    }

    auto path{ movePointCond.first };

    if((fullPath.X != 0.f) || (fullPath.Z != 0.f))
    {
        auto pathCond = calcPath(path, !x);
        while(!pathCond.second)
        {
            const auto offset = (x ? getMoveX() : getMoveZ());
        
            path -= offset;
            fullPath += offset;
            
            pathCond = calcPath(path, !x);
        }
        
        auto r = std::move(pathCond.first);

        r.insert(std::begin(r), path);
        return { r, true };
    }
    else
    {   

        return { {movePointCond.first}, true };
    }
}

Vector MoveTo::getMoveX() const noexcept
{
    return minusX ? moveX * (-1.f) : moveX;
}

Vector MoveTo::getMoveZ() const noexcept
{
    return minusZ ? moveZ * (-1.f) : moveZ;
}

float MoveTo::getMoveAngle(bool xMove) const noexcept
{
    float r{};

    if(xMove)
    {
        const auto moveCurrX = getMoveX();
        r = moveCurrX.X > 0.f ? 270.f : 90.f;
    }
    else
    {
        const auto moveCurrZ = getMoveZ();
        r = moveCurrZ.Z > 0.f ? 180.f : 0.f;
    }

    return r;
} 

bool MoveTo::isPos(float v) const noexcept
{
    return v > 0.f;
}

std::pair<Vector, bool> MoveTo::getMovePoint(const Vector& from, bool x) const noexcept
{  
    if(x)
    {
        if(from.X == moveToLoc.X)
        {
            return { from, true };
        }
    }
    else
    {
        if(from.Z == moveToLoc.Z)
        {
            return { from, true };
        }
    }

    SimpleAsteroid* asteroid{};
    const auto offset = (x ? getMoveX() : getMoveZ());

    auto shipTransform = spacecraft->getTransform();
    shipTransform.translation = from + offset;

    shipTransform.rotation.angle = getMoveAngle(x);
    asteroid = spacecraft->asteroidCollision<SimpleAsteroid>(shipTransform);

    auto fullPathCopy = fullPath;
    fullPathCopy -= offset;

    if(x)
    {
         if(fullPathCopy.X == 0.f || (isPos(fullPath.X) && !isPos(fullPathCopy.X)) || (!isPos(fullPath.X) && isPos(fullPathCopy.X)))
         {
            if(asteroid)
            {
                didntEvenTry = true;
                return {from, false};
            }
            else
            {
                shipTransform.translation.X = moveToLoc.X;
                fullPath.X = 0.f;
                return { shipTransform.translation, true };
            }
         }
    }
    else
    {
        if(fullPathCopy.Z == 0.f || (isPos(fullPath.Z) && !isPos(fullPathCopy.Z)) || (!isPos(fullPath.Z) && isPos(fullPathCopy.Z)))
        {
            if(asteroid)
            {
                didntEvenTry = true;
                return {from, false};
            }
            else
            {
                shipTransform.translation.Z = moveToLoc.Z;
                fullPath.Z = 0.f;

                 return { shipTransform.translation, true };
            }
        }    
    }

    fullPath = fullPathCopy;

    if(asteroid == nullptr)
    {   
        return getMovePoint(shipTransform.translation, x);
    }
    else
    {
        const auto pathCond = (fullPath.X == 0.f) || (fullPath.Z == 0.f);
        return {from - offset, !pathCond};
    }
}

Vector MoveTo::getCloseGridPos() const noexcept
{
    Vector r;

    const auto shipLoc = spacecraft->getTransform().translation;

    r = Vector{
        roundf(shipLoc.X / std::abs(SpaceTravel::getXOffset())),
        roundf(shipLoc.Z / std::abs(SpaceTravel::getZOffset()))  
        };

    return r;
}

Vector MoveTo::getCloseObstacle() const noexcept
{
    auto closeActor = SpaceTravel::getCloseObstacle(*spacecraft);
    return closeActor != nullptr ? closeActor->getTransform().translation : Vector();
}


MoveTask::MoveTask(Spacecraft* pSpacecraft, const Vector& pMoveTo, bool pMoveX)
    :
    spacecraft{pSpacecraft},
    moveTo{pMoveTo},
    moveX{pMoveX}
{
   
}

void MoveTask::init()
{
    const auto loc = spacecraft->getTransform().translation;
}

void MoveTask::tick()
{
    auto newTransform = spacecraft->getTransform();
    auto& loc = newTransform.translation;
    const auto& rot = newTransform.rotation;    

    if(moveX)
    {
        if(!isNearlyEqual(loc.X, moveTo.X, 0.5f))
        {
            loc.X -= sin(rot.angle * M_PI / 180.0);
        }
        else
        {
            if(onEndDelegate)
                onEndDelegate();
            
            pendingToKill = true;
        }
    }
    else
    {
        if(!isNearlyEqual(loc.Z, moveTo.Z, 0.5f))
        {
            loc.Z -= cos(rot.angle * M_PI / 180.0);
        }
        else
        {
            if(onEndDelegate)
                 onEndDelegate();

            pendingToKill = true;
        }
    }

    auto goldenAster = spacecraft->asteroidCollision<GoldenAsteroid>(newTransform);

    if ((goldenAster != nullptr) && (goldenAster != SpaceTravel::staticViewport.getTrackAsteroid()))
    {
        spacecraft->notifyCollide(Tags::GoldenAsteroidTag);
        goldenAster->die();
    }

    spacecraft->setTransform(newTransform);
}

bool MoveTo::isRotateRight(const Vector& forwardVector, const Vector& newLocDir) const noexcept
{
    return false;
}

Vector MoveTo::getLeftDir(const Transform& spacecraftTransform) const noexcept
{
    Vector r;
    auto rotAngle = spacecraftTransform.rotation.angle;

    if(rotAngle == 0.f)
    {
        r = {-1.f, 0.f, 0.f};
    }
    else if(rotAngle == 90.f)
    {
        r = {0.f, 0.f, 1.f};
    }
    else if(rotAngle == 180.f)
    {
        r = {1.f, 0.f, 0.f};
    }
    else if(rotAngle == 270.f)
    {
        r = {0.f, 0.f, -1.f};
    } 

    return r;
}

RotateTask::RotateTask(Spacecraft* pSpacecraft, float pRotateAngleZ, bool pLeftRotate, float pInitialAngle)
		:
		spacecraft{pSpacecraft},
		rotateZAngle{pRotateAngleZ},
		leftRotate{pLeftRotate},
		t{},
        initialAngle{pInitialAngle}
	{

	}

void RotateTask::init()
{
}

float RotateTask::getFutureAngle() const noexcept
{
    auto newTransform = spacecraft->getTransform();
    auto& rot = newTransform.rotation;
    
    rot.angle = initialAngle + (leftRotate ? 1.f : -1.f) * rotateZAngle;
    
    if (rot.angle >= 360.0) rot.angle -= 360.0;
    if (rot.angle < 0.0) rot.angle += 360.0;
    
    return rot.angle;
}

void RotateTask::tick()
{
    t += SpaceTravel::getDeltaTime();
    auto newTransform = spacecraft->getTransform();
    auto& rot = newTransform.rotation;

    if(t >= (M_PI / 2.f))
    {
        pendingToKill = true;
        rot.angle = initialAngle + (leftRotate ? 1.f : -1.f) * rotateZAngle;
        
        if (rot.angle > 360.0) rot.angle -= 360.0;
        if (rot.angle < 0.0) rot.angle += 360.0;

        spacecraft->setTransform(newTransform);
    }
    else
    {   
        rot.angle = initialAngle + (leftRotate ? 1.f : -1.f) * rotateZAngle * sin(t);

        if (rot.angle > 360.0) rot.angle -= 360.0;
        if (rot.angle < 0.0) rot.angle += 360.0;

        spacecraft->setTransform(newTransform);
    }
}