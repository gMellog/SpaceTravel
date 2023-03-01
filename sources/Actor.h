#pragma once

#include <iostream>
#include <vector>
#include "Utils.h"

struct Actor
{
	Actor()
		:
		died{}
	{

	}

	virtual void init() {}
	virtual ~Actor() = default;
	virtual void tick(float deltaTime) = 0;
	virtual void setTransform(const Transform& newTransform) = 0;
	virtual Transform getTransform() const = 0;
	virtual void die();
		
	bool isDied() const noexcept;

	std::vector<std::string> tags;
private:
	bool died;
};
	