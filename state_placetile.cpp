#include "state_placetile.h"
#include "state_pickunplacedcompany.h"
#include "state_pickcontrollingcompany.h"
#include "state_buystock.h"
#include "state_cursor.h"
#include "state_mergeoptions.h"
#include "state_mergerresolution.h"
#include "state_endgame.h"
#include "game.h"
#include "mem.h"
#include "cli.h"

void State_PlaceTile::init(Game * g){
	GameState::init(g);
	m_game->clearCompanySelection();
//	m_game->selectPurchasableStock(m_game->getCurrentPlayer());
}

void State_PlaceTile::processInput(int key){
	Player * p = m_game->getCurrentPlayer();
	Tile * t, *nextTile;
	switch(key)
	{
	case CLI::KEY::LEFT:	p->changeSelectedTile(-1);	break;
	case CLI::KEY::RIGHT:	p->changeSelectedTile(+1);	break;
	case '\n':
	case '\r':
		// if the player wants to place a tile, and has a tile
		if(p->hasTiles()){
			// put tile on board
			t = p->getSelectedTile();
			t->setPlacement(m_game->getTurn());
			// remove tile from player control
			p->removeSelectedTile();
			nextTile = m_game->getRandomUnusedTile();
			if(nextTile)
				p->addTile(nextTile);
			p->orderTiles();
			// resolve the tile placement (lots of logic!)
			placeTile(t);
		}else{ // if the player has no tile... move along...
			nextState(m_game);
			setDone(true);
		}
		break;
	}
}

/**
 * @return true if the tile placement is valid
 */
bool State_PlaceTile::placeTile(Tile * t)
{
	bool tilePlacementResolved = false;
	Player * p = m_game->getCurrentPlayer();
	TileGroup * defaultCompany = m_game->getDefaultCompany();
	// check to see what tiles are neighbors to the placed tile
	Tile * border[4];
	Board * b = m_game->getBoard();
	int x = t->getX(), y = t->getY();
	border[0] = (y<b->getHeight())?	b->getTile(x  , y+1):0;
	border[1] = (x>0)?				b->getTile(x-1, y  ):0;
	border[2] = (x<b->getWidth())?	b->getTile(x+1, y  ):0;
	border[3] = (y>0)?				b->getTile(x  , y-1):0;
	// find out what companies are neighboring this new tile
	Queue<TileGroup*> * involvedCompanies = NEWMEM(Queue<TileGroup*>());
	for(int i = 0; i < 4; ++i){
		if(border[i] && border[i]->getCo()
		&& involvedCompanies->indexOf(border[i]->getCo()) < 0){
			involvedCompanies->enqueue(border[i]->getCo());
		}
	}
	// check to see if a default company is a neighbor.
	int defaultCompanyIndex = involvedCompanies->indexOf(defaultCompany);
	// based on the number of companies that the new tile is being placed next to...
	switch(involvedCompanies->size)
	{
	case 0:	// if there are no companies here, this tile is a defaultCompany tile
		t->setCo(defaultCompany);
		// placement is complete!
		tilePlacementResolved = true;
		break;
	case 1:	// if there is just 1 company neighboring here...
		// if that company is a defaul company, and new companies are available
		if(involvedCompanies->list[0] == defaultCompany && m_game->selectUnplacedCompany()){
			// pick the company for this tile
			m_game->queueState(NEWMEM(State_PickUplacedCompany(t)));
			setDone(true);
			// the tile placement is not yet resolved...
			tilePlacementResolved = false;
		}else{
			// otherwise, add this company to the neighbor
			t->setCo(involvedCompanies->list[0]);//defaultCompany;
			t->getCo()->addTile(t);
			tilePlacementResolved = true;
		}
		break;
	case 2:
	case 3:
	case 4:
		// merger!
		// sort companies by size
		involvedCompanies->sortReferences();
		// biggest first
		involvedCompanies->reverse();
		// if more than one company are 11 or bigger, the merger is not allowed, return false!
		if(involvedCompanies->list[1]->getSize() >= 11){
			// if the player has other placable tiles
			if(p->hasTiles()){
				// stay in this state, which will allow them to be placed
				tilePlacementResolved = false;
			}else{
				// otherwise, go to the next state
				tilePlacementResolved = true;
				break;
			}
			// don't let the merger continue...
			DELMEM(involvedCompanies);
			involvedCompanies = 0;
			break;
		}
		if(involvedCompanies){
			// remove the default company from mergers, if it is in there
			defaultCompanyIndex = involvedCompanies->indexOf(defaultCompany);
			if(defaultCompanyIndex >= 0)
				involvedCompanies->remove(defaultCompanyIndex);
			// determine which company is dominant
			TileGroup * biggest = involvedCompanies->list[0];
			// if top companies have the same size
			if(involvedCompanies->size > 1 && biggest->getSize() == involvedCompanies->list[1]->getSize()){
				// figure out all the companies that could be tied for size
				Queue<TileGroup*> * pickWinner = NEWMEM(Queue<TileGroup*>());
				pickWinner->enqueue(biggest);
				for(int i = 1; i < involvedCompanies->size; ++i){
					if(biggest->getSize() == involvedCompanies->list[i]->getSize()){
						pickWinner->enqueue(involvedCompanies->list[i]);
					}
				}
				// start the state to ask the user which company is the dominant one
				m_game->queueState(NEWMEM(State_PickControllingCompany(t, pickWinner, involvedCompanies)));
				setDone(true);
				// there are more decisions that need to be made before the placement is done
				tilePlacementResolved = false;
			}else{
				// if it is clear what company is the biggest one, setup the merger
				mergerSetup(m_game, t, involvedCompanies, biggest);
				// once the merger is setup, tile manipulation is done, though
				setDone(true);
				// there are more decisions that need to be made before the placement is done
				tilePlacementResolved = false;
			}
			// don't delete the companies involved in the merger, State_MergerResolution will-do that
			involvedCompanies = 0;
		}
		break;
	}
	if(involvedCompanies)
		DELMEM(involvedCompanies);
	// if the tile has been placed, and everything is resolved, do the next state
	if(tilePlacementResolved)
	{
		// if a company has reached the maximum size, game over
		TileGroup * maxSizeReached = m_game->companyOfSize(Game::MAX_COMPANY_SIZE);
		if(maxSizeReached){
			m_game->queueState(NEWMEM(State_EndGame("%s grew to 44+", maxSizeReached->getName())));
		}
		// if stock can be bought
		if(m_game->selectPurchasableStock(p)){
			// go to the buy stock phase
			m_game->queueState(NEWMEM(State_BuyStock));
		}else{
			// otherwise, transition to the next player
			m_game->queueState(NEWMEM(State_Cursor));
		}
		setDone(true);
	}
	return true;
}

