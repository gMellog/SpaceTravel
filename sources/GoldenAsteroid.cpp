#include "GoldenAsteroid.h"

GoldenAsteroid::GoldenAsteroid(const Transform& pTransform)
		:
		r{ Random::get().getRandomFloat(6.f, 8.f)},
		transform{ pTransform },
		t{}
	{
		tags.push_back(tag);
	}


void GoldenAsteroid::tick(float deltaTime)
{
    if (t > (2.0 * M_PI))
        t -= M_PI;

    t += deltaTime;

    draw();
}

void GoldenAsteroid::setTransform(const Transform& newTransform)
{
    transform = newTransform;
}

Transform GoldenAsteroid::getTransform() const
{
    return transform;
}

float GoldenAsteroid::getRadius() const noexcept
{
    return r;
}

void GoldenAsteroid::draw()
{
    const auto& loc = transform.translation;

    glPushMatrix();

    auto color = getGoldenColor();
    const auto coeff = std::abs(sinf(t));
    color.R *= coeff;
    color.G *= coeff;
    color.B *= coeff;

    glTranslatef(loc.X, loc.Y, loc.Z);
    glColor3fv(reinterpret_cast<GLfloat*>(&color));
    glutWireSphere(r, static_cast<int>(r) * 6, static_cast<int>(r) * 6);

    glPopMatrix();
}

Color GoldenAsteroid::getGoldenColor() const noexcept
{
    return { 1.f, 215.f / 255.f, 0.f };
}

std::unique_ptr<GoldenAsteroid> createGoldenAsteroid(const Transform& transform)
{
	return std::make_unique<GoldenAsteroid>(transform);
}

void GoldenAsteroid::onDie(const std::function<void()>& pOnDieDelegate)
{
    onDieDelegate = pOnDieDelegate;
}

void GoldenAsteroid::die()
{
    if(onDieDelegate)
    {   
        onDieDelegate();
    }

    Actor::die();
}

const std::string GoldenAsteroid::tag{ Tags::GoldenAsteroidTag };
