#include "player.h"
#include "stock.h"
#include "tilegroup.h"
#include "tile.h"
#include "game.h"

#include "cli.h"

void Player::drawInventory(Graphics * g, Vector2 loc)
{
	int value, totalStockValue = 0;
	Stock * s;
	int nonZeroStock = 0;
	g->resetColor();
	for(int i = 0; i < 10; ++i)
	{
		if(i < stockSummary->size)
		{
			s = &stockSummary->list[i];
			if(s->amount > 0){
				value = s->company->getShareCost() * s->amount;
				g->move(loc.y+1+i, loc.x);
				g->printf("%13s x%2d = $%d", s->company->getName(), s->amount, value);
				totalStockValue += value;
				nonZeroStock++;
			}
		}
	}
	g->move(loc.y, loc.x);
	g->printf("%12s cash = $%d", name, cash);
	g->move(loc.y+nonZeroStock+1, loc.x);
	g->printf("%11s total = $%d", 
		name, cash+totalStockValue);
}

bool Player::addStock(TileGroup * c, int shares, Game * m_game)
{
	Stock * lastStock = (portfolio->size > 0)?(&portfolio->list[portfolio->size-1]):0;
	// if multiple stock changes happened in one turn, put them all in the same turn
	if(lastStock && lastStock->company == c && lastStock->turnPurchased == m_game->getTurn()){
		lastStock->amount++;
	}else{
		// giving stock to a company that doesn't exist... bad player!
		if(!c || !c->isOnBoard() || c->getTier() < 0)
		{
			int i = 0;
			i = 1 / i;
		}
		portfolio->enqueue(Stock(c, shares, m_game->getTurn(), this));
	}
	c->addFreeShares(-shares);
	calculateStockSummary();
	return true;
}

int Player::getValueOfAllStock(){
	int stockValue = 0;
	Stock * s;
	for(int ss = 0; ss < stockSummary->size; ++ss){
		s = &stockSummary->list[ss];
		stockValue += s->amount * s->company->getShareCost();
	}
	return stockValue;
}
