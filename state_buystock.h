#pragma once
#include "gamestate.h"
#include "player.h"
#include "tilegroup.h"
#include "tile.h"

class State_BuyStock : public GameState
{
	int stockBoughtThisTurn;
public:
	virtual const char *getName();
	virtual void processInput(int key);
	virtual void init(Game * g);
	void nextState();
};
