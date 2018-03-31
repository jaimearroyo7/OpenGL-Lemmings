#include <iostream>
#include <cmath>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include <GL/glew.h>
#include <GL/glut.h>


Scene::Scene()
{
	gamestate = MENU;
	map = NULL;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
}

void Scene::initCursor() {
	spritesheetCursor.loadFromFile("images/cursor.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheetCursor.setMinFilter(GL_NEAREST);
	spritesheetCursor.setMagFilter(GL_NEAREST);
	cursor = Sprite::createSprite(glm::ivec2(15, 15), glm::vec2(0.5f, 1.0f), &spritesheetCursor, &simpleTexProgram);


	/*	Animation 0: Cuadrado
		Animation 1: Flecha		*/

	cursor->setNumberAnimations(2);
	cursor->setAnimationSpeed(0, 1);
	cursor->setAnimationSpeed(1, 1);
	
	cursor->addKeyframe(0, glm::vec2(0.0f, 0.0f));
	cursor->addKeyframe(1, glm::vec2(0.5f, 0.0f));

	cursor->changeAnimation(1);
	cursor->setPosition(glm::vec2(posX, posY));
}

void Scene::initSeleccion() {
	renderSeleccionLemming = false;
	spritesheetSeleccionLemming.loadFromFile("images/seleccion.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheetSeleccionLemming.setMinFilter(GL_NEAREST);
	spritesheetSeleccionLemming.setMagFilter(GL_NEAREST);
	seleccionLemming = Sprite::createSprite(glm::ivec2(20, 32), glm::vec2(1.0f, 1.0f), &spritesheetSeleccionLemming, &simpleTexProgram);


	seleccionLemming->setNumberAnimations(1);
	seleccionLemming->setAnimationSpeed(0, 1);

	seleccionLemming->addKeyframe(0, glm::vec2(0.0f, 0.0f));

	seleccionLemming->changeAnimation(0);
}

void Scene::initOpenDoor() {
	spritesheetDoor.loadFromFile("images/openDoor.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheetDoor.setMinFilter(GL_NEAREST);
	spritesheetDoor.setMagFilter(GL_NEAREST);
	openDoor = Sprite::createSprite(glm::ivec2(41, 21), glm::vec2(1, 1.0f / 10.0f), &spritesheetDoor, &simpleTexProgram);
	openDoor->setNumberAnimations(1);
	openDoor->setAnimationSpeed(0, 10);
	for (int i = 0; i<10; i++)
		openDoor->addKeyframe(0, glm::vec2(0.0f, float(i) / 10));

	openDoor->changeAnimation(0);

	openDoor->setPosition(glm::vec2(77, 30));
}

void Scene::initFinishDoor() {
	spritesheetfinishDoor.loadFromFile("images/finishDoor.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheetfinishDoor.setMinFilter(GL_NEAREST);
	spritesheetfinishDoor.setMagFilter(GL_NEAREST);
	finishDoor = Sprite::createSprite(glm::ivec2(33, 26), glm::vec2(1.0f / 3.0f, 1), &spritesheetfinishDoor, &simpleTexProgram);
	finishDoor->setNumberAnimations(1);
	finishDoor->setAnimationSpeed(0, 6);
	for (int i = 0; i<3; i++)
		finishDoor->addKeyframe(0, glm::vec2(float(i) / 3, 0.0f));

	finishDoor->changeAnimation(0);
	finishDoor->setPosition(glm::vec2(225, 107));
}

void Scene::init()
{
	// Select which font you want to use
	if (!text.init("fonts/OpenSans-Regular.ttf"))
		//if(!text.init("fonts/OpenSans-Bold.ttf"))
		//if(!text.init("fonts/DroidSerif.ttf"))
		cout << "Could not load font!!!" << endl;

	//factor
	float scale = 0.827586f;

	//scale = 1.0f;
	glm::vec2 geom[2] = { glm::vec2(0.f, 0.f), glm::vec2(float(CAMERA_WIDTH), float(CAMERA_HEIGHT)) };

	glm::vec2 texCoords[2] = { glm::vec2(120.f / 512.0, 0.f), glm::vec2((120.f + 320.f) / 512.0f, (160.f / 256.0f)) };
	projection = glm::ortho(0.f, float(CAMERA_WIDTH - 1), float(CAMERA_HEIGHT - 1), 0.f);
	initShaders();
	initCursor();
	switch (gamestate) {
		case PLAYING:
			//init some vars
			currentTime = 0.0f;
			id = -1;
			score = 0;
			lemmings.clear();
			lemmings.resize(5);
			posX = posY = 100;
			finish = false;
			stateSelected = false;
			pause = x2speed = exploding = false;

			geom[1] = glm::vec2(float(CAMERA_WIDTH), float(CAMERA_HEIGHT)*scale);
		

			//create Mao
			map = MaskedTexturedQuad::createTexturedQuad(geom, texCoords, maskedTexProgram);
			colorTexture.loadFromFile("images/fun1.png", TEXTURE_PIXEL_FORMAT_RGBA);
			colorTexture.setMinFilter(GL_NEAREST);
			colorTexture.setMagFilter(GL_NEAREST);
			maskTexture.loadFromFile("images/fun1_mask.png", TEXTURE_PIXEL_FORMAT_L);
			maskTexture.setMinFilter(GL_NEAREST);
			maskTexture.setMagFilter(GL_NEAREST);


			//create UI
			geom[0] = glm::vec2(0.0f, (float(CAMERA_HEIGHT)*scale)-1);
			geom[1] = glm::vec2(float(CAMERA_WIDTH), float(CAMERA_HEIGHT));
			texCoords[0] = glm::vec2(0.f, 0.f); texCoords[1] = glm::vec2(1.0f, 1.0f);
			UI = TexturedQuad::createTexturedQuad(geom, texCoords, imagesProgram);
			UITexture.loadFromFile("images/interface.png", TEXTURE_PIXEL_FORMAT_RGBA);

			//create Lemmings
			for (int i = 0; i < 5; ++i) {
				lemmings[i].init(glm::vec2(90, 30), simpleTexProgram, 2 + i * 5);
				lemmings[i].setMapMask(&maskTexture, &colorTexture);
			}


			//openDoor	
			initOpenDoor();

			//finishDoor
			initFinishDoor();

			//initRejillas de boton seleccionado
			initSeleccion();

			break;

		case MENU:
			texCoords[0] = glm::vec2(0.f, 0.f); texCoords[1] = glm::vec2(1.0f, 1.0f);
			menu = TexturedQuad::createTexturedQuad(geom, texCoords, imagesProgram);

			menuTexture.loadFromFile("images/portada.png", TEXTURE_PIXEL_FORMAT_RGB);
			break;
		default:
			break;
	}
	
}

unsigned int x = 0;

void Scene::update(int deltaTime)
{
	if(x2speed) deltaTime *= 2;
	currentTime += deltaTime;
	bool found = false;
	switch (gamestate) {
		case PLAYING:
			
			//buscar si estamos seleccionando a algun Lemming
			
			for (int i = lemmings.size() - 1; i >= 0 && !found; --i) {
				glm::ivec2 pos = lemmings[i].getSprite()->position();
				pos += glm::vec2(120, 0);

				if (pos.x <= posX && posX <= (pos.x + 16) && pos.y <= posY && posY <= (pos.y + 16) && lemmings[i].getState() != DEAD && lemmings[i].getState() != RESPAWN) {
					id = i;
					found = true;
				}
			}
			if (!found) id = -1;

			//actualizar el cursor
			cursor->setPosition(glm::vec2(posX - 120 - 8, posY - 8));

			if (id == -1) {
				cursor->changeAnimation(1);
			}
			else {
				cursor->changeAnimation(0);
			}
			cursor->update(deltaTime);


			if (openDoor->getKeyframe() != 9) openDoor->update(deltaTime);

			finishDoor->update(deltaTime);

			for (int i = 0; i < 5; ++i) {
				score += lemmings[i].update(deltaTime, int(currentTime / 1000));
			}

			finish = true;
			for (int i = 0; i < lemmings.size() && finish; ++i) {
				if (lemmings[i].getState() != DEAD) finish = false;
			}

			if (finish) {
				//Si has ganado, hacer lo que quieras.
				cout << score << endl;
				gamestate = MENU;
				init();
			}
			break;
		case MENU:
			cursor->setPosition(glm::vec2(posX - 120 - 8, posY - 8));
			cursor->update(deltaTime);
			break;
	}
}

void Scene::render()
{
	glm::mat4 modelview = glm::mat4(1.0f);

	switch (gamestate) {
		case PLAYING:

			imagesProgram.use();
			imagesProgram.setUniformMatrix4f("projection", projection);
			imagesProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
			imagesProgram.setUniformMatrix4f("modelview", modelview);
			UI->render(UITexture);

			maskedTexProgram.use();
			maskedTexProgram.setUniformMatrix4f("projection", projection);
			maskedTexProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
			
			maskedTexProgram.setUniformMatrix4f("modelview", modelview);
			map->render(maskedTexProgram, colorTexture, maskTexture);

			simpleTexProgram.use();
			simpleTexProgram.setUniformMatrix4f("projection", projection);
			simpleTexProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
			simpleTexProgram.setUniformMatrix4f("modelview", modelview);
			finishDoor->render();
			openDoor->render();
			seleccionLemming->setPosition(lemmingSelected);
			if (renderSeleccionLemming)
				seleccionLemming->render();

			if (renderSeleccionPause) {
				seleccionLemming->setPosition(glm::vec2(124, 160));
				seleccionLemming->render();
			}

			if (renderSeleccionExplosion) {
				seleccionLemming->setPosition(glm::vec2(145, 160));
				seleccionLemming->render();
			}

			if (x2speed) {
				seleccionLemming->setPosition(glm::vec2(165, 160));
				seleccionLemming->render();
			}

			for (int i = 0; i < 5; ++i) {
				lemmings[i].render();
			}

			cursor->render();

			break;

		case MENU:
			imagesProgram.use();
			imagesProgram.setUniformMatrix4f("projection", projection);
			imagesProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
			imagesProgram.setUniformMatrix4f("modelview", modelview);
			menu->render(menuTexture);


			textProgram.use();
			textProgram.setUniformMatrix4f("projection", projection);
			textProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
			textProgram.setUniformMatrix4f("modelview", modelview);
			text.render("Videogames!!!_", glm::vec2(10, CAMERA_HEIGHT - 20), 32, glm::vec4(0,0,0,1));


			simpleTexProgram.use();
			simpleTexProgram.setUniformMatrix4f("projection", projection);
			simpleTexProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
			simpleTexProgram.setUniformMatrix4f("modelview", modelview);

			cursor->render();
			
			break;
	}

}


void Scene::mouseMoved(int mouseX, int mouseY, bool bLeftButton, bool bRightButton)
{

	// Transform from mouse coordinates to map coordinates
	//   The map is enlarged 3 times and displaced 120 pixels
	posX = mouseX / 3 + 120;
	posY = mouseY / 3;

	switch (gamestate) {
		case PLAYING:
			if (bLeftButton && posY < 160) {				
				cout << id << endl;
				//convertir un Lemming a Blocker
				cout << mouseX << " " << mouseY << endl;
				cout << posX << " " << posY << endl;
				if (id != -1 && stateSelected) {
					//BLOCKER_STATE, BASHER, DIGGER_STATE, CLIMBER_STATE (Descomentar uno para probarlo).
					
					cout << lemmings[id].getSprite()->position().x << " " << lemmings[id].getSprite()->position().y << endl;
					lemmings[id].setState(lemmingsState);

				}
				eraseMask(mouseX, mouseY);
			}
			else if (bRightButton && posY < 160) {
				if (id != -1) {

					lemmings[id].setState(BLOCKER_STATE);
				}
				applyMask(mouseX, mouseY);
			}
			else if (bLeftButton && posY >= 160) {
				cout << posX << " " << posY << endl;
				int num = (posX - 4 - 120) / 20;
				switch (num) {
					case 0:
						lemmingsState = CLIMBER_STATE;
						renderSeleccionLemming = true;
						lemmingSelected = glm::vec2(2, 160);
						seleccionLemming->setPosition(lemmingSelected);
						
						cout << "climber" << endl;
						stateSelected = true;
						break;
					case 1:
						lemmingsState = EXPLOSION_STATE;
						renderSeleccionLemming = true;
						lemmingSelected = glm::vec2(22, 160);
						seleccionLemming->setPosition(lemmingSelected);
						cout << "Explosion " << endl;
						stateSelected = true;
						break;
					case 2:
						lemmingsState = BLOCKER_STATE;
						renderSeleccionLemming = true;
						lemmingSelected = glm::vec2(43, 160);
						seleccionLemming->setPosition(lemmingSelected);
						cout << "blocker" << endl;
						stateSelected = true;
						break;
					case 3:
						lemmingsState = BUILDER_STATE;
						renderSeleccionLemming = true;
						lemmingSelected = glm::vec2(63, 160);
						seleccionLemming->setPosition(lemmingSelected);
						cout << "Builder" << endl;
						stateSelected = true;
						break;
					case 4:
						lemmingsState = BASHER;
						renderSeleccionLemming = true;
						lemmingSelected = glm::vec2(83, 160);
						seleccionLemming->setPosition(lemmingSelected);
						cout << "basher" << endl;
						stateSelected = true;
						break;
					case 5:
						lemmingsState = DIGGER_STATE;
						renderSeleccionLemming = true;
						lemmingSelected = glm::vec2(104, 160);
						seleccionLemming->setPosition(lemmingSelected);
						cout << "digger" << endl;
						stateSelected = true;
						break;
					case 6:
						//PAUSE
						renderSeleccionPause = !renderSeleccionPause;
						cout << "pasue" << endl;
						break;
					case 7:
						//FULL EXPLOSION
						renderSeleccionExplosion = true;
						if (!exploding) {
							exploding = true;
							for (int i = 0; i < lemmings.size(); ++i) {
								lemmings[i].setState(EXPLOSION_STATE);
							}
						}
						cout << "Full Explosion" << endl;
						break;
					case 8:
						//x2 Speed
						x2speed = !x2speed;
						cout << "x2Speed" << endl;
						break;

				}
			}
			break;

		case MENU:
			if (bLeftButton) {
				gamestate = PLAYING;
				init();
			}
			break;
	}
}

void Scene::eraseMask(int mouseX, int mouseY)
{

	// Transform from mouse coordinates to map coordinates
	//   The map is enlarged 3 times and displaced 120 pixels
	posX = mouseX/3 + 120;
	posY = mouseY/3;

	cout << posX << " " << posY << endl;

	for(int y=max(0, posY-3); y<=min(maskTexture.height()-1, posY+3); y++)
		for(int x=max(0, posX-3); x<=min(maskTexture.width()-1, posX+3); x++)
			maskTexture.setPixel(x, y, 0);
}

void Scene::applyMask(int mouseX, int mouseY)
{

	// Transform from mouse coordinates to map coordinates
	//   The map is enlarged 3 times and displaced 120 pixels
	posX = mouseX/3 + 120;
	posY = mouseY/3;
	for (int y = max(0, posY - 3); y <= min(colorTexture.height() - 1, posY + 3); y++)
		for (int x = max(0, posX - 3); x <= min(colorTexture.width() - 1, posX + 3); x++)
			maskTexture.setPixel(x, y, 255);
			//colorTexture.setPixel(x, y, glm::ivec4(150, 150, 150, 255));
}


void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	simpleTexProgram.init();
	simpleTexProgram.addShader(vShader);
	simpleTexProgram.addShader(fShader);
	simpleTexProgram.link();
	if(!simpleTexProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << simpleTexProgram.log() << endl << endl;
	}
	simpleTexProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();

	vShader.initFromFile(VERTEX_SHADER, "shaders/maskedTexture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/maskedTexture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	maskedTexProgram.init();
	maskedTexProgram.addShader(vShader);
	maskedTexProgram.addShader(fShader);
	maskedTexProgram.link();
	if(!maskedTexProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << maskedTexProgram.log() << endl << endl;
	}
	maskedTexProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();


	vShader.initFromFile(VERTEX_SHADER, "shaders/images.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/images.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	imagesProgram.init();
	imagesProgram.addShader(vShader);
	imagesProgram.addShader(fShader);
	imagesProgram.link();
	if (!imagesProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << imagesProgram.log() << endl << endl;
	}
	imagesProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();



	vShader.initFromFile(VERTEX_SHADER, "shaders/text.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/text.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	textProgram.init();
	textProgram.addShader(vShader);
	textProgram.addShader(fShader);
	textProgram.link();
	if (!textProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << textProgram.log() << endl << endl;
	}
	textProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

int Scene::getgameState() {
	return gamestate;
}

