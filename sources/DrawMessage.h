#pragma once

#include <string>
#include "Utils.h"

struct DrawMessage
{
	explicit DrawMessage(const std::string& pText, const Vector& pLoc, const Color& pColor, float pMessageTime)
		:
		text{ pText },
		loc{ pLoc },
		color{ pColor },
		messageTime{ pMessageTime },
		timeOut{},
		t{}
	{

	}

	void setLoc(const Vector& newLoc);
	Vector getLoc() const noexcept;
	void draw(float deltaTime);
	bool isTimeOut() const noexcept;

private:

	std::string text;
	Vector loc;
	Color color;
	float messageTime;
	bool timeOut;
	float t;
};

DrawMessage createCollideMessage(const Vector& loc);
DrawMessage createGoldMessage(const Vector& loc);
DrawMessage createFrustumCullingMessage(const Vector& loc, bool enabledState);
