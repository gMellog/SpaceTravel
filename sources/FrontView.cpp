#include "FrontView.h"
#include "SpaceTravel.h"

void FrontView::draw()
{
    if (spacecraft == nullptr)
    {
        spacecraft = &SpaceTravel::getActorRef<Spacecraft>();
        spacecraft->addCollideListener(
        [this]
        (const std::string& Tag)
        {
            if(Tag == Tags::AsteroidTag)
            {
                collideAsteroid = SpaceTravel::getCloseObstacle(*spacecraft);
            }
        });
    }

    glViewport(viewportStart.X, viewportStart.Y, viewportSize.X, viewportSize.Y);
    glLoadIdentity();
    const auto height = spacecraft->getHeight();

    const auto transform = spacecraft->getTransform();
    const auto& loc = transform.translation;
    const auto& rot = transform.rotation;

    constexpr auto stareLength = 16.f;

    Vector eyePos;
    Vector forwardEyePoint;

    if(collideAsteroid)
    {
        eyePos = Vector{
            loc.X - height * sinf((M_PI / 180) * rot.angle),
            10.f,
            loc.Z + height * cosf((M_PI / 180.0) * rot.angle)
        };

        const auto asteroidLoc = collideAsteroid->getTransform().translation;

        forwardEyePoint = collideAsteroid->getTransform().translation;
    
        const auto lengthToAsteroid = (spacecraft->getTransform().translation - asteroidLoc).length();

        if(lengthToAsteroid > stareLength)
            collideAsteroid = nullptr;
    }
    else
    {
        eyePos = Vector{
            loc.X - height * sinf((M_PI / 180) * rot.angle),
            0.f,
            loc.Z - height * cosf((M_PI / 180.0) * rot.angle)
        };

        forwardEyePoint = Vector{
            loc.X - (height + 1.f) * sinf((M_PI / 180.0) * rot.angle),
            0.f,
            loc.Z - (height + 1.f) * cosf((M_PI / 180.0) * rot.angle)
        };
    }

    gluLookAt(eyePos.X, eyePos.Y, eyePos.Z,
        forwardEyePoint.X, forwardEyePoint.Y, forwardEyePoint.Z,
        0.f, 1.f, 0.f);
}