#pragma once
#include "gamestate.h"
#include "player.h"
#include "tile.h"
#include "tilegroup.h"

class State_PlaceTile : public GameState
{
	bool placeTile(Tile * t);
	void moveCursorToSelectedTile();
public:
	static void nextState(Game * m_game);
	static void mergerSetup(Game * g, Tile * t, Queue<TileGroup*> * a_companies, TileGroup * dominantCompany);
	virtual void init(Game * g);
	virtual const char *getName(){return ", select a tile to place (left/right/enter)";}
	virtual void processInput(int key);
	virtual void draw(Graphics * g);
};
