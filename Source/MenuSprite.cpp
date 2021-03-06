//-----------------------------------------------------------------------------
// File: MenuSprite.cpp
//
// Desc: This file contains the class that handles all the menu options.
//
// Created by Vasilescu Vlad
//-----------------------------------------------------------------------------

#include "MenuSprite.h"

//-----------------------------------------------------------------------------
// Name : MenuSprite () (Constructor)
// Desc : Constructor for the MenuSprite class.
//-----------------------------------------------------------------------------
MenuSprite::MenuSprite(const Vec2 position, const BackBuffer* BF)
{
	this->BF = BF;
	frameCounter = 0;

	auto posLoad = Vec2(position.x, position.y + 125);
	auto posSave = Vec2(position.x, position.y + 250);

	startText = new Sprite("data/menu_options/startgamedes.bmp", RGB(0xff, 0x00, 0xff));
	startText->mPosition = position;
	startText->mVelocity = Vec2(0, 0);
	startText->setBackBuffer(BF);
	
	loadText = new Sprite("data/menu_options/loadgamedes.bmp", RGB(0xff, 0x00, 0xff));
	loadText->mPosition = posLoad;
	loadText->mVelocity = Vec2(0, 0);
	loadText->setBackBuffer(BF);
	
	saveText = new Sprite("data/menu_options/savegamedes.bmp", RGB(0xff, 0x00, 0xff));
	saveText->mPosition = posSave;
	saveText->mVelocity = Vec2(0, 0);
	saveText->setBackBuffer(BF);

	select = START;
	updateSelect(START, true);
}

//-----------------------------------------------------------------------------
// Name : ~MenuSprite () (Destructor)
// Desc : Destructor for the MenuSprite class.
//-----------------------------------------------------------------------------
MenuSprite::~MenuSprite()
{
	delete startText;
	delete loadText;
	delete saveText;
}

//-----------------------------------------------------------------------------
// Name : draw () (Public)
// Desc : Draws all the objects inside the class.
//-----------------------------------------------------------------------------
void MenuSprite::draw(ULONG gameState)
{
	frameCounter++;

	switch (gameState) {
	case 0: // start menu
		startText->draw();
		loadText->draw();
		break;

	case 1: // ongoing game.
		break;

	case 2: // lost game
		break;

	case 3: // won game
		break;

	case 4: // paused game
		startText->draw();
		loadText->draw();
		saveText->draw();
	}
}

//-----------------------------------------------------------------------------
// Name : opUp () (Public)
// Desc : Moves the selection up.
//-----------------------------------------------------------------------------
void MenuSprite::opUp(ULONG gameState)
{
	updateSelect(select, false);

	switch (select) {
	case CHOICE::START:
		select = (gameState == 0) ? CHOICE::LOAD : CHOICE::SAVE;
		break;

	case CHOICE::LOAD:
		select = CHOICE::START;
		break;

	case CHOICE::SAVE:
		select = CHOICE::LOAD;
		break;
	}

	updateSelect(select, true);
}

//-----------------------------------------------------------------------------
// Name : opDown () (Public)
// Desc : Moves the selection down.
//-----------------------------------------------------------------------------
void MenuSprite::opDown(ULONG gameState)
{
	updateSelect(select, false);

	switch (select) {
	case CHOICE::START:
		select = CHOICE::LOAD;
		break;

	case CHOICE::LOAD:
		select = (gameState == 0) ? CHOICE::START : CHOICE::SAVE;
		break;

	case CHOICE::SAVE:
		select = CHOICE::START;
		break;
	}

	updateSelect(select, true);
}

//-----------------------------------------------------------------------------
// Name : updateSelect () (Private)
// Desc : Changes the selection.
//-----------------------------------------------------------------------------
void MenuSprite::updateSelect(CHOICE text, bool sel)
{
	Vec2 oldPos;

	switch (text) {
	case CHOICE::START:
		oldPos = startText->mPosition;
		delete startText;
		startText = (sel) ? new Sprite("data/menu_options/startgamesel.bmp", RGB(0xff, 0x00, 0xff)) :
			new Sprite("data/menu_options/startgamedes.bmp", RGB(0xff, 0x00, 0xff));
		startText->mPosition = oldPos;
		startText->mVelocity = Vec2(0, 0);
		startText->setBackBuffer(BF);
		break;

	case CHOICE::LOAD:
		oldPos = loadText->mPosition;
		delete loadText;
		loadText = (sel) ? new Sprite("data/menu_options/loadgamesel.bmp", RGB(0xff, 0x00, 0xff)) :
			new Sprite("data/menu_options/loadgamedes.bmp", RGB(0xff, 0x00, 0xff));
		loadText->mPosition = oldPos;
		loadText->mVelocity = Vec2(0, 0);
		loadText->setBackBuffer(BF);
		break;

	case CHOICE::SAVE:
		oldPos = saveText->mPosition;
		delete saveText;
		saveText = (sel) ? new Sprite("data/menu_options/savegamesel.bmp", RGB(0xff, 0x00, 0xff)) :
			new Sprite("data/menu_options/savegamedes.bmp", RGB(0xff, 0x00, 0xff));
		saveText->mPosition = oldPos;
		saveText->mVelocity = Vec2(0, 0);
		saveText->setBackBuffer(BF);
		break;
	}
}

//-----------------------------------------------------------------------------
// Name : getChoice () (Public)
// Desc : Provides infromation about the current menu state.
//-----------------------------------------------------------------------------
MenuSprite::CHOICE MenuSprite::getChoice()
{
	return select;
}