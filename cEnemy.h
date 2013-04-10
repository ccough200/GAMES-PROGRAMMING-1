#ifndef _CENEMY_H
#define _CENEMY_H
#include "cSprite.h"

class cEnemy : public cSprite {
private:
	LPDIRECT3DDEVICE9 device;
	int textureNumber;
	int score;

public:
	cEnemy(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, int textureNumber); // Constructor
	void update(float deltaTime); // Update enemy animation and position
	int getScore(); // Return enemy points worth
};
#endif