#include "GameConstants.h"
#include "GameResources.h"
#include "cD3DManager.h"
#include "cD3DXSpriteMgr.h"
#include "cD3DXTexture.h"
#include "cSprite.h"
#include "cShield.h"
#include "cEnemy.h"
#include "cExplosion.h"
#include "cXAudio.h"
#include "cD3DXFont.h"

using namespace std;

HINSTANCE hInst; // global handle to hold the application instance
HWND wndHandle; // global variable to hold the window handle

// Get a reference to the DirectX Manager
static cD3DManager* d3dMgr = cD3DManager::getInstance();
// Get a reference to the DirectX Sprite renderer Manager 
static cD3DXSpriteMgr* d3dxSRMgr = cD3DXSpriteMgr::getInstance();

int screen = 0;
int score;

cXAudio audio; // Game audio object

// Menu buttons
cSprite* playButton;
cSprite* continueButton;
cSprite* playAgainButton;

// Game timers
time_t previousTime;
time_t backgroundSoundCount;
time_t playerSpawnCount;
time_t enemyMovementCount;
time_t enemyMovementWait;
time_t bonusEnemySoundCount;
time_t bonusEnemySoundWait;
time_t explosionUpdateCount;

// Game Sprites
vector<cSprite*> aLife;
vector<cSprite*> aPlayer;
vector<cSprite*> aBonusEnemy;
vector<cSprite*> aPlayerProjectile;
vector<cSprite*> aEnemyProjectile;
vector<cSprite*>::iterator iter;
vector<cSprite*>::iterator iterB;

vector<cShield*> aShield;
vector<cShield*>::iterator shieldIterator;

vector<cEnemy*> aEnemy;
vector<cEnemy*>::iterator enemyIterator;
vector<cEnemy*>::iterator enemyIteratorB;

vector<cExplosion*> aExplosion;

// Player controls
bool fire = false;
bool moveLeft = false;
bool moveRight = false;

// Spawn Player
void SpawnPlayer() {
	aPlayer.push_back(new cSprite(D3DXVECTOR3(368, 592, 0), d3dMgr->getTheD3DDevice(), "Images\\playerLeft.png"));
}

// Spawn Shields
void SpawnShields() {
	for(int loop = 0; loop < 5; loop++)
		for(int loopB = 0; loopB < 2; loopB++)
			aShield.push_back(new cShield(D3DXVECTOR3((80*(loop+1)) + (64*loop) + (32*loopB), 528, 0), d3dMgr->getTheD3DDevice()));
}

// Spawn Enemies
void SpawnEnemies() {
	for(int loop = 0; loop < 4; loop++) {
		for(int loopB = 0; loopB < 8; loopB++) {
			int textureNumber = 0;
			if(loopB+(loop*8) > 7)
				textureNumber = 2;

			if(loopB+(loop*8) > 15)
				textureNumber = 4;

			if(loopB+(loop*8) > 23)
				textureNumber = 6;

			aEnemy.push_back(new cEnemy(D3DXVECTOR3(160 + (loopB*64), 80 + ((loop + 1)*64), 0), d3dMgr->getTheD3DDevice(), textureNumber));
		}
	}
}

// Spawn Bonus Enemy
void SpawnBonusEnemy() {
	audio.playSound(L"Sounds\\bonusEnemy.wav", false);
	if(rand() % 2 == 0)
		aBonusEnemy.push_back(new cSprite(D3DXVECTOR3(800, 80, 0), D3DXVECTOR2(-80.0f, 0.0f), d3dMgr->getTheD3DDevice(), "Images\\bonusEnemyLeft.png"));
	else
		aBonusEnemy.push_back(new cSprite(D3DXVECTOR3(-64, 80, 0), D3DXVECTOR2(80.0f, 0.0f), d3dMgr->getTheD3DDevice(), "Images\\bonusEnemyRight.png"));
}

// Spawn Explosion
void SpawnExplosion(cSprite* target) {
	audio.playSound(L"Sounds\\explosion.wav", false);
	aExplosion.push_back(new cExplosion(D3DXVECTOR3(target->getSpritePos().x - ((64 - target->getSTWidth())/2), target->getSpritePos().y - 16, 0), d3dMgr->getTheD3DDevice()));
}

