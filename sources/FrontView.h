#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "Spacecraft.h"
#include "Utils.h"

struct FrontView
{
	FrontView(const Vector& pViewportStart, const Vector& pViewportSize)
		:
		viewportStart{ pViewportStart },
		viewportSize(pViewportSize),
		spacecraft{},
		collideAsteroid{}
	{
	}

	void draw();

private:

	Vector viewportStart;
	Vector viewportSize;
	Spacecraft* spacecraft;
	Actor* collideAsteroid;
};