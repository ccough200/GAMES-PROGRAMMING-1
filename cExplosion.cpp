#include "cExplosion.h"

cExplosion::cExplosion(): cSprite() // Default constructor
{
	mActive = true;
	mNoFrames = 15;
	mCurrentFrame = 0;
}

cExplosion::cExplosion(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice)
{
	mActive = true;
	mNoFrames = 15;
	mCurrentFrame = 0;
	setSpritePos(sPosition);
	setTexture(new cD3DXTexture(pd3dDevice, "Images\\explosion.png"));
	SetRect(&mSourceRect,0,0,cExplosion::getSTWidth(),cExplosion::getSTHeight());
}


/*
=================
- update the position etc of the sprite and change frame
=================
*/
void cExplosion::update() // Updates the explosion frame
{
	SetRect(&mSourceRect,mCurrentFrame*64,0,(mCurrentFrame*64)+64,64);
	mCurrentFrame++;
	if(mCurrentFrame > mNoFrames) {
		mCurrentFrame = 0;
		mActive = false;
		OutputDebugString("mActive = False");
	}

	cSprite::update();
}
/*
=================
- Return the current segment of the sprite sheet to draw.
=================
*/
RECT cExplosion::getSourceRect() 					// Return the current segment of the sprite sheet to draw.
{
	return mSourceRect;
}
/*
=================
- Return the current frame.
=================
*/
int cExplosion::getCurrentFrame() 					// Return the current frame.
{
	return mCurrentFrame;
}
/*
=================
- Set the sprite to active.
=================
*/
void cExplosion::setActive(bool sActive) 			// Set the sprite to active.
{
	mActive = sActive;
}
/*
=================
- Determine if the sprite is active.
=================
*/
bool cExplosion::isActive() 						// Determine if the sprite is active.
{
	return mActive;
}
