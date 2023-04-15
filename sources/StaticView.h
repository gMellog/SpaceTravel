#pragma once

#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "Spacecraft.h"
#include "DrawMessage.h"
#include "Utils.h"
#include "GoldenAsteroid.h"

struct StaticView
{
	StaticView(const Vector& pViewportStart, const Vector& pViewportSize)
		:
		viewportStart{ pViewportStart },
		viewportSize(pViewportSize),
		spacecraft{},
		upLoc{ -29.0, 20.0, -30.0 },
		boundToAsteroid{},
		focusCameraOnInitialPos{}
	{
	}

	void  draw();
	Vector getCameraEyePos() const noexcept
	{
		return { cameraEye.X, 0.f, cameraEye.Z };
	}

	void addSetupCameraLocListener(const std::function<void()>& cameraListener)
	{
		setupCameraListener = cameraListener;
		if(boundToAsteroid)
			setupCameraListener();
	}

	void addNoGoldenAsteroidsListener(const std::function<void()>& pNoGoldenAsteroidsListener)
	{
		noGoldenAsteroidsListener = pNoGoldenAsteroidsListener;
	}

	const GoldenAsteroid* getTrackAsteroid() const noexcept
	{
		return boundToAsteroid;
	}
	
	void addMessage(const DrawMessage& drawMessage);

	Vector getUpLoc() const noexcept
	{
		return upLoc;
	}

private:

	Vector getAmountOfSteps() const noexcept;
	void setupCamera();
	void drawMessages();

	Vector viewportStart;
	Vector viewportSize;
	Spacecraft* spacecraft;
	std::vector<DrawMessage> messages;
	Vector upLoc;
	Vector cameraEye;
	Vector prevCameraEye;
	std::function<void()> setupCameraListener;
	std::function<void()> noGoldenAsteroidsListener;
	GoldenAsteroid* boundToAsteroid;

	bool focusCameraOnInitialPos;
};
