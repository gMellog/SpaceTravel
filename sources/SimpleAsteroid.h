#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <string>
#include <functional>
#include "Tags.h"
#include "Actor.h"

struct SimpleAsteroid : Actor
{
SimpleAsteroid(float pR, const Transform& pTransform)
	:
	r{pR},
	transform{pTransform},
	color{getRandomColor()},
	drawFigure{}
{
	setRandomDrawFigureFunc();
	tags.push_back(tag);
}

void setRandomDrawFigureFunc();

void tick(float deltaTime) override;
void setTransform(const Transform& newTransform) override;
Transform getTransform() const override;

float getRadius() const noexcept;

static const std::string tag;

private:
		
void draw();

float r;
Transform transform;
Color color;

std::function<void()> drawFigure;
};

std::unique_ptr<SimpleAsteroid> createSimpleAsteroid(const Transform& transform);
	