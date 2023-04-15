#include "Spacecraft.h"
#include "SpaceTravel.h"
#include "SimpleAsteroid.h"
#include "GoldenAsteroid.h"
#include "StaticView.h"


std::vector<std::pair<bool, std::size_t>> Spacecraft::getInitVKeyMappings() const
{
    return
    {
        { false, GLUT_KEY_LEFT },
        { false, GLUT_KEY_RIGHT },
        { false, GLUT_KEY_UP },
        { false, GLUT_KEY_DOWN }
    };
}

void Spacecraft::init()
{
    SpaceTravel::staticViewport.addNoGoldenAsteroidsListener(
        [this]()
    {
        pathfinder.addOnMoveToEndDelegate(
        [this]
        ()
        {
            moveTime = false;
            stopMoveTo = true;
        });

        pathfinder.moveTo(startPos, true);
    });
}

void Spacecraft::tick(float deltaTime)
{
	Actor::tick(deltaTime);

    if(moveTime)
    {   
        pathfinder.tick();
    }

    if(!moveTime)
    {
        handleInput();
    }

    draw();
}

void Spacecraft::setTransform(const Transform& newTransform)
{
    transform = newTransform;
}

Transform Spacecraft::getTransform() const
{
    return transform;
}

float Spacecraft::getHeight() const noexcept
{
    return height;
}

void Spacecraft::keyInput(int key, int x, int y)
{
    if (key == ' ' && !stopMoveTo)
    {
        moveTime = !moveTime;
        
        if(moveTime)
        {
            if(transform.translation != stopPos)
            {
                pathfinder = MoveTo(this);
            }

            SpaceTravel::staticViewport.addSetupCameraLocListener(
            [this, keyPressed = true]() mutable
            {
                const auto pos = SpaceTravel::staticViewport.getCameraEyePos();
                
                pathfinder.moveTo(pos, keyPressed);
                keyPressed = false;
            });
        }
        else 
        {
            stopPos = transform.translation;
        }
    }
}

    void Spacecraft::handleInput()
    {
        for (const auto& keyMapping : vKeyMappings)
        {
            if (!keyMapping.first) continue;

            int key = keyMapping.second;

            auto newTransform = transform;
            auto& loc = newTransform.translation;
            auto& rot = newTransform.rotation;

            const auto rotateAngle = 2.5f;

            if (key == GLUT_KEY_LEFT) rot.angle += rotateAngle;
            else if (key == GLUT_KEY_RIGHT) rot.angle -= rotateAngle;
            else if (key == GLUT_KEY_UP)
            {
                loc.X -= sin(rot.angle * M_PI / 180.0);
                loc.Z -= cos(rot.angle * M_PI / 180.0);
            }
            else if (key == GLUT_KEY_DOWN)
            {
                loc.X += sin(rot.angle * M_PI / 180.0);
                loc.Z += cos(rot.angle * M_PI / 180.0);
            }

            if (rot.angle > 360.0) rot.angle -= 360.0;
            if (rot.angle < 0.0) rot.angle += 360.0;

            const auto aster = asteroidCollision<SimpleAsteroid>(newTransform);
            collided = aster != nullptr;

            if (!collided)
            {
                auto goldenAster = asteroidCollision<GoldenAsteroid>(newTransform);

                if (goldenAster != nullptr && !goldenAster->isDied())
                {
                    notifyCollide(Tags::GoldenAsteroidTag);
                    goldenAster->die();
                }

                transform = newTransform;
            }
            else
            {
                notifyCollide(Tags::AsteroidTag);
            }
        
        }
    }

    void Spacecraft::specialKeyInput(int key, int x, int y)
    {
        auto it = std::find_if(vKeyMappings.begin(), vKeyMappings.end(),
            [key]
        (const std::pair<bool, std::size_t>& keyMapping)
            {
                return keyMapping.second == key;
            });

        if (it == std::end(vKeyMappings)) return;

        it->first = true;
    }

    void Spacecraft::specialUpFunc(int key, int x, int y)
    {
        auto it = std::find_if(vKeyMappings.begin(), vKeyMappings.end(),
            [key]
        (const std::pair<bool, std::size_t>& keyMapping)
            {
                return keyMapping.second == key;
            });

        if (it == std::end(vKeyMappings)) return;

        it->first = false;
    }

    int Spacecraft::addCollideListener(const Listener & newListener)
    {
        collideListeners.insert({ id, newListener });
        return id++;
    }

    void Spacecraft::removeCollideListener(const int listenerID)
    {
        auto it = collideListeners.find(listenerID);
        if (it != std::end(collideListeners))
        {
            collideListeners.erase(it);
        }
    }

    bool Spacecraft::isCollidedWithAsteroid() const noexcept
    {
        return collided;
    }

    void Spacecraft::draw()
    {
        const auto& loc = transform.translation;
        const auto& rot = transform.rotation;

        glPushMatrix();

        glTranslatef(loc.X, 0.0, loc.Z);

        glRotatef(rot.angle, rot.dirs.X, rot.dirs.Y, rot.dirs.Z);

        glCallList(spacecraftList);

        glPopMatrix();
    }

	Area Spacecraft::getSpacecraftArea() const noexcept
    {
        auto loc = transform.translation;

        Area r;
        
        const auto offsetX = height * 5;
        const auto offsetZ = height * 5;

        r.SW = {loc.X - offsetX, 0.f, loc.Z};
        r.NW = {loc.X - offsetX, 0.f, loc.Z - offsetZ};
        r.NE = {loc.X + offsetX, 0.f, loc.Z - offsetZ};
        r.SE = {loc.X + offsetX, 0.f, loc.Z};

        return r;
    }

    template<typename T>
    T* Spacecraft::asteroidCollision(const Transform & newTransform) const noexcept
    {   
        const auto area = getSpacecraftArea();
        const auto closeActors = SpaceTravel::getActorsCloseToArea(area);

        for (const auto& actor : closeActors)
        {
            for (const auto& tag : actor->tags)
            {
                if (tag == T::tag)
                {
                    auto& asteroid = static_cast<T&>(*actor);

                    const auto& loc = newTransform.translation;
                    const auto angle = newTransform.rotation.angle;

                    const auto offset = Vector{
                        loc.X - (height / 2) * sinf(M_PI / 180.f * angle),
                        0.f,
                        loc.Z - (height / 2) * cosf(M_PI / 180.f * angle)
                    };

                    const auto asteroidLoc = asteroid.getTransform().translation;

                    if(isNearlyEqual(loc.X, asteroidLoc.X, 0.25f) && isNearlyEqual(loc.Z, asteroidLoc.Z, 0.25f) || 
                        checkSphereCollision(offset, 7.072f,
                        asteroid.getTransform().translation, asteroid.getRadius()))
                    {
                        return &asteroid;
                    }
                }
            }
        }

        return nullptr;
    }

    void Spacecraft::notifyCollide(const std::string & collideTag)
    {
        for (const auto& listener : collideListeners)
            listener.second(collideTag);
    }

std::unique_ptr<Spacecraft> createSpacecraft()
{
	return std::make_unique<Spacecraft>(
		Transform{ {0.f, 0.f, 0.f}, {}, {0.f, {0.f, 1.f, 0.f}} });
}

const std::string Spacecraft::tag{ "Spacecraft" };
