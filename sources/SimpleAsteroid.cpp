#include "SimpleAsteroid.h"

void SimpleAsteroid::setRandomDrawFigureFunc()
{
	std::function<void()> setFunc;
	switch (Random::get().getRandomInt(0, 3))
	{
	case 0:
		setFunc = [this]() 
		{ 
			glPushMatrix();
			glScaled(r, r, r);
			glutWireDodecahedron(); 
			glPopMatrix();
		};
		break;
	case 1:
		setFunc = [this]() 
		{
			glPushMatrix();
			glScaled(r, r, r);
			glutWireIcosahedron(); 
			glPopMatrix();
		};
		break;
	case 2:
		setFunc = [this]() { glutWireSphere(r, static_cast<int>(r) * 6, static_cast<int>(r) * 6); };
		break;
	case 3:
		setFunc = [this]() 
		{ 
			glPushMatrix();
			glScaled(r, r, r);
			glutWireOctahedron(); 
			glPopMatrix();
		};
		break;
	default:
		break;
	}

	drawFigure = setFunc;
}

void SimpleAsteroid::tick(float deltaTime)
{
	Actor::tick(deltaTime);
	draw();
}

void SimpleAsteroid::setTransform(const Transform& newTransform)
{
	transform = newTransform;
}

Transform SimpleAsteroid::getTransform() const
{
	return transform;
}

float SimpleAsteroid::getRadius() const noexcept
{
	return r;
}

void SimpleAsteroid::draw()
{
	const auto& loc = transform.translation;

	glPushMatrix();
			
	glTranslatef(loc.X, loc.Y, loc.Z);
	glColor3fv(reinterpret_cast<GLfloat*>(&color));
		
	drawFigure();
	glPopMatrix();
}

std::unique_ptr<SimpleAsteroid> createSimpleAsteroid(const Transform& transform)
{
	return std::make_unique<SimpleAsteroid>(Random::get().getRandomFloat(3.f, 5.f), transform);
}

const std::string SimpleAsteroid::tag{Tags::AsteroidTag};
	