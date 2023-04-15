#include "SpaceTravel.h"
#include "SimpleAsteroid.h"
#include "GoldenAsteroid.h"

	void drawCubeAt(const Vector& loc, int size)
	{
		glPushMatrix();
		glColor3f(1.f, 0.f, 0.f);
		
		glTranslatef(loc.X, -15.f, loc.Z);
		
		glScalef(1.f, 0.f, 1.f);
		glutSolidCube(size);
		
		glPopMatrix();
	}

	void SpaceTravel::drawDebugFrustum()
	{	
		const auto X = spacecraft->getTransform().translation.X;
		const auto Z = spacecraft->getTransform().translation.Z;
		const auto angle = spacecraft->getTransform().rotation.angle;

		drawCubeAt({static_cast<float>(X - 10 * sinf((M_PI / 180.0) * angle) - 7.072 * sinf((M_PI / 180.0) * (45.0 + angle))),
				-15.f,
				static_cast<float>(Z - 10 * cosf((M_PI / 180.0) * angle) - 7.072 * cos((M_PI / 180.0) * (45.0 + angle)))}, 1.f);

		drawCubeAt({static_cast<float>(X - 10 * sinf((M_PI / 180.0) * angle) + 7.072 * sinf((M_PI / 180.0) * (45.0 - angle))),
				-15.f,
				static_cast<float>(Z - 10 * cosf((M_PI / 180.0) * angle) - 7.072 * cos((M_PI / 180.0) * (45.0 - angle)))}, 1.f);

		drawCubeAt({
				static_cast<float>(X - 10 * sinf((M_PI / 180.0) * angle) - 100 * sinf((M_PI / 180.0) * (45.0 + angle))),
				0.f,
				static_cast<float>(Z - 10 * cosf((M_PI / 180.0) * angle) - 100 * cosf((M_PI / 180.0) * (45.0 + angle))),
			}, 1.f);

		drawCubeAt({
				static_cast<float>(X - 10 * sinf((M_PI / 180.0) * angle) + 100 * sinf((M_PI / 180.0) * (45.0 - angle))),
				0.f,
				static_cast<float>(Z - 10 * cosf((M_PI / 180.0) * angle) - 100 * cosf((M_PI / 180.0) * (45.0 - angle))),
			}, 1.f);
		
	}

	void SpaceTravel::drawDebugInitialSquares()
	{
		const float initSize = (asterCols >= asterRows) ? 
			(asterCols - 1) * XOffset + 12.0 : 
			(asterRows - 1) * XOffset + 12.0;

		drawCubeAt({-initSize / 2.0, 0.f, ZOffset}, 5.f);
		drawCubeAt({-initSize / 2.0, 0.f, ZOffset - initSize / 2.0}, 5.f);
		drawCubeAt({0, 0.f, ZOffset - initSize / 2.0}, 5.f);
		drawCubeAt({0, 0.f, ZOffset}, 5.f);
	}

    void SpaceTravel::tick(const Area& area)
	{
		std::vector<Actor*> diedActors;

		if(frustumCulling)
		{
			quadtree.drawObjects(area);
			spacecraft->tick(deltaTime);
		}

		for (auto& actor : actors)
		{
			if (!actor->isDied())
			{
				if(!frustumCulling)
				{
					actor->tick(deltaTime);
				}
			}
			else 
			{
				diedActors.push_back(actor.get());
			}
		}

		for(auto& actor: actors)
		{	
			actor->resetTick();
		}

		if(!diedActors.empty())
		{
			auto dieFirstIt = std::remove_if(std::begin(actors), std::end(actors),
				[&diedActors]
			(std::unique_ptr<Actor>& actor)
				{
					for (auto it = std::begin(diedActors); it != std::end(diedActors); ++it)
					{
						if (*it == actor.get())
						{
							diedActors.erase(it);
							return true;
						}
					}

					return false;
				});

			actors.erase(dieFirstIt, std::end(actors));
		}
	}

	void SpaceTravel::drawSeparateLine()
	{
		const std::vector<Vector> v{
			{0.f, 5.f, -5.f},
			{0.f, -5.f, -5.f}
		};

		glViewport(0, 0, width, height);
		glLoadIdentity();
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, v.data());

		glColor3f(1.f, 1.f, 1.f);
		glDrawArrays(GL_LINES, 0, v.size());
		glDisableClientState(GL_VERTEX_ARRAY);
	}	

	Area getAreaForView(float X, float Z, float angle, float R, float frustumVertex1, float frustumVertex2)
	{	
		const auto v1 = frustumVertex1;
		const auto v2 = frustumVertex2;

		return{
			{
				X - R * sinf(angle) - frustumVertex1 * sinf(angle + M_PI / 4),
				0.f,
				Z - R * cosf(angle) - frustumVertex1 * cosf(angle + M_PI / 4),
			},
			
			{
				X - R * sinf(angle) - frustumVertex2 * sinf(angle + M_PI / 4),
				0.f,
				Z - R * cosf(angle) - frustumVertex2 * cosf(angle + M_PI / 4),
			},

			{
				X - R * sinf(angle) - frustumVertex2 * sinf(angle - M_PI / 4),
				0.f,
				Z - R * cosf(angle) - frustumVertex2 * cosf(angle - M_PI / 4),
			},
			
			{
				X - R * sin(angle) - frustumVertex1 * sin(angle - M_PI / 4),
				0.f,
				Z - R * cos(angle) - frustumVertex1 * cos(angle - M_PI / 4)
			}
		};
	}

	Area SpaceTravel::getFrontArea()
	{
		const auto X = spacecraft->getTransform().translation.X;
		const auto Z = spacecraft->getTransform().translation.Z;
		const auto angle = toRadians(spacecraft->getTransform().rotation.angle);
		
		return getAreaForView(X, Z, angle, 10.f, 7.072, 353.3);
	}

	Area SpaceTravel::getStaticArea()
	{
		const auto loc = staticViewport.getCameraEyePos();
		const auto spacecraftLoc = spacecraft->getTransform().translation;
		const auto X = loc.X;
		const auto Z = loc.Z;
		const auto diff = spacecraftLoc - loc;
        const auto angle = getXZAngle(diff) - M_PI_2;
		
		return getAreaForView(X, Z, angle, 10.f, 7.072, 450);
	}

	void SpaceTravel::drawScene(void)
	{
		using namespace std::chrono;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		auto d = duration_cast<milliseconds>(system_clock::now() - tp);
		deltaTime = float(d.count()) * milliseconds::period::num / milliseconds::period::den;
		tp = system_clock::now();

		frontViewport.draw();

		tick(getFrontArea());

		drawSeparateLine();

		staticViewport.draw();

		tick(getStaticArea());

		glutSwapBuffers();
	}

	void SpaceTravel::animate(int value)
	{
		glutPostRedisplay();
		glutTimerFunc(animationPeriod, animate, 1);
	}

	void SpaceTravel::createAsteroids()
	{
		const auto columns = asterCols;
		const auto rows = asterRows;

		int goldAsteroids = 0;

		for (int i = 0; i < columns; i++)
			for (int j = 0; j < rows; j++)
			{
				auto pos = Vector{
					XOffset * (-columns / 2.f + i),
					0.f,
					-40.f + ZOffset * j
				};

				if (!(columns % 2))
				{
					pos.X += 15;
				}

				std::unique_ptr<Actor> asteroid;

				if (Random::get().getRandomFloat(0.f, 1.f) > 0.8f)
					asteroid = createGoldenAsteroid({ pos });
				else 
					asteroid = createSimpleAsteroid({ pos });

				actors.push_back(std::move(asteroid));
			}
	}

	void SpaceTravel::initActors()
	{		
		auto spacecraftObj = createSpacecraft();
		spacecraft = spacecraftObj.get();
		actors.push_back(std::move(spacecraftObj));
		createAsteroids();

		for(const auto& actor : actors)
			actor->init();
	}

	void SpaceTravel::setup(void)
	{
		glClearColor(.0, .0, .0, .0);
		glEnable(GL_DEPTH_TEST);

		initActors();

		const float initSize = (asterCols >= asterRows) ? 
			(asterCols) * XOffset + 6.0 : 
			(asterRows) * std::abs(ZOffset) + 6.0;
		
		quadtree.init(-initSize / 2.0, ZOffset, initSize);

		tp = std::chrono::system_clock::now();
		animate(1);
	}

	Actor* SpaceTravel::getCloseObstacle(const Spacecraft& spacecraft)
	{
		Actor* closeActor{};
		const auto spacecraftLoc = spacecraft.getTransform().translation;

		for(auto& actor : SpaceTravel::actors)
		{
			for(const auto& tag : actor->tags)
			{
				if(tag == Tags::AsteroidTag)
				{
					const auto actorLoc = actor->getTransform().translation;
					
					if(closeActor != nullptr)
					{
						const auto closeActorLoc = closeActor->getTransform().translation;
						if((spacecraftLoc - closeActorLoc).length() > (spacecraftLoc - actorLoc).length())
							closeActor = actor.get();
					}
					else
					{
						closeActor = actor.get();
					}
				}
			}
		}

		return closeActor;
	}

	void SpaceTravel::resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		
		width = w;
		height = h;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 250.0);

		glMatrixMode(GL_MODELVIEW);
	}

	void SpaceTravel::keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case 'C':
		case 'c':
			frustumCulling = !frustumCulling;
			staticViewport.addMessage(createFrustumCullingMessage( staticViewport.getUpLoc(), frustumCulling));
			break;
		case ' ':
			return;
		default:
			break;
		}

		spacecraft->keyInput(key, x, y);
	}

	void SpaceTravel::specialFunc(int key, int x, int y)
	{	
		spacecraft->specialKeyInput(key, x, y);
		glutPostRedisplay();
	}

	void SpaceTravel::specialUpFunc(int key, int x, int y)
	{
		spacecraft->specialUpFunc(key, x, y);
		glutPostRedisplay();
	}

	void SpaceTravel::printInteraction()
	{
		std::cout << "Wait until huge scene will load, it'll take a bit" << std::endl;
		std::cout << "Probably it will lagging, if so then work around with frustum culling so" << std::endl;

		std::cout << "Interaction:" << std::endl;
		std::cout << "Press space bar to force auto collect of golden asteroids(for now it doesn't work)" << std::endl;
		std::cout << "Or you could use arrow keys to move around" << std::endl;
		std::cout << "Turn on/off frustum culling by pressing C(English version) button" << std::endl;
	}

	int SpaceTravel::main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(width, height);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("SpaceTravel");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutSpecialFunc(specialFunc);
		glutSpecialUpFunc(specialUpFunc);
		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}

	int SpaceTravel::getMinStoreActors()
    {
        return minStoreActors;
    }

	int SpaceTravel::getMaxQuadtreeHeight()
	{
		return maxQuadtreeHeight;
	}

	std::vector<Actor*> SpaceTravel::getActorsCloseToArea(const Area& area)
	{
		return quadtree.getActorsCloseToArea(area);
	}

	Quadtree SpaceTravel::quadtree{};
	Spacecraft* SpaceTravel::spacecraft{};
	std::chrono::system_clock::time_point SpaceTravel::tp{};
	float SpaceTravel::deltaTime{};
	float SpaceTravel::XOffset{40.f};
	float SpaceTravel::ZOffset{-40.f};
	int SpaceTravel::animationPeriod{};
	int SpaceTravel::width{800};
	int SpaceTravel::height{400};

	int SpaceTravel::asterRows{100};
	int SpaceTravel::asterCols{100};
	int SpaceTravel::minStoreActors{12};
	int SpaceTravel::maxQuadtreeHeight{3};
	bool SpaceTravel::frustumCulling{};

	StaticView SpaceTravel::staticViewport{ {SpaceTravel::getScreenWidth() / 2.f, 0.f}, {SpaceTravel::getScreenWidth() / 2.f, SpaceTravel::getScreenHeight() * 1.f} };
	FrontView SpaceTravel::frontViewport{ {0.f, 0.f}, {SpaceTravel::getScreenWidth() / 2.f, SpaceTravel::getScreenHeight() * 1.f} };
	std::vector<std::unique_ptr<Actor>> SpaceTravel::actors{};