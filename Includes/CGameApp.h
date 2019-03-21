//-----------------------------------------------------------------------------
// File: CGameApp.h
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CGAMEAPP_H_
#define _CGAMEAPP_H_

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "CTimer.h"
#include "CPlayer.h"
#include "BackBuffer.h"
#include "ImageFile.h"
#include "ScoreSprite.h"
#include "Bullet.h"

#include <list>

//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp (Class)
// Desc : Central game engine, initialises the game and handles core processes.
//-----------------------------------------------------------------------------
class CGameApp
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
				CGameApp();
	virtual		~CGameApp();

	//-------------------------------------------------------------------------
	// Public Functions for This Class
	//-------------------------------------------------------------------------
	LRESULT	 DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
	bool		InitInstance( LPCTSTR lpCmdLine, int iCmdShow );
	int			BeginGame();
	bool		ShutDown();

	enum GameState {
		ONGOING,
		LOST,
		WON
	};
	
private:
	//-------------------------------------------------------------------------
	// Private Functions for This Class
	//-------------------------------------------------------------------------
	bool		BuildObjects();
	void		ReleaseObjects();
	void		FrameAdvance();
	bool		CreateDisplay();
	void		SetupGameState();
	void		AnimateObjects();
	void		DrawObjects();
	void		ProcessInput();
	void		SpawnBullet(const Vec2 position, const Vec2 velocity, const CPlayer::TEAM team);
	bool		detectCollision(const Bullet* bullet);
	bool		bulletUnitCollision(const Bullet& bullet, CPlayer& unit);
	void		addStars(int noStars);
	void		scrollBackground(float dt);
	void		addEnemies(int noEnemies);
	void		enemyFire();
	void		holdInside(CPlayer& unit);
	void		removeDead();
	void		trackPlayer(Sprite& bullet);
	void		updateGameState();
	void		setPLives(int playerLives);
	void		moveEnemies();
	
	//-------------------------------------------------------------------------
	// Private Static Functions For This Class
	//-------------------------------------------------------------------------
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	//-------------------------------------------------------------------------
	// Private Variables For This Class
	//-------------------------------------------------------------------------
	CTimer						m_Timer;			// Game timer
	ULONG						m_LastFrameRate;	// Used for making sure we update only when fps changes.
	
	HWND						m_hWnd;				// Main window HWND
	HICON						m_hIcon;			// Window Icon
	HMENU						m_hMenu;			// Window Menu
	
	bool						m_bActive;			// Is the application active ?

	ULONG						m_nViewX;			// X Position of render viewport
	ULONG						m_nViewY;			// Y Position of render viewport
	ULONG						m_nViewWidth;		// Width of render viewport
	ULONG						m_nViewHeight;		// Height of render viewport

	POINT						m_OldCursorPos;		// Old cursor position for tracking
	HINSTANCE					m_hInstance;

	CImageFile					m_imgBackground;	// Background image

	BackBuffer*					_Buffer;			// Back buffer
	CPlayer*					_Player1;			// Player one
	CPlayer*					_Player2;			// Player two

	std::list<CPlayer*>			_enemies;			// List containing all enemies alive
	std::list<Bullet*>			_bullets;			// List containing all bullets on the screen
	std::list<Sprite*>			_stars;				// List containing stars scrolling in the background
	std::list<Sprite*>			_livesBlue;			// Lives for blue player
	std::list<Sprite*>			_livesRed;			// Lives for red player

	Vec2						_screenSize;		// Provides easy access to the screen size

	GameState					_gameState;			// Game state (ongoing, won, lost)
	Sprite*						_wonSprite;			// Information to be displayed when game is won
	Sprite*						_lostSprite;		// Information to be displayed when game is lost

	std::pair<Sprite*, Sprite*> _livesText;			// Text above the lives on the screen

	ScoreSprite*				_scoreP1;			// Score for the player 1
	ScoreSprite*				_scoreP2;			// Score for the player 2

	int							frameCounter;
};

#endif // _CGAMEAPP_H_