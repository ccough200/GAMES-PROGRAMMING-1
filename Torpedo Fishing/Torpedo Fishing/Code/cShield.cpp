#include "cShield.h"

// Array of shield image filenames
char* shieldFilenames[] = {"Images\\shield0.png", "Images\\shield1.png", "Images\\shield2.png", "Images\\shield3.png",
						"Images\\shield4.png", "Images\\shield5.png", "Images\\shield6.png"};

// Constructor
cShield::cShield(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice): cSprite() {
	device = pd3dDevice;
	damage = 0;
	setTexture(new cD3DXTexture(device, shieldFilenames[damage]));
	setSpritePos(sPosition);
	cSprite::update();
}

// Update shield apperance by damage
void cShield::update() {
	damage++;
	setTexture(new cD3DXTexture(device, shieldFilenames[damage]));
}

 // Return shield damage
int cShield::getDamage() {
	return damage;
}