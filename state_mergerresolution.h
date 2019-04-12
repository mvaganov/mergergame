#pragma once
#include "gamestate.h"
#include "queue.h"
#include "tilegroup.h"
#include "tile.h"

class State_MergerResolution : public GameState
{
	/** the company taking over other companies */
	TileGroup * dominantCompany;
	/** which companies are being merged */
	Queue<TileGroup*> * m_companies;
	/** the tile that started it all... */
	Tile * tile;
public:
	State_MergerResolution(TileGroup * dominantCompany, Queue<TileGroup*> * a_companies, Tile * tile)
		:dominantCompany(dominantCompany),m_companies(a_companies),tile(tile)
	{}

	virtual const char *getName(){return "Merger Resolution";}
	virtual void init(Game * a_g);
};