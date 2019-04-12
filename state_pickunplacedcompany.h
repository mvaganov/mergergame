#pragma once
#include "gamestate.h"
#include "tile.h"

class State_PickUplacedCompany : public GameState
{
public:
	Tile * selectedTile;
	State_PickUplacedCompany(Tile * tile){selectedTile = tile;}
	virtual const char *getName(){return ", decide which company is forming\n(up/down/enter)";}
	virtual void init(Game * a_g);
	virtual void release();
	virtual void processInput(int key);
	virtual void draw(Graphics * g);
};
