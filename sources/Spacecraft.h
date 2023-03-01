#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>
#include <string>
#include <functional>
#include <map>
#include "Utils.h"
#include "Tags.h"
#include "Actor.h"
#include "MoveTo.h"

struct Spacecraft : Actor
{
	using Listener = std::function<void(const std::string&)>;

	explicit Spacecraft(const Transform pTransform)
		:
		transform{ pTransform },
		spacecraftList{ glGenLists(1) },
		r{ 5.f },
		height{ 10.f },
		collided{},
		id{},
		moveTime{},
		vKeyMappings{
			getInitVKeyMappings()
	},
	pathfinder{ this },
	pathfinderInited{}
	{

		glNewList(spacecraftList, GL_COMPILE);

		glPushMatrix();
		glRotatef(180.0, 0.0, 1.0, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		glutWireCone(r, height, 10, 10);
		glPopMatrix();

		glEndList();

		tags.push_back(tag);
	}

	std::vector<std::pair<bool, std::size_t>> getInitVKeyMappings() const;

	void init() override;
	void tick(float deltaTime) override;
	void setTransform(const Transform& newTransform) override;
	Transform getTransform() const override;

	float getHeight() const noexcept;
	void keyInput(int key, int x, int y);
	void handleInput();
	void specialKeyInput(int key, int x, int y);
	void specialUpFunc(int key, int x, int y);
	int addCollideListener(const Listener & newListener);
	void removeCollideListener(const int listenerID);
	bool isCollidedWithAsteroid() const noexcept;

	static const std::string tag;
	template<typename T>
		T* asteroidCollision(const Transform & newTransform) const noexcept;
	
	void notifyCollide(const std::string & collideTag);
	
	private:
		void draw();	


		Transform transform;
		unsigned int spacecraftList;
		float r;
		float height;

		bool collided;
		bool foundGold;

		unsigned int id;

		std::map<unsigned int, Listener> collideListeners;

		bool moveTime;

		std::vector< std::pair<bool, std::size_t> > vKeyMappings;

		MoveTo pathfinder;
		bool pathfinderInited;
};


std::unique_ptr<Spacecraft> createSpacecraft();