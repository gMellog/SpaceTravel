#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>
#include <functional>
#include "Utils.h"

struct Spacecraft;

struct TransformTask
{
	TransformTask()
		:
	pendingToKill{}
	{

	}
	virtual ~TransformTask() = default;
	
	virtual void init() = 0;
	virtual void tick() = 0;

	bool isPendingToKill() const noexcept
	{
		return pendingToKill;
	}

	protected:

	bool pendingToKill;
};

struct MoveTask : TransformTask
{
	MoveTask(Spacecraft* pSpacecraft, const Vector& pMoveTo, bool pMoveX);
	void init() override;
	void tick() override;

	void addOnTaskEnd(const std::function<void()>& pOnEndDelegate)
	{
		onEndDelegate = pOnEndDelegate;
	}

	private:
	Spacecraft* spacecraft;
	Vector moveTo;
	bool moveX;

	std::function<void()> onEndDelegate;
};

struct RotateTask: TransformTask
{
	RotateTask(Spacecraft* pSpacecraft, float pRotateAngleZ, bool pLeftRotate, float pInitialAngle);

	void init() override;
	void tick() override;

	float getFutureAngle() const noexcept;

	private:

	Spacecraft* spacecraft;
	float rotateZAngle;
	bool leftRotate;
	float t;
	float initialAngle;
};

struct MoveTo
{
	explicit MoveTo(Spacecraft* pSpacecraft);

	void init();
	void moveTo(const Vector& newMoveToLoc, bool startAtGrid);

	void toggleMovement();
	void tick();

	void addOnMoveToEndDelegate(const std::function<void()>& pOnMoveToEndDelegate)
	{
		onMoveToEndDelegate = pOnMoveToEndDelegate;
	}

private:

	bool isRotateRight(const Vector& forwardVector, const Vector& newLocDir) const noexcept;
	
	std::vector<Vector> calcPoints();

	Vector getCloseObstacle() const noexcept;
	Vector getCloseGridPos() const noexcept;

	Vector getLeftDir(const Transform& spacecraftTransform) const noexcept;
	Vector getForwardVector(const Transform& spacecraftTransform) const noexcept;
	std::pair<Vector, bool> getMovePoint(const Vector& from, bool x) const noexcept; 

	std::pair<std::vector<Vector>, bool> calcPath(const Vector& from, bool x);

	Vector getMoveX() const noexcept;
	Vector getMoveZ() const noexcept;

	float getMoveAngle(bool xMove) const noexcept;
	bool isPos(float v) const noexcept;

	Transform getSpacecraftTransform() const noexcept;

	Spacecraft* spacecraft;
	Vector moveToLoc;
	Vector moveX;
	Vector moveZ;
	bool moving;
	bool rotating;

	float rotateZAngle;
	bool leftRotate;
	float t;
	std::vector<std::unique_ptr<TransformTask>> transformTasks;
	TransformTask* currTask;

	std::vector<Vector> movePoints;
	mutable Vector fullPath;

	bool minusX;
	bool minusZ;
	
	mutable bool didntEvenTry;
	std::function<void()> onMoveToEndDelegate;
	//There should be some delegate that will tell that we've reach a destination
};