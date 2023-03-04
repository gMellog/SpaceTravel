#include "SpaceTravel.h"
#include "SimpleAsteroid.h"
#include "GoldenAsteroid.h"

    void SpaceTravel::tick()
	{
		std::vector<Actor*> diedActors;
		
		for (auto& actor : actors)
		{
			if (!actor->isDied())
			{
				actor->tick(deltaTime);
			}
			else 
			{
				diedActors.push_back(actor.get());
			}
		}

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

	void SpaceTravel::drawScene(void)
	{
		using namespace std::chrono;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		auto d = duration_cast<milliseconds>(system_clock::now() - tp);
		deltaTime = float(d.count()) * milliseconds::period::num / milliseconds::period::den;
		tp = system_clock::now();

		frontViewport.draw();
		tick();
		drawSeparateLine();

		staticViewport.draw();
		tick();

		glutSwapBuffers();
	}

	void SpaceTravel::animate(int value)
	{
		glutPostRedisplay();
		glutTimerFunc(animationPeriod, animate, 1);
	}

	void SpaceTravel::createAsteroids()
	{
		const auto columns = 8;
		const auto rows = 6;

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
		auto spacecraft = createSpacecraft();
		actors.push_back(std::move(spacecraft));
		createAsteroids();

		for(const auto& actor : actors)
			actor->init();
	}

	void SpaceTravel::setup(void)
	{
		glClearColor(.0, .0, .0, .0);
		glEnable(GL_DEPTH_TEST);
		initActors();
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
		default:
			break;
		}

		auto& spacecraft = getActorRef<Spacecraft>();
		spacecraft.keyInput(key, x, y);
	}

	void SpaceTravel::specialFunc(int key, int x, int y)
	{
		auto& spacecraft = getActorRef<Spacecraft>();
		spacecraft.specialKeyInput(key, x, y);

		glutPostRedisplay();
	}

	void SpaceTravel::specialUpFunc(int key, int x, int y)
	{
		auto& spacecraft = getActorRef<Spacecraft>();
		spacecraft.specialUpFunc(key, x, y);

		glutPostRedisplay();
	}

	void SpaceTravel::printInteraction()
	{
		std::cout << "Interaction:" << std::endl;
		std::cout << "Press space bar to force auto collect of golden asteroids" << std::endl;
		std::cout << "Or you could use arrow keys to move around" << std::endl;
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

std::chrono::system_clock::time_point SpaceTravel::tp{};
float SpaceTravel::deltaTime{};
float SpaceTravel::XOffset{40.f};
float SpaceTravel::ZOffset{-40.f};
int SpaceTravel::animationPeriod{};
int SpaceTravel::width{800};
int SpaceTravel::height{400};

StaticView SpaceTravel::staticViewport{ {SpaceTravel::getScreenWidth() / 2.f, 0.f}, {SpaceTravel::getScreenWidth() / 2.f, SpaceTravel::getScreenHeight() * 1.f} };
FrontView SpaceTravel::frontViewport{ {0.f, 0.f}, {SpaceTravel::getScreenWidth() / 2.f, SpaceTravel::getScreenHeight() * 1.f} };
std::vector<std::unique_ptr<Actor>> SpaceTravel::actors{};