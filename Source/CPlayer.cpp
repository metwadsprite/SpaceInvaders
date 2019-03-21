//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//       such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "CPlayer.h"

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
CPlayer::CPlayer(const BackBuffer *pBackBuffer, const char* texturePath)
{
	_sprite		= new Sprite(texturePath, RGB(0xff, 0x00, 0xff));
	_speedState	= SPEED_STOP;
	_timer		= 0;
	_isDead		= false;
	_lives		= 1;

	_sprite->setBackBuffer(pBackBuffer);

	// Animation frame crop rectangle
	RECT r;
	r.left		= 0;
	r.top		= 0;
	r.right		= 128;
	r.bottom	= 128;

	// Set animation texture
	_explosionSprite	= new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 16);
	_explosion			= false;
	_explosionFrame		= 0;

	_explosionSprite->setBackBuffer(pBackBuffer);
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
	delete _sprite;
	delete _explosionSprite;
}

void CPlayer::Update(float dt)
{
	// passive slowdown
	if (_sprite->mVelocity.x > 0) {
		_sprite->mVelocity.x--;
	}
	else if (_sprite->mVelocity.x < 0) {
		_sprite->mVelocity.x++;
	}

	if (_sprite->mVelocity.y > 0) {
		_sprite->mVelocity.y--;
	}
	else if (_sprite->mVelocity.y < 0) {
		_sprite->mVelocity.y++;
	}

	// Update sprite
	_sprite->update(dt);

	if (_explosion) {
		AdvanceExplosion();
	}

	// Get velocity
	double v = _sprite->mVelocity.Magnitude();

	// NOTE: for each async sound played Windows creates a thread for you
	// but only one, so you cannot play multiple sounds at once.
	// This creation/destruction of threads also leads to bad performance
	// so this method is not recommanded to be used in complex projects.

	// update internal time counter used in sound handling (not to overlap sounds)
	_timer += dt;

	// A FSM is used for sound manager 
	switch(_speedState)
	{
	case SPEED_STOP:
		if(v > 35.0f)
		{
			_speedState = SPEED_START;
			_timer = 0;
		}
		break;
	case SPEED_START:
		if(v < 25.0f)
		{
			_speedState = SPEED_STOP;
			_timer = 0;
		}
		else
			if(_timer > 1.f)
			{
				_timer = 0;
			}
		break;
	}

	// NOTE: For sound you also can use MIDI but it's Win32 API it is a bit hard
	// see msdn reference: http://msdn.microsoft.com/en-us/library/ms711640.aspx
	// In this case you can use a C++ wrapper for it. See the following article:
	// http://www.codeproject.com/KB/audio-video/midiwrapper.aspx (with code also)
}

void CPlayer::Draw()
{
	if (!_explosion)
		_sprite->draw();
	else
		_explosionSprite->draw();
}

void CPlayer::Move(ULONG ulDirection)
{
	double acc			= 10;		// acceleration
	double brk			= 5 * acc;	// break force
	double topSpeed		= 750;		// maximum speed

	// code to move forward
	if (ulDirection & CPlayer::DIR_FORWARD) {
		if (_sprite->mVelocity.y > 0)
			_sprite->mVelocity.y -= brk;
		else if (_sprite->mVelocity.y >= -topSpeed)
			_sprite->mVelocity.y -= acc / 2;
	}

	// code to move backward
	if (ulDirection & CPlayer::DIR_BACKWARD) {
		if (_sprite->mVelocity.y < 0)
			_sprite->mVelocity.y += brk;
		else if (_sprite->mVelocity.y <= topSpeed)
			_sprite->mVelocity.y += acc / 2;
	}

	// code to move left
	if (ulDirection & CPlayer::DIR_LEFT) {
		if (_sprite->mVelocity.x > 0)
			_sprite->mVelocity.x -= brk;
		else if (_sprite->mVelocity.x >= -topSpeed)
			_sprite->mVelocity.x -= acc;
	}

	// code to move right
	if (ulDirection & CPlayer::DIR_RIGHT) {
		if (_sprite->mVelocity.x < 0)
			_sprite->mVelocity.x += brk;
		else if (_sprite->mVelocity.x <= topSpeed)
			_sprite->mVelocity.x += acc;
	}
}

Vec2& CPlayer::Position()
{
	return _sprite->mPosition;
}

Vec2& CPlayer::Velocity()
{
	return _sprite->mVelocity;
}

void CPlayer::Explode()
{
	_explosionSprite->SetFrame(0);
	
	/// TODO: add explosion sound

	_explosionSprite->mPosition = _sprite->mPosition;
	_sprite->mVelocity			= Vec2(0, 0);
	_explosion = true;
}

bool CPlayer::AdvanceExplosion()
{
	if(_explosion)
	{
		_explosionSprite->SetFrame(_explosionFrame++);

		if(_explosionFrame == _explosionSprite->GetFrameCount())
		{
			_isDead = true;
			_explosion = false;
			_explosionFrame = 0;
			return false;
		}
	}
	return true;
}

bool CPlayer::isDead()
{
	return _isDead;
}

Vec2 CPlayer::getSize() 
{
	return Vec2(_sprite->width(), _sprite->height());
}

int& CPlayer::frameCounter()
{
	return _sprite->frameCounter;
}

void CPlayer::takeDamage()
{
	_lives--;
}

int CPlayer::getLives()
{
	return _lives;
}

void CPlayer::setLives(int noLives)
{
	_lives = noLives;
}

bool CPlayer::hasExploded()
{
	return _explosion;
}
