#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <functional>
#include "Actor.h"
#include "Tags.h"

struct GoldenAsteroid : Actor
{
	explicit GoldenAsteroid(const Transform& pTransform);
	void tick(float deltaTime) override;
	void setTransform(const Transform& newTransform) override;
	Transform getTransform() const override;
	float getRadius() const noexcept;

	void die() override;

	static const std::string tag;

	void onDie(const std::function<void()>& pOnDieDelegate);
private:

	void draw();
	Color getGoldenColor() const noexcept;

	float r;
	Transform transform;
	float t;

	std::vector<std::function<void()>> dieListeners;

	void notifyDieListeners()
	{
		for(const auto& listener : dieListeners)
			listener();
	}
};


std::unique_ptr<GoldenAsteroid> createGoldenAsteroid(const Transform& transform);