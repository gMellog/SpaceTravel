#pragma once
#include <chrono>
#include "Actor.h"
#include "Utils.h"
#include "Tags.h"
#include "FrontView.h"
#include "StaticView.h"

struct SpaceTravel
{
	static void tick();
	static void drawSeparateLine();
	static void drawScene();
	static void animate(int value);
	static void createAsteroids();
	static void initActors();
	static void setup();
	static void resize(int w, int h);
	static void keyInput(unsigned char key, int x, int y);
	static void specialFunc(int key, int x, int y);
	static void specialUpFunc(int key, int x, int y);
	static void printInteraction();
	static int main(int argc, char** argv);
	
	static float getDeltaTime() 
	{
		return deltaTime;
	}

	static float getXOffset() 
	{
		return XOffset;
	}

	static float getZOffset() 
	{
		return ZOffset;
	}

	static int getScreenWidth() 
	{
		return width;
	}

	static int getScreenHeight() 
	{
		return height;
	}

	template<typename T>
	static inline T* getActorPointer()
	{	
		T* r{};

		for (auto& actor : actors)
		{
			for (const auto& tag : actor->tags)
			{
				if (tag == T::tag)
					r = static_cast<T*>(actor.get());
			}
		}
			
		return r;
	}

	template<typename T>
	static inline T& getActorRef()
	{
		T* r{ getActorPointer<T>() };

		if (r == nullptr)
		{
			std::cerr << "can't find " << T::tag << '\n';
			std::exit(1);
		}

		return *r;
	}


	static std::vector<std::unique_ptr<Actor>> actors;
	static StaticView staticViewport;
	static FrontView frontViewport;

	private:

	static std::chrono::system_clock::time_point tp;
	static float deltaTime;
	static float XOffset;
	static float ZOffset;
	static int animationPeriod;
	static int width;
	static int height;
};