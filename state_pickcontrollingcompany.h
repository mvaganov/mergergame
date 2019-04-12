#pragma once
#include "gamestate.h"
#include "tile.h"

class State_PickControllingCompany : public GameState
{
public:
	Tile * t;
	Queue<TileGroup*> * pickWinner;
	Queue<TileGroup*> * involvedCompanies;
	State_PickControllingCompany(Tile * tile, 
		Queue<TileGroup*> * pickWinner, 
		Queue<TileGroup*> * involvedCompanies)
		:t(tile),pickWinner(pickWinner),involvedCompanies(involvedCompanies){}
	virtual const char *getName(){return ", pick the controlling company\n(up/down/enter)";}
	virtual void processInput(int key);
	virtual void draw(Graphics * g);
	virtual void init(Game * g);
};