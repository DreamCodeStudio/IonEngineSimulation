#include <iostream>
#include <vector>
#include <ctime>

/* Engines */
#include <irrlicht.h>
#include <SFML\Graphics.hpp>

/* Prototypes */
void updateIons(irr::scene::ISceneManager *manager, irr::scene::IAnimatedMesh *ionMesh, std::vector<irr::scene::IAnimatedMeshSceneNode*>* ionStorage, std::vector<float>* ionsVelocity, sf::Clock* timer, int creationSpeed, float acceleration);

int main()
{
	/* ########################################### Values for animation ########################################### */
	int ionsPerSecond = 5;										//Creation speed of the ions 
	float acceleration = 1.03f;

	std::vector<irr::scene::IAnimatedMeshSceneNode*> ions;		//vector to store the ions
	std::vector<float> ionsVelocity;

	srand(static_cast<unsigned int>(time(NULL)));				//Random seed (used in the updateIons() function)

	/* ########################################### Create irrlicht window ########################################### */
	irr::IrrlichtDevice *device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(1280, 720), 16U, false, false, true, 0);

	/* Get VideoDriver, SceneManager, Camera etc. */
	irr::video::IVideoDriver *driver = device->getVideoDriver();
	irr::scene::ISceneManager *manager = device->getSceneManager();
	irr::scene::ICameraSceneNode *camera = manager->addCameraSceneNodeMaya(0, -800, 200, 1000, -1, 20, true);
	camera->setTarget(irr::core::vector3df(0, 4, 0));

	/* ########################################### Add the 3D objects ########################################### */
	
	/* Load the Ion Engine */
	irr::scene::IAnimatedMesh *ionEngineMesh = manager->getMesh("Data\\ionEngine.3ds"); //Get the raw mesh
	irr::scene::IAnimatedMeshSceneNode *ionEngine = manager->addAnimatedMeshSceneNode(ionEngineMesh, 0, -1, irr::core::vector3df(0, 0, 0), irr::core::vector3df(0, 0, 0), irr::core::vector3df(1, 1, 1), false); //Add a scenenode so the user can see it
	ionEngine->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);

	/* Load the mesh for the ions */
	irr::scene::IAnimatedMesh *ionMesh = manager->getMesh("Data\\ion.3ds");

	/* Add some light to the scene */
	irr::scene::ILightSceneNode *light1 = manager->addLightSceneNode(0, irr::core::vector3df(35, 5, -55), irr::video::SColor(255, 255, 255, 255), 10, -1);;
	manager->setAmbientLight(irr::video::SColor(90, 90, 90, 90));

	/* Set up the timer clock to create the number of ions in a given time */
	sf::Clock timer;
	timer.restart();

	/* ########################################### Main Loop ########################################### */
	while (device->run())
	{
		/* Begin scene with grey background */
		driver->beginScene(true, true, irr::video::SColor(200, 200, 200, 200));

		/* Draw all the added objects */
		manager->drawAll();

		/* Update */
		light1->setPosition(camera->getPosition());
		updateIons(manager, ionMesh, &ions, &ionsVelocity, &timer, ionsPerSecond, acceleration);

		driver->endScene();
	}
}

void updateIons(irr::scene::ISceneManager *manager, irr::scene::IAnimatedMesh *ionMesh, std::vector<irr::scene::IAnimatedMeshSceneNode*>* ionStorage, std::vector<float>* ionsVelocity, sf::Clock* timer, int creationSpeed, float acceleration)
{
	/* Create new ions */
	if (timer->getElapsedTime().asMilliseconds() > sf::milliseconds(1000 / creationSpeed).asMilliseconds()) //Check every second 
	{
		ionStorage->push_back(manager->addAnimatedMeshSceneNode(ionMesh, 0, -1, irr::core::vector3df(0 + ((rand() % 2) - 0.5f) + ((rand() % 2) - 0.5f), 5 + ((rand() % 2) - 0.5f) + ((rand() % 2) - 0.5f), -5.5f + ((rand() % 2) - 0.5f) + ((rand() % 2) - 0.5f)), irr::core::vector3df(0, 0, 0), irr::core::vector3df(0.2f, 0.2f, 0.2f), false));
		ionsVelocity->push_back(0.05f);
		timer->restart();
	}

	/* Update all the existing ions */
	for (unsigned int i = 0; i < ionStorage->size(); i++)
	{
		ionsVelocity->at(i) = ionsVelocity->at(i) * acceleration;
		ionStorage->at(i)->setPosition(irr::core::vector3df(ionStorage->at(i)->getPosition().X, ionStorage->at(i)->getPosition().Y, ionStorage->at(i)->getPosition().Z + ionsVelocity->at(i)));
		
		/* Delete ion if it's out of range */
		if (ionStorage->at(i)->getPosition().Z > 50)
		{
			ionStorage->at(i)->remove();
			ionStorage->erase(ionStorage->begin() + i, ionStorage->begin() + i + 1);
			ionsVelocity->erase(ionsVelocity->begin() + i, ionsVelocity->begin() + i + 1);
		}
	}
}

