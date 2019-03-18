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
	void		SpawnBullet(const Vec2 position, const Vec2 velocity, const bool team);
	bool		detectCollision(const Sprite *bullet);
	bool		bulletUnitCollision(const Sprite& bullet, CPlayer& unit);
	void		addStars(int noStars);
	void		scrollBackground(float dt);
	void		addEnemies(int noEnemies);
	void		enemyFire();
	void		holdInside(CPlayer& unit);
	void		removeDead();
	void		trackPlayer(Sprite& bullet);
	void		updateGameState();
	
	//-------------------------------------------------------------------------
	// Private Static Functions For This Class
	//-------------------------------------------------------------------------
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	//-------------------------------------------------------------------------
	// Private Variables For This Class
	//-------------------------------------------------------------------------
	CTimer					m_Timer;			// Game timer
	ULONG					m_LastFrameRate;	// Used for making sure we update only when fps changes.
	
	HWND					m_hWnd;				// Main window HWND
	HICON					m_hIcon;			// Window Icon
	HMENU					m_hMenu;			// Window Menu
	
	bool					m_bActive;			// Is the application active ?

	ULONG					m_nViewX;			// X Position of render viewport
	ULONG					m_nViewY;			// Y Position of render viewport
	ULONG					m_nViewWidth;		// Width of render viewport
	ULONG					m_nViewHeight;		// Height of render viewport

	POINT					m_OldCursorPos;		// Old cursor position for tracking
	HINSTANCE				m_hInstance;

	CImageFile				m_imgBackground;

	BackBuffer*				_Buffer;
	CPlayer*				_Player1;
	CPlayer*				_Player2;

	std::list<Sprite*>		_bullets;
	std::list<Sprite*>		_stars;
	std::list<CPlayer*>		_enemies;

	Vec2					_screenSize;

	GameState				_gameState;
	Sprite*					_wonSprite;
	Sprite*					_lostSprite;
};

#endif // _CGAMEAPP_H_