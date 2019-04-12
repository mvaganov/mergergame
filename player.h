#pragma once
#include "queue.h"
#include "stock.h"
#include "tile.h"
#include "graphics.h"
#include "mem.h"

class Game;

class Player
{
private:
	/** */
	const char *name;
	/** how much cash the player has to buy stock with */
	int cash;
	/** a complete history of all stock purchases made */
	Queue<Stock> * portfolio;
	/** a summary of current stock holdings */
	Queue<Stock> * stockSummary;
	/** tiles that this player has to place*/
	Queue<Tile*> * tiles;
	/** which tile the player selected last */
	int selectedTile;
public:
	const char *getName(){return name;}

	bool changeSelectedTile(int dir){
		if(tiles->size){
			selectedTile += dir;
			while(selectedTile < 0)				selectedTile += tiles->size;
			while(selectedTile >= tiles->size)	selectedTile -= tiles->size;
			return true;
		}
		return false;
	}
	bool hasTiles() const {
		return tiles->size > 0;
	}

	Tile * getSelectedTile(){	return tiles->list[selectedTile];	}
	Tile * getTile(int index){	return tiles->list[index];	}
	int getTileCount(){	return tiles->size;	}

	void setName(const char *a_name){name = a_name;}

	void removeSelectedTile(){
		tiles->remove(selectedTile);
	}
	void addTile(Tile * t){
		tiles->enqueue(t);
	}

	Player():name(0),cash(6000),selectedTile(0){
		tiles = NEWMEM(Queue<Tile*>());
		portfolio = NEWMEM(Queue<Stock>());
		stockSummary = NEWMEM(Queue<Stock>());
	}
	~Player(){
		DELMEM(tiles);
		DELMEM(portfolio);
		DELMEM(stockSummary);
	}

	Stock * hasStockIn(TileGroup * c){
		for(int i = 0; i < stockSummary->size; ++i)
		{
			if(stockSummary->list[i].company == c)
				return &stockSummary->list[i];
		}
		return 0;
	}

	void calculateStockSummary()
	{
		// see what stocks are owned
		Queue<TileGroup*> ownedStocks;
		for(int i = 0; i < portfolio->size; ++i)
		{
			if(ownedStocks.indexOf(portfolio->list[i].company) < 0)
				ownedStocks.enqueue(portfolio->list[i].company);
		}
		// prepare to account for those stocks
		stockSummary->setSizeUsed(ownedStocks.size);
		for(int i = 0; i < stockSummary->size; ++i)
		{
			stockSummary->list[i].amount = 0;
			stockSummary->list[i].company = ownedStocks.list[i];
		}
		// account
		for(int i = 0; i < portfolio->size; ++i)
		{
			stockSummary->list[ownedStocks.indexOf(portfolio->list[i].company)].amount += portfolio->list[i].amount;
		}
		// sort
		stockSummary->sort();
		// biggest should be first
		stockSummary->reverse();
	}

	int getValueOfAllStock();

	bool addStock(TileGroup * c, int shares, Game * m_game);

	void addCash(int value){cash += value;}
	int getCash(){return cash;}

	void drawInventory(Graphics * g, Vector2 loc);

	void orderTiles()
	{
#ifdef RIGGED_GAME
		if(true)return;
#endif
		tiles->sortReferences();
	}
};