void State_PlaceTile::nextState(Game * m_game)
{
	// if there are no more placeable tiles
	if(!m_game->isThereAnotherTileToPlace()){
		// the game is over
		m_game->queueState(NEWMEM(State_EndGame("All of the tiles were placed", 0)));
	}else if(m_game->selectPurchasableStock(m_game->getCurrentPlayer())){	// if stock can be bought
		// go to the buy stock phase
		m_game->queueState(NEWMEM(State_BuyStock));
	}else{
		// otherwise, transition to the next player
		m_game->queueState(NEWMEM(State_Cursor));
	}
}

struct PlayerStock{
	Player * p;
	Stock s;
	int bonus;
	PlayerStock():p(0){}
	PlayerStock(Player*p,Stock s,int bonus):p(p),s(s),bonus(bonus){}
	bool operator<(PlayerStock & other){
		return s.amount < other.s.amount;
	}
};

/**
 * @param m_game the game that the merger is happening in
 * @param t the tile that started the merger
 * @param a_companies the companies that are merging
 * @param dominantCompany which company all of the other companies are merging into
 */
void State_PlaceTile::mergerSetup(Game * m_game, Tile * t, Queue<TileGroup*> * a_companies, TileGroup * dominantCompany)
{
	TileGroup * c;
	int fbonus, sbonus;
	Queue<PlayerStock> haveShares;
	// calculate how big the dominant company will be
	int newSizeOfDominantCompany = 1; // include the merging tile!
	for(int i = 0; i < a_companies->size; ++i){
		newSizeOfDominantCompany += a_companies->list[i]->getSize();
	}
	// prepare merger resolution for each company that is involved in the merger process
	for(int i = 0; i < a_companies->size; ++i)
	{
		c = a_companies->list[i];
		// don't do the merger process for the dominant company
		if(c == dominantCompany)
			continue;
		// check each player
		for(int i = 0; i < m_game->getPlayerCount(); ++i){
			Player * p = m_game->getPlayer(i);
			// if the player has stock in this company
			Stock * foundStock = p->hasStockIn(c);
			if(foundStock && foundStock->amount > 0){
				// add the player to the list
				haveShares.enqueue(PlayerStock(p,*foundStock,0));
			}
		}
		// sort the share holders
		haveShares.sort();
		// the biggest share holders should be first
		haveShares.reverse();
		// calculate majority shareholder bonus
		fbonus = c->getMajorityShareholderBonus(1);
		sbonus = c->getMajorityShareholderBonus(2);
		// check for ties
		int tiesForFirst = 0;
		for(int i = 1; i < haveShares.size; ++i){
			if(haveShares.list[i].s.amount == haveShares.list[0].s.amount){
				tiesForFirst++;
			}
		}
		// calculate the shareholder bonus
		// special logic for ties...
		if(tiesForFirst > 0){
			// divide the sum of the first and second among tied holders
			fbonus += sbonus;
			fbonus /= (tiesForFirst+1);
			for(int i = 1; i <= tiesForFirst; ++i){
				haveShares.list[i].bonus = fbonus;
			}
		}else{
			// pit the give the first and second shareholders their bonus 
			haveShares.list[0].bonus = fbonus;
			if(haveShares.size > 1){
				haveShares.list[1].bonus = sbonus;
			}
		}
		// ask each player, starting with the current player, what to do with shares?
		Player * nextToResolve;
		for(int i = 0; i < m_game->getPlayerCount(); ++i)
		{
			int playerIndex = (m_game->getCurrentPlayerIndex()+i)%m_game->getPlayerCount();
			nextToResolve = m_game->getPlayer(playerIndex);
			// if that player has shares
			int haveSharesIndex = -1;
			for(int a = 0; haveSharesIndex < 0 && a < haveShares.size; ++a){
				if(haveShares.list[a].p == nextToResolve)
					haveSharesIndex = a;
			}
			if(haveSharesIndex >= 0 && haveShares.list[haveSharesIndex].s.amount > 0)
			{
				// that player resolves their shares next
				m_game->queueState(NEWMEM(State_MergeOptions(t,
					haveShares.list[haveSharesIndex].p, 
					dominantCompany, 
					haveShares.list[haveSharesIndex].s, haveSharesIndex+1,
					haveShares.list[haveSharesIndex].bonus,
					newSizeOfDominantCompany)));
			}
		}
	}
	// once all of the players have decided how to deal with the merger on their end, do the final resolution
	m_game->queueState(NEWMEM(State_MergerResolution(dominantCompany, a_companies, t)));
}

void State_PlaceTile::draw(Graphics * g)
{
	Vector2 loc(0, m_game->getBoard()->getHeight()*Tile::DRAW_HEIGHT+1);
	Player * p = m_game->getCurrentPlayer();
	int color;
	Tile * t, * currentSelection = p->getSelectedTile();
	int possibleMoveColor = m_game->flashColor(0,g);//Graphics::DARK_GRAY;
	for(int i = 0; i < p->getTileCount(); ++i)
	{
		t = p->getTile(i);
		color = CLI::COLOR::DEFAULT;
		g->resetColor();
		if(t == currentSelection)
		{
			color = m_game->flashColor(1,g);
		}
#define WIDTH_OF_XY_COORDINATE_TEXT	8
		g->move(loc.y, loc.x+i*WIDTH_OF_XY_COORDINATE_TEXT);
		g->printf("[%2d,%2d]",t->getX(), t->getY());
		color = possibleMoveColor;
		if(t == currentSelection)
		{
			t->draw(g, Tile::DEFAULT_PLACEMENT, m_game->flashColor(0,g));
		}else{
			t->draw(g, Tile::DEFAULT_UNPLACED, color = m_game->flashColor(0,g));
		}
	}
}