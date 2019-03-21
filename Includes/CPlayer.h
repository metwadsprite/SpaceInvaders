//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//	   such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CPLAYER_H_
#define _CPLAYER_H_

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"

#include <vector>

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CPlayer (Class)
// Desc : Player class handles all player manipulation, update and management.
//-----------------------------------------------------------------------------
class CPlayer
{
public:
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------
	enum DIRECTION 
	{ 
		DIR_FORWARD		= 1,
		DIR_BACKWARD	= 2,
		DIR_LEFT		= 4,
		DIR_RIGHT		= 8
	};

	enum ESpeedStates
	{
		SPEED_START,
		SPEED_STOP
	};

	enum TEAM {
		PLAYER1		= 1,
		PLAYER2		= 2,
		ENEMY		= 3
	};

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CPlayer(const BackBuffer *pBackBuffer, const char* texturePath);
	virtual ~CPlayer();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					Update( float dt );
	void					Draw();
	void					Move(ULONG ulDirection);
	Vec2&					Position();
	Vec2&					Velocity();
	void					Explode();
	bool					AdvanceExplosion();
	bool					isDead();
	Vec2					getSize();
	int&					frameCounter();
	void					takeDamage();
	int						getLives();
	void					setLives(int noLives);
	bool					hasExploded();
	void					setTeam(TEAM team);

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------

	Sprite*					_sprite;
	ESpeedStates			_speedState;
	float					_timer;
	bool					_explosion;
	AnimatedSprite*			_explosionSprite;
	int						_explosionFrame;
	bool					_isDead;
	int						_lives;
	TEAM					_team;
};

#endif // _CPLAYER_H_