// Setup Game
void Setup() {
	screen = 2; // Start game
	score = 0; // Reset score

	// Reset timers
	previousTime = clock();
	backgroundSoundCount = 0;
	playerSpawnCount = 0;
	enemyMovementCount = 0;
	enemyMovementWait = 2000;
	bonusEnemySoundCount = 0;
	bonusEnemySoundWait = 4000;
	explosionUpdateCount = 0;

	// Reset Lives
	aLife.push_back(new cSprite(D3DXVECTOR3(560, 16, 0), d3dMgr->getTheD3DDevice(), "Images\\playerLeft.png"));
	aLife.push_back(new cSprite(D3DXVECTOR3(640, 16, 0), d3dMgr->getTheD3DDevice(), "Images\\playerLeft.png"));
	aLife.push_back(new cSprite(D3DXVECTOR3(720, 16, 0), d3dMgr->getTheD3DDevice(), "Images\\playerLeft.png"));

	// Respawn initial sprites
	SpawnPlayer();
	SpawnShields();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	// Check any available messages from the queue
	switch (message) {
		case WM_KEYDOWN: {
			if (wParam == VK_SPACE)
				fire = true; // Set player to fire

			if (wParam == VK_LEFT)
				moveLeft = true; // Set player movement to left
				
			if (wParam == VK_RIGHT)
				moveRight = true; // Set player movement to right
			
			return 0;
		}

		 case WM_KEYUP: {
			if (wParam == VK_SPACE)
				fire = false; // Stop player firing
			
			if (wParam == VK_LEFT)
				moveLeft = false; // Stop player moving left
				
			if (wParam == VK_RIGHT)
				moveRight = false; // Stop player moving right

			return 0;
		}
		
		// Check for ingame button presses
		case WM_LBUTTONDOWN: {
			// Get cursor location
			POINT cursor;
			cursor.x = LOWORD(lParam);
			cursor.y = HIWORD(lParam);

			if ((screen == 1 && continueButton->insideRect(continueButton->getBoundingRect(), cursor)) || (screen == 3 && playAgainButton->insideRect(playAgainButton->getBoundingRect(), cursor)))
				Setup(); // Set up game

			if (screen == 0 && playButton->insideRect(playButton->getBoundingRect(), cursor))
				screen = 1; // Go to how to play screen
			
			return 0;
		}
		
		case WM_CLOSE: {
			// Exit the Game
			PostQuitMessage(0);
			return 0;
		}

		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}
	}

	// Always return the message to the default window procedure for further processing
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// initWindow registers the window class for the application, creates the window
bool initWindow(HINSTANCE hInstance) {
	WNDCLASSEX wcex;
	// Fill in the WNDCLASSEX structure. This describes how the window will look to the system
	wcex.cbSize = sizeof(WNDCLASSEX); // the size of the structure
	wcex.style = CS_HREDRAW | CS_VREDRAW; // the class style
	wcex.lpfnWndProc = (WNDPROC)WndProc; // the window procedure callback
	wcex.cbClsExtra = 0; // extra bytes to allocate for this class
	wcex.cbWndExtra = 0; // extra bytes to allocate for this instance
	wcex.hInstance = hInstance; // handle to the application instance
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MyWindowIcon)); // icon to associate with the application
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);// the default cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // the background color
	wcex.lpszMenuName = NULL; // the resource name for the menu
	wcex.lpszClassName = "Torpedo Fishing"; // the class name being created
	wcex.hIconSm = LoadIcon(hInstance, "icon.ico"); // the handle to the small icon
	RegisterClassEx(&wcex);

	// Create the window
	wndHandle = CreateWindow("Torpedo Fishing",	// the window class to use
							 "Torpedo Fishing",	// the title bar text
							WS_OVERLAPPEDWINDOW,	// the window style
							CW_USEDEFAULT,	// the starting x coordinate
							CW_USEDEFAULT,	// the starting y coordinate
							816,	// the pixel width of the window
							678,	// the pixel height of the window
							NULL,	// the parent window; NULL for desktop
							NULL,	// the menu for the application; NULL for none
							hInstance,	// the handle to the application instance
							NULL);	// no values passed to the window
	// Make sure that the window handle that is created is valid
	if(!wndHandle)
		return false;

	// Display the window on the screen
	ShowWindow(wndHandle, SW_SHOW);

	UpdateWindow(wndHandle);
	return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	// Initialize the window
	if(!initWindow(hInstance))
		return false;

	// Called after creating the window
	if(!d3dMgr->initD3DManager(wndHandle) )
		return false;

	if(!d3dxSRMgr->initD3DXSpriteMgr(d3dMgr->getTheD3DDevice()))
		return false;

	LPDIRECT3DSURFACE9 aSurface; // the Direct3D surface
	LPDIRECT3DSURFACE9 theBackbuffer = NULL; // This will hold the back buffer
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	aSurface = d3dMgr->getD3DSurfaceFromFile("Images\\background.png");	// Create the background surface
	
	// Load menu buttons
	playButton = new cSprite(D3DXVECTOR3(337, 479, 0), d3dMgr->getTheD3DDevice(), "Images\\playButton.png");
	continueButton = new cSprite(D3DXVECTOR3(261, 477, 0), d3dMgr->getTheD3DDevice(), "Images\\continueButton.png");
	playAgainButton = new cSprite(D3DXVECTOR3(219, 477, 0), d3dMgr->getTheD3DDevice(), "Images\\playAgainButton.png");

	// Load menu images
	cSprite* title = new cSprite(D3DXVECTOR3(196, 224, 0), d3dMgr->getTheD3DDevice(), "Images\\title.png");
	cSprite* howToPlay = new cSprite(D3DXVECTOR3(81, 96, 0), d3dMgr->getTheD3DDevice(), "Images\\howToPlay.png");
	cSprite* gameOver = new cSprite(D3DXVECTOR3(281, 225, 0), d3dMgr->getTheD3DDevice(), "Images\\gameOver.png");

	cD3DXFont* font = new cD3DXFont(d3dMgr->getTheD3DDevice(), hInstance, "Fonts\\font.ttf"); // Load custom font
	
	char scoreStr[50];
	
	// Set textbox positions
	RECT scoreTitlePos;
	SetRect(&scoreTitlePos, 16, 18, 800, 60);
	RECT scorePos;
	SetRect(&scorePos, 145, 18, 800, 60);
	RECT livesTitlePos;
	SetRect(&livesTitlePos, 433, 18, 800, 60);

	while( msg.message!=WM_QUIT ) {
		// Check the message queue
		if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			d3dMgr->beginRender();
			d3dxSRMgr->beginDraw();

			// Draw Background
			theBackbuffer = d3dMgr->getTheBackBuffer();
			d3dMgr->updateTheSurface(aSurface, theBackbuffer);
			d3dMgr->releaseTheBackbuffer(theBackbuffer);

			// Title screen
			if(screen == 0) {
				d3dxSRMgr->setTheTransform(title->getSpriteTransformMatrix());
				d3dxSRMgr->drawSprite(title->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);

				d3dxSRMgr->setTheTransform(playButton->getSpriteTransformMatrix());
				d3dxSRMgr->drawSprite(playButton->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
			}

			// How to play screen
			if(screen == 1) {
				d3dxSRMgr->setTheTransform(howToPlay->getSpriteTransformMatrix());
				d3dxSRMgr->drawSprite(howToPlay->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);

				d3dxSRMgr->setTheTransform(continueButton->getSpriteTransformMatrix());
				d3dxSRMgr->drawSprite(continueButton->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
			}

			if(screen > 1) {
				// Make score atleast 4 characters long
				if(score < 10)
					sprintf_s(scoreStr, 50, "000%d", score);

				if(score > 9 && score < 100)
					sprintf_s(scoreStr, 50, "00%d", score);

				if(score > 99 && score < 1000)
					sprintf_s(scoreStr, 50, "0%d", score);

				if(score > 999)
					sprintf_s(scoreStr, 50, "%d", score);

				// Draw Score
				font->printText("SCORE:", scoreTitlePos, 0xFF000000);
				font->printText(scoreStr, scorePos, 0xFFFF0000);
			}
			
			// Game
			if(screen == 2) {
				// Calculate DeltaTime
				time_t currentTime = clock();
				time_t deltaTime = currentTime - previousTime; 
				previousTime = currentTime;

				srand((unsigned int)time(NULL)); // Seed Random

				if(aPlayer.empty() == false) {
					backgroundSoundCount += deltaTime; // Count background sound interval
					enemyMovementCount += deltaTime; // Count enemy movement interval
				}
				else {
					// Spawn player after 2 seconds and reset counter
					playerSpawnCount += deltaTime;

					if(playerSpawnCount >= 2000) {
						SpawnPlayer();
						playerSpawnCount = 0;
					}
				}

				// Play background sound after 3 seconds and reset counter
				if(backgroundSoundCount >= 3000) {
					audio.playSound(L"Sounds\\background.wav", false);
					backgroundSoundCount = 0;
				}

				// Spawn wave of enemies
				if (aEnemy.empty()) {
					SpawnEnemies();
					enemyMovementWait = 2000;
				}

				// Spawn bonus enemy at random interval
				if(rand() % 50 == 0 && aBonusEnemy.empty() && aPlayer.empty() == false)
					SpawnBonusEnemy();

				// Play bonus enemy sound after increasingly short interval and reset counter
				if(aBonusEnemy.empty() == false) {
					bonusEnemySoundCount += deltaTime;

					if(bonusEnemySoundCount >= bonusEnemySoundWait) {
						audio.playSound(L"Sounds\\bonusEnemy.wav", false);
						bonusEnemySoundWait /= 2;
						if(bonusEnemySoundWait < 1000)
							bonusEnemySoundWait = 1000;
						bonusEnemySoundCount = 0;
					}
				}
				else
					bonusEnemySoundWait = 4000;
				
				// Show lives
				font->printText("LIVES:", livesTitlePos, 0xFF000000);
				for(iter = aLife.begin(); iter != aLife.end(); iter++) {
					d3dxSRMgr->setTheTransform((*iter)->getSpriteTransformMatrix());
					d3dxSRMgr->drawSprite((*iter)->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
				}

				for(iter = aPlayer.begin(); iter != aPlayer.end(); iter++) {
					// Fire projectile if fire is active and no player projectile is on screen
					if(fire && aPlayerProjectile.empty()) {
						audio.playSound(L"Sounds\\playerProjectile.wav", false);
						aPlayerProjectile.push_back(new cSprite(D3DXVECTOR3((*iter)->getSpritePos().x + 24, 592, 0), D3DXVECTOR2(0.0f, -300.0f), d3dMgr->getTheD3DDevice(), "Images\\playerProjectile.png"));
					}

					// Move left
					if(moveLeft && moveRight == false) {
						(*iter)->setTexture(new cD3DXTexture(d3dMgr->getTheD3DDevice(), "Images\\playerLeft.png"));
						(*iter)->setTranslation(D3DXVECTOR2(-100.0f, 0.0f));
					}
					
					// Move right
					if(moveRight && moveLeft == false) {
						(*iter)->setTexture(new cD3DXTexture(d3dMgr->getTheD3DDevice(), "Images\\playerRight.png"));
						(*iter)->setTranslation(D3DXVECTOR2(100.0f, 0.0f));
					}

					// Stop player moving
					if((((*iter)->getSpritePos().x < 0 && moveLeft) || ((*iter)->getSpritePos().x > 736 && moveRight)) || (moveLeft == false && moveRight == false) || (moveLeft && moveRight))
						(*iter)->setTranslation(D3DXVECTOR2(0.0f, 0.0f));
					
					// Update and draw player
					(*iter)->update(deltaTime);
					d3dxSRMgr->setTheTransform((*iter)->getSpriteTransformMatrix());
					d3dxSRMgr->drawSprite((*iter)->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
				}
				
				// Draw Shields
				for(shieldIterator = aShield.begin(); shieldIterator != aShield.end(); shieldIterator++) {
					d3dxSRMgr->setTheTransform((*shieldIterator)->getSpriteTransformMatrix());
					d3dxSRMgr->drawSprite((*shieldIterator)->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
				}

				for(enemyIterator = aEnemy.begin(); enemyIterator != aEnemy.end(); enemyIterator++) {
					// Move enemies down one row if at the end of screen
					if(((*enemyIterator)->getSpritePos().x < 16 && (*enemyIterator)->getTranslation().x < 0.0f) || ((*enemyIterator)->getSpritePos().x > 752 && (*enemyIterator)->getTranslation().x > 0.0f)) {
						for(enemyIteratorB = aEnemy.begin(); enemyIteratorB != aEnemy.end(); enemyIteratorB++) {
							(*enemyIteratorB)->setSpritePos(D3DXVECTOR3((*enemyIteratorB)->getSpritePos().x, (*enemyIteratorB)->getSpritePos().y+64, 0));
							(*enemyIteratorB)->setTranslation((*enemyIteratorB)->getTranslation()*-2); // Reverse enemy direction and double speed
						}

						enemyMovementWait /= 2; // Update enemy twice as often
					}
					
					// Fire projectile at random interval if no enemy projectile is on screen, player exists, and enemy is not on bottom row
					if(rand() % aEnemy.size() == 0 && aEnemyProjectile.empty() && aPlayer.empty() == false && (*enemyIterator)->getSpritePos().y < 528) {
						audio.playSound(L"Sounds\\enemyProjectile.wav", false);
						aEnemyProjectile.push_back(new cSprite(D3DXVECTOR3((*enemyIterator)->getSpritePos().x + 8, (*enemyIterator)->getSpritePos().y + 16, 0), D3DXVECTOR2(0.0f, 300.0f), d3dMgr->getTheD3DDevice(), "Images\\enemyProjectile.png"));
					}

					// Go to game over screen if past bottom row
					if ((*enemyIterator)->getSpritePos().y > 528)
						screen = 3;
					
					// Update enemy after interval and play sound effect
					if(enemyMovementCount >= enemyMovementWait) {
						audio.playSound(L"Sounds\\enemyMovement.wav", false);
						for(enemyIteratorB = aEnemy.begin(); enemyIteratorB != aEnemy.end(); enemyIteratorB++)
							(*enemyIteratorB)->update(enemyMovementCount);

						enemyMovementCount = 0;
					}
					
					// Draw enemy
					d3dxSRMgr->setTheTransform((*enemyIterator)->getSpriteTransformMatrix());
					d3dxSRMgr->drawSprite((*enemyIterator)->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
				}

				for(iter = aBonusEnemy.begin(); iter != aBonusEnemy.end();) {
					// Erase bonus enemy if off screen
					if(((*iter)->getSpritePos().x < -64 && (*iter)->getTranslation().x < 0.0f) || ((*iter)->getSpritePos().x > 800 && (*iter)->getTranslation().x > 0.0f))
						iter = aBonusEnemy.erase(iter);
					else {
						// Update and draw bonus enemy
						(*iter)->update(deltaTime);
						d3dxSRMgr->setTheTransform((*iter)->getSpriteTransformMatrix());
						d3dxSRMgr->drawSprite((*iter)->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
						iter++;
					}
				}

				for(iter = aPlayerProjectile.begin(); iter != aPlayerProjectile.end();) {
					bool erase = false; // Controls if to erase once current iteration if complete

					for(shieldIterator = aShield.begin(); shieldIterator != aShield.end();) {
						// Check for collision with shield
						if ((*iter)->collidedWith((*iter)->getBoundingRect(), (*shieldIterator)->getBoundingRect())) {
							erase = true;
							(*shieldIterator)->update(); // Update shield damage
						}

						if((*shieldIterator)->getDamage() == 7)
							shieldIterator = aShield.erase(shieldIterator); // Erase shield if broken
						else
							shieldIterator++;
					}

					for(enemyIterator = aEnemy.begin(); enemyIterator != aEnemy.end();) {
						// Check for collision with enemy
						if ((*iter)->collidedWith((*iter)->getBoundingRect(), (*enemyIterator)->getBoundingRect())) {
							SpawnExplosion((*enemyIterator));
							
							erase = true;
							score += (*enemyIterator)->getScore(); // Add enemy points worth to score
							enemyIterator = aEnemy.erase(enemyIterator); // Erase enemy
						}
						else
							enemyIterator++;
					}

					for(iterB = aBonusEnemy.begin(); iterB != aBonusEnemy.end();) {
						// Check for collision with bonus enemy
						if ((*iter)->collidedWith((*iter)->getBoundingRect(), (*iterB)->getBoundingRect())) {
							SpawnExplosion((*iterB));
							
							erase = true;
							score += 200; // Add to score
							iterB = aBonusEnemy.erase(iterB); // Erase bonus enemy
						}
						else
							iterB++;
					}

					for(iterB = aEnemyProjectile.begin(); iterB != aEnemyProjectile.end();) {
						if ((*iter)->collidedWith((*iter)->getBoundingRect(), (*iterB)->getBoundingRect())) {
							erase = true;
							iterB = aEnemyProjectile.erase(iterB); // Erase enemy projectile
						}
						else
							iterB++;
					}

					if(erase || (*iter)->getSpritePos().y < 0 - (*iter)->getSTHeight())
						iter = aPlayerProjectile.erase(iter); // Erase player projectile
					else {
						// Update and draw player projectile
						(*iter)->update(deltaTime);
						d3dxSRMgr->setTheTransform((*iter)->getSpriteTransformMatrix());
						d3dxSRMgr->drawSprite((*iter)->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
						iter++;
					}
				}

				for(iter = aEnemyProjectile.begin(); iter != aEnemyProjectile.end();) {
					bool erase = false;
					
					// Check for collision with player
					for(iterB = aPlayer.begin(); iterB != aPlayer.end();) {
						if ((*iter)->collidedWith((*iter)->getBoundingRect(), (*iterB)->getBoundingRect())) {
							SpawnExplosion((*iterB)); // Spawn explosion at player's location

							if(aLife.size() > 0)
								aLife.pop_back(); // Remove one life
							else
								screen = 3; // Go to game over screen f lives are empty

							erase = true;
							iterB = aPlayer.erase(iterB); // Erase player

							for(vector<cSprite*>::iterator iterC = aBonusEnemy.begin(); iterC != aBonusEnemy.end();)
								iterC = aBonusEnemy.erase(iterC); // Erase bonus enemy
						}
						else
							iterB++;
					}
					
					// Check for collision with shield
					for(shieldIterator = aShield.begin(); shieldIterator != aShield.end();) {
						if ((*iter)->collidedWith((*iter)->getBoundingRect(), (*shieldIterator)->getBoundingRect())) {
							erase = true;
							(*shieldIterator)->update(); // Update shield damage
						}
						if((*shieldIterator)->getDamage() == 7)
							shieldIterator = aShield.erase(shieldIterator); // Erase shield if broken
						else
							shieldIterator++;
					}

					if(erase || (*iter)->getSpritePos().y > 640)
						iter = aEnemyProjectile.erase(iter); // Erase enemy projectile
					else {
						// Draw and update enemy projectile
						(*iter)->update(deltaTime);
						d3dxSRMgr->setTheTransform((*iter)->getSpriteTransformMatrix());
						d3dxSRMgr->drawSprite((*iter)->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
						iter++;
					}
				}

				
				// Draw explosion
				for(vector<cExplosion*>::iterator explosionIterator = aExplosion.begin(); explosionIterator != aExplosion.end();) {
					if((*explosionIterator)->isActive() == false)
						explosionIterator = aExplosion.erase(explosionIterator); // Erase explosion
					else {
						explosionUpdateCount += deltaTime;
						if(explosionUpdateCount >= 62) {
							(*explosionIterator)->update();
							explosionUpdateCount = 0;
						}

						d3dxSRMgr->setTheTransform((*explosionIterator)->getSpriteTransformMatrix());
						d3dxSRMgr->drawSprite((*explosionIterator)->getTexture(), &((*explosionIterator)->getSourceRect()), NULL, NULL, 0xFFFFFFFF);
						explosionIterator++;
					}
				}
			}

			// Game over screen
			if(screen == 3) {
				//Clear all objects
				aLife.clear();
				aPlayer.clear();
				aShield.clear();
				aEnemy.clear();
				aPlayerProjectile.clear();
				aEnemyProjectile.clear();
				aExplosion.clear();

				// Draw game over message
				d3dxSRMgr->setTheTransform(gameOver->getSpriteTransformMatrix());
				d3dxSRMgr->drawSprite(gameOver->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);

				// Draw play again button
				d3dxSRMgr->setTheTransform(playAgainButton->getSpriteTransformMatrix());
				d3dxSRMgr->drawSprite(playAgainButton->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
			}

			d3dxSRMgr->endDraw();
			d3dMgr->endRender();
		}
	}

	d3dxSRMgr->cleanUp();
	d3dMgr->clean();

	return (int) msg.wParam;
}