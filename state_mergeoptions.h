#pragma once
#include "gamestate.h"
#include "player.h"
#include "tilegroup.h"

class State_MergeOptions : public GameState
{
	/** which tile is causing this merger */
	Tile * tile;
	virtual const char *getName();
	/** which player is resolving */
	Player * p;
	/** which company is absorbing the other company */
	TileGroup * boss;
	/** the other company shares that are being resolved */
	Stock s;
	/** stock holder rank */
	int rank;
	/** bonus for being the stork holder of this rank */
	int bonus;
	/** how many shares will be sold (take from kept, give to trade)*/
	int sell;
	/** how many shares will be traded (take from sell, give to keep) */
	int trade;
	/** how many shares will be kept (take from trade, give to sell) */
	int keep;
	/** 0: sell, 1: trade, 2: keep */
	int uiState;
	/** how big the new company will be */
	int newCompanySize;
public:
	State_MergeOptions(Tile * t, Player * p, TileGroup * boss, Stock & s, int rank, int bonus, int newCompanySize)
		:tile(t),p(p),boss(boss),s(s),rank(rank),bonus(bonus),uiState(0),newCompanySize(newCompanySize)
	{
		sell = s.amount;
		trade = 0;
		keep = 0;
	}
	virtual void init(Game * a_g);
	virtual void release();
	virtual void draw(Graphics * g);
	virtual void processInput(int key);
};