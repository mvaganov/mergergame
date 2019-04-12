#pragma once
#include "gamestate.h"
#include "player.h"
#include "tile.h"

class State_Cursor : public GameState
{
	Vector2 cursor;
	void triggerNextTurn();
public:
	virtual const char *getName(){return "Intermission";}
	virtual void draw(Graphics * g);
	virtual void processInput(int key);
	virtual void init(Game * g);
};
