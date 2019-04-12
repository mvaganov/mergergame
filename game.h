#pragma once
#include "board.h"
#include "queue.h"
#include "player.h"
#include "tile.h"
#include "gamestate.h"
#include "tilegroup.h"
#include "graphics.h"
#include "companySelector.h"

class Game
{
	/** manages and draws the board, including tiles */
	Board * board;
	/** all players in this game */
	Queue<Player> * players;
	/** all companies in this game */
	Queue<TileGroup> * companies;
	/** the UI filter for the company list */
	CompanySelector * companySelector;
	/** who the current active player is */
	int currentPlayer;
	/** whether or not the game wants to keep running */
	bool m_running;
	/** stores the next user input to process */
	int userInput;
	/** what turn it is right now */
	int turn;
	/** how many updates have happened */
	unsigned int updates;
	/** the current (and pending) UI */
	Queue<GameState*> * m_stateQueue;
	/** which player to display the inventory of (will use current player if null) */
	Player * inventoryPlayer;
public:
	static const int MAX_COMPANY_SIZE = 44;

	/** @return whether or not the game wants to keep running */
	bool isRunning(){return m_running;}
	/** get a color for a frame-dependent color animation */
	int flashColor(int style, Graphics * g);

	int getTurn(){return turn;}

	void nextTurn(){
		turn++;
		currentPlayer++;
		if(currentPlayer >= players->size)
			currentPlayer = 0;
	}

	bool isThereAnotherTileToPlace()
	{
		bool placableTilesExist = board->countUnusuedTiles() != 0;
		if(!placableTilesExist){
			Player * p;
			// and none of the players have any tiles either
			for(int i = 0; !placableTilesExist && i < players->size; ++i){
				p = &players->list[i];
				placableTilesExist = p->hasTiles();
			}
		}
		return placableTilesExist;
	}

	TileGroup * companyOfSize(int a_size){
		TileGroup * maxSizeReached = 0;
		for(int i = 0; !maxSizeReached && i < companies->size; ++i){
			if(companies->list[i].getSize() >= a_size)
				maxSizeReached = &companies->list[i];
		}
		return maxSizeReached;
	}

	void drawInventoryForPlayerThisState(Player * p){
		inventoryPlayer = p;
	}

	Board * getBoard(){return board;}

	Player * getPlayer(int index){return &players->list[index];}
	int getPlayerCount(){return players->size;}

	Tile * getRandomUnusedTile(){return board->getRandomUnusedTile();}

	TileGroup * getDefaultCompany(){
		return &companies->list[0];
	}

	/** add a game state to the queue */
	void queueState(GameState * a_state){
		m_stateQueue->enqueue(a_state);
	}

	/** advance to the next state in the queue */
	void nextState(){
		inventoryPlayer = 0;	// draw the default player inventory
		GameState * state;
		if(m_stateQueue->size > 0){
			state = m_stateQueue->pop();
			state->release();
			DELMEM(state);
		}
		if(m_stateQueue->size > 0){
			state = m_stateQueue->peek();
			state->init(this);
		}
	}

	void initStateMachine();

	Game(int numPlayers, int width, int height);
	void release();
	~Game(){release();}

	void draw(Graphics * g);

	bool selectPurchasableStock(Player * p){
		TileGroup * c;
		companySelector->setSelectabilityForAll(SelectionOption(false,false));
		for(int i = 1; i < companies->size; ++i)
		{
			c = &companies->list[i];
			companySelector->setSelectability(c, SelectionOption(
				// selectable if the player can buy it
				c->getFreeShares() > 0 && p->getCash() >= c->getShareCost(),
				// visible if it is on the board
				c->isOnBoard()));
		}
		return companySelector->ensureValidSelection();
	}

	bool selectUnplacedCompany(){
		TileGroup * c;
		companySelector->setSelectabilityForAll(SelectionOption(false,false));
		for(int i = 1; i < companies->size; ++i)
		{
			c = &companies->list[i];
			companySelector->setSelectability(c, SelectionOption(
				// selectable if the player can place it
				!c->isOnBoard(),
				// all companies visible
				true));
		}
		return companySelector->ensureValidSelection();
	}

	bool selectOneOfThese(Queue<TileGroup*> * list){
		TileGroup * c;
		companySelector->setSelectabilityForAll(SelectionOption(false,false));
		for(int i = 1; i < companies->size; ++i)
		{
			c = &companies->list[i];
			bool inList = list->indexOf(c) >= 0;
			companySelector->setSelectability(c, SelectionOption(
				// selectable and visible if it is in the list
				inList, inList));
		}
		return companySelector->ensureValidSelection();
	}

	void clearCompanySelection(){
		companySelector->setSelectabilityForAll(SelectionOption(true,true));
		companySelector->clearSelection();
	}

	bool changeCompanySelection(int direction){
		return companySelector->changeSelection(direction);
	}
	CompanySelector * getCompanySelector(){
		return companySelector;
	}

	/**
	 * @param t which tile to start the fill at
	 * @param src which company to replace in that connected region
	 * @param dest which company to replace to in that connected region
	 * @param whatIf put an empty Queue<Step> here to do a "what-if" calculation instead of an actual fill
	 */
	void fillTilesHereAs(Tile * t, TileGroup * src, TileGroup * dest, Queue<Step> * whatIf)
	{
		if(!whatIf)
		{
			t->setCo(src);
			Queue<Step> doIt; 
			int changed =
				board->floodFill(t->getX(), t->getY(), src, dest, &doIt);
			Tile * acquired;
			for(int i = 0; i < doIt.size; ++i){
				acquired = board->getTile(doIt.list[i].x, doIt.list[i].y);
				dest->addTile(acquired);
				acquired->setCo(dest);
			}
		}
		else
		{
			TileGroup * oldCo = t->getCo();
			t->setCo(src);
			int changed =
				board->floodFill(t->getX(), t->getY(), src, dest, whatIf);
			t->setCo(oldCo);
		}
	}

	void setInput(int a_input);
	void processInput();

	bool isAcceptingInput(){
		GameState * state = m_stateQueue->peek();
		return !state->isDone();
	}

	void update()
	{
		processInput();
		GameState * state = m_stateQueue->peek();
		if(state->isDone())
			nextState();
		updates++;
	}

	Player * getCurrentPlayer(){
		return &players->list[currentPlayer];
	}

	int getCurrentPlayerIndex(){return currentPlayer;}

	TileGroup * getSelectedCompany(){
		return companySelector->getSelectedCompany();
	}
};
