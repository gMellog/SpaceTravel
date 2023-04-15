#pragma once

#include <iostream>
#include <vector>
#include <functional>
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

	void resetTick()
	{
		tickCalled = false;
	}	

	bool isDied() const noexcept;
	bool isTickCalled() const noexcept
	{
		return tickCalled;
	}

	std::vector<std::string> tags;
	
private:
	bool died;
	bool tickCalled;
};
	