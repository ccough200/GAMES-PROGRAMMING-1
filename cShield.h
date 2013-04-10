#ifndef _CSHIELD_H
#define _CSHIELD_H
#include "cSprite.h"

class cShield : public cSprite {
private:
	LPDIRECT3DDEVICE9 device;
	int damage;

public:
	cShield(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice); // Contructor
	void update(); // Update shield apperance by damage
	int getDamage(); // Return shield damage
};
#endif