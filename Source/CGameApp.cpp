//-----------------------------------------------------------------------------
// File: CGameApp.cpp
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "CGameApp.h"

#include <ctime>
#include <cstdlib>
#include <cmath>

#define PLAYER false
#define ENEMY true

extern		HINSTANCE g_hInst;
bool		p1Shoot = false;
bool		p2Shoot = false;

//-----------------------------------------------------------------------------
// CGameApp Member Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp () (Constructor)
// Desc : CGameApp Class Constructor
//-----------------------------------------------------------------------------
CGameApp::CGameApp()
{
	// Reset / Clear all required values
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	_Buffer			= NULL;
	_Player1		= NULL;
	_Player2		= NULL;
	_score			= NULL;
	m_LastFrameRate = 0;
	_gameState		= GameState::ONGOING;
}

//-----------------------------------------------------------------------------
// Name : ~CGameApp () (Destructor)
// Desc : CGameApp Class Destructor
//-----------------------------------------------------------------------------
CGameApp::~CGameApp()
{
	// Shut the engine down
	ShutDown();
}

//-----------------------------------------------------------------------------
// Name : InitInstance ()
// Desc : Initialises the entire Engine here.
//-----------------------------------------------------------------------------
bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow )
{
	// Create the primary display device
	if (!CreateDisplay()) { ShutDown(); return false; }

	// Build Objects
	if (!BuildObjects()) 
	{ 
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	// Set up all required game states
	SetupGameState();

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplay ()
// Desc : Create the display windows, devices etc, ready for rendering.
//-----------------------------------------------------------------------------
bool CGameApp::CreateDisplay()
{
	HMONITOR hM = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mInfo = { sizeof(mInfo) };

	if (!GetMonitorInfo(hM, &mInfo)) return NULL;

	_screenSize = Vec2(mInfo.rcMonitor.right, mInfo.rcMonitor.bottom);

	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width			= _screenSize.x;
	USHORT			Height			= _screenSize.y;
	RECT			rc;
	WNDCLASSEX		wcex;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	// Retrieve the final client size of the window
	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	// Show the window
	ShowWindow(m_hWnd, SW_SHOW);

	// Success!!
	return true;
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
// Desc : Signals the beginning of the physical post-initialisation stage.
//		From here on, the game engine has control over processing.
//-----------------------------------------------------------------------------
int CGameApp::BeginGame()
{
	MSG		msg;

	// Start main loop
	while(true) 
	{
		// Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else 
		{
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
	} // Until quit message is receieved

	return 0;
}

//-----------------------------------------------------------------------------
// Name : ShutDown ()
// Desc : Shuts down the game engine, and frees up all resources.
//-----------------------------------------------------------------------------
bool CGameApp::ShutDown()
{
	// Release any previously built objects
	ReleaseObjects ( );
	
	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	// Shutdown Success
	return true;
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc () (Static Callback)
// Desc : This is the main messge pump for ALL display devices, it captures
//		the appropriate messages, and routes them through to the application
//		class for which it was intended, therefore giving full class access.
// Note : It is VITALLY important that you should pass your 'this' pointer to
//		the lpParam parameter of the CreateWindow function if you wish to be
//		able to pass messages back to that app object.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name : DisplayWndProc ()
// Desc : The display devices internal WndProc function. All messages being
//		passed to this function are relative to the window it owns.
//-----------------------------------------------------------------------------
LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	static UINT			fTimer;	

	// Determine message type
	switch (Message)
	{
		case WM_CREATE:
			break;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED )
			{
				// App is inactive
				m_bActive = false;
			
			} // App has been minimized
			else
			{
				// App is active
				m_bActive = true;

				// Store new viewport sizes
				m_nViewWidth  = LOWORD( lParam );
				m_nViewHeight = HIWORD( lParam );
		
			
			} // End if !Minimized

			break;

		case WM_LBUTTONDOWN:
			// Capture the mouse
			SetCapture( m_hWnd );
			GetCursorPos( &m_OldCursorPos );
			break;

		case WM_LBUTTONUP:
			// Release the mouse
			ReleaseCapture( );
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			case VK_RETURN:
				fTimer = SetTimer(m_hWnd, 1, 250, NULL);
				_Player1->Explode();
				_Player2->Explode();
			
			}
			break;

		case WM_TIMER:
			switch(wParam)
			{
			case 1:
				if (!_Player1->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
				if (!_Player2->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
			}
			break;

		case WM_COMMAND:
			break;

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

	} // End Message Switch
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name : BuildObjects ()
// Desc : Build our demonstration meshes, and the objects that instance them
//-----------------------------------------------------------------------------
bool CGameApp::BuildObjects()
{
	_Buffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);
	_Player1 = new CPlayer(_Buffer, "data/ship1.bmp");
	_Player2 = new CPlayer(_Buffer, "data/ship2.bmp");
	_wonSprite = new Sprite("data/winscreen.bmp", RGB(0xff, 0x00, 0xff));
	_lostSprite = new Sprite("data/losescreen.bmp", RGB(0xff, 0x00, 0xff));

	_score = new ScoreSprite(Vec2(100, 200), _Buffer);

	_wonSprite->setBackBuffer(_Buffer);
	_lostSprite->setBackBuffer(_Buffer);

	addStars(20);
	addEnemies(ceil((_screenSize.x - 600) / 100) * 3);
	setPLives(3);

	if(!m_imgBackground.LoadBitmapFromFile("data/background.bmp", GetDC(m_hWnd)))
		return false;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : SetupGameState ()
// Desc : Sets up all the initial states required by the game.
//-----------------------------------------------------------------------------
void CGameApp::SetupGameState()
{
	_Player1->Position() = Vec2(int(_screenSize.x / 3 * 1), int(_screenSize.y / 3 * 2));
	_Player2->Position() = Vec2(int(_screenSize.x / 3 * 2), int(_screenSize.y / 3 * 2));
	_wonSprite->mPosition = Vec2(int(_screenSize.x / 2), int(_screenSize.y / 2));
	_lostSprite->mPosition = Vec2(int(_screenSize.x / 2), int(_screenSize.y / 2));
}

//-----------------------------------------------------------------------------
// Name : ReleaseObjects ()
// Desc : Releases our objects and their associated memory so that we can
//		rebuild them, if required, during our applications life-time.
//-----------------------------------------------------------------------------
void CGameApp::ReleaseObjects( )
{
	if(_Player1 != NULL)
	{
		delete _Player1;
		_Player1 = NULL;
	}

	if (_Player2 != NULL) {
		delete _Player2;
		_Player2 = NULL;
	}

	if (_wonSprite != NULL) {
		delete _wonSprite;
		_wonSprite = NULL;
	}

	if (_lostSprite != NULL) {
		delete _lostSprite;
		_lostSprite = NULL;
	}

	if (_livesText.first != NULL) {
		delete _livesText.first;
		_livesText.first = NULL;
	}

	if (_livesText.second != NULL) {
		delete _livesText.second;
		_livesText.second = NULL;
	}

	if (_score != NULL) {
		delete _score;
		_score = NULL;
	}

	while (!_enemies.empty()) delete _enemies.front(), _enemies.pop_front();
	while (!_bullets.empty()) delete _bullets.front(), _bullets.pop_front();
	while (!_stars.empty()) delete _stars.front(), _stars.pop_front();
	while (!_livesBlue.empty()) delete _livesBlue.front(), _livesBlue.pop_front();
	while (!_livesRed.empty()) delete _livesRed.front(), _livesRed.pop_front();

	if(_Buffer != NULL)
	{
		delete _Buffer;
		_Buffer = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name : FrameAdvance () (Private)
// Desc : Called to signal that we are now rendering the next frame.
//-----------------------------------------------------------------------------
void CGameApp::FrameAdvance()
{
	static TCHAR FrameRate[ 50 ];
	static TCHAR TitleBuffer[ 255 ];

	// Advance the timer
	m_Timer.Tick( );

	// Skip if app is inactive
	if ( !m_bActive ) return;
	
	// Get / Display the framerate
	if ( m_LastFrameRate != m_Timer.GetFrameRate() )
	{
		m_LastFrameRate = m_Timer.GetFrameRate( FrameRate, 50 );
		sprintf_s( TitleBuffer, _T("Game : %s"), FrameRate );
		SetWindowText( m_hWnd, TitleBuffer );

	} // End if Frame Rate Altered

	// Poll & Process input devices
	ProcessInput();

	// Animate the game objects
	AnimateObjects();

	// Remove all dead units
	removeDead();

	// Drawing the game objects
	DrawObjects();
}

//-----------------------------------------------------------------------------
// Name : ProcessInput () (Private)
// Desc : Simply polls the input devices and performs basic input operations
//-----------------------------------------------------------------------------
void CGameApp::ProcessInput()
{
	static UCHAR	pKeyBuffer[256];
	ULONG			Direction = 0;
	POINT			CursorPos;
	float			X = 0.0f, Y = 0.0f;
	ULONG			p2Direction = 0;

	// Retrieve keyboard state
	if (!GetKeyboardState(pKeyBuffer)) return;

	// Check the relevant keys
	// keybinds for player
	// only do something if player is alive
	if (!_Player1->isDead()) {
		if (pKeyBuffer[0x57] & 0xF0) Direction |= CPlayer::DIR_FORWARD;
		if (pKeyBuffer[0x53] & 0xF0) Direction |= CPlayer::DIR_BACKWARD;
		if (pKeyBuffer[0x41] & 0xF0) Direction |= CPlayer::DIR_LEFT;
		if (pKeyBuffer[0x44] & 0xF0) Direction |= CPlayer::DIR_RIGHT;
		if (pKeyBuffer[VK_SPACE] & 0xF0) {
			p1Shoot = true;
		}
		else {
			if (p1Shoot)
				SpawnBullet(_Player1->Position(), Vec2(0, -800), PLAYER);
			p1Shoot = false;
		}
	}

	// keybinds for enemy
	// only do something if enemy is alive
	if (!_Player2->isDead()) {
		if (pKeyBuffer[VK_NUMPAD8] & 0xF0) p2Direction |= CPlayer::DIR_FORWARD;
		if (pKeyBuffer[VK_NUMPAD5] & 0xF0) p2Direction |= CPlayer::DIR_BACKWARD;
		if (pKeyBuffer[VK_NUMPAD4] & 0xF0) p2Direction |= CPlayer::DIR_LEFT;
		if (pKeyBuffer[VK_NUMPAD6] & 0xF0) p2Direction |= CPlayer::DIR_RIGHT;
		if (pKeyBuffer[VK_NUMPAD0] & 0xF0) {
			p2Shoot = true;
		}
		else {
			if (p2Shoot)
				SpawnBullet(_Player2->Position(), Vec2(0, -800), PLAYER);
			p2Shoot = false;
		}
	}
	
	// Move the player
	_Player1->Move(Direction);

	// move the enemy
	_Player2->Move(p2Direction);


	// Now process the mouse (if the button is pressed)
	if ( GetCapture() == m_hWnd )
	{
		// Hide the mouse pointer
		SetCursor( NULL );

		// Retrieve the cursor position
		GetCursorPos( &CursorPos );

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );

	} // End if Captured
}

//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	if (!_Player1->isDead()) {
		_Player1->Update(m_Timer.GetTimeElapsed());
	}

	holdInside(*_Player1);
	
	if (!_Player2->isDead()) {
		_Player2->Update(m_Timer.GetTimeElapsed());
	}

	holdInside(*_Player2);

	for (auto bul : _bullets) {
		bul->update(m_Timer.GetTimeElapsed());

		if (bul->team == ENEMY) {
			trackPlayer(*bul);
		}

		if (detectCollision(bul) || bul->mPosition.y >= _screenSize.y || bul->mPosition.y <= 0) {
			_bullets.remove(bul);
			break;
		}
	}

	for (auto enem : _enemies) {
		enem->Update(m_Timer.GetTimeElapsed());
	}

	enemyFire();
	updateGameState();

	scrollBackground(m_Timer.GetTimeElapsed());
}

//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------
void CGameApp::DrawObjects()
{
	_Buffer->reset();

	m_imgBackground.Paint(_Buffer->getDC(), 0, 0);

	for (auto star : _stars) {
		star->draw();
	}

	if (_gameState == GameState::ONGOING) {
		if (!_Player1->isDead())
			_Player1->Draw();

		if (!_Player2->isDead())
			_Player2->Draw();

		for (auto lb : _livesBlue)
			lb->draw();

		for (auto lr : _livesRed)
			lr->draw();

		for (auto bul : _bullets)
			bul->draw();

		_livesText.first->draw();
		_livesText.second->draw();

		_score->draw();

		for (auto enem : _enemies) {
			enem->Draw();
			enem->frameCounter()++;
		}
	}
	else if (_gameState == GameState::WON) {
		_wonSprite->draw();
	}
	else if (_gameState == GameState::LOST) {
		_lostSprite->draw();
	}

	_Buffer->present();
}

//-----------------------------------------------------------------------------
// Name : SpawnBullet () (Private)
// Desc : Spawns a bullet with position, velocity and team given as arguments
//-----------------------------------------------------------------------------
void CGameApp::SpawnBullet(const Vec2 position, const Vec2 velocity, const bool team)
{
	_bullets.push_back(new Sprite("data/projectile.bmp", RGB(0xff, 0x00, 0xff)));
	_bullets.back()->setBackBuffer(_Buffer);
	_bullets.back()->mPosition = position;
	_bullets.back()->mVelocity = velocity;
	_bullets.back()->team = team;

	if (velocity.y < 0) {
		_bullets.back()->mPosition.y -= 75;
	}
	else {
		_bullets.back()->mPosition.y += 75;
	}
}

//-----------------------------------------------------------------------------
// Name : detectCollision () (Private)
// Desc : This function is called for each bullet spawned at one time to detect
// if it has hit something.
//-----------------------------------------------------------------------------
bool CGameApp::detectCollision(const Sprite *bullet)
{
	if (bulletUnitCollision(*bullet, *_Player1) && bullet->team) {
		_Player1->takeDamage();

		if (_livesBlue.size())
			_livesBlue.pop_back();
		
		return true;
	}
	
	if (bulletUnitCollision(*bullet, *_Player2) && bullet->team) {
		_Player2->takeDamage();

		if (_livesRed.size())
			_livesRed.pop_back();
		
		return true;
	}

	for (auto enem : _enemies) {
		if (bulletUnitCollision(*bullet, *enem) && !bullet->team) {
			enem->Explode();
			return true;
		}
	}

	return false;
 }

//-----------------------------------------------------------------------------
// Name : bulletUnitCollision () (Private)
// Desc : Function that checks when a bullet has colided with a given 
// player unit
//-----------------------------------------------------------------------------
bool CGameApp::bulletUnitCollision(const Sprite& bullet, CPlayer& unit)
{
	if (unit.isDead())
		return false;

	if (bullet.mPosition.x >= unit.Position().x - (unit.getSize().x / 2))
		if (bullet.mPosition.x <= unit.Position().x + (unit.getSize().x / 2))
			if (bullet.mPosition.y >= unit.Position().y - (unit.getSize().y / 2))
				if (bullet.mPosition.y <= unit.Position().y + (unit.getSize().y / 2))
					return true;

	return false;
}


//-----------------------------------------------------------------------------
// Name : addStars () (Private)
// Desc : Adds specified number of stars in the background.
//-----------------------------------------------------------------------------
void CGameApp::addStars(int noStars)
{
	srand(time(NULL));

	for (int it = 0; it != noStars; ++it) {
		_stars.push_back(new Sprite("data/star.bmp", RGB(0xff, 0x00, 0xff)));
		_stars.back()->setBackBuffer(_Buffer);
		_stars.back()->mVelocity = Vec2(0, rand() % 100 + 50);
		_stars.back()->mPosition = Vec2(rand() % int(_screenSize.x), -int(_screenSize.x / 2));
	}

	Sleep(rand() % 5);
}

//-----------------------------------------------------------------------------
// Name : scrollBackground () (Private)
// Desc : Moves the stars in the background.
//-----------------------------------------------------------------------------
void CGameApp::scrollBackground(float dt)
{
	srand(time(NULL));

	for (auto star : _stars) {
		if (star->mPosition.y >= _screenSize.y) {
			star->mPosition.y = 0;
		}

		star->update(dt);
	}
}

//-----------------------------------------------------------------------------
// Name : addEnemies () (Private)
// Desc : Adds specified number of enemies.
//-----------------------------------------------------------------------------
void CGameApp::addEnemies(int noEnemies)
{
	srand(time(NULL));
	Vec2 position = Vec2(300, 50);
	
	for (int it = 0; it != noEnemies; ++it) {
		_enemies.push_back(new CPlayer(_Buffer, "data/enemyship.bmp"));
		_enemies.back()->Position() = position;
		_enemies.back()->Velocity() = Vec2(0, 0);
		_enemies.back()->frameCounter() = rand() % 2000;

		position.x += 100;
		if (position.x > _screenSize.x - 300) {
			position.x = 300;
			position.y += 90;
		}
	}
}

//-----------------------------------------------------------------------------
// Name : enemyFire () (Private)
// Desc : Iterates trough all the enemies and makes them fire using a
// random pattern.
//-----------------------------------------------------------------------------
void CGameApp::enemyFire()
{
	srand(time(NULL));

	for (auto enem : _enemies) {
		if (enem->frameCounter() == 2000) {
			enem->frameCounter() = rand() % 1500;
			SpawnBullet(enem->Position(), Vec2(0, 200), ENEMY);
		}
	}
}

//-----------------------------------------------------------------------------
// Name : holdInside () (Private)
// Desc : Holds a player unit inside of the playable window.
//-----------------------------------------------------------------------------
void CGameApp::holdInside(CPlayer& unit) 
{
	// bind player inside right margin
	if (unit.Position().x + (unit.getSize().x / 2) >= _screenSize.x) {
		unit.Position().x = _screenSize.x - (unit.getSize().x / 2);
	}

	// bind player inside left margin
	if (unit.Position().x - (unit.getSize().x / 2) <= 0) {
		unit.Position().x = unit.getSize().x / 2;
	}

	// bind player inside bottom marin
	if (unit.Position().y + (unit.getSize().y / 2) >= _screenSize.y) {
		unit.Position().y = _screenSize.y - (unit.getSize().y / 2);
	}

	// bind player inside top margin
	if (unit.Position().y - (unit.getSize().y / 2) <= 0) {
		unit.Position().y = unit.getSize().y / 2;
	}
}

//-----------------------------------------------------------------------------
// Name : removeDead () (Private)
// Desc : Removes all dead enemies from the game.
//-----------------------------------------------------------------------------
void CGameApp::removeDead()
{
	if (!_Player1->getLives() && !_Player1->hasExploded()) {
		_Player1->Explode();
	}
		

	if (!_Player2->getLives() && !_Player2->hasExploded())
		_Player2->Explode();

	for (auto enem : _enemies) {
		if (enem->isDead()) {
			delete enem;
			_enemies.remove(enem);
			_score->updateScore(10);
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Name : btpDistance () (Private)
// Desc : Calculates bullet to player unit distance.
//-----------------------------------------------------------------------------
double btpDistance(Sprite& bullet, CPlayer& player)
{
	return bullet.mPosition.Distance(player.Position());
}

//-----------------------------------------------------------------------------
// Name : moveTowards () (Private)
// Desc : Moves a bullet towards a specified destination.
//-----------------------------------------------------------------------------
void moveTowards(Sprite& bullet, const Vec2 destination)
{
	if (bullet.mPosition.x > destination.x) {
		bullet.mVelocity.x -= 2;
	}
	else {
		bullet.mVelocity.x += 2;
	}
}

//-----------------------------------------------------------------------------
// Name : trackPlayer () (Private)
// Desc : Moves a bullet towards the closest alive player.
//-----------------------------------------------------------------------------
void CGameApp::trackPlayer(Sprite& bullet) 
{
	if (_Player1->isDead() && !_Player2->isDead()) {
		moveTowards(bullet, _Player2->Position());
	}
	else if (_Player2->isDead() && !_Player1->isDead()) {
		moveTowards(bullet, _Player1->Position());
	}
	else if (!_Player1->isDead() && !_Player2->isDead()) {
		if (btpDistance(bullet, *_Player1) < btpDistance(bullet, *_Player2)) {
			moveTowards(bullet, _Player1->Position());
		}
		else {
			moveTowards(bullet, _Player2->Position());
		}
	}
}

//-----------------------------------------------------------------------------
// Name : updateGameState () (Private)
// Desc : Updates the game state when all players or all enemies have died.
//-----------------------------------------------------------------------------
void CGameApp::updateGameState()
{
	if (_Player1->isDead() && _Player2->isDead() && _gameState == ONGOING) {
		_gameState = LOST;
	}
	else if (!_enemies.size() && _gameState == ONGOING) {
		_gameState = WON;
	}
	else if (_gameState == ONGOING) {
		_gameState = ONGOING;
	}
}

//-----------------------------------------------------------------------------
// Name : setPLives () (Private)
// Desc : Sets number of lives for all players and also adds the heart sprite.
//-----------------------------------------------------------------------------
void CGameApp::setPLives(int playerLives)
{
	_Player1->setLives(playerLives);
	_Player2->setLives(playerLives);

	Vec2 bluePos(30, 125);
	Vec2 redPos(_screenSize.x - 50, 125.0);
	Vec2 increment(55, 0);

	_livesText.first = new Sprite("data/lives_text.bmp", RGB(0xff, 0x00, 0xff));
	_livesText.second = new Sprite("data/lives_text.bmp", RGB(0xff, 0x00, 0xff));

	_livesText.first->mPosition = Vec2(100, 50);
	_livesText.first->mVelocity = Vec2(0, 0);
	_livesText.first->setBackBuffer(_Buffer);

	_livesText.second->mPosition = Vec2(_screenSize.x - 140, 50.0);
	_livesText.second->mVelocity = Vec2(0, 0);
	_livesText.second->setBackBuffer(_Buffer);

	for (int it = 0; it != playerLives; ++it) {
		_livesBlue.push_back(new Sprite("data/heart_blue.bmp", RGB(0xff, 0x00, 0xff)));
		_livesRed.push_back(new Sprite("data/heart_red.bmp", RGB(0xff, 0x00, 0xff)));

		auto& lastBlue = _livesBlue.back();
		auto& lastRed = _livesRed.back();

		lastBlue->mPosition = bluePos;
		lastBlue->mVelocity = Vec2(0, 0);
		lastBlue->setBackBuffer(_Buffer);

		lastRed->mPosition = redPos;
		lastRed->mVelocity = Vec2(0, 0);
		lastRed->setBackBuffer(_Buffer);

		bluePos += increment;
		redPos -= increment;
	}
}
