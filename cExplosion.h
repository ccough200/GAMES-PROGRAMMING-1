#ifndef _CEXPLOSION_H
#define _CEXPLOSION_H
#include "cSprite.h"

class cExplosion : public cSprite
{
private:
	bool mActive;
	int mNoFrames;
	int mCurrentFrame;
	RECT mSourceRect;

public:
	cExplosion(); // Default constructor
	cExplosion(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice);
	void update();							// Updates the explosion frame
	RECT getSourceRect();					// Return the current segment of the sprite sheet to draw.
	int getCurrentFrame();					// Return the current frame.
	void setActive(bool sActive);			// Set the sprite to active.
	bool isActive();						// Determine if the sprite is active.
};
#endif