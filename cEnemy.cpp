#include "cEnemy.h"

// Array of enemy image filenames
char* enemyFilenames[] = {"Images\\enemy0.png", "Images\\enemy1.png", "Images\\enemy2.png", "Images\\enemy3.png",
						"Images\\enemy4.png", "Images\\enemy5.png", "Images\\enemy6.png", "Images\\enemy7.png"};

cEnemy::cEnemy(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, int textureNumber) {
	device = pd3dDevice;
	score = 120/(textureNumber+2);
	cEnemy::textureNumber = textureNumber;
	setTranslation(D3DXVECTOR2(8.0f, 0.0f));
	setTexture(new cD3DXTexture(device, enemyFilenames[textureNumber]));
	setSpritePos(sPosition);
	cSprite::update();
}

 // Update enemy animation and position
void cEnemy::update(float deltaTime) {
	if(textureNumber % 2 == 0)
		textureNumber += 1;
	else
		textureNumber -= 1;
	setTexture(new cD3DXTexture(device, enemyFilenames[textureNumber]));
	cSprite::update(deltaTime);
}

 // Return enemy points worth
int cEnemy::getScore() {
	return score;
}