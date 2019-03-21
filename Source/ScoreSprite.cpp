#include "ScoreSprite.h"

#include <string>

//-----------------------------------------------------------------------------
// Name : ScoreSprite () (Constructor)
// Desc : Constructor for the ScoreSprite class.
//-----------------------------------------------------------------------------
ScoreSprite::ScoreSprite(const Vec2 position, const BackBuffer* BF)
{
	this->position = position;
	this->BF = BF;

	scoreText = new Sprite("data/score_text.bmp", RGB(0xff, 0x00, 0xff));
	scoreText->mPosition = position;
	scoreText->mVelocity = Vec2(0, 0);
	scoreText->setBackBuffer(BF);

	posDig0 = Vec2(position.x - 75, position.y + 75);
	posDig1 = Vec2(position.x - 25, position.y + 75);
	posDig2 = Vec2(position.x + 25, position.y + 75);
	posDig3 = Vec2(position.x + 75, position.y + 75);

	scoreDig0 = new Sprite("data/numbers/0.bmp", RGB(0xff, 0x00, 0xff));
	scoreDig0->mPosition = posDig0;
	scoreDig0->mVelocity = Vec2(0, 0);
	scoreDig0->setBackBuffer(BF);

	scoreDig1 = new Sprite("data/numbers/0.bmp", RGB(0xff, 0x00, 0xff));
	scoreDig1->mPosition = posDig1;
	scoreDig1->mVelocity = Vec2(0, 0);
	scoreDig1->setBackBuffer(BF);

	scoreDig2 = new Sprite("data/numbers/0.bmp", RGB(0xff, 0x00, 0xff));
	scoreDig2->mPosition = posDig2;
	scoreDig2->mVelocity = Vec2(0, 0);
	scoreDig2->setBackBuffer(BF);

	scoreDig3 = new Sprite("data/numbers/0.bmp", RGB(0xff, 0x00, 0xff));
	scoreDig3->mPosition = posDig3;
	scoreDig3->mVelocity = Vec2(0, 0);
	scoreDig3->setBackBuffer(BF);

	scoreInt = 0;
}

//-----------------------------------------------------------------------------
// Name : ~ScoreSprite () (Destructor)
// Desc : Destructor for the ScoreSprite class.
//-----------------------------------------------------------------------------
ScoreSprite::~ScoreSprite()
{
	delete scoreText;
	delete scoreDig0;
	delete scoreDig1;
	delete scoreDig2;
	delete scoreDig3;
}

//-----------------------------------------------------------------------------
// Name : updateScore () (Public)
// Desc : Updates the internal score and sets the digits accordingly.
//-----------------------------------------------------------------------------
void ScoreSprite::updateScore(int increment)
{
	scoreInt += increment;

	updateDigit(3, scoreInt % 10);
	updateDigit(2, scoreInt % 100 / 10);
	updateDigit(1, scoreInt % 1000 / 100);
	updateDigit(0, scoreInt % 10000 / 1000);
}

//-----------------------------------------------------------------------------
// Name : updateDigit () (Private)
// Desc : Sets a given digit to a given number.
//-----------------------------------------------------------------------------
void ScoreSprite::updateDigit(int digID, int no)
{
	std::string path = "data/numbers/" + std::to_string(no) + ".bmp";
	
	switch (digID) {
	case 0:
		delete scoreDig0;
		scoreDig0 = new Sprite(path.c_str(), RGB(0xff, 0x00, 0xff));
		scoreDig0->mPosition = posDig0;
		scoreDig0->mVelocity = Vec2(0, 0);
		scoreDig0->setBackBuffer(BF);
		break;

	case 1:
		delete scoreDig1;
		scoreDig1 = new Sprite(path.c_str(), RGB(0xff, 0x00, 0xff));
		scoreDig1->mPosition = posDig1;
		scoreDig1->mVelocity = Vec2(0, 0);
		scoreDig1->setBackBuffer(BF);
		break;

	case 2:
		delete scoreDig2;
		scoreDig2 = new Sprite(path.c_str(), RGB(0xff, 0x00, 0xff));
		scoreDig2->mPosition = posDig2;
		scoreDig2->mVelocity = Vec2(0, 0);
		scoreDig2->setBackBuffer(BF);
		break;

	case 3:
		delete scoreDig3;
		scoreDig3 = new Sprite(path.c_str(), RGB(0xff, 0x00, 0xff));
		scoreDig3->mPosition = posDig3;
		scoreDig3->mVelocity = Vec2(0, 0);
		scoreDig3->setBackBuffer(BF);
		break;

	default:
		break;
	}

}

//-----------------------------------------------------------------------------
// Name : draw () (Public)
// Desc : Draws all the objects inside the class.
//-----------------------------------------------------------------------------
void ScoreSprite::draw()
{
	scoreText->draw();
	scoreDig0->draw();
	scoreDig1->draw();
	scoreDig2->draw();
	scoreDig3->draw();
}