#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>

/* Engines */
#include <irrlicht.h>
#include <SFML\Graphics.hpp>

/* Prototypes */
void updateIons(irr::scene::ISceneManager *manager, irr::scene::IAnimatedMesh *ionMesh, std::vector<irr::scene::IAnimatedMeshSceneNode*>* ionStorage, std::vector<float>* ionsVelocity, sf::Clock* timer, irr::gui::IGUIStaticText *forceText, 
				irr::gui::IGUIStaticText *ampereText, irr::gui::IGUIStaticText *voltText, irr::gui::IGUIStaticText *velocityText, int creationSpeed, float acceleration);

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

	/* ########################################### Create GUI ########################################### */
	irr::gui::IGUIEnvironment *gui = manager->getGUIEnvironment();
	irr::gui::IGUIFont *font = gui->getFont("Data\\Font\\rpgfont.xml");
	gui->getSkin()->setFont(font);

	irr::gui::IGUIScrollBar *scrollBarVolt = gui->addScrollBar(true, irr::core::rect<irr::s32>(10, 10, 300, 30), 0, -1);
	scrollBarVolt->setMax(150);
	scrollBarVolt->setMin(99);
	scrollBarVolt->setPos(101);
	gui->addStaticText(irr::core::stringw("Spannung in V Min: 0 V Max: 1000 V").c_str(), irr::core::rect<irr::s32>(320, 10, 540, 30), true, true, 0, -1, false);
	
	irr::gui::IGUIScrollBar *scrollBarAmount = gui->addScrollBar(true, irr::core::rect<irr::s32>(10, 70, 300, 90), 0, -1);
	scrollBarAmount->setMax(20);
	scrollBarAmount->setMin(0);
	gui->addStaticText(irr::core::stringw("Anzahl Ionen pro Sekunde in *10^13 Min: 0 Max: 1 * 10^15 ").c_str(), irr::core::rect<irr::s32>(320, 70, 670, 90), true, true, 0, -1, false);
	
	/* Output */
	irr::gui::IGUIStaticText *force = gui->addStaticText(irr::core::stringw("0").c_str(), irr::core::rect<irr::s32>(10, 100, 100, 120), true, true, 0, -1, false);
	irr::gui::IGUIStaticText *ampere = gui->addStaticText(irr::core::stringw("0").c_str(), irr::core::rect<irr::s32>(10, 130, 100, 150), true, true, 0, -1, false);
	irr::gui::IGUIStaticText *volt = gui->addStaticText(irr::core::stringw("0").c_str(), irr::core::rect<irr::s32>(10, 160, 100, 180), true, true, 0, -1, false);
	irr::gui::IGUIStaticText *velocity = gui->addStaticText(irr::core::stringw("0").c_str(), irr::core::rect<irr::s32>(10, 190, 100, 210), true, true, 0, -1, false);

	/* ########################################### Main Loop ########################################### */
	while (device->run())
	{
		/* Begin scene with grey background */
		driver->beginScene(true, true, irr::video::SColor(200, 200, 200, 200));

		/* Draw all the added objects */
		manager->drawAll();
		gui->drawAll();

		/* Update */
		light1->setPosition(camera->getPosition());
		updateIons(manager, ionMesh, &ions, &ionsVelocity, &timer, force, ampere, volt, velocity, ionsPerSecond, acceleration);

		acceleration = scrollBarVolt->getPos() / 100.0f;
		ionsPerSecond = scrollBarAmount->getPos();

		driver->endScene();
	}
}

void updateIons(irr::scene::ISceneManager *manager, irr::scene::IAnimatedMesh *ionMesh, std::vector<irr::scene::IAnimatedMeshSceneNode*>* ionStorage, std::vector<float>* ionsVelocity, sf::Clock* timer, irr::gui::IGUIStaticText *forceText, 
				irr::gui::IGUIStaticText *ampereText, irr::gui::IGUIStaticText *voltText, irr::gui::IGUIStaticText *velocityText, int creationSpeed, float acceleration)
{
	/* Create new ions */
	if (creationSpeed != 0) //Never ever devide by zero
	{
		if (timer->getElapsedTime().asMilliseconds() > sf::milliseconds(1000 / creationSpeed).asMilliseconds()) //Check every second 
		{
			ionStorage->push_back(manager->addAnimatedMeshSceneNode(ionMesh, 0, -1, irr::core::vector3df(0 + ((rand() % 2) - 0.5f) + ((rand() % 2) - 0.5f), 5 + ((rand() % 2) - 0.5f) + ((rand() % 2) - 0.5f), -5.5f + ((rand() % 2) - 0.5f) + ((rand() % 2) - 0.5f)), irr::core::vector3df(0, 0, 0), irr::core::vector3df(0.2f, 0.2f, 0.2f), false));
			ionsVelocity->push_back(0.05f);
			timer->restart();
		}
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

	/* Calculate force */
	float ionsAmount = static_cast<float>(creationSpeed * pow(10, 13));

	float ampere = static_cast<float>(ionsAmount * 1.602 * pow(10, -19));
	//std::cout << "Ampere: " << ampere << std::endl;

	float volt = ((acceleration * 100) - 99) * 19.60f;
	//std::cout << "Volt: " << volt << std::endl;

	float velocity = static_cast<float>(sqrt((2 * 1.602 * pow(10, -19) * volt) / (14 * 1.66054 * pow(10, -27))));
	//std::cout << "Geschwindigkeit: " << velocity << std::endl;

	float force = static_cast<float>((14 * (1.66054 * pow(10, -27))) * (ampere / (1.602 * pow(10, -19))) * velocity);
	//std::cout << "Kraft: " << force << std::endl;

	std::string forceStr;
	std::stringstream forceStream;
	forceStream << force;
	forceStream >> forceStr;
	forceStr = forceStr + " N";

	std::string ampereStr;
	std::stringstream ampereStream;
	ampereStream << ampere;
	ampereStream >> ampereStr;
	ampereStr = ampereStr + " A";

	std::string voltStr;
	std::stringstream voltStream;
	voltStream << volt;
	voltStream >> voltStr;
	voltStr = voltStr + " V";

	std::string velocityStr;
	std::stringstream velocityStream;
	velocityStream << velocity;
	velocityStream >> velocityStr;
	velocityStr = velocityStr + " m/s";

	forceText->setText(irr::core::stringw(forceStr.c_str()).c_str());
	ampereText->setText(irr::core::stringw(ampereStr.c_str()).c_str());
	voltText->setText(irr::core::stringw(voltStr.c_str()).c_str());
	velocityText->setText(irr::core::stringw(velocityStr.c_str()).c_str());

}

